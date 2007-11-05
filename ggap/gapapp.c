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
#include "gapprefs-glade.h"
#include "gapoutput.h"
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


gboolean GAP_APP_EDITOR_MODE;


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

static void         new_editor_action       (MooApp     *app);
static void         open_in_editor_action   (GapTermWindow *term_window);

#ifdef __WIN32__
static void         open_gap_manual         (void);
#endif


G_DEFINE_TYPE(GapApp, gap_app, MOO_TYPE_APP)

enum {
    PROP_0,
    PROP_GAP_CMD_LINE,
    PROP_EDITOR_MODE,
    PROP_FANCY
};

static void
gap_app_class_init (GapAppClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    MooAppClass *app_class = MOO_APP_CLASS (klass);
    MooWindowClass *edit_class, *term_class;

    app_class->init = gap_app_init_real;
    app_class->run = gap_app_run;
    app_class->quit = gap_app_quit;
    app_class->try_quit = gap_app_try_quit;
    app_class->prefs_dialog = gap_app_prefs_dialog;

    gobject_class->set_property = gap_app_set_property;
    gobject_class->get_property = gap_app_get_property;

    g_object_class_install_property (gobject_class,
                                     PROP_GAP_CMD_LINE,
                                     g_param_spec_string ("gap-cmd-line",
                                             "gap-cmd-line",
                                             "gap-cmd-line",
                                             NULL,
                                             G_PARAM_WRITABLE));

    g_object_class_install_property (gobject_class,
                                     PROP_EDITOR_MODE,
                                     g_param_spec_boolean ("editor-mode",
                                             "editor-mode",
                                             "editor-mode",
                                             FALSE,
                                             G_PARAM_WRITABLE));

    g_object_class_install_property (gobject_class,
                                     PROP_FANCY,
                                     g_param_spec_boolean ("fancy",
                                             "fancy",
                                             "fancy",
                                             FALSE,
                                             G_PARAM_WRITABLE));

    if (GAP_APP_EDITOR_MODE)
        return;

    edit_class = g_type_class_ref (GAP_TYPE_EDIT_WINDOW);
    term_class = g_type_class_ref (GAP_TYPE_WINDOW);

//     moo_window_class_new_action (edit_class, "ExecuteSelection", NULL,
//                                  "display-name", "Execute Selection",
//                                  "label", "_Execute Selection",
//                                  "tooltip", "Execute Selection",
//                                  "stock-id", GTK_STOCK_EXECUTE,
//                                  "accel", "<shift><alt>Return",
//                                  "closure-callback", execute_selection,
//                                  NULL);

    moo_window_class_new_action (term_class, "Restart", NULL,
                                 "display-name", _("Restart"),
                                 "label", _("_Restart"),
                                 "tooltip", _("Restart GAP"),
                                 "stock-id", MOO_STOCK_RESTART,
                                 "accel", "<alt>R",
                                 "closure-callback", gap_app_restart_gap,
                                 "closure-proxy-func", moo_app_get_instance,
                                 NULL);

    moo_window_class_new_action (term_class, "Interrupt", NULL,
                                 "display-name", _("Interrupt"),
                                 "label", _("_Interrupt"),
                                 "tooltip", _("Interrupt computation"),
                                 "stock-id", GTK_STOCK_STOP,
                                 "closure-callback", gap_app_send_intr,
                                 "closure-proxy-func", moo_app_get_instance,
                                 NULL);

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

    moo_window_class_new_action (term_class, "NewEditor", NULL,
                                 "display-name", "New Editor",
                                 "label", "_New Editor",
                                 "tooltip", "New Editor",
                                 "stock-id", GTK_STOCK_EDIT,
                                 "accel", "<Alt>E",
                                 "closure-callback", new_editor_action,
                                 "closure-proxy-func", moo_app_get_instance,
                                 NULL);

    moo_window_class_new_action (term_class, "OpenInEditor", NULL,
                                 "display-name", "Open In Editor",
                                 "label", "_Open In Editor",
                                 "tooltip", "Open In Editor",
                                 "stock-id", GTK_STOCK_OPEN,
                                 "accel", "<Alt>O",
                                 "closure-callback", open_in_editor_action,
                                 NULL);

    g_type_class_unref (edit_class);
    g_type_class_unref (term_class);
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
        case PROP_GAP_CMD_LINE:
            g_free (app->gap_cmd_line);
            app->gap_cmd_line = g_strdup (g_value_get_string (value));
            break;

        case PROP_EDITOR_MODE:
            app->editor_mode = g_value_get_boolean (value);
            break;

        case PROP_FANCY:
            app->fancy = g_value_get_boolean (value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}


static void
gap_app_init (G_GNUC_UNUSED GapApp *app)
{
    if (GAP_APP_EDITOR_MODE)
        return;

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


static void
editor_windows_closed (GapApp *app)
{
    if (!app->gap_window)
        moo_app_quit (MOO_APP (app));
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
    g_object_set (editor, "allow-empty-window", TRUE, NULL);
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


static void
gap_app_ensure_terminal (GapApp *app)
{
    g_return_if_fail (GAP_IS_APP (app));

    if (!app->terminal)
    {
        GType win_type;

        if (app->fancy)
            win_type = GAP_TYPE_WS_WINDOW;
        else
            win_type = GAP_TYPE_TERM_WINDOW;

        app->gap_window = g_object_new (win_type, "ui-xml",
                                        moo_app_get_ui_xml (MOO_APP (app)),
                                        NULL);
        gtk_widget_show (GTK_WIDGET (app->gap_window));
        app->terminal = gap_window_get_terminal (app->gap_window);
        g_signal_connect_swapped (app->gap_window, "close",
                                  G_CALLBACK (gap_window_close), app);
    }
}


static int
gap_app_run (MooApp *mapp)
{
    GapApp *app;
    MooEditor *editor;

    app = GAP_APP (mapp);
    gap_app_output_start ();

    if (!GAP_APP_EDITOR_MODE && !app->editor_mode)
    {
        gap_app_ensure_terminal (app);
        gap_app_start_gap (app);
    }

    if (GAP_APP_EDITOR_MODE || app->editor_mode)
    {
        editor = moo_app_get_editor (mapp);
        if (!moo_editor_get_active_window (editor))
            moo_editor_new_window (editor);
    }

    return MOO_APP_CLASS(gap_app_parent_class)->run (mapp);
}


static void
gap_app_quit (MooApp *app)
{
    gap_app_output_shutdown ();

    if (GAP_APP (app)->gap_window)
        moo_window_close (MOO_WINDOW (GAP_APP (app)->gap_window));

    MOO_APP_CLASS(gap_app_parent_class)->quit (app);
}


static gboolean
gap_app_try_quit (MooApp *app)
{
    if (MOO_APP_CLASS(gap_app_parent_class)->try_quit (app))
        return TRUE;

    if (GAP_APP(app)->gap_window && !moo_window_close (MOO_WINDOW (GAP_APP(app)->gap_window)))
        return TRUE;

    return FALSE;
}


void
gap_app_send_intr (GapApp *app)
{
    g_return_if_fail (GAP_IS_APP (app) && GAP_IS_VIEW (app->terminal));

    if (gap_view_child_alive (app->terminal))
        gap_view_send_intr (app->terminal);
}


void
gap_app_restart_gap (GapApp *app)
{
    gap_app_stop_gap (app);
    g_usleep (100000);
    gap_app_start_gap (app);
}


void
gap_app_open_workspace (GapApp     *app,
                        const char *file)
{
    g_return_if_fail (GAP_IS_APP (app));
    g_return_if_fail (file != NULL);

    gap_app_ensure_terminal (app);
    g_return_if_fail (GAP_IS_VIEW (app->terminal));

    gap_view_stop_gap (app->terminal);
    gap_view_start_gap (app->terminal, app->gap_cmd_line, file);
}


char *
gap_saved_workspace_filename (void)
{
    return moo_get_user_data_file (GGAP_WORKSPACE_FILE);
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


static void
new_editor_action (MooApp *app)
{
    MooEditWindow *window;

    g_return_if_fail (GAP_IS_APP (app));

    window = moo_editor_new_window (moo_app_get_editor (app));
    gtk_window_present (GTK_WINDOW (window));
}

static void
open_in_editor_action (GapTermWindow *terminal)
{
    MooApp *app = moo_app_get_instance ();
    g_return_if_fail (app != NULL);
    moo_editor_open (moo_app_get_editor (app), NULL,
                     GTK_WIDGET (terminal), NULL);
}


void
gap_app_start_gap (GapApp *app)
{
    g_return_if_fail (GAP_IS_APP (app));

    gap_app_ensure_terminal (app);
    g_return_if_fail (!gap_view_child_alive (app->terminal));

    gap_view_start_gap (app->terminal, app->gap_cmd_line, NULL);
}


void
gap_app_stop_gap (GapApp *app)
{
    g_return_if_fail (GAP_IS_APP (app));
    g_return_if_fail (GAP_IS_VIEW (app->terminal));
    gap_view_stop_gap (app->terminal);
}


void
gap_app_feed_gap (GapApp     *app,
                  const char *text)
{
    g_return_if_fail (GAP_IS_APP (app));
    g_return_if_fail (text != NULL);

    gap_app_ensure_terminal (app);

    if (!gap_view_child_alive (app->terminal))
        gap_app_start_gap (app);

    g_return_if_fail (gap_view_child_alive (app->terminal));
    gap_view_feed_gap (app->terminal, text);
}


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

    if (!GAP_APP_EDITOR_MODE)
    {
        moo_prefs_dialog_append_page (dialog, gap_prefs_page_new ());
        moo_prefs_dialog_append_page (dialog, moo_term_prefs_page_new ());
    }

    moo_prefs_dialog_append_page (dialog, moo_edit_prefs_page_new (moo_app_get_editor (app)));
    moo_prefs_dialog_append_page (dialog, moo_user_tools_prefs_page_new ());
    moo_plugin_attach_prefs (GTK_WIDGET (dialog));

    return GTK_WIDGET (dialog);
}
