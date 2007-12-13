/*
 *   gapapp.c
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

#include "config.h"
#include "ggap-i18n.h"
#include "gapapp.h"
#include "gaptermwindow.h"
#include "gapeditwindow.h"
#include "gap.h"
#include "gapwswindow.h"
#include "gapworksheet.h"
#include "gapprefs-glade.h"
#include "gapoutput.h"
#include "gapwswindow-ui.h"
#include <mooui/mdmanager.h>
#include <mooutils/moostock.h>
#include <mooutils/mooprefsdialog.h>
#include <mooutils/mooutils-misc.h>
#include <mooutils/mooutils-fs.h>
#include <mooterm/mooterm-prefs.h>
#include <mooterm/mootermpt.h>
#include <mooedit/mooeditprefs.h>
#include <mooedit/mooplugin.h>
#include <mooedit/moousertools-prefs.h>
#include <string.h>


struct GapAppPrivate {
//     GapView *terminal;
//     GapWindow *gap_window;
    GtkWidget *window;
    MdManager *gd_mgr;
    gboolean fancy;
};

static void         gap_app_get_property    (GObject    *object,
                                             guint       prop_id,
                                             GValue     *value,
                                             GParamSpec *pspec);
static void         gap_app_set_property    (GObject    *object,
                                             guint       prop_id,
                                             const GValue *value,
                                             GParamSpec *pspec);
static gboolean     gap_app_init_real       (MooApp     *app);
static int          gap_app_run             (MooApp     *app);
static void         gap_app_quit            (MooApp     *app);
static gboolean     gap_app_try_quit        (MooApp     *app);
static GtkWidget   *gap_app_prefs_dialog    (MooApp     *app);

// static void         new_editor_action       (MooApp     *app);
// static void         open_in_editor_action   (GapTermWindow *term_window);

#ifdef __WIN32__
static void         open_gap_manual         (void);
#endif


G_DEFINE_TYPE(GapApp, gap_app, MOO_TYPE_APP)

enum {
    PROP_0,
    PROP_FANCY
};

static void
gap_app_class_init (GapAppClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    MooAppClass *app_class = MOO_APP_CLASS (klass);
//     MooWindowClass *edit_class, *term_class;

    app_class->init = gap_app_init_real;
    app_class->run = gap_app_run;
    app_class->quit = gap_app_quit;
    app_class->try_quit = gap_app_try_quit;
    app_class->prefs_dialog = gap_app_prefs_dialog;

    gobject_class->set_property = gap_app_set_property;
    gobject_class->get_property = gap_app_get_property;

    g_type_class_add_private (klass, sizeof (GapAppPrivate));

    g_object_class_install_property (gobject_class, PROP_FANCY,
        g_param_spec_boolean ("fancy", "fancy", "fancy",
                              FALSE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

//     edit_class = g_type_class_ref (GAP_TYPE_EDIT_WINDOW);
//     term_class = g_type_class_ref (GAP_TYPE_WINDOW);

//     moo_window_class_new_action (edit_class, "ExecuteSelection", NULL,
//                                  "display-name", "Execute Selection",
//                                  "label", "_Execute Selection",
//                                  "tooltip", "Execute Selection",
//                                  "stock-id", GTK_STOCK_EXECUTE,
//                                  "accel", "<shift><alt>Return",
//                                  "closure-callback", execute_selection,
//                                  NULL);

#if 0
    moo_window_class_new_action (g_type_class_peek (MOO_TYPE_WINDOW), "GapDoc", NULL,
                                 "display-name", _("GAP Documentation"),
                                 "label", _("_GAP Documentation"),
                                 "stock-id", GTK_STOCK_HELP,
                                 "closure-callback", gap_doc_window_show,
                                 NULL);
#endif

#ifdef __WIN32__
    moo_window_class_new_action (g_type_class_peek (MOO_TYPE_WINDOW), "GapManual", NULL,
                                 "display-name", _("GAP Manual"),
                                 "label", _("GAP Manual"),
                                 "stock-id", GTK_STOCK_HELP,
                                 "accel", "F1",
                                 "closure-callback", open_gap_manual,
                                 NULL);
#endif

//     moo_window_class_new_action (term_class, "NewEditor", NULL,
//                                  "display-name", "New Editor",
//                                  "label", "_New Editor",
//                                  "tooltip", "New Editor",
//                                  "stock-id", GTK_STOCK_EDIT,
//                                  "accel", "<Alt>E",
//                                  "closure-callback", new_editor_action,
//                                  "closure-proxy-func", moo_app_get_instance,
//                                  NULL);
//
//     moo_window_class_new_action (term_class, "OpenInEditor", NULL,
//                                  "display-name", "Open In Editor",
//                                  "label", "_Open In Editor",
//                                  "tooltip", "Open In Editor",
//                                  "stock-id", GTK_STOCK_OPEN,
//                                  "accel", "<Alt>O",
//                                  "closure-callback", open_in_editor_action,
//                                  NULL);

//     g_type_class_unref (edit_class);
//     g_type_class_unref (term_class);
}


static void
gap_app_get_property (GObject    *object,
                      guint       prop_id,
                      G_GNUC_UNUSED GValue *value,
                      GParamSpec *pspec)
{
    switch (prop_id)
    {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}


static void
gap_app_set_property (GObject    *object,
                      guint       prop_id,
                      const GValue *value,
                      GParamSpec *pspec)
{
    GapApp *app = GAP_APP (object);

    switch (prop_id)
    {
        case PROP_FANCY:
            app->priv->fancy = g_value_get_boolean (value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
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
    moo_prefs_new_key_bool (GGAP_PREFS_GAP_INIT_PKG, TRUE); /* XXX */
    moo_prefs_new_key_string (GGAP_PREFS_GAP_WORKING_DIR, NULL);
    moo_prefs_new_key_bool (GGAP_PREFS_GAP_SAVE_WORKSPACE, TRUE);
    moo_prefs_new_key_bool (GGAP_PREFS_GAP_CLEAR_TERMINAL, TRUE);
}


static gboolean
gap_app_init_real (MooApp *app)
{
    MooEditor *editor;

    /* this should be before MooApp::init since that reads rc file */
    moo_prefs_new_key_string (moo_edit_setting (MOO_EDIT_PREFS_DEFAULT_LANG), "gap");

    if (!MOO_APP_CLASS(gap_app_parent_class)->init (app))
        return FALSE;

#if defined(__WIN32__)
    {
        char *dir = moo_win32_get_app_dir ();
        moo_term_set_helper_directory (dir);
        g_free (dir);
    }
#endif /* __WIN32__ */

    editor = moo_app_get_editor (app);
    moo_editor_set_window_type (editor, GAP_TYPE_EDIT_WINDOW);
//     g_object_set (editor, "allow-empty-window", TRUE, NULL);
//     g_signal_connect_swapped (editor, "all-windows-closed",
//                               G_CALLBACK (editor_windows_closed), app);

    return TRUE;
}


static gboolean
gap_window_close (GapApp *app)
{
    g_return_val_if_fail (GAP_IS_APP (app), FALSE);
    return !moo_app_quit (MOO_APP (app));
}


static int
gap_app_run (MooApp *mapp)
{
    GapApp *app;

    app = GAP_APP (mapp);
    gap_app_output_start ();

    if (app->priv->fancy)
    {
        MooUIXML *xml;

        app->priv->gd_mgr = g_object_new (MD_TYPE_MANAGER, NULL);
        md_manager_set_doc_type (app->priv->gd_mgr, GAP_TYPE_WORKSHEET);
        md_manager_set_window_type (app->priv->gd_mgr, GAP_TYPE_WS_WINDOW);

        xml = moo_ui_xml_new ();
        moo_ui_xml_add_ui_from_string (xml, GAP_WS_WINDOW_UI, -1);
        md_manager_set_ui_xml (app->priv->gd_mgr, xml);
        g_object_unref (xml);

        _md_manager_action_new_window (app->priv->gd_mgr);
    }
    else
    {
        app->priv->window = g_object_new (GAP_TYPE_TERM_WINDOW, "ui-xml",
                                          moo_app_get_ui_xml (MOO_APP (app)),
                                          NULL);
        gtk_widget_show (app->priv->window);
        g_signal_connect_swapped (app->priv->window, "close",
                                  G_CALLBACK (gap_window_close), app);
    }

    return MOO_APP_CLASS(gap_app_parent_class)->run (mapp);
}


static void
gap_app_quit (MooApp *app)
{
    gap_app_output_shutdown ();

    if (GAP_APP (app)->priv->window)
        moo_window_close (MOO_WINDOW (GAP_APP (app)->priv->window));

    MOO_APP_CLASS(gap_app_parent_class)->quit (app);
}


static gboolean
gap_app_try_quit (MooApp *app)
{
    if (MOO_APP_CLASS(gap_app_parent_class)->try_quit (app))
        return TRUE;

    if (GAP_APP(app)->priv->window && !moo_window_close (MOO_WINDOW (GAP_APP(app)->priv->window)))
        return TRUE;

    return FALSE;
}


#ifdef __WIN32__
static void
open_gap_manual (void)
{
    const char *cmd_base;
    char *root_dir, *pdf;
    GapApp *app = GAP_APP_INSTANCE;

    if (app->gap_cmd_line)
        cmd_base = app->gap_cmd_line;
    else
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

    if (g_file_test (wsp, G_FILE_TEST_EXISTS))
        _moo_unlink (wsp);
    if (g_file_test (gzipped, G_FILE_TEST_EXISTS))
        _moo_unlink (gzipped);

    g_free (wsp);
    g_free (gzipped);
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
                                               GAP_PREFS_GLADE_UI,
                                               "page", GGAP_PREFS_PREFIX);

    button = moo_glade_xml_get_widget (page->xml, "clear_workspace");
    g_signal_connect (button, "clicked", G_CALLBACK (remove_saved_workspace), NULL);
    g_signal_connect (page, "apply", G_CALLBACK (prefs_page_apply), NULL);

    return GTK_WIDGET (page);
}


static GtkWidget*
gap_app_prefs_dialog (MooApp     *app)
{
    char *title;
    const MooAppInfo *info;
    MooPrefsDialog *dialog;

    info = moo_app_get_info (app);
    title = g_strdup_printf ("%s Preferences", info->full_name);
    dialog = MOO_PREFS_DIALOG (moo_prefs_dialog_new (title));
    g_free (title);

    moo_prefs_dialog_append_page (dialog, gap_prefs_page_new ());
    moo_prefs_dialog_append_page (dialog, moo_term_prefs_page_new ());

    moo_prefs_dialog_append_page (dialog, moo_edit_prefs_page_new (moo_app_get_editor (app)));
    moo_prefs_dialog_append_page (dialog, moo_user_tools_prefs_page_new ());
    moo_plugin_attach_prefs (GTK_WIDGET (dialog));

    return GTK_WIDGET (dialog);
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
