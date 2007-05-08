/*
 *   gaptermwindow.c
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

#include "gapapp.h"
#include "gap.h"
#include "mooutils/moofiledialog.h"
#include "mooapp/mooapp.h"
#include <gtk/gtk.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


static void     switch_to_editor            (void);
static void     gap_read_file               (GapTermWindow      *window);
static void     gap_open_workspace          (GapTermWindow      *window);


/* GAP_TYPE_TERM_WINDOW */
G_DEFINE_TYPE (GapTermWindow, gap_term_window, MOO_TYPE_TERM_WINDOW)


static void
gap_term_window_class_init (GapTermWindowClass *klass)
{
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);

    moo_window_class_set_id (window_class, "Terminal", "Terminal");

    moo_window_class_new_action (window_class, "SwitchToEditor", NULL,
                                 "display-name", "Switch to Editor",
                                 "label", "Switch to Editor",
                                 "tooltip", "Switch to Editor",
                                 "stock-id", GTK_STOCK_EDIT,
                                 "closure-callback", switch_to_editor,
                                 NULL);

    moo_window_class_new_action (window_class, "GAPRead", NULL,
                                 "display-name", "Read File",
                                 "label", "Read File",
                                 "tooltip", "Read File",
                                 "stock-id", GTK_STOCK_OPEN,
                                 "closure-callback", gap_read_file,
                                 NULL);
    moo_window_class_new_action (window_class, "GAPOpenWorkspace", NULL,
                                 "display-name", "Open Workspace",
                                 "label", "Open _Workspace",
                                 "stock-id", GTK_STOCK_OPEN,
                                 "closure-callback", gap_open_workspace,
                                 NULL);
}


static void
gap_term_window_init (GapTermWindow *window)
{
    moo_term_window_set_term_type (MOO_TERM_WINDOW (window), GAP_TYPE_TERM);
    moo_window_set_global_accels (MOO_WINDOW (window), TRUE);
}


static void
switch_to_editor (void)
{
    MooApp *app = moo_app_get_instance ();
    MooEditor *editor = moo_app_get_editor (app);
    moo_editor_present (editor, 0);
}


static void
gap_read_file (GapTermWindow *window)
{
    const char *file;
    char *string;

    file = moo_file_dialogp (GTK_WIDGET (window),
                             MOO_FILE_DIALOG_OPEN,
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
gap_open_workspace (GapTermWindow *window)
{
    const char *file;

    file = moo_file_dialogp (GTK_WIDGET (window),
                             MOO_FILE_DIALOG_OPEN,
                             "Choose Saved Workspace File",
                             GGAP_PREFS_PREFIX "/open_workspace",
                             NULL);

    if (!file)
        return;

    gap_app_open_workspace (GAP_APP_INSTANCE, file);
}
