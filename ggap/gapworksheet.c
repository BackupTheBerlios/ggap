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
#include "gapapp.h"
#include "gapparser.h"
#include "ggapfile.h"
#include "gap.h"
#include "gapwscompletion.h"
#include "mooui/mdview.h"
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

typedef struct {
    guint stamp;
    GMainLoop *loop;
    gboolean success;
    gboolean destroyed;
    char *output;
    gboolean finished;
} GapCommandInfo;

typedef enum {
    INPUT_NORMAL,
    INPUT_MAYBE_MAGIC,
    INPUT_DATA_TYPE,
    INPUT_DATA_FIXED_LEN,
    INPUT_DATA_FIXED,
    INPUT_DATA_VAR,
    INPUT_MAYBE_DATA_END
} InputState;

typedef enum {
    INPUT_DATA_COMMAND,
    INPUT_DATA_OUTPUT
} InputDataType;

struct _GapWorksheetPrivate {
    MooTermPt *pt;
    gboolean in_stderr;
    GapState gap_state;
    gboolean allow_error_break;

    GString *input_buf;
    GString *input_buf2;
    gsize input_data_len;
    InputState input_state;

    int width;
    int height;

    MooTextCompletion *completion;
    MooCompletionGroup *cmpl_group;

    guint last_stamp;
    GapCommandInfo *cmd_info;

    gboolean loaded;
    GapFileType file_type;
};


static void     doc_iface_init                  (MdDocumentIface *iface);
static void     gap_worksheet_close             (MdDocument     *doc);

static GObject *gap_worksheet_constructor       (GType           type,
                                                 guint           n_props,
                                                 GObjectConstructParam *props);
static void     gap_worksheet_modified_changed  (GtkTextBuffer  *buffer);

static void     gap_worksheet_process_input     (MooWorksheet   *mws,
                                                 char          **lines);

static gboolean gap_worksheet_fork_command      (GapWorksheet   *ws,
                                                 const char     *cmd_line,
                                                 GError        **error);
static void     gap_worksheet_kill_child        (GapWorksheet   *ws);
static gboolean gap_worksheet_child_alive       (GapWorksheet   *ws);

static void     stop_running_command_loop       (GapCommandInfo *ci);
static gboolean has_running_command_loop        (GapWorksheet   *ws);

static void     gap_worksheet_load_file         (MdDocument     *doc,
                                                 MdFileInfo     *file_info,
                                                 MdFileOpInfo   *op_info);
static void     gap_worksheet_save_file         (MdDocument     *doc,
                                                 MdFileInfo     *file_info,
                                                 MdFileOpInfo   *op_info);

static void     gap_worksheet_free_completion   (GapWorksheet   *ws);
static void     gap_worksheet_add_globals       (GapWorksheet   *ws,
                                                 const char     *data,
                                                 gsize           data_len);
static void     gap_worksheet_delete_globals    (GapWorksheet   *ws,
                                                 const char     *data,
                                                 gsize           data_len);


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
            g_value_set_enum (value, ws->priv->gap_state);
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
    GtkTextBufferClass *buffer_class = GTK_TEXT_BUFFER_CLASS (klass);
    MooWorksheetClass *ws_class = MOO_WORKSHEET_CLASS (klass);

    object_class->constructor = gap_worksheet_constructor;
    object_class->get_property = gap_worksheet_get_property;
    object_class->set_property = gap_worksheet_set_property;

    buffer_class->modified_changed = gap_worksheet_modified_changed;
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

GType
gap_state_get_type (void)
{
    static GType type;

    if (G_UNLIKELY (!type))
    {
        static GEnumValue values[] = {
            { GAP_DEAD, "GAP_DEAD", "dead" },
            { GAP_IN_PROMPT, "GAP_IN_PROMPT", "in-prompt" },
            { GAP_LOADING, "GAP_LOADING", "loading" },
            { GAP_BUSY, "GAP_BUSY", "busy" },
            { 0, NULL, NULL },
        };
        type = g_enum_register_static ("GapState", values);
    }

    return type;
}


static void
set_state (GapWorksheet *ws,
           GapState      state)
{
    if (ws->priv && ws->priv->gap_state != state)
    {
        ws->priv->gap_state = state;
        g_object_notify (G_OBJECT (ws), "gap-state");
    }
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
    ws->priv->loaded = FALSE;

    if (!gap_worksheet_fork_command (ws, cmd_line, &error))
    {
        moo_worksheet_write_error (MOO_WORKSHEET (ws), error->message);
        g_error_free (error);
        return FALSE;
    }

    set_state (ws, GAP_LOADING);

    g_free (cmd_line);
    return TRUE;
}


static void
gap_worksheet_init (GapWorksheet *ws)
{
    ws->priv = G_TYPE_INSTANCE_GET_PRIVATE (ws, GAP_TYPE_WORKSHEET, GapWorksheetPrivate);

    ws->priv->input_buf = NULL;
    ws->priv->input_buf2 = NULL;
    ws->priv->input_data_len = 0;
    ws->priv->input_state = INPUT_NORMAL;

    ws->priv->pt = NULL;
    ws->priv->in_stderr = FALSE;
    ws->priv->width = -1;
    ws->priv->height = -1;

    ws->priv->file_type = GAP_FILE_WORKSHEET;

    md_document_set_capabilities (MD_DOCUMENT (ws),
                                  MD_DOCUMENT_SUPPORTS_SAVE);
}


static void
gap_worksheet_modified_changed (GtkTextBuffer *buffer)
{
    md_document_set_modified (MD_DOCUMENT (buffer),
                              gtk_text_buffer_get_modified (buffer));
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

    md_document_set_modified (MD_DOCUMENT (object), FALSE);

    return object;
}


static void
gap_worksheet_close (MdDocument *doc)
{
    GapWorksheet *ws = GAP_WORKSHEET (doc);

    if (gap_worksheet_child_alive (ws))
        gap_worksheet_kill_child (ws);
    if (ws->priv->input_buf)
        g_string_free (ws->priv->input_buf, TRUE);
    if (ws->priv->input_buf2)
        g_string_free (ws->priv->input_buf2, TRUE);
    gap_worksheet_free_completion (ws);

    gap_worksheet_doc_parent_iface->close (doc);
}


static GdkPixbuf *
gap_worksheet_get_icon (G_GNUC_UNUSED MdDocument *doc,
                        GtkWidget   *widget,
                        GtkIconSize  size)
{
    return gtk_widget_render_icon (widget, GTK_STOCK_FILE, size, NULL);
}

static void
gap_worksheet_set_doc_status (MdDocument       *doc,
                              MdDocumentStatus  status)
{
    gap_worksheet_doc_parent_iface->set_status (doc, status);
    gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (doc),
                                  (status & MD_DOCUMENT_MODIFIED) != 0);
}

static void
doc_iface_init (MdDocumentIface *iface)
{
    gap_worksheet_doc_parent_iface =
        md_document_interface_peek_parent (iface);

    iface->close = gap_worksheet_close;
    iface->get_icon = gap_worksheet_get_icon;
    iface->load_file = gap_worksheet_load_file;
    iface->save_file = gap_worksheet_save_file;
    iface->set_status = gap_worksheet_set_doc_status;
}

static void
write_log (const char *text,
           int         len,
           gboolean    in)
{
MOO_DEBUG({
    static int last = -1;

    if (last != in)
    {
        if (last != -1)
            g_print ("\n");

        if (!in)
            g_print ("<-- ");
        else
            g_print ("--> ");
    }

    last = in;

    if (len < 0)
        len = strlen (text);

    while (len)
    {
        int i;

        for (i = 0; i < len && text[i] != '\n'; ++i) ;

        g_print ("%.*s", (int) i, text);

        if (i != len)
        {
            if (!in)
                g_print ("\n<-- ");
            else
                g_print ("\n--> ");
        }

        if (i < len)
        {
            len -= i + 1;
            text += i + 1;
        }
        else
        {
            len = 0;
            text += len;
        }
    }

    fflush (stdout);
}); /* MOO_DEBUG */
}

static void
write_child (GapWorksheet *ws,
             const char   *text)
{
    moo_term_pt_write (ws->priv->pt, text, -1);
    write_log (text, -1, FALSE);
}

static void
write_input (GapWorksheet  *ws,
             char         **lines,
             const char    *text)
{
    g_return_if_fail (lines && *lines);

    if (lines[1])
    {
        char *string = g_strjoinv (" ", lines);
        write_child (ws, string);
        g_free (string);
    }
    else
    {
        write_child (ws, lines[0]);
    }

    write_child (ws, "\n");
    moo_worksheet_add_history (MOO_WORKSHEET (ws), text);
    set_state (ws, GAP_BUSY);
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
    set_state (GAP_WORKSHEET (mws), GAP_IN_PROMPT);
}

static void
gap_worksheet_process_input (MooWorksheet   *mws,
                             char          **lines)
{
    GapWorksheet *ws = GAP_WORKSHEET (mws);
    GapParser *parser;
    char *text;

    g_return_if_fail (ws->priv->gap_state == GAP_IN_PROMPT);

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
child_died (GapWorksheet *ws)
{
    g_object_unref (ws->priv->pt);
    ws->priv->pt = NULL;
    g_signal_emit_by_name (ws, "gap-exited");
    set_state (ws, GAP_DEAD);
}


static void
do_normal_text (GapWorksheet *ws,
                const char   *data,
                gsize         data_len)
{
    MooWorksheet *mws = MOO_WORKSHEET (ws);

    if (has_running_command_loop (ws))
    {
        g_critical ("%s: oops", G_STRLOC);
        stop_running_command_loop (ws->priv->cmd_info);
    }

    if (ws->priv->loaded)
    {
        if (ws->priv->in_stderr)
            moo_worksheet_write_error_len (mws, data, data_len);
        else
            moo_worksheet_write_output (mws, data, data_len);
    }
}

static void
do_prompt (GapWorksheet *ws,
           const char   *string,
           gsize         len)
{
    MooWorksheet *mws = MOO_WORKSHEET (ws);

    if (has_running_command_loop (ws))
    {
        g_critical ("%s: oops", G_STRLOC);
        stop_running_command_loop (ws->priv->cmd_info);
    }

    if (ws->priv->loaded)
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

    ws->priv->loaded = TRUE;
    set_state (ws, GAP_IN_PROMPT);
}

static void
do_output (GapWorksheet *ws,
           const char   *data,
           gsize         data_len)
{
    if (has_running_command_loop (ws))
    {
        ws->priv->cmd_info->output = g_strndup (data, data_len);
        ws->priv->cmd_info->success = TRUE;
        stop_running_command_loop (ws->priv->cmd_info);
    }
    else
    {
        g_critical ("%s: output: %.*s",
                    G_STRLOC, (int) data_len, data);
    }
}

static void
do_data (GapWorksheet *ws,
         const char   *data,
         gsize         data_len)
{
    if (data_len >= strlen ("prompt:") && strncmp (data, "prompt:", strlen ("prompt:")) == 0)
        do_prompt (ws, data + strlen ("prompt:"), data_len - strlen ("prompt:"));
    else if (data_len >= strlen ("output:") && strncmp (data, "output:", strlen ("output:")) == 0)
        do_output (ws, data + strlen ("output:"), data_len - strlen ("output:"));
    else if (data_len >= strlen ("globals-added:") && strncmp (data, "globals-added:", strlen ("globals-added:")) == 0)
        gap_worksheet_add_globals (ws, data + strlen ("globals-added:"), data_len - strlen ("globals-added:"));
    else if (data_len >= strlen ("globals-deleted:") && strncmp (data, "globals-deleted:", strlen ("globals-deleted:")) == 0)
        gap_worksheet_delete_globals (ws, data + strlen ("globals-deleted:"), data_len - strlen ("globals-deleted:"));
    else
    {
        g_critical ("%s: got unknown data: '%.*s'",
                    G_STRLOC, (int) data_len, data);
    }
}

#define MAGIC       "@GGAP@"
#define MAGIC_LEN   6
#define LENGTH_LEN  8

static void
read_chars_normal (GapWorksheet  *ws,
                   char const   **data_p,
                   gsize         *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;
    gsize i;

    g_assert (!ws->priv->input_buf);

    for (i = 0; i < data_len; ++i)
    {
        if (data[i] == '@')
        {
            gsize n = MIN (MAGIC_LEN, data_len - i);

            if (!n || strncmp (data + i, MAGIC, n) == 0)
            {
                if (i > 0)
                    do_normal_text (ws, data, i);

                if (n == MAGIC_LEN)
                {
                    ws->priv->input_state = INPUT_DATA_TYPE;
                }
                else
                {
                    ws->priv->input_state = INPUT_MAYBE_MAGIC;
                    ws->priv->input_buf = g_string_new_len (data + i, n);
                }

                *data_p = data + i + n;
                *data_len_p = data_len - i - n;

                return;
            }
        }
    }

    do_normal_text (ws, data, data_len);
    *data_p = data + data_len;
    *data_len_p = 0;
}

static void
read_chars_maybe_magic (GapWorksheet *ws,
                        char const  **data_p,
                        gsize        *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;
    gsize n;

    n = MIN (data_len, MAGIC_LEN - ws->priv->input_buf->len);
    g_string_append_len (ws->priv->input_buf, data, n);
    *data_p = data + n;
    *data_len_p = data_len - n;

    if (strncmp (ws->priv->input_buf->str, MAGIC, ws->priv->input_buf->len) != 0)
    {
        GString *tmp = ws->priv->input_buf;
        ws->priv->input_buf = NULL;
        ws->priv->input_state = INPUT_NORMAL;
        do_normal_text (ws, tmp->str, tmp->len);
    }
    else if (ws->priv->input_buf->len == MAGIC_LEN)
    {
        g_string_free (ws->priv->input_buf, TRUE);
        ws->priv->input_buf = NULL;
        ws->priv->input_state = INPUT_DATA_TYPE;
    }
}

static void
read_chars_data_type (GapWorksheet *ws,
                      char const  **data_p,
                      gsize        *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;

    g_assert (ws->priv->input_state == INPUT_DATA_TYPE);
    g_assert (ws->priv->input_buf == 0);

    g_return_if_fail (data_len != 0);

    *data_p = data + 1;
    *data_len_p = data_len - 1;

    switch (data[0])
    {
        case GGAP_DTC_FIXED:
            ws->priv->input_state = INPUT_DATA_FIXED_LEN;
            ws->priv->input_buf = g_string_new (NULL);
            break;
        case GGAP_DTC_VARIABLE:
            ws->priv->input_state = INPUT_DATA_VAR;
            ws->priv->input_buf = g_string_new (NULL);
            break;
        default:
            g_critical ("%s: unknown data type '%c'", G_STRLOC, data[0]);
            ws->priv->input_state = INPUT_NORMAL;
            break;
    }
}

static gsize
get_length (const char *str)
{
    gulong n;
    char *end;

    errno = 0;
    n = strtoul (str, &end, 16);

    if (errno != 0 || end == NULL || *end != 0)
    {
        g_critical ("%s: could not convert '%s' to a number", G_STRLOC, str);
        return 0;
    }

    return n;
}

static void
read_chars_data_fixed_len (GapWorksheet *ws,
                           char const  **data_p,
                           gsize        *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;
    gsize n;

    n = MIN (data_len, LENGTH_LEN - ws->priv->input_buf->len);
    g_string_append_len (ws->priv->input_buf, data, n);
    *data_p = data + n;
    *data_len_p = data_len - n;

    if (ws->priv->input_buf->len == LENGTH_LEN)
    {
        gsize input_data_len = get_length (ws->priv->input_buf->str);

        if (input_data_len == 0)
        {
            ws->priv->input_state = INPUT_NORMAL;
            g_string_free (ws->priv->input_buf, TRUE);
            ws->priv->input_buf = NULL;
        }
        else
        {
            ws->priv->input_state = INPUT_DATA_FIXED;
            g_string_truncate (ws->priv->input_buf, 0);
            ws->priv->input_data_len = input_data_len;
        }
    }
}

static void
read_chars_data_fixed (GapWorksheet *ws,
                       char const  **data_p,
                       gsize        *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;
    gsize remain = ws->priv->input_data_len - ws->priv->input_buf->len;
    gsize n;

    n = MIN (remain, data_len);
    g_string_append_len (ws->priv->input_buf, data, n);
    *data_p = data + n;
    *data_len_p = data_len - n;

    if (ws->priv->input_buf->len == ws->priv->input_data_len)
    {
        GString *tmp = ws->priv->input_buf;
        ws->priv->input_buf = NULL;
        ws->priv->input_data_len = 0;
        ws->priv->input_state = INPUT_NORMAL;
        do_data (ws, tmp->str, tmp->len);
        g_string_free (tmp, TRUE);
    }
}

static void
read_chars_data_inf (GapWorksheet *ws,
                     char const  **data_p,
                     gsize        *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;
    gsize i;

    g_assert (ws->priv->input_buf != NULL);
    g_assert (ws->priv->input_buf2 == NULL);

    for (i = 0; i < data_len; ++i)
    {
        if (data[i] == '@')
        {
            gsize n = MIN (MAGIC_LEN, data_len - i);

            if (!n || strncmp (data + i, MAGIC, n) == 0)
            {
                if (i > 0)
                    g_string_append_len (ws->priv->input_buf, data, i);

                ws->priv->input_state = INPUT_MAYBE_DATA_END;
                ws->priv->input_buf2 = g_string_new_len (data + i, n);

                *data_p = data + i + n;
                *data_len_p = data_len - i - n;

                return;
            }
        }
    }

    g_string_append_len (ws->priv->input_buf, data, data_len);
    *data_p = data + data_len;
    *data_len_p = 0;
}

static void
read_chars_maybe_data_end (GapWorksheet *ws,
                           char const  **data_p,
                           gsize        *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;

    g_assert (ws->priv->input_buf != NULL);
    g_assert (ws->priv->input_buf2 != NULL);

    if (ws->priv->input_buf2->len < MAGIC_LEN)
    {
        gsize n = MIN (data_len, MAGIC_LEN - ws->priv->input_buf2->len);

        g_string_append_len (ws->priv->input_buf2, data, n);
        data += n;
        data_len -= n;
        *data_p = data;
        *data_len_p = data_len;

        if (ws->priv->input_buf2->len < MAGIC_LEN)
            return;

        if (strncmp (ws->priv->input_buf2->str, MAGIC, ws->priv->input_buf2->len) != 0)
        {
            g_string_append_len (ws->priv->input_buf,
                                 ws->priv->input_buf2->str,
                                 ws->priv->input_buf2->len);
            g_string_free (ws->priv->input_buf2, TRUE);
            ws->priv->input_buf2 = NULL;
            ws->priv->input_state = INPUT_DATA_VAR;
            return;
        }
    }

    if (data_len == 0)
        return;

    *data_p = data + 1;
    *data_len_p = data_len - 1;

    if (data[0] == GGAP_DTC_END)
    {
        GString *tmp;

        g_string_free (ws->priv->input_buf2, TRUE);
        ws->priv->input_buf2 = NULL;
        tmp = ws->priv->input_buf;
        ws->priv->input_buf = NULL;
        ws->priv->input_state = INPUT_NORMAL;

        do_data (ws, tmp->str, tmp->len);
        g_string_free (tmp, TRUE);
    }
    else
    {
        g_message ("%s: got '%c', expecting '%c'",
                   G_STRLOC, data[0], GGAP_DTC_END);
        g_string_append_len (ws->priv->input_buf,
                             ws->priv->input_buf2->str,
                             ws->priv->input_buf2->len);
        g_string_append_c (ws->priv->input_buf, data[0]);

        ws->priv->input_state = INPUT_DATA_VAR;

        g_string_free (ws->priv->input_buf2, TRUE);
        ws->priv->input_buf2 = NULL;
    }
}

static void
io_func (const char *buf,
         gsize       len,
         gpointer    data)
{
    GapWorksheet *ws = data;

    g_return_if_fail (len > 0);

    write_log (buf, len, TRUE);

    g_object_ref (ws);

    while (len != 0 && ws->priv != NULL)
    {
        switch (ws->priv->input_state)
        {
            case INPUT_NORMAL:
                read_chars_normal (ws, &buf, &len);
                break;

            case INPUT_MAYBE_MAGIC:
                read_chars_maybe_magic (ws, &buf, &len);
                break;
            case INPUT_DATA_TYPE:
                read_chars_data_type (ws, &buf, &len);
                break;

            case INPUT_DATA_FIXED_LEN:
                read_chars_data_fixed_len (ws, &buf, &len);
                break;
            case INPUT_DATA_FIXED:
                read_chars_data_fixed (ws, &buf, &len);
                break;

            case INPUT_DATA_VAR:
                read_chars_data_inf (ws, &buf, &len);
                break;
            case INPUT_MAYBE_DATA_END:
                read_chars_maybe_data_end (ws, &buf, &len);
                break;
        }
    }

    g_object_unref (ws);
}


static gboolean
gap_worksheet_fork_command (GapWorksheet   *ws,
                            const char     *cmd_line,
                            GError        **error)
{
    MooTermPt *pt;
    MooTermCommand *cmd;

    g_return_val_if_fail (ws->priv->pt == NULL, FALSE);
    g_return_val_if_fail (cmd_line != NULL, FALSE);

    pt = moo_term_pt_new (io_func, ws);
    moo_term_pt_set_echo_input (pt, FALSE);
    moo_term_pt_set_priority (pt, G_PRIORITY_HIGH);

    if (ws->priv->width > 0 && ws->priv->height > 0)
        moo_term_pt_set_size (pt, ws->priv->width, ws->priv->height);

    cmd = moo_term_command_new_command_line (cmd_line, NULL, NULL);

    if (moo_term_pt_fork_command (pt, cmd, error))
    {
        g_signal_connect_swapped (pt, "child-died", G_CALLBACK (child_died), ws);
        ws->priv->pt = g_object_ref (pt);
    }

    set_state (ws, GAP_LOADING);

    moo_term_command_free (cmd);
    g_object_unref (pt);
    return TRUE;
}


static void
gap_worksheet_kill_child (GapWorksheet *ws)
{
    g_return_if_fail (ws->priv && ws->priv->pt);
    moo_term_pt_kill_child (ws->priv->pt);
}


static gboolean
gap_worksheet_child_alive (GapWorksheet *ws)
{
    return ws->priv && ws->priv->pt &&
           moo_term_pt_child_alive (ws->priv->pt);
}


void
_gap_worksheet_set_size (GapWorksheet *ws,
                         int           width,
                         int           height)
{
    ws->priv->width = width;
    ws->priv->height = height;

    if (ws->priv->pt)
        moo_term_pt_set_size (ws->priv->pt, ws->priv->width, ws->priv->height);
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
    moo_ws_buffer_append_block (MOO_WS_BUFFER (ws), block);

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


static void
stop_running_command_loop (GapCommandInfo *ci)
{
    if (g_main_loop_is_running (ci->loop))
        g_main_loop_quit (ci->loop);
}

static gboolean
has_running_command_loop (GapWorksheet *ws)
{
    return ws->priv && ws->priv->cmd_info &&
            ws->priv->cmd_info->loop &&
            g_main_loop_is_running (ws->priv->cmd_info->loop);
}

static void
run_command_destroy (G_GNUC_UNUSED GapWorksheet *ws,
                     GapCommandInfo *ci)
{
    ci->destroyed = TRUE;
    stop_running_command_loop (ci);
}

static gboolean
gap_worksheet_run_command (GapWorksheet *ws,
                           const char   *command,
                           const char   *args,
                           const char   *gap_cmd_line,
                           char        **output)
{
    char *string;
    GapCommandInfo ci;
    gulong destroy_cb_id;
    guint stamp;

    g_return_val_if_fail (ws->priv->cmd_info == NULL, FALSE);
    g_return_val_if_fail (command != NULL, FALSE);

    stamp = ++ws->priv->last_stamp;
    string = ggap_pkg_exec_command (stamp, command, args);
    write_child (ws, string);

    if (gap_cmd_line)
        write_child (ws, gap_cmd_line);

    g_object_ref (ws);

    ws->priv->cmd_info = &ci;
    ci.success = FALSE;
    ci.loop = NULL;
    ci.destroyed = FALSE;
    ci.stamp = stamp;
    ci.output = NULL;

    destroy_cb_id = g_signal_connect (ws, "close",
                                      G_CALLBACK (run_command_destroy),
                                      &ci);

    ci.loop = g_main_loop_new (NULL, FALSE);

    gdk_threads_leave ();
    g_main_loop_run (ci.loop);
    gdk_threads_enter ();

    g_main_loop_unref (ci.loop);
    ci.loop = NULL;

    if (!ci.destroyed)
        g_signal_handler_disconnect (ws, destroy_cb_id);

    if (ci.destroyed)
        ci.success = FALSE;

    *output = ci.output;
    ws->priv->cmd_info = NULL;

    g_object_unref (ws);
    return ci.success;
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

    g_return_val_if_fail (ws->priv->cmd_info == NULL, FALSE);

    filename = moo_tempnam ();
    cmd = gap_cmd_save_workspace (filename);

    result = gap_worksheet_run_command (ws, GGAP_CMD_RUN_COMMAND, NULL, cmd, &output);

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
    char *markup;
    char *workspace = NULL;
    gboolean save_workspace = TRUE;
    GError *error = NULL;
    char *filename;
    GapWorksheet *ws = GAP_WORKSHEET (doc);

    if (gap_file_info_get_file_type (file_info, ws->priv->file_type) == GAP_FILE_TEXT)
    {
        gap_worksheet_save_text (ws, file_info, op_info);
        return;
    }

    if (!(ws->priv->gap_state == GAP_DEAD || ws->priv->gap_state == GAP_IN_PROMPT))
    {
        md_file_op_info_set_error (op_info, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
                                   "Failed");
        g_return_if_fail (ws->priv->gap_state == GAP_DEAD ||
                          ws->priv->gap_state == GAP_IN_PROMPT);
    }

    if (!(filename = md_file_info_get_filename (file_info)))
    {
        md_file_op_info_set_error (op_info, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
                                   "Failed");
        g_return_if_fail (filename != NULL);
    }

    if (save_workspace && ws->priv->gap_state != GAP_DEAD &&
        !gap_worksheet_save_workspace (ws, &workspace, &error))
    {
        md_file_op_info_take_error (op_info, error);
        g_free (filename);
        return;
    }

    gap_file_info_get_file_type (file_info, GAP_FILE_WORKSHEET);
    markup = moo_worksheet_format (MOO_WORKSHEET (ws));

    if (!ggap_file_save_xml (markup, workspace, filename, &error))
        md_file_op_info_take_error (op_info, error);
    else
        op_info->status = MD_FILE_OP_STATUS_SUCCESS;

    g_free (markup);
    g_free (filename);
}


/**************************************************************************/
/* Completion
 */

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

static MooCompletionGroup *
gap_worksheet_ensure_completion (GapWorksheet *ws)
{
    if (!ws->priv->completion)
    {
        ws->priv->completion = g_object_new (GAP_TYPE_WS_COMPLETION, NULL);
        ws->priv->cmpl_group = moo_completion_simple_new_group (MOO_COMPLETION_SIMPLE (ws->priv->completion), NULL);
        moo_completion_group_set_pattern (ws->priv->cmpl_group, "[A-Za-z0-9_]*", NULL, 0);
    }

    return ws->priv->cmpl_group;
}

gpointer
_gap_worksheet_get_completion (GapWorksheet *ws)
{
    g_return_val_if_fail (GAP_IS_WORKSHEET (ws), NULL);
    gap_worksheet_ensure_completion (ws);
    return ws->priv->completion;
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
gap_worksheet_add_globals (GapWorksheet *ws,
                           const char   *data,
                           gsize         data_len)
{
    MooCompletionGroup *group = gap_worksheet_ensure_completion (ws);
    GList *words = parse_words (data, data_len);
    moo_completion_group_add_data (group, words);
}

static void
gap_worksheet_delete_globals (GapWorksheet *ws,
                              const char   *data,
                              gsize         data_len)
{
    GList *words;
    MooCompletionGroup *group;

    group = gap_worksheet_ensure_completion (ws);
    words = parse_words (data, data_len);
    moo_completion_group_remove_data (group, words);

    g_list_foreach (words, (GFunc) g_free, NULL);
    g_list_free (words);
}

void
_gap_worksheet_ask_for_completions (GapWorksheet *ws)
{
    char *string;
    guint stamp;

    if (ws->priv->gap_state != GAP_IN_PROMPT)
        return;

    stamp = ++ws->priv->last_stamp;
    string = ggap_pkg_exec_command (stamp, "get-globals", NULL);
    write_child (ws, string);

    g_free (string);
}
