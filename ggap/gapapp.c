/*
 *   gapapp.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "config.h"
#include "ggap-i18n.h"
#include "gapapp.h"
#include "gaptermwindow.h"
#include "gapwswindow.h"
#include "gapworksheet.h"
#include "gapeditwindow.h"
#include "gap.h"
#include "gapwswindow.h"
#include "gapprefs-glade.h"
#include "gapwswindow-ui.h"
#include "gaptermwindow-ui.h"
#include "ggap-credits.h"
#include "ggapfile.h"
#include <mooui/mdmanager.h>
#include <mooutils/moostock.h>
#include <mooutils/mooprefsdialog.h>
#include <mooutils/mooutils-misc.h>
#include <mooutils/mooprefs.h>
#include <mooutils/mooutils-fs.h>
#include <mooterm/mooterm-prefs.h>
#include <mooterm/mootermpt.h>
#include <mooedit/mooeditprefs.h>
#include <mooedit/mooplugin.h>
#include <mooedit/moousertools-prefs.h>
#include <string.h>
#include <stdlib.h>
#include <glib/gstdio.h>


struct GapAppPrivate {
    GtkWidget *window;
    MdManager *gd_mgr;
    MooEditor *editor;
    gboolean fancy;
};

static void     gap_app_initialize          (MooApp         *app);
static void     gap_app_start               (MooApp         *app);
static void     gap_app_add_options         (MooApp         *app,
                                             GOptionContext *ctx);
static void     gap_app_quit                (MooApp         *app);
    /* signal and a virtual method */
static MooAppQuitReply gap_app_quit_requested
                                            (MooApp         *app);

static void     gap_app_setup_prefs_dialog  (MooApp         *app,
                                             MooPrefsDialog *dialog);
static void     gap_app_open_files          (MooApp         *app,
                                             char          **files,
                                             MooAppOpenCtx  *ctx);

#ifdef __WIN32__
static void     open_gap_manual             (void);
#endif


G_DEFINE_TYPE (GapApp, gap_app, MOO_TYPE_APP)

static void
gap_app_class_init (GapAppClass *klass)
{
    MooAppClass *app_class = MOO_APP_CLASS (klass);

    app_class->initialize = gap_app_initialize;
    app_class->start = gap_app_start;
    app_class->add_options = gap_app_add_options;
    app_class->quit = gap_app_quit;
    app_class->quit_requested = gap_app_quit_requested;
//     app_class->open_files = gap_app_open_files;
    app_class->setup_prefs_dialog = gap_app_setup_prefs_dialog;

    g_type_class_add_private (klass, sizeof (GapAppPrivate));

#ifdef __WIN32__
    moo_window_class_new_action (g_type_class_peek (MOO_TYPE_WINDOW), "GapManual", NULL,
                                 "display-name", _("GAP Manual"),
                                 "label", _("GAP Manual"),
                                 "stock-id", GTK_STOCK_HELP,
                                 "accel", "F1",
                                 "closure-callback", open_gap_manual,
                                 NULL);
#endif
}


static void
gap_app_init (GapApp *app)
{
    app->priv = G_TYPE_INSTANCE_GET_PRIVATE (app, GAP_TYPE_APP, GapAppPrivate);

#ifdef __WIN32__
    moo_prefs_new_key_string (GGAP_PREFS_GAP_COMMAND, "c:\\gap4r4\\bin\\gapw95.exe -l c:\\gap4r4");
#else
    moo_prefs_new_key_string (GGAP_PREFS_GAP_COMMAND, "gap");
#endif
    moo_prefs_new_key_string (GGAP_PREFS_GAP_WORKING_DIR, NULL);
    moo_prefs_new_key_bool (GGAP_PREFS_GAP_SAVE_WORKSPACE, TRUE);
    moo_prefs_new_key_bool (GGAP_PREFS_GAP_CLEAR_TERMINAL, TRUE);

    g_object_set (app,
                  "short-name", "ggap",
                  "full-name", "GGAP",
                  "description", "GGAP is a front end for GAP",
                  "logo", "ggap",
                  "credits", THANKS,
                  "version", VERSION,
                  NULL);
}


static void
gap_app_initialize (MooApp *app)
{
    /* this should be before MooApp::initialize because that reads rc file */
    moo_prefs_new_key_string (moo_edit_setting (MOO_EDIT_PREFS_DEFAULT_LANG), "gap");

    MOO_APP_CLASS (gap_app_parent_class)->initialize (app);

#if defined(__WIN32__)
    {
        char *dir = moo_win32_get_app_dir ();
        moo_term_set_helper_directory (dir);
        g_free (dir);
    }
#endif /* __WIN32__ */
}


enum {
    FILE_WORKSHEET,
    FILE_TEXT
};

static char *
check_name_func (G_GNUC_UNUSED MooFileDialog *dialog,
                 const char    *uri,
                 gpointer       data)
{
    GtkComboBox *combo = data;

    if (gtk_combo_box_get_active (combo) == FILE_WORKSHEET &&
        !g_str_has_suffix (uri, ".gws"))
            return g_strdup_printf ("%s.gws", uri);
    else
        return g_strdup (uri);
}

static MdFileInfo *
run_save_dialog (G_GNUC_UNUSED MdManager *mgr,
                 MdDocument    *doc,
                 MooFileDialog *dialog)
{
    GapWorksheet *ws;
    MdFileInfo *ret;
    const char *uri;
    GtkWidget *hbox, *label, *combo;

    hbox = gtk_hbox_new (FALSE, 0);
    label = gtk_label_new ("File type:");
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
    combo = gtk_combo_box_new_text ();
    gtk_box_pack_start (GTK_BOX (hbox), combo, FALSE, FALSE, 0);
    gtk_widget_show_all (hbox);
    moo_file_dialog_set_extra_widget (dialog, hbox);
    moo_file_dialog_set_check_name_func (dialog, check_name_func, combo, NULL);

    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), "GGAP Worksheet (*.gws)");
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), "Text File");
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FILE_WORKSHEET);

    ws = GAP_WORKSHEET (doc);
    if (gap_worksheet_get_file_type (ws) == GAP_FILE_WORKSHEET)
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FILE_WORKSHEET);
    else
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FILE_TEXT);

    g_object_ref (combo);

    if (!moo_file_dialog_run (dialog))
    {
        g_object_unref (combo);
        return NULL;
    }

    uri = moo_file_dialog_get_uri (dialog);
    g_return_val_if_fail (uri != NULL, NULL);

    ret = md_file_info_new (uri);
    if (gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) == FILE_TEXT)
        gap_file_info_set_file_type (ret, GAP_FILE_TEXT);

    g_object_unref (combo);
    return ret;
}

static void
gap_app_start (MooApp *app)
{
    GapApp *ggap = GAP_APP (app);

    ggap->priv->editor = moo_editor_create_instance ();
    g_object_set (ggap->priv->editor, "allow-empty-window", TRUE, NULL);

    if (ggap->priv->fancy)
    {
        MooUIXML *xml;
        MooFilterMgr *filter_mgr;
        GSList *windows;

        ggap->priv->gd_mgr = g_object_new (MD_TYPE_MANAGER, "name", "GAP", NULL);
        md_manager_set_doc_type (ggap->priv->gd_mgr, GAP_TYPE_WORKSHEET);
        md_manager_set_window_type (ggap->priv->gd_mgr, GAP_TYPE_WS_WINDOW);
        g_signal_connect (ggap->priv->gd_mgr, "run-save-dialog",
                          G_CALLBACK (run_save_dialog), NULL);

        filter_mgr = md_manager_get_filter_mgr (ggap->priv->gd_mgr);
        moo_filter_mgr_new_builtin_filter (filter_mgr, "GAP Source Files (*.g,*.gd,*.gi)",
                                           "*.g;*.gd;*.gi", "GAP", -1);
        moo_filter_mgr_new_builtin_filter (filter_mgr, "GGAP Worksheets (*.gws)",
                                           "*.gws", "GAP", -1);

        xml = moo_ui_xml_new ();
        moo_ui_xml_add_ui_from_string (xml, gapwswindow_ui_xml, -1);
        md_manager_set_ui_xml (ggap->priv->gd_mgr, xml);
        g_object_unref (xml);

        windows = md_manager_list_windows (ggap->priv->gd_mgr);
        if (!windows)
            md_manager_ui_new_doc (ggap->priv->gd_mgr, NULL);
        g_slist_free (windows);

//         moo_app_set_document_manager (app, ggap->priv->gd_mgr);
    }
    else
    {
        MooUIXML *xml;

        xml = moo_ui_xml_new ();
        moo_ui_xml_add_ui_from_string (xml, gaptermwindow_ui_xml, -1);

        ggap->priv->window = g_object_new (GAP_TYPE_TERM_WINDOW,
                                           "ui-xml", xml, NULL);
        gtk_widget_show (ggap->priv->window);
//         moo_app_set_main_window (app, MOO_WINDOW (ggap->priv->window));

        g_object_unref (xml);
    }

    MOO_APP_CLASS (gap_app_parent_class)->start (app);
}


static void
gap_app_add_options (MooApp         *app,
                     GOptionContext *ctx)
{
    GapApp *ggap = GAP_APP (app);
    GOptionGroup *group;
    GOptionEntry entries[] = {
        { "fancy", 0, G_OPTION_FLAG_NO_ARG | G_OPTION_FLAG_IN_MAIN,
            G_OPTION_ARG_NONE, NULL, "Run in worksheet mode", NULL },
        { NULL }
    };

    entries[0].arg_data = &ggap->priv->fancy;

    group = g_option_group_new ("ggap", "", "", NULL, NULL);
    g_option_group_add_entries (group, entries);

    g_option_context_add_group (ctx, group);

    MOO_APP_CLASS (gap_app_parent_class)->add_options (app, ctx);
}


static void
extract_file (const char *filename)
{
    GError *error = NULL;
    char *text = NULL;
    gsize text_len = 0;
    char *workspace = NULL;
    GapFileType type;

    if (g_file_test ("worksheet.xml", G_FILE_TEST_EXISTS))
    {
        g_printerr ("File worksheet.xml already exists\n");
        exit (EXIT_FAILURE);
    }

    if (g_file_test ("workspace", G_FILE_TEST_EXISTS))
    {
        g_printerr ("File workspace already exists\n");
        exit (EXIT_FAILURE);
    }

    if (!ggap_file_load (filename, &type, &text, &text_len, &workspace, &error))
    {
        if (error)
            g_printerr ("%s\n", error->message);
        else
            g_printerr ("Failed\n");
        exit (EXIT_FAILURE);
    }

    if (type != GAP_FILE_WORKSHEET)
    {
        g_printerr ("A text file\n");
        exit (EXIT_FAILURE);
    }

    if (g_rename (workspace, "workspace") != 0)
    {
        perror ("rename");
        exit (EXIT_FAILURE);
    }

    if (!g_file_set_contents ("worksheet.xml", text, text_len, &error))
    {
        g_printerr ("%s\n", error->message);
        exit (EXIT_FAILURE);
    }
}

G_GNUC_NORETURN static void
extract_files (char **files)
{
    while (*files)
        extract_file (*files++);
    exit (EXIT_SUCCESS);
}


static MooAppQuitReply
gap_app_quit_requested (G_GNUC_UNUSED MooApp *app)
{
    return MOO_APP_QUIT_NOW;
}

static void
gap_app_quit (MooApp *app)
{
    GapApp *ggap = GAP_APP (app);

    if (ggap->priv->editor)
        g_object_unref (ggap->priv->editor);
    if (ggap->priv->gd_mgr)
        g_object_unref (ggap->priv->gd_mgr);

    MOO_APP_CLASS (gap_app_parent_class)->quit (app);
}


#ifdef __WIN32__
static void
open_gap_manual (void)
{
    const char *cmd_base;
    char *root_dir, *pdf;
    GapApp *app = GAP_APP_INSTANCE;

    cmd_base = moo_prefs_get_string (GGAP_PREFS_GAP_COMMAND);
    g_return_if_fail (cmd_base && cmd_base[0]);

    if (!gap_parse_cmd_line (cmd_base, NULL, &root_dir))
    {
        g_warning ("%s: could not parse command line `%s`",
                   G_STRLOC, cmd_base);
        return;
    }

    pdf = g_build_filename (root_dir, "doc", "ref", "manual.pdf", NULL);

    if (g_file_test (pdf, G_FILE_TEST_EXISTS))
        moo_open_file (pdf);
    else
        g_warning ("%s: could not find manual file, tried `%s`",
                   G_STRLOC, pdf);

    g_free (pdf);
    g_free (root_dir);
}
#endif


// static void
// new_editor_action (MooApp *app)
// {
//     MooEditWindow *window;
//
//     g_return_if_fail (GAP_IS_APP (app));
//
//     window = moo_editor_new_window (moo_app_get_editor (app));
//     gtk_window_present (GTK_WINDOW (window));
// }
//
// static void
// open_in_editor_action (GapTermWindow *terminal)
// {
//     MooApp *app = moo_app_get_instance ();
//     g_return_if_fail (app != NULL);
//     moo_editor_open (moo_app_get_editor (app), NULL,
//                      GTK_WIDGET (terminal), NULL);
// }


static void
remove_saved_workspace (void)
{
    char *wsp, *gzipped;

    wsp = gap_saved_workspace_filename ();
    g_return_if_fail (wsp != NULL);
    gzipped = g_strdup_printf ("%s.gz", wsp);

    _moo_unlink (wsp);
    _moo_unlink (gzipped);

    g_free (gzipped);
    g_free (wsp);
}

static void
prefs_page_apply (void)
{
    if (!moo_prefs_get_bool (GGAP_PREFS_GAP_SAVE_WORKSPACE))
        remove_saved_workspace ();
}

static GtkWidget *
gap_prefs_page_new (void)
{
    MooPrefsDialogPage *page;
    GtkWidget *button;

    page = moo_prefs_dialog_page_new_from_xml ("GAP", "gap", NULL,
                                               gapprefs_glade_xml,
                                               "page", GGAP_PREFS_PREFIX);

    button = moo_glade_xml_get_widget (page->xml, "clear_workspace");
    g_signal_connect (button, "clicked", G_CALLBACK (remove_saved_workspace), NULL);
    g_signal_connect (page, "apply", G_CALLBACK (prefs_page_apply), NULL);

    return GTK_WIDGET (page);
}


static void
gap_app_setup_prefs_dialog (MooApp         *app,
                            MooPrefsDialog *dialog)
{
    GapApp *ggap = GAP_APP (app);

    if (!ggap->priv->fancy)
    {
        moo_prefs_dialog_append_page (dialog, gap_prefs_page_new ());
        moo_prefs_dialog_append_page (dialog, moo_term_prefs_page_new ());
    }

    moo_prefs_dialog_append_page (dialog, moo_edit_prefs_page_new (ggap->priv->editor));
    moo_prefs_dialog_append_page (dialog, moo_user_tools_prefs_page_new ());
    moo_plugin_attach_prefs (GTK_WIDGET (dialog));
}


void
gap_app_send_and_bring_to_front (const char *string)
{
    GapApp *app = GAP_APP_INSTANCE;

    g_return_if_fail (string != NULL);
    g_return_if_fail (GAP_IS_APP (app));

    if (!app->priv->fancy)
    {
        gap_term_window_feed_gap (GAP_TERM_WINDOW (app->priv->window), string);
        moo_window_present (GTK_WINDOW (app->priv->window), 0);
    }
    else
    {
        g_warning ("%s: implement me", G_STRLOC);
    }
}
