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
#include "gapeditwindow.h"
#include "gap.h"
#include "gapwswindow.h"
#include "gapwsview.h"
#include "gapprefs-glade.h"
#include "gapwswindow-ui.h"
#include "gaptermwindow-ui.h"
#include "ggap-credits.h"
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


struct GapAppPrivate {
    GtkWidget *window;
    MdManager *gd_mgr;
    MooEditor *editor;
    gboolean fancy;
};

static void         gap_app_get_property            (GObject        *object,
                                                     guint           prop_id,
                                                     GValue         *value,
                                                     GParamSpec     *pspec);
static void         gap_app_set_property            (GObject        *object,
                                                     guint           prop_id,
                                                     const GValue   *value,
                                                     GParamSpec     *pspec);
static gboolean     gap_app_initialize              (MdApp          *app);
static int          gap_app_run                     (MdApp          *app);
static void         gap_app_quit                    (MdApp          *app);
static void         gap_app_setup_option_context    (MdApp          *app,
                                                     GOptionContext *ctx);
static void         gap_app_setup_prefs_dialog      (MdApp          *app,
                                                     GtkWidget      *dialog);

// static void         new_editor_action       (MooApp     *app);
// static void         open_in_editor_action   (GapTermWindow *term_window);

#ifdef __WIN32__
static void         open_gap_manual         (void);
#endif


G_DEFINE_TYPE (GapApp, gap_app, MD_TYPE_APP)

enum {
    PROP_0,
    PROP_FANCY
};

static void
gap_app_class_init (GapAppClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    MdAppClass *app_class = MD_APP_CLASS (klass);
//     MooWindowClass *edit_class, *term_class;

    gobject_class->set_property = gap_app_set_property;
    gobject_class->get_property = gap_app_get_property;

    app_class->initialize = gap_app_initialize;
    app_class->run = gap_app_run;
    app_class->quit = gap_app_quit;
    app_class->setup_option_context = gap_app_setup_option_context;
    app_class->setup_prefs_dialog = gap_app_setup_prefs_dialog;

    g_type_class_add_private (klass, sizeof (GapAppPrivate));

    g_object_class_install_property (gobject_class, PROP_FANCY,
        g_param_spec_boolean ("fancy", "fancy", "fancy",
                              FALSE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

//     edit_class = g_type_class_ref (GAP_TYPE_EDIT_WINDOW);
//     term_class = g_type_class_ref (GAP_TYPE_WINDOW);

//     md_app_window_class_new_action (edit_class, "ExecuteSelection", NULL,
//                                  "display-name", "Execute Selection",
//                                  "label", "_Execute Selection",
//                                  "tooltip", "Execute Selection",
//                                  "stock-id", GTK_STOCK_EXECUTE,
//                                  "accel", "<shift><alt>Return",
//                                  "closure-callback", execute_selection,
//                                  NULL);

#if 0
    md_app_window_class_new_action (g_type_class_peek (MOO_TYPE_WINDOW), "GapDoc", NULL,
                                 "display-name", _("GAP Documentation"),
                                 "label", _("_GAP Documentation"),
                                 "stock-id", GTK_STOCK_HELP,
                                 "closure-callback", gap_doc_window_show,
                                 NULL);
#endif

#ifdef __WIN32__
    md_app_window_class_new_action (g_type_class_peek (MOO_TYPE_WINDOW), "GapManual", NULL,
                                    "display-name", _("GAP Manual"),
                                    "label", _("GAP Manual"),
                                    "stock-id", GTK_STOCK_HELP,
                                    "accel", "F1",
                                    "closure-callback", open_gap_manual,
                                    NULL);
#endif

//     md_app_window_class_new_action (term_class, "NewEditor", NULL,
//                                  "display-name", "New Editor",
//                                  "label", "_New Editor",
//                                  "tooltip", "New Editor",
//                                  "stock-id", GTK_STOCK_EDIT,
//                                  "accel", "<Alt>E",
//                                  "closure-callback", new_editor_action,
//                                  "closure-proxy-func", moo_app_get_instance,
//                                  NULL);
//
//     md_app_window_class_new_action (term_class, "OpenInEditor", NULL,
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
    moo_prefs_new_key_string (GGAP_PREFS_GAP_WORKING_DIR, NULL);
    moo_prefs_new_key_bool (GGAP_PREFS_GAP_SAVE_WORKSPACE, TRUE);
    moo_prefs_new_key_bool (GGAP_PREFS_GAP_CLEAR_TERMINAL, TRUE);

    g_object_set (app,
                  "short-name", "ggap",
                  "full-name", "GGAP",
                  "description", "GGAP is a front end for GAP",
                  "icon-name", "ggap",
                  "credits", THANKS,
                  "authors-markup", "Yevgen Muntyan <a href=\"mailto://muntyan@tamu.edu\">"
                                    "&lt;muntyan@tamu.edu&gt;</a>",
                  "authors", "Yevgen Muntyan <muntyan@tamu.edu>",
                  "copyright", "\302\251 2004-2008 Yevgen Muntyan",
                  "version", VERSION,
                  NULL);
}


static gboolean
gap_app_initialize (MdApp *app)
{
//     MooEditor *editor;

    /* this should be before MdApp::initialize because that reads rc file */
    moo_prefs_new_key_string (moo_edit_setting (MOO_EDIT_PREFS_DEFAULT_LANG), "gap");

    if (!MD_APP_CLASS (gap_app_parent_class)->initialize (app))
        return FALSE;

#if defined(__WIN32__)
    {
        char *dir = moo_win32_get_app_dir ();
        moo_term_set_helper_directory (dir);
        g_free (dir);
    }
#endif /* __WIN32__ */

    return TRUE;
}


static void
gap_app_setup_option_context (MdApp          *md_app,
                              GOptionContext *ctx)
{
    GapApp *app = GAP_APP (md_app);
    GOptionGroup *group;
    GOptionEntry entries[] = {
        { "fancy", 0, G_OPTION_FLAG_NO_ARG | G_OPTION_FLAG_IN_MAIN,
            G_OPTION_ARG_NONE, NULL, "Run in worksheet mode", NULL },
        { NULL }
    };

    entries[0].arg_data = &app->priv->fancy;

    group = g_option_group_new ("ggap", "", "", NULL, NULL);
    g_option_group_add_entries (group, entries);

    g_option_context_add_group (ctx, group);

    MD_APP_CLASS (gap_app_parent_class)->setup_option_context (md_app, ctx);
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
                 MdView        *view,
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

    ws = GAP_WORKSHEET (md_view_get_doc (view));
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

static int
gap_app_run (MdApp *md_app)
{
    GapApp *app;

    app = GAP_APP (md_app);

    if (app->priv->fancy)
    {
        MooUIXML *xml;
        MooFilterMgr *filter_mgr;

        app->priv->gd_mgr = g_object_new (MD_TYPE_MANAGER, "name", "GAP", NULL);
        md_manager_set_doc_type (app->priv->gd_mgr, GAP_TYPE_WORKSHEET);
        md_manager_set_view_type (app->priv->gd_mgr, GAP_TYPE_WS_VIEW);
        md_manager_set_window_type (app->priv->gd_mgr, GAP_TYPE_WS_WINDOW);
        g_signal_connect (app->priv->gd_mgr, "run-save-dialog",
                          G_CALLBACK (run_save_dialog), NULL);

        filter_mgr = md_manager_get_filter_mgr (app->priv->gd_mgr);
        moo_filter_mgr_new_builtin_filter (filter_mgr, "GAP Source Files (*.g,*.gd,*.gi)",
                                           "*.g;*.gd;*.gi", "GAP", -1);
        moo_filter_mgr_new_builtin_filter (filter_mgr, "GGAP Worksheets (*.gws)",
                                           "*.gws", "GAP", -1);

        xml = moo_ui_xml_new ();
        moo_ui_xml_add_ui_from_string (xml, GAP_WS_WINDOW_UI, -1);
        md_manager_set_ui_xml (app->priv->gd_mgr, xml);
        g_object_unref (xml);

        md_app_set_document_manager (md_app, app->priv->gd_mgr);
    }
    else
    {
        MooUIXML *xml;

        xml = moo_ui_xml_new ();
        moo_ui_xml_add_ui_from_string (xml, GAP_TERM_WINDOW_UI, -1);

        app->priv->window = g_object_new (GAP_TYPE_TERM_WINDOW,
                                          "ui-xml", xml, NULL);
        gtk_widget_show (app->priv->window);
        md_app_set_main_window (md_app, MD_APP_WINDOW (app->priv->window));

        app->priv->editor = g_object_new (MOO_TYPE_EDITOR, NULL);
        md_manager_set_window_type (MD_MANAGER (app->priv->editor),
                                    GAP_TYPE_EDIT_WINDOW);
//         g_object_set (editor, "allow-empty-window", TRUE, NULL);
//         g_signal_connect_swapped (editor, "all-windows-closed",
//                                   G_CALLBACK (editor_windows_closed), app);

        g_object_unref (xml);
    }

    return MD_APP_CLASS (gap_app_parent_class)->run (md_app);
}


static void
gap_app_quit (MdApp *app)
{
    MD_APP_CLASS (gap_app_parent_class)->quit (app);
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

    wsp = gap_saved_workspace_filename (FALSE);
    g_return_if_fail (wsp != NULL);
    gzipped = g_strdup_printf ("%s.gz", wsp);

    if (g_file_test (wsp, G_FILE_TEST_EXISTS))
        _moo_unlink (wsp);
    if (g_file_test (gzipped, G_FILE_TEST_EXISTS))
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
                                               GAP_PREFS_GLADE_UI,
                                               "page", GGAP_PREFS_PREFIX);

    button = moo_glade_xml_get_widget (page->xml, "clear_workspace");
    g_signal_connect (button, "clicked", G_CALLBACK (remove_saved_workspace), NULL);
    g_signal_connect (page, "apply", G_CALLBACK (prefs_page_apply), NULL);

    return GTK_WIDGET (page);
}


static void
gap_app_setup_prefs_dialog (MdApp     *mdapp,
                            GtkWidget *dialog)
{
    GapApp *app = GAP_APP (mdapp);

    if (app->priv->fancy)
    {
    }
    else
    {
        moo_prefs_dialog_append_page (MOO_PREFS_DIALOG (dialog), gap_prefs_page_new ());
        moo_prefs_dialog_append_page (MOO_PREFS_DIALOG (dialog), moo_term_prefs_page_new ());

        moo_prefs_dialog_append_page (MOO_PREFS_DIALOG (dialog),
                                      moo_edit_prefs_page_new (app->priv->editor));
        moo_prefs_dialog_append_page (MOO_PREFS_DIALOG (dialog), moo_user_tools_prefs_page_new ());
        moo_plugin_attach_prefs (GTK_WIDGET (dialog));
    }
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
