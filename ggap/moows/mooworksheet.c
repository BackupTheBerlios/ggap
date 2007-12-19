/*
 *   mooworksheet.c
 *
 *   Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "mooworksheet.h"
#include "moowspromptblock.h"
#include "moowstextblock.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/mooutils-gobject.h"
#include "mooutils/moomarkup.h"
#include <gdk/gdkkeysyms.h>
#include <glib/gprintf.h>
#include <string.h>


G_DEFINE_TYPE (MooWorksheet, moo_worksheet, MOO_TYPE_WS_BUFFER)


typedef enum {
    OUTPUT_OUT,
    OUTPUT_ERR
} OutputType;

struct MooWorksheetPrivate {
    MooWsPromptBlock *input;
    MooWsTextBlock *output;

    GQueue *history;
    GList *history_ptr;
    char *history_tmp;

    gboolean in_input : 1;
    guint allow_multiline : 1;
    guint output_newline : 1;
};

enum {
    PROCESS_INPUT,
    SCROLL_INSERT_ONSCREEN,
    N_SIGNALS
};

enum {
    PROP_0,
    PROP_ACCEPTING_INPUT,
    PROP_ALLOW_MULTILINE
};

static guint signals[N_SIGNALS];

static void
moo_worksheet_init (MooWorksheet *ws)
{
    ws->priv = G_TYPE_INSTANCE_GET_PRIVATE (ws, MOO_TYPE_WORKSHEET,
                                            MooWorksheetPrivate);
    ws->priv->in_input = FALSE;
    ws->priv->allow_multiline = TRUE;
    ws->priv->history = g_queue_new ();
    ws->priv->history_ptr = NULL;
}

static void
moo_worksheet_dispose (GObject *object)
{
    MooWorksheet *ws = MOO_WORKSHEET (object);

    if (ws->priv)
    {
        g_queue_foreach (ws->priv->history, (GFunc) g_free, NULL);
        g_queue_free (ws->priv->history);

        ws->priv = NULL;
    }

    G_OBJECT_CLASS (moo_worksheet_parent_class)->dispose (object);
}

static void
moo_worksheet_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    MooWorksheet *ws = MOO_WORKSHEET (object);

    switch (prop_id)
    {
        case PROP_ALLOW_MULTILINE:
            ws->priv->allow_multiline = g_value_get_boolean (value);
            g_object_notify (object, "allow-multiline");
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
moo_worksheet_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    MooWorksheet *ws = MOO_WORKSHEET (object);

    switch (prop_id)
    {
        case PROP_ACCEPTING_INPUT:
            g_value_set_boolean (value, ws->priv->in_input);
            break;

        case PROP_ALLOW_MULTILINE:
            g_value_set_boolean (value, ws->priv->allow_multiline);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
moo_worksheet_class_init (MooWorksheetClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = moo_worksheet_dispose;
    object_class->set_property = moo_worksheet_set_property;
    object_class->get_property = moo_worksheet_get_property;

    g_object_class_install_property (object_class,
                                     PROP_ACCEPTING_INPUT,
                                     g_param_spec_boolean ("accepting-input",
                                                           "accepting-input",
                                                           "accepting-input",
                                                           FALSE,
                                                           G_PARAM_READABLE));

    g_object_class_install_property (object_class,
                                     PROP_ALLOW_MULTILINE,
                                     g_param_spec_boolean ("allow-multiline",
                                                           "allow-multiline",
                                                           "allow-multiline",
                                                           TRUE,
                                                           G_PARAM_READWRITE));

    signals[PROCESS_INPUT] =
        g_signal_new ("process-input",
                      G_OBJECT_CLASS_TYPE (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MooWorksheetClass, process_input),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__BOXED,
                      G_TYPE_NONE, 1,
                      G_TYPE_STRV | G_SIGNAL_TYPE_STATIC_SCOPE);

    signals[SCROLL_INSERT_ONSCREEN] =
        g_signal_new ("scroll-insert-onscreen",
                      G_OBJECT_CLASS_TYPE (klass),
                      G_SIGNAL_RUN_LAST,
                      0, NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    g_type_class_add_private (klass, sizeof (MooWorksheetPrivate));
}


void
moo_worksheet_set_accepting_input (MooWorksheet *ws,
                                   gboolean      accepting_input)
{
    g_return_if_fail (MOO_IS_WORKSHEET (ws));

    if (ws->priv->in_input != accepting_input)
    {
        ws->priv->in_input = accepting_input;

        if (accepting_input)
            ws->priv->output_newline = FALSE;

        g_object_notify (G_OBJECT (ws), "accepting-input");
    }
}


void
moo_worksheet_reset (MooWorksheet *ws)
{
    MooWsBlock *block;

    g_return_if_fail (MOO_IS_WORKSHEET (ws));

    while ((block = _moo_ws_buffer_get_first_block (MOO_WS_BUFFER (ws))))
        moo_ws_buffer_delete_block (MOO_WS_BUFFER (ws), block);

    ws->priv->input = NULL;
    ws->priv->output = NULL;
    moo_worksheet_set_accepting_input (ws, FALSE);
}

gboolean
moo_worksheet_get_accepting_input (MooWorksheet *ws)
{
    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), FALSE);
    return ws->priv->in_input;
}


static void
scroll_insert_onscreen (MooWorksheet *ws)
{
    g_signal_emit (ws, signals[SCROLL_INSERT_ONSCREEN], 0);
}


static MooWsBlock *
create_ws_prompt_block (const char *ps,
                        const char *ps2,
                        const char *text)
{
    MooWsBlock *block;

    block = moo_ws_prompt_block_new (ps, ps2);
    g_return_val_if_fail (block != NULL, NULL);

    g_object_set (moo_ws_prompt_block_get_ps_tag (MOO_WS_PROMPT_BLOCK (block)),
                  "foreground", "darkred",
#if 0
                  "background", "blue",
#endif
                  NULL);
    g_object_set (moo_ws_prompt_block_get_text_tag (MOO_WS_PROMPT_BLOCK (block)),
                  "foreground", "darkgreen",
#if 0
                  "background", "magenta",
#endif
                  NULL);

    if (text)
        moo_ws_prompt_block_set_text (MOO_WS_PROMPT_BLOCK (block), text);

    return block;
}

void
moo_worksheet_start_input (MooWorksheet   *ws,
                           const char     *ps,
                           const char     *ps2)
{
    MooWsBlock *block = NULL;

    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    g_return_if_fail (!ws->priv->in_input);

    if (ws->priv->output)
    {
        block = MOO_WS_BLOCK (ws->priv->output)->next;

        if (!block || !MOO_IS_WS_PROMPT_BLOCK (block))
        {
            block = NULL;
        }
        else
        {
            const char *old_ps = moo_ws_prompt_block_get_ps (MOO_WS_PROMPT_BLOCK (block));

            if (strcmp (old_ps ? old_ps : "", ps ? ps : "") != 0)
                block = NULL;
        }
    }

    if (!block)
    {
        block = create_ws_prompt_block (ps, ps2, NULL);

        if (ws->priv->output)
            moo_ws_buffer_insert_block (MOO_WS_BUFFER (ws), block,
                                        MOO_WS_BLOCK (ws->priv->output));
        else if (ws->priv->input)
            moo_ws_buffer_insert_block (MOO_WS_BUFFER (ws), block,
                                        MOO_WS_BLOCK (ws->priv->input));
        else
            moo_ws_buffer_append_block (MOO_WS_BUFFER (ws), block);
    }

    moo_ws_prompt_block_place_cursor (MOO_WS_PROMPT_BLOCK (block), 0, 0);
    scroll_insert_onscreen (ws);
    moo_worksheet_set_accepting_input (ws, TRUE);
}


void
moo_worksheet_continue_input (MooWorksheet *ws)
{
    GtkTextIter iter;

    g_return_if_fail (!ws->priv->in_input);
    g_return_if_fail (ws->priv->input != NULL);

    moo_ws_prompt_block_new_line (ws->priv->input);
    _moo_ws_block_get_end_iter (MOO_WS_BLOCK (ws->priv->input), &iter);
    gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (ws), &iter);
    scroll_insert_onscreen (ws);

    moo_worksheet_set_accepting_input (ws, TRUE);
}


void
moo_worksheet_resume_input (MooWorksheet *ws,
                            int           line,
                            int           column)
{
    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    g_return_if_fail (!ws->priv->in_input);
    g_return_if_fail (ws->priv->input != NULL);

    moo_ws_prompt_block_place_cursor (ws->priv->input, line, column);
    scroll_insert_onscreen (ws);

    moo_worksheet_set_accepting_input (ws, TRUE);
}


static void
moo_worksheet_get_cursor (MooWorksheet *ws,
                          GtkTextIter  *iter)
{
    gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER (ws), iter,
        gtk_text_buffer_get_insert (GTK_TEXT_BUFFER (ws)));
}


gboolean
_moo_worksheet_commit_input (MooWorksheet *ws)
{
    GtkTextIter iter;
    MooWsBlock *block;
    char **lines;

    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), FALSE);

    if (!ws->priv->in_input)
    {
        _moo_ws_buffer_beep (MOO_WS_BUFFER (ws));
        return TRUE;
    }

    moo_worksheet_get_cursor (ws, &iter);
    block = _moo_ws_iter_get_block (&iter);

    if (!block || !MOO_IS_WS_PROMPT_BLOCK (block))
        return FALSE;

    moo_worksheet_set_accepting_input (ws, FALSE);

    ws->priv->input = MOO_WS_PROMPT_BLOCK (block);
    ws->priv->output = NULL;
    while (block->next && !MOO_IS_WS_PROMPT_BLOCK (block->next))
        moo_ws_buffer_delete_block (MOO_WS_BUFFER (ws), block->next);

    lines = moo_ws_prompt_block_get_lines (MOO_WS_PROMPT_BLOCK (block));
    g_signal_emit (ws, signals[PROCESS_INPUT], 0, lines);
    g_strfreev (lines);

    return TRUE;
}


gboolean
_moo_worksheet_get_allow_multiline (MooWorksheet *ws)
{
    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), FALSE);
    return ws->priv->allow_multiline;
}


static gboolean
text_block_check_type (MooWsBlock *block,
                       OutputType  out_type)
{
    g_return_val_if_fail (MOO_IS_WS_TEXT_BLOCK (block), FALSE);

    if (out_type == OUTPUT_OUT)
        return g_object_get_data (G_OBJECT (block), "moo-worksheet-stderr") == NULL;
    else
        return g_object_get_data (G_OBJECT (block), "moo-worksheet-stderr") != NULL;
}

static MooWsBlock *
create_output_block (OutputType out_type)
{
    MooWsTextBlock *block;

    block = moo_ws_text_block_new ();

    if (out_type == OUTPUT_ERR)
    {
        g_object_set_data (G_OBJECT (block), "moo-worksheet-stderr",
                           GINT_TO_POINTER (TRUE));
        g_object_set (MOO_WS_BLOCK (block)->tag,
                      "foreground", "red",
#if 0
                      "background", "green",
#endif
                      NULL);
    }
    else
    {
#if 0
        g_object_set (MOO_WS_BLOCK (block)->tag,
                      "background", "yellow",
                      NULL);
#endif
    }

    return MOO_WS_BLOCK (block);
}

static MooWsTextBlock *
create_output (MooWorksheet *ws,
               OutputType    out_type)
{
    MooWsBlock *block;

    block = create_output_block (out_type);
    g_return_val_if_fail (block != NULL, NULL);

    if (ws->priv->output)
        moo_ws_buffer_insert_block (MOO_WS_BUFFER (ws), block,
                                    MOO_WS_BLOCK (ws->priv->output));
    else if (ws->priv->input)
        moo_ws_buffer_insert_block (MOO_WS_BUFFER (ws), block,
                                    MOO_WS_BLOCK (ws->priv->input));
    else
        moo_ws_buffer_append_block (MOO_WS_BUFFER (ws), block);

    ws->priv->output = MOO_WS_TEXT_BLOCK (block);
    ws->priv->output_newline = FALSE;

    return MOO_WS_TEXT_BLOCK (block);
}

static void
moo_worksheet_write_output_real (MooWorksheet *ws,
                                 const char   *string,
                                 gssize        len,
                                 OutputType    out_type)
{
    char **lines, **p;
    GtkTextIter iter;
    MooWsTextBlock *output = NULL;

    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    g_return_if_fail (string != NULL);

    if (!string[0])
        return;

    if (ws->priv->output && text_block_check_type (MOO_WS_BLOCK (ws->priv->output), out_type))
        output = ws->priv->output;
    else
        output = create_output (ws, out_type);

    if (ws->priv->output_newline)
    {
        moo_ws_text_block_append (output, "\n");
        ws->priv->output_newline = FALSE;
    }

    lines = moo_strnsplit_lines (string, len, NULL);

    for (p = lines; p && *p; ++p)
    {
        if (p != lines)
        {
            if (!p[1] && !**p)
                ws->priv->output_newline = TRUE;
            else
                moo_ws_text_block_append (output, "\n");
        }

        moo_ws_text_block_append (output, *p);
    }

    _moo_ws_block_get_end_iter (MOO_WS_BLOCK (output), &iter);
    gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (ws), &iter);
    scroll_insert_onscreen (ws);

    g_strfreev (lines);
}

void
moo_worksheet_write_output (MooWorksheet *ws,
                            const char   *string,
                            gssize        len)
{
    moo_worksheet_write_output_real (ws, string, len, OUTPUT_OUT);
}

void
moo_worksheet_write_error (MooWorksheet *ws,
                           const char   *format,
                           ...)
{
    va_list args;
    char *text = NULL;

    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    g_return_if_fail (format != NULL);

    va_start (args, format);
    g_vasprintf (&text, format, args);
    va_end (args);

    moo_worksheet_write_output_real (ws, text, -1, OUTPUT_ERR);
}

void
moo_worksheet_write_error_len (MooWorksheet *ws,
                               const char   *string,
                               gssize        len)
{
    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    g_return_if_fail (string != NULL);
    moo_worksheet_write_output_real (ws, string, len, OUTPUT_ERR);
}


void
moo_worksheet_add_history (MooWorksheet *ws,
                           const char   *string)
{
    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    g_return_if_fail (string != NULL);

    moo_worksheet_reset_history (ws);

    if (string[0] &&
        (!ws->priv->history->length || strcmp (ws->priv->history->head->data, string) != 0))
            g_queue_push_head (ws->priv->history, g_strdup (string));
}

void
moo_worksheet_reset_history (MooWorksheet *ws)
{
    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    ws->priv->history_ptr = NULL;
    g_free (ws->priv->history_tmp);
    ws->priv->history_tmp = NULL;
}


static void
history_go (MooWorksheet *ws,
            gboolean      forth)
{
    GQueue *history = ws->priv->history;
    const char *new_text = NULL;
    GtkTextIter iter;
    MooWsBlock *block;

    moo_worksheet_get_cursor (ws, &iter);
    block = _moo_ws_iter_get_block (&iter);

    if (!block || !MOO_IS_WS_PROMPT_BLOCK (block))
        goto beep_and_return;

    if (forth)
    {
        if (!ws->priv->history_ptr)
            goto beep_and_return;

        if (ws->priv->history_ptr == history->head)
        {
            ws->priv->history_ptr = NULL;
            new_text = ws->priv->history_tmp;
        }
        else
        {
            ws->priv->history_ptr = ws->priv->history_ptr->prev;
            new_text = ws->priv->history_ptr->data;
        }
    }
    else
    {
        if (!history->tail || ws->priv->history_ptr == history->tail)
            goto beep_and_return;

        if (ws->priv->history_ptr)
        {
            ws->priv->history_ptr = ws->priv->history_ptr->next;
            new_text = ws->priv->history_ptr->data;
        }
        else
        {
            char *text = moo_ws_prompt_block_get_text (MOO_WS_PROMPT_BLOCK (block));
            g_free (ws->priv->history_tmp);
            ws->priv->history_tmp = text;
            ws->priv->history_ptr = history->head;
            new_text = ws->priv->history_ptr->data;
        }
    }

    g_return_if_fail (new_text != NULL);
    moo_ws_prompt_block_set_text (MOO_WS_PROMPT_BLOCK (block), new_text);
    moo_ws_prompt_block_place_cursor (MOO_WS_PROMPT_BLOCK (block), -1, -1);
    scroll_insert_onscreen (ws);

    return;

beep_and_return:
    _moo_ws_buffer_beep (MOO_WS_BUFFER (ws));
}

void
_moo_worksheet_history_next (MooWorksheet *ws)
{
    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    history_go (ws, TRUE);
}

void
_moo_worksheet_history_prev (MooWorksheet *ws)
{
    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    history_go (ws, FALSE);
}


/**********************************************************************/
/* Loading and saving
 */

#define MOO_WORKSHEET_FILE_ERROR (g_quark_from_static_string ("moo-worksheet-file-error"))
#define MOO_WORKSHEET_FILE_ERROR_FORMAT 0

#define ELM_WORKSHEET   "ggap-worksheet"
#define ELM_CONTENT     "content"
#define ELM_INPUT       "input"
#define ELM_OUTPUT      "output"

#define PROP_PS         "ps"
#define PROP_PS2        "ps2"
#define PROP_TYPE       "type"

#define PROP_OUTPUT_TYPE_STDOUT "out"
#define PROP_OUTPUT_TYPE_STDERR "err"

static void
load_input (MooWorksheet  *ws,
            MooMarkupNode *elm)
{
    const char *ps = NULL, *ps2 = NULL;
    MooWsBlock *block;

    ps = moo_markup_get_prop (elm, PROP_PS);
    ps2 = moo_markup_get_prop (elm, PROP_PS2);

    if (!ps)
        g_critical ("%s: %s property missing", G_STRLOC, PROP_PS);
    if (!ps2)
        g_critical ("%s: %s property missing", G_STRLOC, PROP_PS2);

    block = create_ws_prompt_block (ps, ps2, moo_markup_get_content (elm));
    g_return_if_fail (block != NULL);

    moo_ws_buffer_append_block (MOO_WS_BUFFER (ws), block);
}

static void
load_output (MooWorksheet  *ws,
             MooMarkupNode *elm)
{
    MooWsBlock *block;
    const char *type;
    OutputType out_type = OUTPUT_OUT;

    type = moo_markup_get_prop (elm, PROP_TYPE);
    if (!type)
    {
        g_critical ("%s: %s property missing", G_STRLOC, PROP_TYPE);
        type = PROP_OUTPUT_TYPE_STDOUT;
    }

    if (!strcmp (type, PROP_OUTPUT_TYPE_STDERR))
        out_type = OUTPUT_ERR;

    block = create_output_block (out_type);
    g_return_if_fail (block != NULL);

    moo_ws_text_block_set_text (MOO_WS_TEXT_BLOCK (block),
                                moo_markup_get_content (elm));
    moo_ws_buffer_append_block (MOO_WS_BUFFER (ws), block);
}

gboolean
moo_worksheet_load (MooWorksheet   *ws,
                    const char     *text,
                    gsize           text_len,
                    GError        **error)
{
    MooMarkupDoc *doc;
    MooMarkupNode *root, *elm, *child;

    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), FALSE);
    g_return_val_if_fail (text != NULL, FALSE);

    if (!(doc = moo_markup_parse_memory (text, text_len, error)))
        return FALSE;

    if (!(root = moo_markup_get_root_element (doc, ELM_WORKSHEET)))
    {
        g_set_error (error, MOO_WORKSHEET_FILE_ERROR,
                     MOO_WORKSHEET_FILE_ERROR_FORMAT,
                     "%s element missing", ELM_WORKSHEET);
        goto error;
    }

    if (!(elm = moo_markup_get_element (root, ELM_CONTENT)))
    {
        g_set_error (error, MOO_WORKSHEET_FILE_ERROR,
                     MOO_WORKSHEET_FILE_ERROR_FORMAT,
                     "%s element missing", ELM_CONTENT);
        goto error;
    }

    moo_worksheet_reset (ws);

    for (child = elm->children; child != NULL; child = child->next)
    {
        if (!MOO_MARKUP_IS_ELEMENT (child))
            continue;

        if (!strcmp (child->name, ELM_INPUT))
            load_input (ws, child);
        else if (!strcmp (child->name, ELM_OUTPUT))
            load_output (ws, child);
        else
        {
            g_critical ("%s: unknown element %s", G_STRFUNC, child->name);
        }
    }

    moo_markup_doc_unref (doc);
    return TRUE;

error:
    moo_markup_doc_unref (doc);
    return FALSE;
}


char *
moo_worksheet_format (MooWorksheet *ws)
{
    MooMarkupDoc *doc;
    MooMarkupNode *root, *content;
    MooWsBlock *block;
    char *markup;

    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), NULL);

    doc = moo_markup_doc_new ("moo-worksheet");
    root = moo_markup_create_root_element (doc, ELM_WORKSHEET);
    content = moo_markup_create_element (root, ELM_CONTENT);

    for (block = _moo_ws_buffer_get_first_block (MOO_WS_BUFFER (ws));
         block != NULL; block = block->next)
    {
        MooMarkupNode *elm;

        if (MOO_IS_WS_PROMPT_BLOCK (block))
        {
            const char *ps, *ps2;
            char *text;

            ps = moo_ws_prompt_block_get_ps (MOO_WS_PROMPT_BLOCK (block));
            ps2 = moo_ws_prompt_block_get_ps2 (MOO_WS_PROMPT_BLOCK (block));
            text = moo_ws_prompt_block_get_text (MOO_WS_PROMPT_BLOCK (block));

            elm = moo_markup_create_text_element (content, ELM_INPUT, text);

            moo_markup_set_prop (elm, PROP_PS, ps);
            moo_markup_set_prop (elm, PROP_PS2, ps2);

            g_free (text);
        }
        else if (MOO_IS_WS_TEXT_BLOCK (block))
        {
            char *text;

            text = moo_ws_text_block_get_text (MOO_WS_TEXT_BLOCK (block));
            elm = moo_markup_create_text_element (content, ELM_OUTPUT, text);

            if (!g_object_get_data (G_OBJECT (block), "moo-worksheet-stderr"))
                moo_markup_set_prop (elm, PROP_TYPE, PROP_OUTPUT_TYPE_STDOUT);
            else
                moo_markup_set_prop (elm, PROP_TYPE, PROP_OUTPUT_TYPE_STDERR);

            g_free (text);
        }
        else
        {
            g_critical ("%s: unknown block", G_STRLOC);
        }
    }

    markup = moo_markup_format_pretty (doc, 2);
    moo_markup_doc_unref (doc);
    return markup;
}
