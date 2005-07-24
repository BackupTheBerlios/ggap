/*
 *   @(#)$Id: gapprefsdialog.c,v 1.5 2005/06/06 14:45:55 emuntyan Exp $
 *
 *   Copyright (C) 2004-2005 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "mooapp/mooapp.h"
#include "gap/gapprefsdialog.h"
#include "gap/gapprefs.h"
#include "gap/gapapp.h"
#include "gap/gaptermwindow.h"
#include "mooutils/moostock.h"
#include "mooutils/moodialogs.h"
#include "mooutils/mooprefsdialog.h"
#include "mooedit/mooeditprefs.h"
#include "mooterm/mooterm-prefs.h"


GtkWidget *_ggap_create_gap_prefs_page (GtkWidget *vbox);


static GtkWidget *gap_prefs_page_new (void);
static void hookup_prefs (MooPrefsDialogPage *page);
static const char *browse_dialog (void *data, GtkWidget *parent);


void gap_prefs_dialog (GtkWidget *parent)
{
    MooPrefsDialog *dialog = MOO_PREFS_DIALOG (moo_prefs_dialog_new ("GGAP Preferences"));
    moo_prefs_dialog_append_page (dialog, gap_prefs_page_new ());
    moo_prefs_dialog_append_page (dialog, moo_term_prefs_page_new ());
    moo_prefs_dialog_append_page (dialog, moo_edit_prefs_page_new (moo_app_get_editor (moo_app_get_instance())));

    g_signal_connect_swapped (dialog, "apply", G_CALLBACK (gap_term_window_apply_settings),
                              moo_app_get_term_window (moo_app_get_instance ()));

    moo_prefs_dialog_run (dialog, parent);
}


static GtkWidget *gap_prefs_page_new (void)
{
    GtkWidget *page = moo_prefs_dialog_page_new ("GAP", MOO_STOCK_GAP);
    _ggap_create_gap_prefs_page (page);
    hookup_prefs (MOO_PREFS_DIALOG_PAGE (page));
    return page;
}


#ifndef G_DISABLE_ASSERT
#define GETWID(name) \
    GtkWidget *name = GTK_WIDGET (g_object_get_data (gobj, #name)); g_assert (name != NULL)
#else /* G_DISABLE_ASSERT */
#define GETWID(name) \
    GtkWidget *name = GTK_WIDGET (g_object_get_data (gobj, #name))
#endif /* G_DISABLE_ASSERT */

#define BIND(name,setting_name) \
    moo_prefs_dialog_page_bind_setting (page, name, setting_name)

enum {
    COMMAND,
    WORKING_DIR,
    WORKSPACE
};

static void hookup_prefs (MooPrefsDialogPage *page)
{
    GObject *gobj = G_OBJECT (page);

    GETWID (command);
    GETWID (command_browse);
    GETWID (working_dir);
    GETWID (working_dir_browse);
    GETWID (load_workspace_browse);
    GETWID (load_workspace_entry);

    moo_bind_button (GTK_BUTTON (command_browse),
                     GTK_ENTRY (command),
                     browse_dialog, moo_quote_text,
                     GINT_TO_POINTER (COMMAND));

    moo_bind_button (GTK_BUTTON (working_dir_browse),
                     GTK_ENTRY (working_dir),
                     browse_dialog, NULL,
                     GINT_TO_POINTER (WORKING_DIR));

    moo_bind_button (GTK_BUTTON (load_workspace_browse),
                     GTK_ENTRY (load_workspace_entry),
                     browse_dialog, moo_quote_text,
                     GINT_TO_POINTER (WORKSPACE));
}


static const char *browse_dialog (void *data, GtkWidget*parent)
{
    switch (GPOINTER_TO_INT (data)) {
    case COMMAND:
        return moo_file_dialogp (parent,
                                 MOO_DIALOG_FILE_OPEN_EXISTING,
                                 "Choose GAP Executable",
                                 GAP_PREFS_DIALOGS_GAP_EXE, NULL);
    case WORKING_DIR:
        return moo_file_dialogp (parent,
                                 MOO_DIALOG_DIR_OPEN,
                                 "Choose GAP Working Directory",
                                 GAP_PREFS_DIALOGS_GAP_WORKING_DIR, NULL);
    case WORKSPACE:
        return moo_file_dialogp (parent,
                                 MOO_DIALOG_FILE_OPEN_ANY,
                                 "Choose Saved Workspace File",
                                 GAP_PREFS_DIALOGS_LOAD_WORKSPACE, NULL);
    }
    return NULL; /* to avoid warnings */
}
