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

#include "ggap-i18n.h"
#include "mooworksheet.h"
#include "mooutils/moomarshals.h"
#include "mooutils/mooutils-misc.h"
#include <gdk/gdkkeysyms.h>
#include <glib/gprintf.h>
#include <string.h>
#include <stdarg.h>


enum {PS1, PS2};

// #define INPUT_START_MARK_S  "\363\260\200\200"
// #define INPUT_CONT_MARK_S   "\363\260\200\201"
// #define OUTPUT_MARK_S       "\363\260\200\202"
// #define BLOCK_MARK_LEN      4

#define INPUT_START_MARK 0x2060
#define INPUT_CONT_MARK  0xFEFF
// #define OUTPUT_MARK      0x200B
#define INPUT_START_MARK_S  "\342\201\240"
#define INPUT_CONT_MARK_S   "\357\273\277"
// #define OUTPUT_MARK_S       "\342\200\213"
#define BLOCK_MARK_LEN      3

// 0x200C, 0x200D

// U+202F NARROW NO-BREAK SPACE
// UTF-16: 0x202F
// C octal escaped UTF-8: \342\200\257

struct _MooWorksheetPrivate {
    GtkTextBuffer *buffer;
    GtkTextMark *out_mark;
    GtkTextMark *input_mark;
    char *prompt[2];
    gboolean in_input;
    gboolean allow_multiline;
    GtkTextTag *ps_tag;
    GtkTextTag *input_tag;
    GtkTextTag *out_tag;
    GtkTextTag *err_tag;
    GtkTextTag *center_tag;
    GSList *history;

    guint output_newline : 1;
    guint in_key_press : 1;
};


static GObject *moo_worksheet_constructor       (GType               type,
                                                 guint               n_construct_properties,
                                                 GObjectConstructParam *construct_param);
static void     moo_worksheet_set_property      (GObject            *object,
                                                 guint               prop_id,
                                                 const GValue       *value,
                                                 GParamSpec         *pspec);
static void     moo_worksheet_get_property      (GObject            *object,
                                                 guint               prop_id,
                                                 GValue             *value,
                                                 GParamSpec         *pspec);
static void     moo_worksheet_destroy           (GtkObject          *object);

static gboolean moo_worksheet_key_press         (GtkWidget          *widget,
                                                 GdkEventKey        *event);

static void     moo_worksheet_commit_input      (MooWorksheet       *ws,
                                                 GtkTextIter        *iter);
static void     moo_worksheet_new_input_line    (MooWorksheet       *ws,
                                                 GtkTextIter        *iter);
static char    *moo_worksheet_get_text          (const GtkTextIter  *start,
                                                 const GtkTextIter  *end,
                                                 gboolean            include_prompt);

static void     set_accepting_input             (MooWorksheet       *ws,
                                                 gboolean            accepting_input);
static void     error_bell                      (MooWorksheet       *ws);

static void     get_insert_iter                 (MooWorksheet       *ws,
                                                 GtkTextIter        *iter);
static void     get_output_iter                 (MooWorksheet       *ws,
                                                 GtkTextIter        *iter);
static void     get_input_iter                  (MooWorksheet       *ws,
                                                 GtkTextIter        *iter);

static gboolean iter_is_input                   (const GtkTextIter  *iter);
static gboolean iter_is_input_text              (const GtkTextIter  *iter);
static gboolean iter_forward_input_block        (GtkTextIter        *iter);
static void     iter_skip_prompt                (GtkTextIter        *iter);
static void     iter_to_input_block_start       (GtkTextIter        *iter);
static void     iter_to_input_block_end         (GtkTextIter        *iter);
static int      iter_get_input_block_size       (const GtkTextIter  *iter);

G_DEFINE_TYPE (MooWorksheet, moo_worksheet, MOO_TYPE_TEXT_VIEW)

enum {
    PROP_0,
    PROP_ACCEPTING_INPUT,
    PROP_ALLOW_MULTILINE
};

enum {
    PROCESS_INPUT,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

static void
moo_worksheet_class_init (MooWorksheetClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GtkObjectClass *gtk_object_class = GTK_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gobject_class->constructor = moo_worksheet_constructor;
    gobject_class->set_property = moo_worksheet_set_property;
    gobject_class->get_property = moo_worksheet_get_property;
    gtk_object_class->destroy = moo_worksheet_destroy;

    widget_class->key_press_event = moo_worksheet_key_press;

    g_object_class_install_property (gobject_class,
                                     PROP_ACCEPTING_INPUT,
                                     g_param_spec_boolean ("accepting-input",
                                                           "accepting-input",
                                                           "accepting-input",
                                                           FALSE,
                                                           G_PARAM_READABLE));

    g_object_class_install_property (gobject_class,
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
                      _moo_marshal_VOID__STRING,
                      G_TYPE_NONE, 1,
                      G_TYPE_STRING);
}


static void
moo_worksheet_init (MooWorksheet *ws)
{
    ws->priv = g_new0 (MooWorksheetPrivate, 1);
    ws->priv->history = NULL;
    ws->priv->in_input = FALSE;
    ws->priv->prompt[PS1] = NULL;
    ws->priv->prompt[PS2] = NULL;
    ws->priv->allow_multiline = TRUE;

    moo_worksheet_set_ps1 (ws, NULL);
    moo_worksheet_set_ps2 (ws, NULL);

    moo_text_view_set_font_from_string (MOO_TEXT_VIEW (ws), "Monospace");
    gtk_text_view_set_editable (GTK_TEXT_VIEW (ws), FALSE);
}


static GObject *
moo_worksheet_constructor (GType           type,
                           guint           n_props,
                           GObjectConstructParam *props)
{
    GObject *object;
    MooWorksheet *ws;
    GtkTextIter iter;
    GtkTextBuffer *buffer;

    object = G_OBJECT_CLASS (moo_worksheet_parent_class)->constructor (type, n_props, props);
    ws = MOO_WORKSHEET (object);

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (ws));
    ws->priv->buffer = buffer;

    ws->priv->ps_tag =
        gtk_text_buffer_create_tag (buffer,
                                    "moo-worksheet-prompt",
                                    "editable", FALSE,
                                    "foreground", "darkred",
                                    NULL);
    ws->priv->input_tag =
        gtk_text_buffer_create_tag (buffer,
                                    "moo-worksheet-input",
                                    "foreground", "darkgreen",
                                    NULL);
    ws->priv->out_tag =
        gtk_text_buffer_create_tag (buffer,
                                    "moo-worksheet-output",
                                    "editable", FALSE,
                                    "foreground", "blue",
                                    NULL);
    ws->priv->err_tag =
        gtk_text_buffer_create_tag (buffer,
                                    "moo-worksheet-error",
                                    "editable", FALSE,
                                    "foreground", "red",
                                    NULL);
    ws->priv->center_tag =
        gtk_text_buffer_create_tag (buffer,
                                    "moo-worksheet-centered",
                                    "justification", GTK_JUSTIFY_CENTER,
                                    NULL);

    gtk_text_buffer_get_start_iter (buffer, &iter);
    ws->priv->out_mark = gtk_text_buffer_create_mark (buffer,
                                                      "moo-worksheet-output",
                                                      &iter, FALSE);
    ws->priv->input_mark = gtk_text_buffer_create_mark (buffer,
                                                        "moo-worksheet-input",
                                                        &iter, TRUE);

    return object;
}


static void
moo_worksheet_destroy (GtkObject *object)
{
    MooWorksheet *ws = MOO_WORKSHEET (object);

    if (ws->priv)
    {
        g_slist_foreach (ws->priv->history, (GFunc) g_free, NULL);
        g_slist_free (ws->priv->history);
        g_free (ws->priv->prompt[PS1]);
        g_free (ws->priv->prompt[PS2]);
        g_free (ws->priv);
        ws->priv = NULL;
    }

    GTK_OBJECT_CLASS (moo_worksheet_parent_class)->destroy (object);
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
moo_worksheet_set_ps (MooWorksheet *ws,
                      const char   *prompt,
                      int           index)
{
    char *tmp;
    const char *mark;

    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    g_return_if_fail (index < 2);

    mark = index == PS1 ? INPUT_START_MARK_S : INPUT_CONT_MARK_S;
    tmp = ws->priv->prompt[index];
    ws->priv->prompt[index] =
        g_strdup_printf ("%s%s%s", mark, prompt ? prompt : "", mark);
    g_free (tmp);
}

static char *
moo_worksheet_get_ps (MooWorksheet *ws,
                      int           index)
{
    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), NULL);
    g_return_val_if_fail (index < 2, NULL);
    return g_strndup (ws->priv->prompt[index] + BLOCK_MARK_LEN,
                      strlen (ws->priv->prompt[index]) - 2*BLOCK_MARK_LEN);
}

void
moo_worksheet_set_ps1 (MooWorksheet *ws,
                       const char   *prompt)
{
    moo_worksheet_set_ps (ws, prompt, PS1);
}

char *
moo_worksheet_get_ps1 (MooWorksheet *ws)
{
    return moo_worksheet_get_ps (ws, PS1);
}

void
moo_worksheet_set_ps2 (MooWorksheet *ws,
                       const char   *prompt)
{
    moo_worksheet_set_ps (ws, prompt, PS2);
}

char *
moo_worksheet_get_ps2 (MooWorksheet *ws)
{
    return moo_worksheet_get_ps (ws, PS2);
}


static void
set_accepting_input (MooWorksheet *ws,
                     gboolean      accepting_input)
{
    ws->priv->in_input = accepting_input;

    if (accepting_input)
        ws->priv->output_newline = FALSE;

    gtk_text_view_set_editable (GTK_TEXT_VIEW (ws), accepting_input);
    g_object_notify (G_OBJECT (ws), "accepting-input");
}


void
moo_worksheet_reset (MooWorksheet *ws)
{
    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    gtk_text_buffer_set_text (ws->priv->buffer, "", -1);
    set_accepting_input (ws, FALSE);
}


gboolean
moo_worksheet_accepting_input (MooWorksheet *ws)
{
    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), FALSE);
    return ws->priv->in_input;
}


static void
error_bell (MooWorksheet *ws)
{
    if (GTK_WIDGET_REALIZED (ws))
        gdk_display_beep (gtk_widget_get_display (GTK_WIDGET (ws)));
}

static void
scroll_insert_onscreen (MooWorksheet *ws)
{
    gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (ws),
                                        gtk_text_buffer_get_insert (ws->priv->buffer));
}

static void
get_insert_iter (MooWorksheet *ws,
                 GtkTextIter  *iter)
{
    gtk_text_buffer_get_iter_at_mark (ws->priv->buffer, iter,
                                      gtk_text_buffer_get_insert (ws->priv->buffer));
}

static void
get_output_iter (MooWorksheet *ws,
                 GtkTextIter  *iter)
{
    gtk_text_buffer_get_iter_at_mark (ws->priv->buffer, iter,
                                      ws->priv->out_mark);
}

static void
get_input_iter (MooWorksheet *ws,
                GtkTextIter  *iter)
{
    gtk_text_buffer_get_iter_at_mark (ws->priv->buffer, iter,
                                      ws->priv->input_mark);
}


static void
moo_worksheet_new_input_line (MooWorksheet *ws,
                              GtkTextIter  *iter)
{
    g_return_if_fail (iter_is_input_text (iter));

    gtk_text_buffer_insert_with_tags (ws->priv->buffer, iter,
                                      "\n", -1,
                                      ws->priv->ps_tag, NULL);
    gtk_text_buffer_insert_with_tags (ws->priv->buffer, iter,
                                      ws->priv->prompt[PS2], -1,
                                      ws->priv->ps_tag, NULL);
    gtk_text_iter_set_line_offset (iter, 0);
}


static void
moo_worksheet_new_input_block (MooWorksheet *ws,
                               GtkTextIter  *iter)
{
    if (gtk_text_iter_is_end (iter))
    {
        if (!gtk_text_iter_starts_line (iter))
            gtk_text_buffer_insert_with_tags (ws->priv->buffer, iter,
                                              "\n", 1,
                                              ws->priv->ps_tag, NULL);
    }
    else
    {
        gtk_text_iter_set_line_offset (iter, 0);
        gtk_text_buffer_insert_with_tags (ws->priv->buffer, iter,
                                          "\n", 1,
                                          ws->priv->ps_tag, NULL);
        gtk_text_iter_backward_line (iter);
    }

    gtk_text_buffer_insert_with_tags (ws->priv->buffer, iter,
                                      ws->priv->prompt[PS1], -1,
                                      ws->priv->ps_tag, NULL);
    gtk_text_iter_set_line_offset (iter, 0);
}


void
moo_worksheet_start_input (MooWorksheet *ws)
{
    GtkTextIter iter;

    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    g_return_if_fail (!ws->priv->in_input);

    get_output_iter (ws, &iter);

    if (iter_forward_input_block (&iter))
    {
        char *tmp;
        GtkTextIter iter2 = iter;
        iter_skip_prompt (&iter2);
        tmp = gtk_text_iter_get_slice (&iter, &iter2);
        if (strcmp (tmp, ws->priv->prompt[PS1]))
            moo_worksheet_new_input_block (ws, &iter);
        g_free (tmp);
    }
    else
    {
        moo_worksheet_new_input_block (ws, &iter);
    }

    iter_skip_prompt (&iter);
    gtk_text_buffer_place_cursor (ws->priv->buffer, &iter);
    scroll_insert_onscreen (ws);

    set_accepting_input (ws, TRUE);
}


void
moo_worksheet_continue_input (MooWorksheet *ws)
{
    GtkTextIter iter;

    g_return_if_fail (!ws->priv->in_input);

    get_input_iter (ws, &iter);
    g_return_if_fail (iter_is_input (&iter));

    iter_to_input_block_end (&iter);
    moo_worksheet_new_input_line (ws, &iter);

    iter_skip_prompt (&iter);
    gtk_text_buffer_place_cursor (ws->priv->buffer, &iter);
    scroll_insert_onscreen (ws);

    set_accepting_input (ws, TRUE);
}


void
moo_worksheet_resume_input (MooWorksheet *ws,
                            int           line,
                            int           column)
{
    GtkTextIter iter, iter2;
    int max_line;
    int max_column;

    g_return_if_fail (!ws->priv->in_input);

    get_input_iter (ws, &iter);
    g_return_if_fail (iter_is_input (&iter));

    iter_to_input_block_start (&iter);
    max_line = iter_get_input_block_size (&iter) - 1;
    line = CLAMP (line, 0, max_line);
    gtk_text_iter_forward_lines (&iter, line);

    iter_skip_prompt (&iter);
    iter2 = iter;
    if (!gtk_text_iter_ends_line (&iter2))
        gtk_text_iter_forward_to_line_end (&iter2);
    max_column = gtk_text_iter_get_line_offset (&iter2) -
                    gtk_text_iter_get_line_offset (&iter);
    column = CLAMP (column, 0, max_column);
    gtk_text_iter_forward_chars (&iter, column);

    gtk_text_buffer_place_cursor (ws->priv->buffer, &iter);
    scroll_insert_onscreen (ws);

    set_accepting_input (ws, TRUE);
}


/* assumes iter points to the end of input block,
 * makes it point to the same spot after deletion */
static void
delete_output (GtkTextIter *iter)
{
    int offset;
    GtkTextIter start, end;
    GtkTextBuffer *buffer;

    buffer = gtk_text_iter_get_buffer (iter);
    offset = gtk_text_iter_get_offset (iter);
    start = *iter;

    if (!gtk_text_iter_forward_line (&start))
        return;

    if (iter_is_input (&start))
        return;

    end = start;

    while (gtk_text_iter_forward_line (&end) &&
           !iter_is_input (&end))
    {
    }

    gtk_text_buffer_delete (buffer, &start, &end);
    gtk_text_buffer_get_iter_at_offset (buffer, iter, offset);
}

static void
moo_worksheet_commit_input (MooWorksheet *ws,
                            GtkTextIter  *iter)
{
    char *text;
    GtkTextIter start;

    g_return_if_fail (iter_is_input (iter));

    start = *iter;
    iter_to_input_block_start (&start);
    gtk_text_buffer_move_mark (ws->priv->buffer, ws->priv->input_mark, &start);

    iter_to_input_block_end (iter);

    text = moo_worksheet_get_text (&start, iter, FALSE);
    g_return_if_fail (text != NULL);

    set_accepting_input (ws, FALSE);

    delete_output (iter);
    gtk_text_buffer_move_mark (ws->priv->buffer, ws->priv->out_mark, iter);
    gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (ws), ws->priv->out_mark);

    ws->priv->history = g_slist_prepend (ws->priv->history, text);
    g_signal_emit (ws, signals[PROCESS_INPUT], 0, text);
}


static void
commit_input (MooWorksheet *ws)
{
    GtkTextIter iter;

    get_insert_iter (ws, &iter);

    if (!iter_is_input (&iter))
    {
        error_bell (ws);
        return;
    }

    moo_worksheet_commit_input (ws, &iter);
}

static void
new_input_line (MooWorksheet *ws)
{
    GtkTextIter iter;

    get_insert_iter (ws, &iter);

    if (!iter_is_input_text (&iter))
        error_bell (ws);
    else
        moo_worksheet_new_input_line (ws, &iter);
}

static gboolean
moo_worksheet_key_press (GtkWidget   *widget,
                         GdkEventKey *event)
{
    gboolean result;
    MooWorksheet *ws = MOO_WORKSHEET (widget);

    if (!ws->priv->in_input)
        goto parent;

    switch (event->keyval)
    {
        case GDK_Return:
            if (event->state & GDK_SHIFT_MASK)
            {
                if (ws->priv->allow_multiline)
                    new_input_line (ws);
            }
            else
            {
                commit_input (ws);
            }
            return TRUE;
    }

parent:
    ws->priv->in_key_press = TRUE;
    result = GTK_WIDGET_CLASS (moo_worksheet_parent_class)->key_press_event (widget, event);
    ws->priv->in_key_press = FALSE;

    return result;
}


static void
moo_worksheet_write_output_real (MooWorksheet *ws,
                                 GtkTextTag   *tag,
                                 const char   *format,
                                 va_list       args)
{
    char **lines, **p;
    GtkTextIter iter;
    char *text = NULL;

    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    g_return_if_fail (format != NULL);

    g_vasprintf (&text, format, args);
    g_return_if_fail (text != NULL);

    if (!text[0])
    {
        g_free (text);
        return;
    }

    get_output_iter (ws, &iter);

    if (iter_is_input (&iter) || ws->priv->output_newline)
    {
        gtk_text_buffer_insert_with_tags (ws->priv->buffer, &iter,
                                          "\n", -1, tag, NULL);
        ws->priv->output_newline = FALSE;
    }

    lines = g_strsplit (text, "\n", 0);

    for (p = lines; p && *p; ++p)
    {
        if (p != lines)
        {
            if (!p[1] && !**p)
                ws->priv->output_newline = TRUE;
            else
                gtk_text_buffer_insert_with_tags (ws->priv->buffer, &iter,
                                                  "\n", -1, tag, NULL);
        }

        gtk_text_buffer_insert_with_tags (ws->priv->buffer, &iter,
                                          *p, -1, tag, NULL);
    }

    gtk_text_buffer_place_cursor (ws->priv->buffer, &iter);
    scroll_insert_onscreen (ws);

    g_strfreev (lines);
    g_free (text);
}

void
moo_worksheet_write_output (MooWorksheet *ws,
                            const char   *format,
                            ...)
{
    va_list args;
    va_start (args, format);
    moo_worksheet_write_output_real (ws, ws->priv->out_tag, format, args);
    va_end (args);
}

void
moo_worksheet_write_error (MooWorksheet *ws,
                           const char   *format,
                           ...)
{
    va_list args;
    va_start (args, format);
    moo_worksheet_write_output_real (ws, ws->priv->err_tag, format, args);
    va_end (args);
}


/****************************************************************************
 * Iterators
 */

static gboolean
iter_is_input (const GtkTextIter *citer)
{
    GtkTextIter iter = *citer;
    gunichar c;

    gtk_text_iter_set_line_offset (&iter, 0);
    c = gtk_text_iter_get_char (&iter);

    return c == INPUT_START_MARK || c == INPUT_CONT_MARK;
}

static gboolean
iter_is_input_text (const GtkTextIter *citer)
{
    GtkTextIter iter = *citer;
    gunichar c;

    gtk_text_iter_set_line_offset (&iter, 0);
    c = gtk_text_iter_get_char (&iter);

    if (c != INPUT_START_MARK && c != INPUT_CONT_MARK)
        return FALSE;

    iter_skip_prompt (&iter);
    return gtk_text_iter_compare (&iter, citer) <= 0;
}

static void
iter_skip_prompt (GtkTextIter *iter)
{
    gunichar c;

    c = gtk_text_iter_get_char (iter);
    g_return_if_fail (c == INPUT_START_MARK || c == INPUT_CONT_MARK);

    while (gtk_text_iter_forward_char (iter) && gtk_text_iter_get_char (iter) != c)
        {
        }

    gtk_text_iter_forward_char (iter);
}

static gboolean
iter_forward_input_block (GtkTextIter *iter)
{
    if (iter_is_input (iter))
    {
        iter_to_input_block_end (iter);

        if (!gtk_text_iter_forward_line (iter))
            return FALSE;
    }

    while (!iter_is_input (iter))
        if (!gtk_text_iter_forward_line (iter))
            return FALSE;

    return TRUE;
}

static void
iter_to_input_block_end (GtkTextIter *iter)
{
    g_return_if_fail (iter_is_input (iter));

    while (TRUE)
    {
        if (!gtk_text_iter_forward_line (iter))
            return;

        if (gtk_text_iter_get_char (iter) != INPUT_CONT_MARK)
        {
            gtk_text_iter_backward_char (iter);
            return;
        }
    }
}

static void
iter_to_input_block_start (GtkTextIter *iter)
{
    gunichar c;

    gtk_text_iter_set_line_offset (iter, 0);

    while ((c = gtk_text_iter_get_char (iter)) != INPUT_START_MARK)
    {
        g_return_if_fail (c == INPUT_CONT_MARK);
        if (!gtk_text_iter_backward_line (iter))
            g_return_if_reached ();
    }
}

static int
iter_get_input_block_size (const GtkTextIter *iter)
{
    GtkTextIter start = *iter, end = *iter;
    iter_to_input_block_start (&start);
    iter_to_input_block_end (&end);
    return gtk_text_iter_get_line (&end) -
            gtk_text_iter_get_line (&start) + 1;
}


static int
get_prompt_len (const GtkTextIter *citer)
{
    GtkTextIter iter = *citer;
    gtk_text_iter_set_line_offset (&iter, 0);
    iter_skip_prompt (&iter);
    return gtk_text_iter_get_line_offset (&iter) - 2;
}

static void
append_slice (GString           *dest,
              const GtkTextIter *start,
              const GtkTextIter *end)
{
    char *tmp = gtk_text_iter_get_slice (start, end);
    g_string_append (dest, tmp);
    g_free (tmp);
}

static void
get_input_line_chunk (const GtkTextIter *start,
                      const GtkTextIter *end,
                      gboolean           include_prompt,
                      GString           *dest)
{
    GtkTextIter chunk_start, chunk_end;
    int start_offset, end_offset;
    int prompt_len;

    start_offset = gtk_text_iter_get_line_offset (start);
    end_offset = gtk_text_iter_get_line_offset (end);
    prompt_len = get_prompt_len (start);

    chunk_start = *start;

    if (start_offset == 0)
    {
        start_offset = 1;
        gtk_text_iter_forward_char (&chunk_start);
    }

    if (start_offset >= end_offset)
        return;

    if (include_prompt && start_offset < 1 + prompt_len)
    {
        chunk_end = chunk_start;
        gtk_text_iter_set_line_offset (&chunk_end, MIN (prompt_len + 1, end_offset));
        append_slice (dest, &chunk_start, &chunk_end);
    }

    start_offset = MAX (start_offset, prompt_len + 2);

    if (start_offset >= end_offset)
        return;

    gtk_text_iter_set_line_offset (&chunk_start, start_offset);
    chunk_end = chunk_start;
    gtk_text_iter_set_line_offset (&chunk_end, end_offset);
    append_slice (dest, &chunk_start, &chunk_end);
}

static void
get_input_line (const GtkTextIter *citer,
                gboolean           include_prompt,
                GString           *dest)
{
    GtkTextIter start;
    GtkTextIter iter;
    int prompt_len;

    start = iter = *citer;
    gtk_text_iter_set_line_offset (&iter, 0);
    iter_skip_prompt (&iter);
    prompt_len = gtk_text_iter_get_line_offset (&iter) - 2;

    if (include_prompt)
    {
        gtk_text_iter_set_line_offset (&start, 1);
        gtk_text_iter_set_line_offset (&iter, prompt_len + 1);
        append_slice (dest, &start, &iter);
    }

    gtk_text_iter_set_line_offset (&start, prompt_len + 2);

    if (!gtk_text_iter_ends_line (&start))
    {
        iter = start;
        gtk_text_iter_forward_to_line_end (&iter);
        append_slice (dest, &start, &iter);
    }
}

static char *
moo_worksheet_get_text (const GtkTextIter *start,
                        const GtkTextIter *end,
                        gboolean           include_prompt)
{
    GString *string;
    GtkTextIter chunk_start, chunk_end;
    int start_line, end_line;

    string = g_string_new (NULL);

    if (gtk_text_iter_compare (start, end) > 0)
    {
        const GtkTextIter *tmp = start;
        start = end;
        end = tmp;
    }
    else if (gtk_text_iter_compare (start, end) == 0)
    {
        return g_strdup ("");
    }

    start_line = gtk_text_iter_get_line (start);
    end_line = gtk_text_iter_get_line (end);

    if (iter_is_input (start))
    {
        if (end_line > start_line)
        {
            chunk_end = *start;
            if (!gtk_text_iter_ends_line (&chunk_end))
                gtk_text_iter_forward_to_line_end (&chunk_end);
        }
        else
        {
            chunk_end = *end;
        }

        get_input_line_chunk (start, &chunk_end, include_prompt, string);
    }
    else
    {
        chunk_end = *start;
        if (!gtk_text_iter_ends_line (&chunk_end))
            gtk_text_iter_forward_to_line_end (&chunk_end);
        append_slice (string, start, &chunk_end);
    }

    if (end_line > start_line)
        g_string_append (string, "\n");

    if (end_line > start_line + 1)
    {
        int i;

        chunk_start = *start;
        gtk_text_iter_forward_line (&chunk_start);

        for (i = start_line + 1; i < end_line; ++i)
        {
            if (iter_is_input (&chunk_start))
            {
                get_input_line (&chunk_start, include_prompt, string);
            }
            else if (!gtk_text_iter_ends_line (&chunk_start))
            {
                chunk_end = chunk_start;
                gtk_text_iter_forward_to_line_end (&chunk_end);
                append_slice (string, &chunk_start, &chunk_end);
            }

            g_string_append (string, "\n");
            gtk_text_iter_forward_line (&chunk_start);
        }
    }

    if (end_line > start_line)
    {
        chunk_start = *end;

        gtk_text_iter_set_line_offset (&chunk_start, 0);

        if (iter_is_input (&chunk_start))
            get_input_line_chunk (&chunk_start, end, include_prompt, string);
        else
            append_slice (string, &chunk_start, end);
    }

    return g_string_free (string, FALSE);
}


// void
// moo_worksheet_insert_widget (MooWorksheet *ws,
//                              GtkWidget    *widget)
// {
//     GtkTextIter iter, iter2;
//     GtkTextChildAnchor *anchor;
//
//     g_return_if_fail (MOO_IS_WORKSHEET (ws));
//     g_return_if_fail (GTK_IS_WIDGET (widget));
//
//     get_output_iter (ws, &iter);
//
//     gtk_text_buffer_insert (ws->priv->buffer, &iter, OUTPUT_MARK_S "\n", -1);
//     gtk_text_iter_backward_char (&iter);
//
//     anchor = gtk_text_buffer_create_child_anchor (ws->priv->buffer, &iter);
//     gtk_widget_set_size_request (widget, 200, 200);
//     gtk_text_view_add_child_at_anchor (GTK_TEXT_VIEW (ws), widget, anchor);
//
//     iter2 = iter;
//     gtk_text_iter_backward_char (&iter);
//     gtk_text_iter_forward_line (&iter2);
//     gtk_text_buffer_apply_tag (ws->priv->buffer, ws->priv->out_tag, &iter, &iter2);
//     gtk_text_buffer_apply_tag (ws->priv->buffer, ws->priv->center_tag, &iter, &iter2);
//
//     gtk_text_buffer_place_cursor (ws->priv->buffer, &iter2);
//     scroll_insert_onscreen (ws);
// }
