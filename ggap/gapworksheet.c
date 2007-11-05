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
#include "gap.h"
#include "mooterm/mootermpt.h"
#include "mooutils/mooutils-misc.h"
#include <glib/gregex.h>

#define GGAP_PROMPT     "@GGAP-PROMPT@"
#define GGAP_PROMPT_LEN 13
#define SPECIAL_MAX_LEN 40

struct _GapWorksheetPrivate {
    MooTermPt *pt;
    GString *buffer;
    gboolean in_stderr;
    GapState state;

    gboolean loaded;

    guint resize_idle;
    int width;
    int height;
};


static void     gap_worksheet_view_init         (GapViewIface   *iface);
static void     gap_worksheet_destroy           (GtkObject      *object);

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

/* GAP_TYPE_WORKSHEET */
G_DEFINE_TYPE_WITH_CODE (GapWorksheet, gap_worksheet, MOO_TYPE_WORKSHEET,
                         G_IMPLEMENT_INTERFACE (GAP_TYPE_VIEW, gap_worksheet_view_init))

enum {
    PROP_0,
    PROP_GAP_STATE
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
            g_value_set_enum (value, ws->priv->state);
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
    if (ws->priv && ws->priv->state != state)
    {
        ws->priv->state = state;
        g_object_notify (G_OBJECT (ws), "gap-state");
    }
}


static gboolean
gap_worksheet_view_start_gap (GapView    *view,
                              const char *cmd_line)
{
    GError *error = NULL;
    GapWorksheet *ws = GAP_WORKSHEET (view);

    moo_worksheet_reset (MOO_WORKSHEET (view));
    ws->priv->loaded = FALSE;

    if (!gap_worksheet_fork_command (GAP_WORKSHEET (view), cmd_line, &error))
    {
        moo_worksheet_write_error (MOO_WORKSHEET (view), error->message);
        g_error_free (error);
        return FALSE;
    }

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
    ws->priv = g_new0 (GapWorksheetPrivate, 1);
    ws->priv->buffer = NULL;
    ws->priv->pt = NULL;
    ws->priv->in_stderr = FALSE;
    ws->priv->width = -1;
    ws->priv->height = -1;
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
        if (ws->priv->buffer)
            g_string_free (ws->priv->buffer, TRUE);
        g_free (ws->priv);
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
process_input (GapWorksheet *ws,
               const char   *chunk,
               gsize         len)
{
    MooWorksheet *mws = MOO_WORKSHEET (ws);

    if (ws->priv->in_stderr)
        moo_worksheet_write_error_len (mws, chunk, len);
    else
        moo_worksheet_write_output (mws, chunk, len);
}

static void
do_prompt (GapWorksheet *ws,
           const char   *string,
           gsize         len)
{
    MooWorksheet *mws = MOO_WORKSHEET (ws);
    gboolean continue_input;
    static GRegex *regex;

    if (!regex)
        regex = g_regex_new ("^(gap|brk(_\\d\\d)?)?> $",
                             G_REGEX_ANCHORED | G_REGEX_OPTIMIZE,
                             0, NULL);

    if (!g_regex_match_full (regex, string, len, 0, 0, NULL, NULL))
    {
        char *s = g_strndup (string, len);
        g_critical ("%s: wrong prompt '%s'", G_STRLOC, s);
        g_free (s);
        return;
    }

    continue_input = !strncmp (string, "> ", 2);

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

    ws->priv->loaded = TRUE;
    set_state (ws, GAP_IN_PROMPT);
}

static void
do_special (GapWorksheet *ws,
            const char   *string,
            gsize         len)
{
    if (len > GGAP_PROMPT_LEN && !strncmp (string, GGAP_PROMPT, GGAP_PROMPT_LEN))
    {
        do_prompt (ws, string + GGAP_PROMPT_LEN, len - GGAP_PROMPT_LEN);
    }
    else
    {
        char *s = g_strndup (string, len);
        g_critical ("%s: %s", G_STRLOC, s);
        g_free (s);
    }
}


static void
find_special (const char *string,
              gsize       len,
              int        *special_start,
              int        *special_len)
{
    gsize i;

    *special_start = -1;
    *special_len = -1;

    for (i = 0; i < len; ++i)
    {
        if (string[i] == '@')
        {
            const char *remainder;
            gsize remainder_len;
            guint j;
            static struct {
                const char *prompt;
                gsize len;
            } fixed[] = {{"> ", 2}, {"gap> ", 5}, {"brk> ", 5}};
            gboolean error = FALSE;

            remainder = string + i;
            remainder_len = len - i;

            if (strncmp (remainder, GGAP_PROMPT, MIN (remainder_len, GGAP_PROMPT_LEN)) != 0)
                continue;

            if (remainder_len < GGAP_PROMPT_LEN)
            {
                *special_start = i;
                return;
            }

            remainder += GGAP_PROMPT_LEN;
            remainder_len -= GGAP_PROMPT_LEN;

            for (j = 0; j < G_N_ELEMENTS (fixed); ++j)
            {
                if (strncmp (remainder, fixed[j].prompt, MIN (fixed[j].len, remainder_len)) == 0)
                {
                    *special_start = i;
                    if (remainder_len >= fixed[j].len)
                        *special_len = GGAP_PROMPT_LEN + fixed[j].len;
                    return;
                }
            }

            if (strncmp (remainder, "brk", MIN (3, remainder_len)) == 0)
            {
                *special_start = i;

                if (remainder_len <= 3)
                    return;

#define IS_DIGIT(c) ((c) <= '9' && (c) >= '0')
                if (remainder[3] != '_')
                    error = TRUE;
                else if (remainder_len <= 4)
                    return;
                else if (!IS_DIGIT (remainder[4]))
                    error = TRUE;
                else if (remainder_len <= 5)
                    return;
                else if (!IS_DIGIT (remainder[5]))
                    error = TRUE;
                else if (remainder_len <= 6)
                    return;
                else if (remainder[6] != '>')
                    error = TRUE;
                else if (remainder_len <= 7)
                    return;
                else if (remainder[7] != ' ')
                    error = TRUE;
                else
                {
                    *special_len = GGAP_PROMPT_LEN + 8;
                    return;
                }
#undef IS_DIGIT

                *special_start = -1;
            }

            {
                char *s = g_strndup (remainder, remainder_len);
                g_critical ("%s: '%s'", G_STRLOC, s);
                g_free (s);
            }
        }
    }
}

static void
io_func (const char *buf,
         gsize       len,
         gpointer    data)
{
    GapWorksheet *ws = data;

    g_return_if_fail (len > 0);

    while (len || ws->priv->buffer)
    {
        const char *chunk;
        gsize chunk_len;
        int special_start, special_len;

        if (ws->priv->buffer)
        {
            if (len)
            {
                gsize piece_len = MIN (SPECIAL_MAX_LEN, len);
                g_string_append_len (ws->priv->buffer, buf, piece_len);
                buf += piece_len;
                len -= piece_len;
            }

            chunk = ws->priv->buffer->str;
            chunk_len = ws->priv->buffer->len;
        }
        else
        {
            chunk = buf;
            chunk_len = len;
            len = 0;
        }

        find_special (chunk, chunk_len, &special_start, &special_len);

        if (special_start != 0)
            process_input (ws, chunk, special_start > 0 ? (gsize) special_start : chunk_len);

        if (special_start < 0)
        {
            if (ws->priv->buffer)
            {
                g_string_free (ws->priv->buffer, TRUE);
                ws->priv->buffer = NULL;
            }
        }
        else if (special_len < 0)
        {
            if (ws->priv->buffer)
            {
                g_assert (chunk == ws->priv->buffer->str);
                memmove (ws->priv->buffer->str + special_start,
                         ws->priv->buffer->str,
                         ws->priv->buffer->len - special_start + 1);
                ws->priv->buffer->len -= special_start;
            }
            else
            {
                ws->priv->buffer = g_string_new_len (chunk + special_start,
                                                     chunk_len - special_start);
            }

            if (!len)
                break;
        }
        else
        {
            do_special (ws, chunk + special_start, special_len);

            if (ws->priv->buffer)
            {
                g_assert (chunk == ws->priv->buffer->str);
                memmove (ws->priv->buffer->str + special_start + special_len,
                         ws->priv->buffer->str,
                         ws->priv->buffer->len - special_start - special_len + 1);
                ws->priv->buffer->len -= special_start + special_len;
            }
            else
            {
                buf = chunk + special_start + special_len;
                len = chunk_len - special_start - special_len;
            }
        }
    }
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
