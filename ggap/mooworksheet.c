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
#include <string.h>


#define DEFAULT_PS1         ">>> "
#define DEFAULT_PS2         "... "

static gunichar INPUT_START_MARK;
static gunichar INPUT_CONT_MARK;
static gunichar OUTPUT_MARK;

// #define INPUT_START_MARK_S  "\363\260\200\200"
// #define INPUT_CONT_MARK_S   "\363\260\200\201"
// #define OUTPUT_MARK_S       "\363\260\200\202"
// #define BLOCK_MARK_LEN      4

#define INPUT_START_MARK_S  "\342\201\240"
#define INPUT_CONT_MARK_S   "\357\273\277"
#define OUTPUT_MARK_S       "\342\200\213"
#define BLOCK_MARK_LEN      3


struct _MooWorksheetPrivate {
    GtkTextBuffer *buffer;
    GtkTextMark *out_mark;
    char *ps1;
    char *ps2;
    gboolean in_input;
    gboolean allow_multiline;
    GtkTextTag *ps_tag;
    GtkTextTag *input_tag;
    GtkTextTag *out_tag;
    GtkTextTag *err_tag;
    GSList *history;
};


static GObject *moo_worksheet_constructor       (GType           type,
                                                 guint           n_construct_properties,
                                                 GObjectConstructParam *construct_param);
static void     moo_worksheet_set_property      (GObject        *object,
                                                 guint           prop_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void     moo_worksheet_get_property      (GObject        *object,
                                                 guint           prop_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);
static void     moo_worksheet_destroy           (GtkObject      *object);

static gboolean moo_worksheet_key_press         (GtkWidget      *widget,
                                                 GdkEventKey    *event);

// static void     moo_worksheet_move_cursor       (GtkTextView    *text_view,
//                                                  GtkMovementStep step,
//                                                  int             count,
//                                                  gboolean        extend_selection);
// static void     moo_worksheet_insert_at_cursor  (GtkTextView    *text_view,
//                                                  const char     *str);
// static void     moo_worksheet_delete_from_cursor(GtkTextView    *text_view,
//                                                  GtkDeleteType   type,
//                                                  int             count);
// static void     moo_worksheet_backspace         (GtkTextView    *text_view);

static void     error_bell                      (MooWorksheet   *ws);
static char    *get_input_text                  (const GtkTextIter *iter);
static void     commit_input                    (MooWorksheet   *ws,
                                                 const GtkTextIter *iter);

static void     get_insert_iter                 (MooWorksheet   *ws,
                                                 GtkTextIter    *iter);
static void     get_output_iter                 (MooWorksheet   *ws,
                                                 GtkTextIter    *iter);

static void     scroll_insert_onscreen          (MooWorksheet   *ws);

static gboolean line_is_input                   (const GtkTextIter *iter);
static gboolean iter_forward_to_input_line      (GtkTextIter    *iter);
static void     iter_forward_to_input_text      (GtkTextIter    *iter);
static gboolean iter_forward_past_input         (GtkTextIter    *iter);


G_DEFINE_TYPE (MooWorksheet, moo_worksheet, MOO_TYPE_TEXT_VIEW)

enum {
    PROP_0,
    PROP_PS1,
    PROP_PS2,
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
//     GtkTextViewClass *textview_class = GTK_TEXT_VIEW_CLASS (klass);

    INPUT_START_MARK = g_utf8_get_char (INPUT_START_MARK_S);
    INPUT_CONT_MARK = g_utf8_get_char (INPUT_CONT_MARK_S);
    OUTPUT_MARK = g_utf8_get_char (OUTPUT_MARK_S);

    gobject_class->constructor = moo_worksheet_constructor;
    gobject_class->set_property = moo_worksheet_set_property;
    gobject_class->get_property = moo_worksheet_get_property;
    gtk_object_class->destroy = moo_worksheet_destroy;

    widget_class->key_press_event = moo_worksheet_key_press;

//     textview_class->move_cursor = moo_worksheet_move_cursor;
//     textview_class->insert_at_cursor = moo_worksheet_insert_at_cursor;
//     textview_class->delete_from_cursor = moo_worksheet_delete_from_cursor;
//     textview_class->backspace = moo_worksheet_backspace;

    g_object_class_install_property (gobject_class,
                                     PROP_PS1,
                                     g_param_spec_string ("ps1",
                                                          "ps1",
                                                          "ps1",
                                                          DEFAULT_PS1,
                                                          G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class,
                                     PROP_PS2,
                                     g_param_spec_string ("ps2",
                                                          "ps2",
                                                          "ps2",
                                                          DEFAULT_PS2,
                                                          G_PARAM_READWRITE));

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
    ws->priv->ps1 = NULL;
    ws->priv->ps2 = NULL;
    ws->priv->allow_multiline = TRUE;

    moo_worksheet_set_ps1 (ws, DEFAULT_PS1);
    moo_worksheet_set_ps2 (ws, DEFAULT_PS2);

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
                                    NULL);
    ws->priv->input_tag =
        gtk_text_buffer_create_tag (buffer,
                                    "moo-worksheet-input",
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

    gtk_text_buffer_get_start_iter (buffer, &iter);
    ws->priv->out_mark =  gtk_text_buffer_create_mark (buffer,
                                                       "moo-worksheet-output",
                                                       &iter, FALSE);

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
        g_free (ws->priv->ps1);
        g_free (ws->priv->ps2);
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
        case PROP_PS1:
            moo_worksheet_set_ps1 (ws, g_value_get_string (value));
            break;
        case PROP_PS2:
            moo_worksheet_set_ps2 (ws, g_value_get_string (value));
            break;

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
        case PROP_PS1:
            g_value_take_string (value, moo_worksheet_get_ps1 (ws));
            break;
        case PROP_PS2:
            g_value_take_string (value, moo_worksheet_get_ps2 (ws));
            break;

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


void
moo_worksheet_set_ps1 (MooWorksheet *ws,
                       const char   *prompt)
{
    char *tmp;

    g_return_if_fail (MOO_IS_WORKSHEET (ws));

    tmp = ws->priv->ps1;
    ws->priv->ps1 = g_strdup_printf (INPUT_START_MARK_S "%s" INPUT_START_MARK_S,
                                     prompt ? prompt : "");
    g_free (tmp);

    g_object_notify (G_OBJECT (ws), "ps1");
}

char *
moo_worksheet_get_ps1 (MooWorksheet *ws)
{
    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), NULL);
    return g_strndup (ws->priv->ps1 + BLOCK_MARK_LEN,
                      strlen (ws->priv->ps1) - 2*BLOCK_MARK_LEN);
}

void
moo_worksheet_set_ps2 (MooWorksheet *ws,
                       const char   *prompt)
{
    char *tmp;

    g_return_if_fail (MOO_IS_WORKSHEET (ws));

    tmp = ws->priv->ps2;
    ws->priv->ps2 = g_strdup_printf (INPUT_CONT_MARK_S "%s" INPUT_CONT_MARK_S,
                                     prompt ? prompt : "");
    g_free (tmp);

    g_object_notify (G_OBJECT (ws), "ps2");
}

char *
moo_worksheet_get_ps2 (MooWorksheet *ws)
{
    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), NULL);
    return g_strndup (ws->priv->ps2 + BLOCK_MARK_LEN,
                      strlen (ws->priv->ps2) - 2*BLOCK_MARK_LEN);
}


void
moo_worksheet_reset (MooWorksheet *ws)
{
    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    ws->priv->in_input = FALSE;
    gtk_text_buffer_set_text (ws->priv->buffer, "", -1);
    g_object_notify (G_OBJECT (ws), "accepting-input");
}


gboolean
moo_worksheet_accepting_input (MooWorksheet *ws)
{
    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), FALSE);
    return ws->priv->in_input;
}


void
moo_worksheet_start_input (MooWorksheet *ws)
{
    GtkTextIter iter;

    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    g_return_if_fail (!ws->priv->in_input);

    ws->priv->in_input = TRUE;
    gtk_text_view_set_editable (GTK_TEXT_VIEW (ws), TRUE);

    get_output_iter (ws, &iter);

    if (!line_is_input (&iter) && !iter_forward_to_input_line (&iter))
    {
        gtk_text_buffer_get_end_iter (ws->priv->buffer, &iter);

        if (!gtk_text_iter_starts_line (&iter))
        {
            gtk_text_buffer_insert_with_tags (ws->priv->buffer, &iter,
                                              "\n", -1,
                                              ws->priv->ps_tag, NULL);
        }
        else if (!gtk_text_iter_is_start (&iter))
        {
            GtkTextIter iter2 = iter;
            gtk_text_iter_backward_char (&iter2);
            gtk_text_buffer_apply_tag (ws->priv->buffer,
                                       ws->priv->ps_tag,
                                       &iter2, &iter);
        }

        gtk_text_buffer_insert_with_tags (ws->priv->buffer, &iter,
                                          ws->priv->ps1, -1,
                                          ws->priv->ps_tag, NULL);
    }
    else
    {
        gtk_text_iter_set_line_offset (&iter, 0);
        iter_forward_to_input_text (&iter);
    }

    gtk_text_buffer_place_cursor (ws->priv->buffer, &iter);
    scroll_insert_onscreen (ws);

    g_object_notify (G_OBJECT (ws), "accepting-input");
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
new_input_line (MooWorksheet      *ws,
                const GtkTextIter *citer)
{
    GtkTextIter iter;

    if (citer)
        iter = *citer;
    else
        get_insert_iter (ws, &iter);

    if (!line_is_input (&iter))
    {
        error_bell (ws);
        return;
    }

    if (!gtk_text_iter_ends_line (&iter))
        gtk_text_iter_forward_to_line_end (&iter);

    gtk_text_buffer_insert (ws->priv->buffer, &iter, "\n", -1);
    gtk_text_buffer_insert_with_tags (ws->priv->buffer, &iter,
                                      ws->priv->ps2, -1,
                                      ws->priv->ps_tag, NULL);
    gtk_text_buffer_place_cursor (ws->priv->buffer, &iter);

    scroll_insert_onscreen (ws);
}


static void
error_bell (MooWorksheet *ws)
{
    if (GTK_WIDGET_REALIZED (ws))
        gdk_display_beep (gtk_widget_get_display (GTK_WIDGET (ws)));
}


static void
commit_input (MooWorksheet      *ws,
              const GtkTextIter *citer)
{
    GtkTextIter iter;
    char *text;
    gboolean complete = TRUE;

    if (citer)
        iter = *citer;
    else
        get_insert_iter (ws, &iter);

    if (!line_is_input (&iter))
    {
        error_bell (ws);
        return;
    }

    text = get_input_text (&iter);
    g_return_if_fail (text != NULL);

    if (ws->priv->allow_multiline && MOO_WORKSHEET_GET_CLASS(ws)->input_complete)
        complete = MOO_WORKSHEET_GET_CLASS(ws)->input_complete (ws, text);

    if (!complete)
    {
        new_input_line (ws, &iter);
    }
    else
    {
        ws->priv->in_input = FALSE;
        gtk_text_view_set_editable (GTK_TEXT_VIEW (ws), FALSE);
        g_object_notify (G_OBJECT (ws), "accepting-input");

        if (!iter_forward_past_input (&iter))
        {
            gtk_text_iter_forward_to_end (&iter);
            gtk_text_buffer_insert (ws->priv->buffer, &iter, "\n", -1);
        }
        else
        {
            if (!line_is_input (&iter))
            {
                GtkTextIter iter2 = iter;
                iter_forward_to_input_line (&iter2);
                gtk_text_buffer_delete (ws->priv->buffer, &iter, &iter2);
            }
        }

        gtk_text_buffer_place_cursor (ws->priv->buffer, &iter);
        gtk_text_buffer_move_mark (ws->priv->buffer,
                                   ws->priv->out_mark,
                                   &iter);
        gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (ws),
                                            ws->priv->out_mark);

        ws->priv->history = g_slist_prepend (ws->priv->history, text);
        g_signal_emit (ws, signals[PROCESS_INPUT], 0, text);
        text = NULL;
    }

    g_free (text);
}

static gboolean
moo_worksheet_key_press (GtkWidget   *widget,
                         GdkEventKey *event)
{
    MooWorksheet *ws = MOO_WORKSHEET (widget);

    if (!ws->priv->in_input)
        goto parent;

    switch (event->keyval)
    {
        case GDK_Return:
            if ((event->state & GDK_SHIFT_MASK) && ws->priv->allow_multiline)
                new_input_line (ws, NULL);
            else
                commit_input (ws, NULL);
            return TRUE;
    }

parent:
    return GTK_WIDGET_CLASS (moo_worksheet_parent_class)->key_press_event (widget, event);
}


static gboolean
line_is_input (const GtkTextIter *citer)
{
    GtkTextIter iter = *citer;
    gunichar c;

    gtk_text_iter_set_line_offset (&iter, 0);
    c = gtk_text_iter_get_char (&iter);

    return c == INPUT_START_MARK || c == INPUT_CONT_MARK;
}

/* assumes iter points to an input line */
static gboolean
iter_forward_past_input (GtkTextIter *iter)
{
    gunichar ch;

    while (TRUE)
    {
        if (!gtk_text_iter_forward_line (iter))
            return FALSE;

        ch = gtk_text_iter_get_char (iter);

        if (ch != INPUT_START_MARK && ch != INPUT_CONT_MARK)
            return TRUE;
    }
}

static gboolean
iter_forward_to_input_line (GtkTextIter *iter)
{
    while (gtk_text_iter_forward_line (iter))
    {
        gunichar c;

        gtk_text_iter_set_line_offset (iter, 0);
        c = gtk_text_iter_get_char (iter);

        if (c == INPUT_START_MARK || c == INPUT_CONT_MARK)
            return TRUE;
    }

    return FALSE;
}

static void
iter_forward_to_input_text (GtkTextIter *iter)
{
    gunichar ch;

    ch = gtk_text_iter_get_char (iter);
    g_return_if_fail (ch == INPUT_START_MARK || ch == INPUT_CONT_MARK);

    do
    {
        if (!gtk_text_iter_forward_char (iter))
        {
            g_critical ("%s: oops", G_STRLOC);
            return;
        }
    }
    while (gtk_text_iter_get_char (iter) != ch);

    gtk_text_iter_forward_char (iter);
}


static char *
get_input_text (const GtkTextIter *citer)
{
    GString *string;
    GtkTextIter start = *citer;
    GtkTextIter end = *citer;
    gunichar c;

    gtk_text_iter_set_line_offset (&start, 0);

    while ((c = gtk_text_iter_get_char (&start)) != INPUT_START_MARK)
    {
        g_return_val_if_fail (c == INPUT_CONT_MARK, NULL);
        if (!gtk_text_iter_backward_line (&start))
            g_return_val_if_reached (NULL);
    }

    string = g_string_new (NULL);

    while ((c = gtk_text_iter_get_char (&start)) == INPUT_START_MARK ||
           c == INPUT_CONT_MARK)
    {
        char *piece;
        iter_forward_to_input_text (&start);
        end = start;
        if (!gtk_text_iter_ends_line (&end))
            gtk_text_iter_forward_to_line_end (&end);
        piece = gtk_text_iter_get_slice (&start, &end);
        if (string->len)
            g_string_append_c (string, '\n');
        g_string_append (string, piece);
        g_free (piece);
        if (!gtk_text_iter_forward_line (&start))
            break;
    }

    return g_string_free (string, FALSE);
}


static void
moo_worksheet_write_output_real (MooWorksheet *ws,
                                 const char   *text,
                                 GtkTextTag   *tag)
{
    char **lines, **p;
    GtkTextIter iter;

    g_return_if_fail (MOO_IS_WORKSHEET (ws));
    g_return_if_fail (text != NULL);

    lines = g_strsplit (text, "\n", 0);
    get_output_iter (ws, &iter);

    for (p = lines; p && *p; ++p)
    {
        if (p != lines)
            gtk_text_buffer_insert_with_tags (ws->priv->buffer, &iter,
                                              "\n", -1, tag, NULL);
        gtk_text_buffer_insert_with_tags (ws->priv->buffer, &iter,
                                          *p, -1, tag, NULL);
    }

    gtk_text_buffer_place_cursor (ws->priv->buffer, &iter);
    scroll_insert_onscreen (ws);

    g_strfreev (lines);
}

void
moo_worksheet_write_output (MooWorksheet *ws,
                            const char   *text)
{
    moo_worksheet_write_output_real (ws, text, ws->priv->out_tag);
}

void
moo_worksheet_write_error (MooWorksheet *ws,
                           const char   *text)
{
    moo_worksheet_write_output_real (ws, text, ws->priv->err_tag);
}
