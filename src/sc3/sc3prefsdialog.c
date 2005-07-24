/*
 *   @(#)$Id: sc3prefsdialog.c,v 1.6 2005/06/06 14:45:55 emuntyan Exp $
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

#include "sc3/sc3prefsdialog.h"
#include "sc3/sc3prefs.h"
#include "sc3/sc3app.h"
#include "sc3/sc3termwindow.h"
#include "mooutils/moostock.h"
#include "mooutils/moodialogs.h"
#include "mooutils/mooprefsdialog.h"
#include "mooedit/mooeditprefs.h"
#include "mooterm/mootermprefs.h"


GtkWidget *_ggap_create_sc3_prefs_page (GtkWidget *vbox);


static GtkWidget *sc3_prefs_page_new (void);
static void hookup_prefs (MooPrefsDialogPage *page);
static const char *browse_dialog (void *data, GtkWidget *parent);


void sc3_prefs_dialog (GtkWidget *parent)
{
    MooPrefsDialog *dialog = MOO_PREFS_DIALOG (moo_prefs_dialog_new ("Sclang Preferences"));
    moo_prefs_dialog_append_page (dialog, sc3_prefs_page_new ());
    moo_prefs_dialog_append_page (dialog, moo_term_prefs_page_new ());
    moo_prefs_dialog_append_page (dialog, moo_edit_prefs_page_new (moo_app_get_editor (moo_app_get_instance())));

    g_signal_connect_swapped (dialog, "apply", G_CALLBACK (sc3_term_window_apply_settings),
                              moo_app_get_term_window (moo_app_get_instance ()));

    moo_prefs_dialog_run (dialog, parent);
}


static GtkWidget *sc3_prefs_page_new (void)
{
    GtkWidget *page = moo_prefs_dialog_page_new ("Sclang", MOO_STOCK_GAP);
    _ggap_create_sc3_prefs_page (page);
    hookup_prefs (MOO_PREFS_DIALOG_PAGE (page));
    return page;
}


#ifndef G_DISABLE_ASSERT
#define GETWID(name) \
    GtkWidget *name = GTK_WIDGET (g_object_get_data (gobj, #name)); g_assert (name != NULL)
#else /* G_DISABLE_ASSERT */
#define GETWID(name) \
    GtkWidget *name = GTK_WIDGET (g_object_get_data (gobj, #name));
#endif /* G_DISABLE_ASSERT */

#define BIND(name,setting_name) \
    moo_prefs_dialog_page_bind_setting (page, name, setting_name)

enum {
    COMMAND,
    WORKING_DIR,
    HELP_DIR
};

static void hookup_prefs (MooPrefsDialogPage *page)
{
    GObject *gobj = G_OBJECT (page);

    GETWID (command);
    GETWID (command_browse);
    GETWID (working_dir);
    GETWID (working_dir_browse);
    GETWID (help_dir);
    GETWID (help_dir_browse);

    moo_bind_button (GTK_BUTTON (command_browse),
                     GTK_ENTRY (command),
                     browse_dialog, moo_quote_text,
                     GINT_TO_POINTER (COMMAND));

    moo_bind_button (GTK_BUTTON (working_dir_browse),
                     GTK_ENTRY (working_dir),
                     browse_dialog, NULL,
                     GINT_TO_POINTER (WORKING_DIR));

    moo_bind_button (GTK_BUTTON (help_dir_browse),
                     GTK_ENTRY (help_dir),
                     browse_dialog, NULL,
                     GINT_TO_POINTER (HELP_DIR));
}


static const char *browse_dialog (void *data, GtkWidget*parent)
{
    switch (GPOINTER_TO_INT (data)) {
        case COMMAND:
            return moo_file_dialogp (parent,
                                     MOO_DIALOG_FILE_OPEN_EXISTING,
                                     "Choose Sclang Executable",
                                     SC3_PREFS_DIALOGS_SCLANG_EXE, NULL);
        case WORKING_DIR:
            return moo_file_dialogp (parent,
                                     MOO_DIALOG_DIR_OPEN,
                                     "Choose SC3 Working Directory",
                                     SC3_PREFS_DIALOGS_SCLANG_WORKING_DIR, NULL);
        case HELP_DIR:
            return moo_file_dialogp (parent,
                                     MOO_DIALOG_DIR_OPEN,
                                     "Choose Help Directory",
                                     SC3_PREFS_DIALOGS_HELP_DIR, NULL);
    }
    return NULL; /* to avoid warning */
}
