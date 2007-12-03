/*
 *   gapwindow.c
 *
 *   Copyright (C) 2004-2006 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "ggap-i18n.h"
#include "gapwindow.h"
#include "gapapp.h"
#include "gap.h"
#include "mooutils/moofiledialog.h"
#include "mooutils/moocompat.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/moostock.h"
#include "mooapp/mooapp.h"
#include <gtk/gtk.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


struct _GapWindowPrivate {
    GType view_type;
};


static void     switch_to_editor        (void);
static void     action_read_file        (GapWindow      *window);
static void     action_open_workspace   (GapWindow      *window);
static void     action_restart_gap      (GapWindow      *window);
static void     action_send_intr        (GapWindow      *window);


G_DEFINE_TYPE (GapWindow, gap_window, MOO_TYPE_WINDOW)


static void
gap_window_class_init (GapWindowClass *klass)
{
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);

    moo_window_class_set_id (window_class, "GAP", "GAP");

    moo_window_class_new_action (window_class, "SwitchToEditor", NULL,
                                 "display-name", _("Switch to Editor"),
                                 "label", _("Switch to Editor"),
                                 "tooltip", _("Switch to editor"),
                                 "stock-id", GTK_STOCK_EDIT,
                                 "closure-callback", switch_to_editor,
                                 NULL);

    moo_window_class_new_action (window_class, "GAPRead", NULL,
                                 "display-name", _("Read File"),
                                 "label", _("Read File"),
                                 "tooltip", _("Read file"),
                                 "stock-id", GTK_STOCK_OPEN,
                                 "closure-callback", action_read_file,
                                 NULL);
    moo_window_class_new_action (window_class, "GAPOpenWorkspace", NULL,
                                 "display-name", _("Open Workspace"),
                                 "label", _("Open _Workspace"),
                                 "tooltip", _("Open workspace"),
                                 "stock-id", GTK_STOCK_OPEN,
                                 "closure-callback", action_open_workspace,
                                 NULL);

    moo_window_class_new_action (window_class, "Restart", NULL,
                                 "display-name", _("Restart"),
                                 "label", _("_Restart"),
                                 "tooltip", _("Restart GAP"),
                                 "stock-id", MOO_STOCK_RESTART,
                                 "accel", "<alt>R",
                                 "closure-callback", action_restart_gap,
                                 NULL);

    moo_window_class_new_action (window_class, "Interrupt", NULL,
                                 "display-name", _("Interrupt"),
                                 "label", _("_Interrupt"),
                                 "tooltip", _("Interrupt computation"),
                                 "stock-id", GTK_STOCK_STOP,
                                 "closure-callback", action_send_intr,
                                 NULL);
}


static void
gap_window_init (G_GNUC_UNUSED GapWindow *window)
{
}


// static gboolean
// apply_prefs (GapWindow *window)
// {
//     window->priv->apply_prefs_idle = 0;
//     moo_term_apply_settings (window->terminal);
//     return FALSE;
// }
//
// static void
// prefs_notify (G_GNUC_UNUSED const char *key,
//               G_GNUC_UNUSED const GValue *newval,
//               GapWindow *window)
// {
//     g_return_if_fail (GAP_IS_TERM_WINDOW (window));
//     if (!window->priv->apply_prefs_idle)
//         window->priv->apply_prefs_idle =
//                 _moo_idle_add ((GSourceFunc) apply_prefs, window);
// }


GapView *
gap_window_get_terminal (GapWindow *window)
{
    g_return_val_if_fail (GAP_IS_WINDOW (window), NULL);
    g_return_val_if_fail (GAP_WINDOW_GET_CLASS (window)->get_terminal != NULL, NULL);
    return GAP_WINDOW_GET_CLASS (window)->get_terminal (window);
}


// static void
// copy_clipboard (MooTerm *term)
// {
//     moo_term_copy_clipboard (term, GDK_SELECTION_CLIPBOARD);
// }
//
//
// static void
// paste_clipboard (MooTerm *term)
// {
//     moo_term_paste_clipboard (term, GDK_SELECTION_CLIPBOARD);
// }


static void
switch_to_editor (void)
{
    MooApp *app = moo_app_get_instance ();
    MooEditor *editor = moo_app_get_editor (app);
    moo_editor_present (editor, 0);
}


static void
action_read_file (GapWindow *window)
{
    const char *file;
    char *string;

    file = moo_file_dialogp (GTK_WIDGET (window),
                             MOO_FILE_DIALOG_OPEN,
                             NULL,
                             "Read File",
                             GGAP_PREFS_PREFIX "/read_file",
                             NULL);

    if (!file)
        return;

    string = gap_read_file_string (file);
    gap_app_feed_gap (GAP_APP_INSTANCE, string);
    g_free (string);
}


static void
action_open_workspace (GapWindow *window)
{
    const char *file;

    file = moo_file_dialogp (GTK_WIDGET (window),
                             MOO_FILE_DIALOG_OPEN,
                             NULL,
                             "Choose Saved Workspace File",
                             GGAP_PREFS_PREFIX "/open_workspace",
                             NULL);

    if (!file)
        return;

    gap_app_open_workspace (GAP_APP_INSTANCE, file);
}

static void
gap_window_start_gap (GapWindow *window)
{
    GapView *view = gap_window_get_terminal (window);
    g_return_if_fail (!gap_view_child_alive (view));
    gap_view_start_gap (view, NULL);
}

static void
gap_window_stop_gap (GapWindow *window)
{
    GapView *view = gap_window_get_terminal (window);
    gap_view_stop_gap (view);
}

static void
action_restart_gap (GapWindow *window)
{
    gap_window_stop_gap (window);
    g_usleep (100000);
    gap_window_start_gap (window);
}

static void
action_send_intr (GapWindow *window)
{
    GapView *view = gap_window_get_terminal (window);
    if (gap_view_child_alive (view))
        gap_view_send_intr (view);
}
