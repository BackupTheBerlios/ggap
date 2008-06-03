/*
 *   gapworksheet.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "ggap-i18n.h"
#include "gapworksheet.h"
#include "gapprocess.h"
#include "gapapp.h"
#include "gapparser.h"
#include "ggapfile.h"
#include "gap.h"
#include "gapwscompletion.h"
#include "ggap-enums.h"
#include "mooui/mdfileops.h"
#include "mooterm/mootermpt.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/mooutils-debug.h"
#include "mooutils/mootype-macros.h"
#include "moows/moowsblock.h"
#include <glib/gregex.h>
#include <gdk/gdkkeysyms.h>
#include <errno.h>
#include <stdlib.h>

MOO_DEBUG_INIT(gap, FALSE)

struct GapWorksheetPrivate {
    GapProcess *proc;
    gboolean allow_error_break;

    MooTextCompletion *completion;
    MooCompletionGroup *cmpl_group;

    GapFileType file_type;

    int width;
    int height;
    guint resize_idle;
};


static void     doc_iface_init                  (MdDocumentIface *iface);
static void     gap_worksheet_close             (MdDocument     *doc);

static GObject *gap_worksheet_constructor       (GType           type,
                                                 guint           n_props,
                                                 GObjectConstructParam *props);
static void     gap_worksheet_dispose           (GObject        *object);
static void     gap_worksheet_modified_changed  (GapWorksheet   *ws);

static void     gap_worksheet_size_allocate     (GtkWidget      *widget,
                                                 GtkAllocation  *allocation);
static void     gap_worksheet_style_set         (GtkWidget      *widget,
                                                 GtkStyle       *old_style);
static void     gap_worksheet_realize           (GtkWidget      *widget);
static gboolean gap_worksheet_key_press         (GtkWidget      *widget,
                                                 GdkEventKey    *event);

static void     gap_worksheet_process_input     (MooWorksheet   *mws,
                                                 char          **lines);

static gboolean gap_worksheet_fork_command      (GapWorksheet   *ws,
                                                 const char     *cmd_line,
                                                 GError        **error);
static void     gap_worksheet_kill_child        (GapWorksheet   *ws);

static void     gap_worksheet_load_file         (MdDocument     *doc,
                                                 MdFileInfo     *file_info,
                                                 MdFileOpInfo   *op_info);
static void     gap_worksheet_save_file         (MdDocument     *doc,
                                                 MdFileInfo     *file_info,
                                                 MdFileOpInfo   *op_info);

static void     gap_worksheet_free_completion   (GapWorksheet   *ws);
static void     gap_globals_changed             (GapWorksheet   *ws,
                                                 const char     *data,
                                                 guint           data_len,
                                                 gboolean        added);


/* GAP_TYPE_WORKSHEET */
G_DEFINE_TYPE_WITH_CODE (GapWorksheet, gap_worksheet, MOO_TYPE_WORKSHEET,
                         G_IMPLEMENT_INTERFACE (MD_TYPE_DOCUMENT, doc_iface_init))
static MdDocumentIface *gap_worksheet_doc_parent_iface;


enum {
    PROP_0,
    PROP_GAP_STATE,
    PROP_ALLOW_ERROR_BREAK,
    PROP_MD_DOC_STATUS,
    PROP_MD_DOC_STATE,
    PROP_MD_DOC_READONLY,
    PROP_MD_DOC_FILE_INFO,
    PROP_MD_DOC_URI
};

static void
gap_worksheet_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    GapWorksheet *ws = GAP_WORKSHEET (object);

    switch (prop_id)
    {
        case PROP_ALLOW_ERROR_BREAK:
            ws->priv->allow_error_break = g_value_get_boolean (value);
            g_object_notify (G_OBJECT (ws), "allow-error-break");
            break;

        case PROP_MD_DOC_FILE_INFO:
            md_document_set_file_info (MD_DOCUMENT (ws), g_value_get_boxed (value));
            break;

        case PROP_MD_DOC_STATUS:
            md_document_set_status (MD_DOCUMENT (ws), g_value_get_flags (value));
            break;
        case PROP_MD_DOC_STATE:
            md_document_set_state (MD_DOCUMENT (ws), g_value_get_enum (value));
            break;
        case PROP_MD_DOC_READONLY:
            md_document_set_readonly (MD_DOCUMENT (ws), g_value_get_boolean (value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
gap_worksheet_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    GapWorksheet *ws = GAP_WORKSHEET (object);

    switch (prop_id)
    {
        case PROP_ALLOW_ERROR_BREAK:
            g_value_set_boolean (value, ws->priv->allow_error_break);
            break;

        case PROP_GAP_STATE:
            if (ws->priv->proc)
                g_value_set_enum (value, gap_process_get_state (ws->priv->proc));
            else
                g_value_set_enum (value, GAP_DEAD);
            break;

        case PROP_MD_DOC_FILE_INFO:
            g_value_set_boxed (value, md_document_get_file_info (MD_DOCUMENT (ws)));
            break;
        case PROP_MD_DOC_URI:
            g_value_take_string (value, md_document_get_uri (MD_DOCUMENT (ws)));
            break;

        case PROP_MD_DOC_STATUS:
            g_value_set_flags (value, md_document_get_status (MD_DOCUMENT (ws)));
            break;
        case PROP_MD_DOC_STATE:
            g_value_set_enum (value, md_document_get_state (MD_DOCUMENT (ws)));
            break;
        case PROP_MD_DOC_READONLY:
            g_value_set_boolean (value, md_document_get_readonly (MD_DOCUMENT (ws)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
gap_worksheet_class_init (GapWorksheetClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
    MooWorksheetClass *ws_class = MOO_WORKSHEET_CLASS (klass);

    object_class->constructor = gap_worksheet_constructor;
    object_class->get_property = gap_worksheet_get_property;
    object_class->set_property = gap_worksheet_set_property;
    object_class->dispose = gap_worksheet_dispose;

    widget_class->size_allocate = gap_worksheet_size_allocate;
    widget_class->style_set = gap_worksheet_style_set;
    widget_class->realize = gap_worksheet_realize;
    widget_class->key_press_event = gap_worksheet_key_press;

    ws_class->process_input = gap_worksheet_process_input;

    g_signal_new ("gap-exited",
                  GAP_TYPE_WORKSHEET,
                  G_SIGNAL_RUN_LAST,
                  0, NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

    g_object_class_install_property (object_class, PROP_ALLOW_ERROR_BREAK,
        g_param_spec_boolean ("allow-error-break", "allow-error-break", "allow-error-break",
                              FALSE, G_PARAM_READWRITE));

    g_object_class_install_property (object_class, PROP_GAP_STATE,
        g_param_spec_enum ("gap-state", "gap-state", "gap-state",
                           GAP_TYPE_STATE, GAP_DEAD, G_PARAM_READABLE));

    g_object_class_override_property (object_class, PROP_MD_DOC_STATUS, "md-doc-status");
    g_object_class_override_property (object_class, PROP_MD_DOC_READONLY, "md-doc-readonly");
    g_object_class_override_property (object_class, PROP_MD_DOC_STATE, "md-doc-state");
    g_object_class_override_property (object_class, PROP_MD_DOC_FILE_INFO, "md-doc-file-info");
    g_object_class_override_property (object_class, PROP_MD_DOC_URI, "md-doc-uri");

    g_type_class_add_private (klass, sizeof (GapWorksheetPrivate));
}


static void
gap_state_notify (GapWorksheet *ws)
{
    g_object_notify (G_OBJECT (ws), "gap-state");
}


static gboolean
gap_worksheet_start_gap (GapWorksheet *ws,
                         const char   *workspace)
{
    GError *error = NULL;
    char *cmd_line;
    const char *args;

    if (ws->priv->allow_error_break)
        args = "-n";
    else
        args = "-n -T";

    cmd_line = gap_make_cmd_line (workspace, args, TRUE);
    g_return_val_if_fail (cmd_line != NULL, FALSE);

//     moo_worksheet_reset (MOO_WORKSHEET (view));

    if (!gap_worksheet_fork_command (ws, cmd_line, &error))
    {
        moo_worksheet_write_error (MOO_WORKSHEET (ws), error->message);
        g_error_free (error);
        return FALSE;
    }

    g_free (cmd_line);
    return TRUE;
}


static void
gap_worksheet_dispose (GObject *object)
{
    GapWorksheet *ws = GAP_WORKSHEET (object);

    if (ws->priv->resize_idle)
        g_source_remove (ws->priv->resize_idle);
    ws->priv->resize_idle = 0;

    G_OBJECT_CLASS (gap_worksheet_parent_class)->dispose (object);
}

static void
gap_worksheet_init (GapWorksheet *ws)
{
    ws->priv = G_TYPE_INSTANCE_GET_PRIVATE (ws, GAP_TYPE_WORKSHEET, GapWorksheetPrivate);

    ws->priv->proc = NULL;
    ws->priv->width = -1;
    ws->priv->height = -1;

    ws->priv->file_type = GAP_FILE_WORKSHEET;

    md_document_set_capabilities (MD_DOCUMENT (ws),
                                  MD_DOCUMENT_SUPPORTS_SAVE);
}


static void
gap_worksheet_modified_changed (GapWorksheet *ws)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (ws));
    md_document_set_modified (MD_DOCUMENT (ws), gtk_text_buffer_get_modified (buffer));
}

static GObject *
gap_worksheet_constructor (GType           type,
                           guint           n_props,
                           GObjectConstructParam *props)
{
    GObject *object;

    object = G_OBJECT_CLASS (gap_worksheet_parent_class)->constructor (type, n_props, props);

    moo_worksheet_start_input (MOO_WORKSHEET (object), "gap> ", "> ");
    gap_worksheet_start_gap (GAP_WORKSHEET (object), NULL);
    moo_worksheet_set_accepting_input (MOO_WORKSHEET (object), FALSE);

    gtk_text_buffer_set_modified (gtk_text_view_get_buffer (GTK_TEXT_VIEW (object)), FALSE);
//     md_document_set_modified (MD_DOCUMENT (object), FALSE);

    g_signal_connect_swapped (gtk_text_view_get_buffer (GTK_TEXT_VIEW (object)),
                              "modified-changed", G_CALLBACK (gap_worksheet_modified_changed),
                              object);

    return object;
}


static void
gap_worksheet_close (MdDocument *doc)
{
    GapWorksheet *ws = GAP_WORKSHEET (doc);

    if (ws->priv->proc)
        gap_worksheet_kill_child (ws);

    gap_worksheet_free_completion (ws);

    gap_worksheet_doc_parent_iface->close (doc);
}


static gboolean
resize_idle (GapWorksheet *ws)
{
    GtkAllocation *allocation = &GTK_WIDGET(ws)->allocation;

    if (!GTK_WIDGET_REALIZED (ws) || allocation->width <= 1 || allocation->height <= 1)
    {
        ws->priv->width = -1;
        ws->priv->height = -1;
    }
    else
    {
        PangoLayout *layout;
        int width, height;

        layout = gtk_widget_create_pango_layout (GTK_WIDGET (ws), " ");
        pango_layout_get_pixel_size (layout, &width, &height);

#define HOW_MANY(x__,y__) (((x__) + (y__) - 1) / (y__))
        ws->priv->width = HOW_MANY (allocation->width, width);
        ws->priv->height = HOW_MANY (allocation->height, height);
#undef HOW_MANY

        /* leave one char to make sure horizontal scrollbar won't appear */
        ws->priv->width = MAX (ws->priv->width - 1, 10);
        ws->priv->height = MAX (ws->priv->height, 10);
        g_object_unref (layout);
    }

    if (ws->priv->proc)
        gap_process_set_size (ws->priv->proc, ws->priv->width, ws->priv->height);

    ws->priv->resize_idle = 0;
    return FALSE;
}

static void
queue_resize (GapWorksheet *ws)
{
    if (!ws->priv->resize_idle)
        ws->priv->resize_idle = g_idle_add ((GSourceFunc) resize_idle, ws);
}

static void
gap_worksheet_size_allocate (GtkWidget     *widget,
                             GtkAllocation *allocation)
{
    GTK_WIDGET_CLASS (gap_worksheet_parent_class)->size_allocate (widget, allocation);
    queue_resize (GAP_WORKSHEET (widget));
}

static void
gap_worksheet_style_set (GtkWidget *widget,
                         GtkStyle  *prev_style)
{
    GTK_WIDGET_CLASS (gap_worksheet_parent_class)->style_set (widget, prev_style);
    queue_resize (GAP_WORKSHEET (widget));
}

static void
gap_worksheet_realize (GtkWidget *widget)
{
    GTK_WIDGET_CLASS (gap_worksheet_parent_class)->realize (widget);
    queue_resize (GAP_WORKSHEET (widget));
}


static void
gap_worksheet_set_doc_status (MdDocument       *doc,
                              MdDocumentStatus  status)
{
    gap_worksheet_doc_parent_iface->set_status (doc, status);
    gtk_text_buffer_set_modified (gtk_text_view_get_buffer (GTK_TEXT_VIEW (doc)),
                                  (status & MD_DOCUMENT_MODIFIED) != 0);
}

// static void
// gap_worksheet_get_empty (MdDocument *doc)
// {
//     GapWorksheet *ws = GAP_WORKSHEET (doc);
//     GtkTextView *view = GTK_TEXT_VIEW (doc);
//     MooWsBuffer *buffer = MOO_WS_BUFFER (gtk_text_view_get_buffer (view));
//     MooWsBlock *block;
//
//     if (md_document_get_modified (doc))
//         return FALSE;
//
//     block = _moo_ws_buffer_get_first_block (buffer);
//     if (!block || block->next || !MOO_IS_WS_PROMPT_BLOCK (block))
//         return FALSE;
//
//     text =
// }

static void
doc_iface_init (MdDocumentIface *iface)
{
    gap_worksheet_doc_parent_iface =
        md_document_interface_peek_parent (iface);

    iface->close = gap_worksheet_close;
    iface->load_file = gap_worksheet_load_file;
    iface->save_file = gap_worksheet_save_file;
    iface->set_status = gap_worksheet_set_doc_status;
//     iface->get_empty = gap_worksheet_get_empty;
}

// static void
// write_child (GapWorksheet *ws,
//              const char   *text)
// {
//     moo_term_pt_write (ws->priv->pt, text, -1);
//     write_log (text, -1, FALSE);
// }

static void
write_input (GapWorksheet  *ws,
             char         **lines,
             const char    *text)
{
    g_return_if_fail (lines && *lines);
    gap_process_write_input (ws->priv->proc, lines);
    moo_worksheet_add_history (MOO_WORKSHEET (ws), text);
}

static void
write_errors (MooWorksheet *mws,
              GapParser    *parser)
{
    int line = -1, column = -1;
    GSList *list;

    list = gap_parser_get_errors (parser);

    if (!list)
    {
        line = 0;
        column = 0;
        moo_worksheet_write_error (mws, "Syntax error");
    }
    else while (list)
    {
        GapParseError *error;

        error = list->data;
        list = list->next;

        moo_worksheet_write_error (mws, "Line %d, chars %d:%d: %s\n",
                                   error->line + 1,
                                   error->first_column + 1,
                                   error->last_column + 1,
                                   error->message);

        if (line < 0)
        {
            line = error->line;
            column = error->first_column;
        }
    }

    moo_worksheet_resume_input (mws, line, column);
}

static void
gap_worksheet_process_input (MooWorksheet   *mws,
                             char          **lines)
{
    GapWorksheet *ws = GAP_WORKSHEET (mws);
    GapParser *parser;
    char *text;

    g_return_if_fail (gap_process_get_state (ws->priv->proc) == GAP_IN_PROMPT);

    moo_worksheet_reset_history (mws);

    parser = gap_parser_new ();
    text = g_strjoinv ("\n", lines);

    switch (gap_parser_parse (parser, text))
    {
        case GAP_PARSE_OK:
            write_input (ws, lines, text);
            break;

        case GAP_PARSE_INCOMPLETE:
            moo_worksheet_continue_input (mws);
            break;

        case GAP_PARSE_ERROR:
            write_errors (mws, parser);
            break;
    }

    g_free (text);
    gap_parser_free (parser);
}


static void
gap_died (GapWorksheet *ws)
{
    g_object_unref (ws->priv->proc);
    ws->priv->proc = NULL;
    g_signal_emit_by_name (ws, "gap-exited");
    gap_state_notify (ws);
}

static void
gap_output (GapWorksheet *ws,
            const char   *data,
            guint         data_len,
            gboolean      is_stderr)
{
    if (is_stderr)
        moo_worksheet_write_error_len (MOO_WORKSHEET (ws), data, data_len);
    else
        moo_worksheet_write_output (MOO_WORKSHEET (ws), data, data_len);
}

static void
gap_prompt (GapWorksheet *ws,
            const char   *string,
            guint         len,
            gboolean      first_time)
{
    MooWorksheet *mws = MOO_WORKSHEET (ws);

    if (!first_time)
    {
        gboolean continue_input = (len == 2 && strncmp (string, "> ", 2) == 0);

        if (continue_input)
        {
            moo_worksheet_continue_input (mws);
        }
        else
        {
            char *prompt = g_strndup (string, len);
            moo_worksheet_start_input (mws, prompt, "> ");
            g_free (prompt);
        }
    }
    else
    {
        moo_worksheet_set_accepting_input (mws, TRUE);
    }
}

static gboolean
gap_worksheet_fork_command (GapWorksheet   *ws,
                            const char     *cmd_line,
                            GError        **error)
{
    g_return_val_if_fail (ws->priv->proc == NULL, FALSE);
    g_return_val_if_fail (cmd_line != NULL, FALSE);

    if (!(ws->priv->proc = gap_process_start (cmd_line,
                                              ws->priv->width,
                                              ws->priv->height,
                                              error)))
        return FALSE;

    g_signal_connect_swapped (ws->priv->proc, "gap-output",
                              G_CALLBACK (gap_output), ws);
    g_signal_connect_swapped (ws->priv->proc, "gap-prompt",
                              G_CALLBACK (gap_prompt), ws);
    g_signal_connect_swapped (ws->priv->proc, "gap-globals-changed",
                              G_CALLBACK (gap_globals_changed), ws);
    g_signal_connect_swapped (ws->priv->proc, "gap-died",
                              G_CALLBACK (gap_died), ws);
    g_signal_connect_swapped (ws->priv->proc, "notify::gap-state",
                              G_CALLBACK (gap_state_notify), ws);
    gap_state_notify (ws);

    return TRUE;
}

static void
gap_worksheet_kill_child (GapWorksheet *ws)
{
    g_return_if_fail (ws->priv && ws->priv->proc);
    g_signal_handlers_disconnect_matched (ws->priv->proc, G_SIGNAL_MATCH_DATA,
					  0, 0, NULL, NULL, ws);
    gap_process_die (ws->priv->proc);
    gap_died (ws);
}


static gboolean
gap_worksheet_load_text (GapWorksheet *ws,
                         const char   *text,
                         gsize         text_len)
{
    MooWsBlock *block;
    GString *buf;
    MooLineReader lr;
    const char *line;
    gsize line_len;

    buf = g_string_new (NULL);

    for (moo_line_reader_init (&lr, text, text_len);
         (line = moo_line_reader_get_line (&lr, &line_len, NULL)); )
    {
        guint i;
        struct {const char *str; gsize len;} pr[] = {{"gap> ", 5}, {"> ", 2}};

        while (line_len)
        {
            gboolean got_prompt = FALSE;

            for (i = 0; i < G_N_ELEMENTS (pr); ++i)
                if (line_len >= pr[i].len && strncmp (text, pr[i].str, pr[i].len) == 0)
                {
                    line_len -= pr[i].len;
                    line += pr[i].len;
                    got_prompt = TRUE;
                }

            if (!got_prompt)
                break;
        }

        if (buf->len)
            g_string_append_c (buf, '\n');
        g_string_append_len (buf, line, line_len);
    }

    moo_worksheet_reset (MOO_WORKSHEET (ws));
    block = moo_worksheet_create_prompt_block (MOO_WORKSHEET (ws), "gap> ", "> ", buf->str);
    moo_ws_buffer_append_block (MOO_WS_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (ws))), block);

    g_string_free (buf, TRUE);
    return TRUE;
}

static void
gap_worksheet_load_file (MdDocument   *doc,
                         MdFileInfo   *file_info,
                         MdFileOpInfo *op_info)
{
    char *text;
    gsize text_len;
    char *workspace_file;
    GError *error = NULL;
    char *filename;
    GapFileType type;
    gboolean result;
    GapWorksheet *ws = GAP_WORKSHEET (doc);

    if (!(filename = md_file_info_get_filename (file_info)))
    {
        md_file_op_info_set_error (op_info, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
                                   "Not a local file");
        return;
    }

    if (!ggap_file_load (filename, &type, &text, &text_len, &workspace_file, &error))
    {
        g_free (filename);
        md_file_op_info_take_error (op_info, error);
        return;
    }

    if (type == GAP_FILE_WORKSHEET)
    {
        moo_dprint ("xml: %d\n%.*s\n", (int) text_len, (int) text_len, text);
        moo_dprint ("workspace: %s\n", workspace_file ? workspace_file : "NULL");
    }
    else
    {
        moo_dprint ("text: %d\n%.*s\n", (int) text_len, (int) text_len, text);
    }

    if (type == GAP_FILE_WORKSHEET)
        result = moo_worksheet_load_xml (MOO_WORKSHEET (ws), text, text_len, &error);
    else
        result = gap_worksheet_load_text (ws, text, text_len);

    if (!result)
    {
        g_free (filename);
        md_file_op_info_take_error (op_info, error);
        return;
    }

    gap_file_info_set_file_type (file_info, type);
    ws->priv->file_type = type;

    gap_worksheet_kill_child (ws);
    gap_worksheet_start_gap (ws, workspace_file);

    op_info->status = MD_FILE_OP_STATUS_SUCCESS;

    g_free (text);
    g_free (workspace_file);
    g_free (filename);
}


GapFileType
gap_worksheet_get_file_type (GapWorksheet *ws)
{
    g_return_val_if_fail (GAP_IS_WORKSHEET (ws), 0);
    return ws->priv->file_type;
}

GapFileType
gap_file_info_get_file_type (MdFileInfo *file_info,
                             GapFileType dflt)
{
    const char *s;

    g_return_val_if_fail (file_info != NULL, 0);

    if (!(s = md_file_info_get (file_info, "gap-file-type")))
        return dflt;
    else if (strcmp (s, "text") == 0)
        return GAP_FILE_TEXT;
    else if (strcmp (s, "worksheet") == 0)
        return GAP_FILE_TEXT;

    g_warning ("%s: invalid file type string '%s'", G_STRFUNC, s);
    return dflt;
}

void
gap_file_info_set_file_type (MdFileInfo *file_info,
                             GapFileType type)
{
    if (type == GAP_FILE_TEXT)
        md_file_info_set (file_info, "gap-file-type", "text");
    else
        md_file_info_set (file_info, "gap-file-type", NULL);
}


static gboolean
parse_save_workspace_output (const char  *output,
                             GError     **error)
{
    char **lines, **p;
    gboolean success = FALSE;

    if (!output)
    {
        g_set_error (error, GGAP_FILE_ERROR,
                     GGAP_FILE_ERROR_FAILED,
                     "no output");
        return FALSE;
    }

    lines = moo_splitlines (output);

    for (p = lines; p && *p; ++p)
    {
        if (p[0][0] == '#')
        {
            continue;
        }
        else if (!strcmp (*p, "true"))
        {
            success = TRUE;
            goto out;
        }
        else if (!strncmp (*p, "Couldn't open file", strlen ("Couldn't open file")))
        {
            g_set_error (error, GGAP_FILE_ERROR,
                         GGAP_FILE_ERROR_FAILED,
                         "could not save workspace");
            goto out;
        }
        else
        {
            g_set_error (error, GGAP_FILE_ERROR,
                         GGAP_FILE_ERROR_FAILED,
                         "unknown output: %s",
                         output);
            goto out;
        }
    }

    g_set_error (error, GGAP_FILE_ERROR,
                 GGAP_FILE_ERROR_FAILED,
                 "empty output");

out:
    g_strfreev (lines);
    return success;
}

static gboolean
gap_worksheet_save_workspace (GapWorksheet  *ws,
                              char         **filename_p,
                              GError       **error)
{
    char *filename, *cmd;
    char *output = NULL;
    gboolean result;

    g_return_val_if_fail (ws->priv->proc != NULL, FALSE);

    filename = moo_tempnam ();
    cmd = gap_cmd_save_workspace (filename);

    result = gap_process_run_command (ws->priv->proc, GGAP_CMD_RUN_COMMAND, NULL, cmd, &output);

    if (!result)
    {
        g_set_error (error, GGAP_FILE_ERROR,
                     GGAP_FILE_ERROR_FAILED,
                     "Failed");
    }
    else
    {
        result = parse_save_workspace_output (output, error);

        if (result)
        {
            *filename_p = filename;
            filename = NULL;
        }
    }

    g_free (output);
    g_free (cmd);
    g_free (filename);
    return result;
}

static void
gap_worksheet_save_text (GapWorksheet *ws,
                         MdFileInfo   *file_info,
                         MdFileOpInfo *op_info)
{
    char *filename;
    char *text;
    GError *error = NULL;

    if (!(filename = md_file_info_get_filename (file_info)))
    {
        md_file_op_info_set_error (op_info, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
                                   "Failed");
        g_return_if_fail (filename != NULL);
    }

    gap_file_info_get_file_type (file_info, GAP_FILE_TEXT);
    text = moo_worksheet_get_input_text (MOO_WORKSHEET (ws));

    if (!g_file_set_contents (filename, text, -1, &error))
        md_file_op_info_take_error (op_info, error);
    else
        op_info->status = MD_FILE_OP_STATUS_SUCCESS;

    g_free (text);
    g_free (filename);
}

static void
gap_worksheet_save_file (MdDocument   *doc,
                         MdFileInfo   *file_info,
                         MdFileOpInfo *op_info)
{
    MooFileWriter *writer;
    char *workspace = NULL;
    gboolean save_workspace = TRUE;
    GError *error = NULL;
    char *filename;
    GapWorksheet *ws = GAP_WORKSHEET (doc);
    GapState gap_state;

    if (gap_file_info_get_file_type (file_info, ws->priv->file_type) == GAP_FILE_TEXT)
    {
        gap_worksheet_save_text (ws, file_info, op_info);
        return;
    }

    gap_state = ws->priv->proc ? gap_process_get_state (ws->priv->proc) : GAP_DEAD;

    if (!(gap_state == GAP_DEAD || gap_state == GAP_IN_PROMPT))
    {
        md_file_op_info_set_error (op_info, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
                                   "Failed");
        g_return_if_fail (gap_state == GAP_DEAD || gap_state == GAP_IN_PROMPT);
    }

    if (!(filename = md_file_info_get_filename (file_info)))
    {
        md_file_op_info_set_error (op_info, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
                                   "Failed");
        g_return_if_fail (filename != NULL);
    }

    if (save_workspace && gap_state != GAP_DEAD &&
        !gap_worksheet_save_workspace (ws, &workspace, &error))
    {
        md_file_op_info_take_error (op_info, error);
        g_free (filename);
        return;
    }

    gap_file_info_get_file_type (file_info, GAP_FILE_WORKSHEET);
    writer = moo_string_writer_new ();
    moo_worksheet_format (MOO_WORKSHEET (ws), writer);

    if (!ggap_file_save_xml (moo_string_writer_get_string (writer, NULL),
                             workspace, filename, &error))
        md_file_op_info_take_error (op_info, error);
    else
        op_info->status = MD_FILE_OP_STATUS_SUCCESS;

    g_object_unref (writer);
    g_free (filename);
}


/**************************************************************************/
/* Completion
 */

static MooCompletionGroup *
gap_worksheet_ensure_completion (GapWorksheet *ws)
{
    if (!ws->priv->completion)
    {
        ws->priv->completion = g_object_new (GAP_TYPE_WS_COMPLETION, NULL);
        ws->priv->cmpl_group = moo_completion_simple_new_group (MOO_COMPLETION_SIMPLE (ws->priv->completion), NULL);
        moo_completion_group_set_pattern (ws->priv->cmpl_group, "[A-Za-z0-9_$]*", NULL, 0);
        moo_text_completion_set_doc (ws->priv->completion, GTK_TEXT_VIEW (ws));
    }

    return ws->priv->cmpl_group;
}


static void
gap_worksheet_complete (GapWorksheet *ws)
{
    gap_worksheet_ensure_completion (ws);

    if (ws->priv->proc)
        gap_process_ask_completions (ws->priv->proc);

    moo_text_completion_try_complete (ws->priv->completion, FALSE);
}

static gboolean
gap_worksheet_key_press (GtkWidget   *widget,
                         GdkEventKey *event)
{
    if (event->keyval == GDK_Tab)
    {
        gap_worksheet_complete (GAP_WORKSHEET (widget));
        return TRUE;
    }

    return GTK_WIDGET_CLASS(gap_worksheet_parent_class)->key_press_event (widget, event);
}


static void
gap_worksheet_free_completion (GapWorksheet *ws)
{
    if (ws->priv->completion)
    {
        g_object_unref (ws->priv->completion);
        ws->priv->completion = NULL;
        ws->priv->cmpl_group = NULL;
    }
}


static GList *
parse_words (const char *data,
             gsize       data_len)
{
    GQueue words = {0};
    gsize start, end;

    for (start = 0, end = 0; end < data_len; end++)
    {
        if (data[end] == '\r' || data[end] == '\n')
        {
            if (start < end)
                g_queue_push_tail (&words, g_strndup (data + start, end - start));
            start = end + 1;
        }
    }

    return words.head;
}

static void
gap_globals_changed (GapWorksheet *ws,
                     const char   *data,
                     guint         data_len,
                     gboolean      added)
{
    MooCompletionGroup *group = gap_worksheet_ensure_completion (ws);

    if (added)
    {
        GList *words = parse_words (data, data_len);
        moo_completion_group_add_data (group, words);
    }
    else
    {
        GList *words;

        words = parse_words (data, data_len);
        moo_completion_group_remove_data (group, words);

        g_list_foreach (words, (GFunc) g_free, NULL);
        g_list_free (words);
    }
}
