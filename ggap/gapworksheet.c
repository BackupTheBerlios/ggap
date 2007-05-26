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
#include "gap.h"
#include "mooterm/mootermpt.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/eggregex.h"


struct _GapWorksheetPrivate {
    MooTermPt *pt;
    GString *line;
};


static void     gap_worksheet_view_init         (GapViewIface   *iface);
static void     gap_worksheet_destroy           (GtkObject      *object);

static void     gap_worksheet_process_input     (MooWorksheet   *mws,
                                                 const char     *input);

static gboolean gap_worksheet_fork_command      (GapWorksheet   *ws,
                                                 const char     *cmd_line,
                                                 GError        **error);
static void     gap_worksheet_kill_child        (GapWorksheet   *ws);
static gboolean gap_worksheet_child_alive       (GapWorksheet   *ws);

/* GAP_TYPE_WORKSHEET */
G_DEFINE_TYPE_WITH_CODE (GapWorksheet, gap_worksheet, MOO_TYPE_WORKSHEET,
                         G_IMPLEMENT_INTERFACE (GAP_TYPE_VIEW, gap_worksheet_view_init))

enum {
    PROP_0
};

static void
gap_worksheet_class_init (GapWorksheetClass *klass)
{
    GtkObjectClass *gtk_object_class = GTK_OBJECT_CLASS (klass);
    MooWorksheetClass *ws_class = MOO_WORKSHEET_CLASS (klass);

    gtk_object_class->destroy = gap_worksheet_destroy;

    ws_class->process_input = gap_worksheet_process_input;
}


static gboolean
gap_worksheet_view_start_gap (GapView    *view,
                              const char *cmd_line)
{
    GError *error = NULL;

    moo_worksheet_reset (MOO_WORKSHEET (view));

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
gap_worksheet_view_display_graph (GapView *view,
                                  GObject *obj)
{
    g_return_if_fail (GTK_IS_WIDGET (obj));
    moo_worksheet_insert_widget (MOO_WORKSHEET (view), GTK_WIDGET (obj));
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
    iface->display_graph = gap_worksheet_view_display_graph;
}


static void
gap_worksheet_init (GapWorksheet *ws)
{
    ws->priv = g_new0 (GapWorksheetPrivate, 1);
    ws->priv->line = g_string_new (NULL);
    ws->priv->pt = NULL;
    g_object_set (ws, "allow-multiline", FALSE, NULL);
}


static void
gap_worksheet_destroy (GtkObject *object)
{
    GapWorksheet *ws = GAP_WORKSHEET (object);

    if (ws->priv)
    {
        if (gap_worksheet_child_alive (ws))
            gap_worksheet_kill_child (ws);
        g_free (ws->priv);
        ws->priv = NULL;
    }

    GTK_OBJECT_CLASS (gap_worksheet_parent_class)->destroy (object);
}


static void
gap_worksheet_process_input (MooWorksheet *mws,
                             const char   *input)
{
    GapWorksheet *ws = GAP_WORKSHEET (mws);
    gap_parse (input);
    moo_term_pt_write (ws->priv->pt, input, -1);
    moo_term_pt_write (ws->priv->pt, "\n", -1);
}


static void
child_died (GapWorksheet *ws)
{
    g_object_unref (ws->priv->pt);
    ws->priv->pt = NULL;
    g_signal_emit_by_name (ws, "gap-exited");
}


static gboolean
line_starts_prompt (GString *line)
{
    static EggRegex *regex;

    if (!regex)
    {
        regex = egg_regex_new ("^ggap-prompt-(gap|brk(_\\d\\d)?|)> ", EGG_REGEX_ANCHORED,
                               EGG_REGEX_MATCH_PARTIAL, NULL);
        egg_regex_optimize (regex, NULL);
    }

    egg_regex_clear (regex);
    return egg_regex_match (regex, line->str, 0) ||
           egg_regex_is_partial_match (regex);
}

static gboolean
line_is_prompt (GString *line)
{
    static EggRegex *regex;

    if (!regex)
    {
        regex = egg_regex_new ("^ggap-prompt-(gap|brk(_\\d\\d)?|)> ", EGG_REGEX_ANCHORED, 0, NULL);
        egg_regex_optimize (regex, NULL);
    }

    egg_regex_clear (regex);
    return egg_regex_match (regex, line->str, 0);
}


static void
io_func (const char *buf,
         gsize       len,
         gpointer    data)
{
    GapWorksheet *ws = data;
    MooWorksheet *mws = data;
    char **lines;
    guint n_lines;

    if (!len)
        return;

    if (moo_worksheet_accepting_input (mws))
    {
        char *c = g_strndup (buf, len);
        g_print ("oops: %s\n", c);
        g_free (c);
        return;
    }

    lines = moo_strnsplit_lines (buf, len, &n_lines);

    if (n_lines > 1)
    {
        guint i;

        g_string_truncate (ws->priv->line, 0);

        for (i = 0; i + 1 < n_lines; ++i)
        {
            moo_worksheet_write_output (mws, lines[i]);
            moo_worksheet_write_output (mws, "\n");
        }
    }

    g_string_append (ws->priv->line, lines[n_lines-1]);

    if (!line_starts_prompt (ws->priv->line))
    {
        g_string_truncate (ws->priv->line, 0);
        moo_worksheet_write_output (mws, lines[n_lines-1]);
    }
    else if (line_is_prompt (ws->priv->line))
    {
        const char *prompt;
        gboolean continue_input;

        prompt = ws->priv->line->str + strlen ("ggap-prompt-");
        continue_input = !strcmp (prompt, "> ");

        if (!continue_input)
            moo_worksheet_set_ps1 (mws, prompt);

        g_string_truncate (ws->priv->line, 0);

        if (continue_input)
            moo_worksheet_continue_input (mws);
        else
            moo_worksheet_start_input (mws);
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

    cmd = moo_term_command_new_command_line (cmd_line, NULL, NULL);

    if (moo_term_pt_fork_command (pt, cmd, error))
    {
        g_signal_connect_swapped (pt, "child-died", G_CALLBACK (child_died), ws);
        ws->priv->pt = g_object_ref (pt);
    }

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
