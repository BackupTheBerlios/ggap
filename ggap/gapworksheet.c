/*
 *   gapworksheet.c
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
#include "gapworksheet.h"
#include "gapapp.h"
#include "gapparser.h"
#include "ggapfile.h"
#include "gap.h"
#include "mooterm/mootermpt.h"
#include "mooutils/mooutils-misc.h"
#include <glib/gregex.h>
#include <errno.h>
#include <stdlib.h>

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

    GString *input_buf;
    GString *input_buf2;
    gsize input_data_len;
    InputState input_state;

    char *filename;
    gboolean loaded;

    guint resize_idle;
    int width;
    int height;

    guint last_stamp;
    GapCommandInfo *cmd_info;
};


static void     gap_worksheet_view_init         (GapViewIface   *iface);
static void     gap_worksheet_destroy           (GtkObject      *object);
static GObject *gap_worksheet_constructor       (GType           type,
                                                 guint           n_props,
                                                 GObjectConstructParam *props);

static void     gap_worksheet_size_allocate     (GtkWidget      *widget,
                                                 GtkAllocation  *allocation);
static void     gap_worksheet_style_set         (GtkWidget      *widget,
                                                 GtkStyle       *prev_style);
static void     gap_worksheet_realize           (GtkWidget      *widget);

static void     gap_worksheet_update_size       (GapWorksheet   *ws);

static void     gap_worksheet_process_input     (MooWorksheet   *mws,
                                                 char          **lines);

static gboolean gap_worksheet_fork_command      (GapWorksheet   *ws,
                                                 const char     *cmd_line,
                                                 GError        **error);
static void     gap_worksheet_kill_child        (GapWorksheet   *ws);
static gboolean gap_worksheet_child_alive       (GapWorksheet   *ws);

static void     stop_running_command_loop       (GapCommandInfo *ci);
static gboolean has_running_command_loop        (GapWorksheet   *ws);


/* GAP_TYPE_WORKSHEET */
G_DEFINE_TYPE_WITH_CODE (GapWorksheet, gap_worksheet, MOO_TYPE_WORKSHEET,
                         G_IMPLEMENT_INTERFACE (GAP_TYPE_VIEW, gap_worksheet_view_init))

enum {
    PROP_0,
    PROP_GAP_STATE,
    PROP_FILENAME,
    PROP_MODIFIED
};

// static void
// gap_worksheet_set_property (GObject      *object,
//                             guint         prop_id,
//                             const GValue *value,
//                             GParamSpec   *pspec)
// {
//     GapWorksheet *ws = GAP_WORKSHEET (object);
//
//     switch (prop_id)
//     {
//         case PROP_ALLOW_MULTILINE:
//             ws->priv->allow_multiline = g_value_get_boolean (value);
//             g_object_notify (object, "allow-multiline");
//             break;
//
//         default:
//             G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
//             break;
//     }
// }

static void
gap_worksheet_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
    GapWorksheet *ws = GAP_WORKSHEET (object);

    switch (prop_id)
    {
        case PROP_GAP_STATE:
            g_value_set_enum (value, ws->priv->gap_state);
            break;

        case PROP_FILENAME:
            g_value_set_string (value, ws->priv->filename);
            break;

        case PROP_MODIFIED:
            g_value_set_boolean (value, gap_worksheet_get_modified (ws));
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
    GtkObjectClass *gtk_object_class = GTK_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
    MooWorksheetClass *ws_class = MOO_WORKSHEET_CLASS (klass);

    object_class->constructor = gap_worksheet_constructor;
    object_class->get_property = gap_worksheet_get_property;
    gtk_object_class->destroy = gap_worksheet_destroy;

    widget_class->size_allocate = gap_worksheet_size_allocate;
    widget_class->style_set = gap_worksheet_style_set;
    widget_class->realize = gap_worksheet_realize;

    ws_class->process_input = gap_worksheet_process_input;

    g_object_class_install_property (object_class,
                                     PROP_GAP_STATE,
                                     g_param_spec_enum ("gap-state",
                                                        "gap-state",
                                                        "gap-state",
                                                        GAP_TYPE_STATE,
                                                        GAP_DEAD,
                                                        G_PARAM_READABLE));

    g_object_class_install_property (object_class,
                                     PROP_FILENAME,
                                     g_param_spec_string ("filename",
                                                          "filename",
                                                          "filename",
                                                          NULL,
                                                          G_PARAM_READABLE));

    g_object_class_install_property (object_class,
                                     PROP_MODIFIED,
                                     g_param_spec_boolean ("modified",
                                                           "modified",
                                                           "modified",
                                                           FALSE,
                                                           G_PARAM_READABLE));

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
gap_worksheet_view_start_gap (GapView    *view,
                              const char *cmd_line)
{
    GError *error = NULL;
    GapWorksheet *ws = GAP_WORKSHEET (view);

//     moo_worksheet_reset (MOO_WORKSHEET (view));
    ws->priv->loaded = FALSE;

    if (!gap_worksheet_fork_command (GAP_WORKSHEET (view), cmd_line, &error))
    {
        moo_worksheet_write_error (MOO_WORKSHEET (view), error->message);
        g_error_free (error);
        return FALSE;
    }

    set_state (ws, GAP_LOADING);
    return TRUE;
}

static void
gap_worksheet_view_stop_gap (GapView *view)
{
    gap_worksheet_kill_child (GAP_WORKSHEET (view));
}

static void
gap_worksheet_view_feed_gap (G_GNUC_UNUSED GapView    *view,
                             G_GNUC_UNUSED const char *text)
{
    g_warning ("%s: implement me", G_STRLOC);
}

static gboolean
gap_worksheet_view_child_alive (GapView *view)
{
    return gap_worksheet_child_alive (GAP_WORKSHEET (view));
}

static void
gap_worksheet_view_send_intr (G_GNUC_UNUSED GapView *view)
{
    g_warning ("%s: implement me", G_STRLOC);
}

static void
gap_worksheet_view_get_gap_flags (G_GNUC_UNUSED GapView *view,
                                  char **flags, gboolean *fancy)
{
    *flags = g_strdup ("-n");
    *fancy = TRUE;
}

static void
gap_worksheet_view_init (GapViewIface *iface)
{
    iface->start_gap = gap_worksheet_view_start_gap;
    iface->stop_gap = gap_worksheet_view_stop_gap;
    iface->feed_gap = gap_worksheet_view_feed_gap;
    iface->child_alive = gap_worksheet_view_child_alive;
    iface->send_intr = gap_worksheet_view_send_intr;
    iface->get_gap_flags = gap_worksheet_view_get_gap_flags;
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
}


static void
emit_modified_changed (GObject *object)
{
    g_object_notify (object, "modified");
}

static GObject *
gap_worksheet_constructor (GType           type,
                           guint           n_props,
                           GObjectConstructParam *props)
{
    GObject *object;
    GtkTextBuffer *buffer;

    object = G_OBJECT_CLASS (gap_worksheet_parent_class)->constructor (type, n_props, props);

    moo_worksheet_start_input (MOO_WORKSHEET (object), "gap> ", "> ");
    gap_view_start_gap (GAP_VIEW (object), NULL);
    gap_worksheet_set_modified (GAP_WORKSHEET (object), FALSE);
    moo_worksheet_set_accepting_input (MOO_WORKSHEET (object), FALSE);

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (object));
    g_signal_connect_swapped (buffer, "modified-changed",
                              G_CALLBACK (emit_modified_changed),
                              object);

    return object;
}


static void
gap_worksheet_destroy (GtkObject *object)
{
    GapWorksheet *ws = GAP_WORKSHEET (object);

    if (ws->priv)
    {
        if (ws->priv->resize_idle)
            g_source_remove (ws->priv->resize_idle);

        if (gap_worksheet_child_alive (ws))
            gap_worksheet_kill_child (ws);
        if (ws->priv->input_buf)
            g_string_free (ws->priv->input_buf, TRUE);
        if (ws->priv->input_buf2)
            g_string_free (ws->priv->input_buf2, TRUE);

        ws->priv = NULL;
    }

    GTK_OBJECT_CLASS (gap_worksheet_parent_class)->destroy (object);
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
        moo_term_pt_write (ws->priv->pt, string, -1);
        g_free (string);
    }
    else
    {
        moo_term_pt_write (ws->priv->pt, lines[0], -1);
    }

    moo_term_pt_write (ws->priv->pt, "\n", -1);
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
        g_critical ("%s: output: %.*s", G_STRLOC, data_len, data);
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
//     else if (data_len >= strlen ("globals:") && strncmp (data, "globals:", strlen ("globals:")) == 0)
//         do_globals (ws, data + strlen ("globals:"), data_len - strlen ("globals:"));
    else
    {
        g_critical ("%s: got unknown data: '%.*s'", G_STRLOC, data_len, data);
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


static void
gap_worksheet_update_size (GapWorksheet *ws)
{
    if (ws->priv->pt)
        moo_term_pt_set_size (ws->priv->pt, ws->priv->width, ws->priv->height);
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

    gap_worksheet_update_size (ws);

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
    GTK_WIDGET_CLASS(gap_worksheet_parent_class)->size_allocate (widget, allocation);
    queue_resize (GAP_WORKSHEET (widget));
}

static void
gap_worksheet_style_set (GtkWidget *widget,
                         GtkStyle  *prev_style)
{
    GTK_WIDGET_CLASS(gap_worksheet_parent_class)->style_set (widget, prev_style);
    queue_resize (GAP_WORKSHEET (widget));
}

static void
gap_worksheet_realize (GtkWidget *widget)
{
    GTK_WIDGET_CLASS(gap_worksheet_parent_class)->realize (widget);
    queue_resize (GAP_WORKSHEET (widget));
}


gboolean
gap_worksheet_get_empty (GapWorksheet *ws)
{
    g_return_val_if_fail (GAP_IS_WORKSHEET (ws), FALSE);

    return ws->priv->filename == NULL && !gap_worksheet_get_modified (ws);
}

gboolean
gap_worksheet_get_modified (GapWorksheet *ws)
{
    g_return_val_if_fail (GAP_IS_WORKSHEET (ws), FALSE);

    return gtk_text_buffer_get_modified (gtk_text_view_get_buffer (GTK_TEXT_VIEW (ws)));
}

void
gap_worksheet_set_modified (GapWorksheet *ws,
                            gboolean      modified)
{
    g_return_if_fail (GAP_IS_WORKSHEET (ws));

    gtk_text_buffer_set_modified (gtk_text_view_get_buffer (GTK_TEXT_VIEW (ws)),
                                  modified);
}

const char *
gap_worksheet_get_filename (GapWorksheet *ws)
{
    g_return_val_if_fail (GAP_IS_WORKSHEET (ws), NULL);
    return ws->priv->filename;
}

gboolean
gap_worksheet_load (GapWorksheet   *ws,
                    const char     *filename,
                    GError        **error)
{
    char *text;
    gsize text_len;
    char *workspace_file;
    char *tmp;

    g_return_val_if_fail (GAP_IS_WORKSHEET (ws), FALSE);
    g_return_val_if_fail (filename != NULL, FALSE);
    g_return_val_if_fail (gap_worksheet_get_empty (ws), FALSE);

    if (!ggap_file_unpack (filename, &text, &text_len, &workspace_file, error))
        return FALSE;

    g_print ("text: %d\n%.*s\n", (int) text_len, (int) text_len, text);
    g_print ("workspace: %s\n", workspace_file ? workspace_file : "NULL");

    if (!moo_worksheet_load (MOO_WORKSHEET (ws), text, text_len, error))
        return FALSE;

    tmp = ws->priv->filename;
    ws->priv->filename = g_strdup (filename);
    g_free (tmp);
    g_object_notify (G_OBJECT (ws), "filename");

    gap_view_stop_gap (GAP_VIEW (ws));
    gap_view_start_gap (GAP_VIEW (ws), workspace_file);
    gap_worksheet_set_modified (ws, FALSE);

    g_free (text);
    g_free (workspace_file);
    return TRUE;
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
    moo_term_pt_write (ws->priv->pt, string, -1);

    if (gap_cmd_line)
        moo_term_pt_write (ws->priv->pt, gap_cmd_line, -1);

    g_object_ref (ws);

    ws->priv->cmd_info = &ci;
    ci.success = FALSE;
    ci.loop = NULL;
    ci.destroyed = FALSE;
    ci.stamp = stamp;
    ci.output = NULL;

    destroy_cb_id = g_signal_connect (ws, "destroy",
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

    filename = moo_app_tempnam (moo_app_get_instance ());
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

gboolean
gap_worksheet_save (GapWorksheet   *ws,
                    const char     *filename,
                    gboolean        save_workspace,
                    GError        **error)
{
    char *markup;
    char *workspace = NULL;
    gboolean result;

    g_return_val_if_fail (GAP_IS_WORKSHEET (ws), FALSE);
    g_return_val_if_fail (filename != NULL, FALSE);
    g_return_val_if_fail (ws->priv->gap_state == GAP_DEAD ||
                          ws->priv->gap_state == GAP_IN_PROMPT, FALSE);

    if (save_workspace)
        if (ws->priv->gap_state != GAP_DEAD && !gap_worksheet_save_workspace (ws, &workspace, error))
            return FALSE;

    markup = moo_worksheet_format (MOO_WORKSHEET (ws));

    result = ggap_file_pack (markup, workspace, filename, error);

    if (result)
    {
        char *tmp = ws->priv->filename;
        ws->priv->filename = g_strdup (filename);
        g_free (tmp);
        gap_worksheet_set_modified (ws, FALSE);
        g_object_notify (G_OBJECT (ws), "filename");
    }

    g_free (markup);
    return result;
}
