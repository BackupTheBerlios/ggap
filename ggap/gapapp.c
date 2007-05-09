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
#include "gapprefs-glade.h"
#include "gapoutput.h"
#include <mooutils/moostock.h>
#include <mooutils/mooprefsdialog.h>
#include <mooutils/mooutils-misc.h>
#include <mooutils/mooutils-fs.h>
#include <mooterm/mooterm-prefs.h>
#include <mooedit/mooeditprefs.h>
#include <mooedit/mooplugin.h>
#include <string.h>


#define APP_PREFS_GAP_COMMAND           GGAP_PREFS_PREFIX "/command"
#define APP_PREFS_GAP_INIT_PKG          GGAP_PREFS_PREFIX "/init_pkg"
#define APP_PREFS_GAP_WORKING_DIR       GGAP_PREFS_PREFIX "/working_dir"
#define APP_PREFS_GAP_SAVE_WORKSPACE    GGAP_PREFS_PREFIX "/save_workspace"
#define APP_PREFS_GAP_CLEAR_TERMINAL    GGAP_PREFS_PREFIX "/clear_terminal"

#define WORKSPACE_FILE "workspace"


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
static void         gap_app_exec_cmd        (MooApp     *app,
                                             char        cmd,
                                             const char *data,
                                             guint       len);
static GtkWidget   *gap_app_prefs_dialog    (MooApp     *app);
// static void         gap_app_cmd_setup       (MooApp     *app,
//                                              MooCommand *cmd,
//                                              GtkWindow  *window);

static void         gap_app_start_gap_real  (GapApp     *app,
                                             const char *workspace);
#ifdef __WIN32__
static void         open_gap_manual         (void);
#endif


G_DEFINE_TYPE(GapApp, gap_app, MOO_TYPE_APP)

enum {
    PROP_0,
    PROP_GAP_CMD_LINE,
    PROP_EDITOR_MODE,
    PROP_SIMPLE
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
    app_class->exec_cmd = gap_app_exec_cmd;
//     app_class->cmd_setup = gap_app_cmd_setup;

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
                                     PROP_SIMPLE,
                                     g_param_spec_boolean ("simple",
                                             "simple",
                                             "simple",
                                             FALSE,
                                             G_PARAM_WRITABLE));

    if (GAP_APP_EDITOR_MODE)
        return;

    edit_class = g_type_class_ref (GAP_TYPE_EDIT_WINDOW);
    term_class = g_type_class_ref (GAP_TYPE_TERM_WINDOW);

//     moo_window_class_new_action (edit_class, "ExecuteSelection", NULL,
//                                  "display-name", "Execute Selection",
//                                  "label", "_Execute Selection",
//                                  "tooltip", "Execute Selection",
//                                  "stock-id", GTK_STOCK_EXECUTE,
//                                  "accel", "<shift><alt>Return",
//                                  "closure-callback", execute_selection,
//                                  NULL);

    moo_window_class_new_action (term_class, "Quit", NULL,
                                 "display-name", GTK_STOCK_QUIT,
                                 "label", GTK_STOCK_QUIT,
                                 "tooltip", GTK_STOCK_QUIT,
                                 "stock-id", GTK_STOCK_QUIT,
                                 "accel", "<ctrl>Q",
                                 "closure-callback", moo_app_quit,
                                 "closure-proxy-func", moo_app_get_instance,
                                 NULL);

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

        case PROP_SIMPLE:
            app->simple = g_value_get_boolean (value);
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
    moo_prefs_new_key_string (APP_PREFS_GAP_COMMAND, "c:\\gap4r4\\bin\\gapw95.exe -l c:\\gap4r4");
#else
    moo_prefs_new_key_string (APP_PREFS_GAP_COMMAND, "gap");
#endif
    moo_prefs_new_key_bool (APP_PREFS_GAP_INIT_PKG, TRUE); /* XXX */
    moo_prefs_new_key_string (APP_PREFS_GAP_WORKING_DIR, NULL);
    moo_prefs_new_key_bool (APP_PREFS_GAP_SAVE_WORKSPACE, TRUE);
    moo_prefs_new_key_bool (APP_PREFS_GAP_CLEAR_TERMINAL, TRUE);
}


static void
editor_windows_closed (GapApp *app)
{
    if (!app->term_window)
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

    editor = moo_app_get_editor (app);
    moo_editor_set_window_type (editor, GAP_TYPE_EDIT_WINDOW);
    g_object_set (editor, "allow-empty-window", TRUE, NULL);
    g_signal_connect_swapped (editor, "all-windows-closed",
                              G_CALLBACK (editor_windows_closed), app);

    return TRUE;
}


static gboolean
term_window_close (GapApp         *app,
                   G_GNUC_UNUSED GdkEventAny *event,
                   MooTermWindow  *window)
{
    g_return_val_if_fail (GAP_IS_APP (app), FALSE);
    g_return_val_if_fail (MOO_IS_TERM_WINDOW (window), FALSE);

    if (app->term_window != window)
        return FALSE;

    return !moo_app_quit (MOO_APP (app));
}


static void
gap_died (GapApp *app)
{
    if (app->session)
    {
        gap_session_shutdown (app->session);
        g_object_unref (app->session);
        app->session = NULL;
    }

    gap_app_output_restart ();
}


static void
gap_app_ensure_terminal (GapApp *app)
{
    g_return_if_fail (GAP_IS_APP (app));

    if (!app->term)
    {
        app->term_window = g_object_new (GAP_TYPE_TERM_WINDOW, "ui-xml",
                                         moo_app_get_ui_xml (MOO_APP (app)),
                                         NULL);
        gtk_widget_show (GTK_WIDGET (app->term_window));
        app->term = moo_term_window_get_term (app->term_window);
        g_signal_connect_swapped (app->term, "child-died",
                                  G_CALLBACK (gap_died), app);
        g_signal_connect_swapped (app->term_window, "delete-event",
                                  G_CALLBACK (term_window_close), app);
    }
}


static int
gap_app_run (MooApp     *mapp)
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
    MOO_APP_CLASS(gap_app_parent_class)->quit (app);
}


static gboolean
gap_app_try_quit (MooApp     *app)
{
    return MOO_APP_CLASS(gap_app_parent_class)->try_quit (app);
}


static void
gap_app_exec_cmd (MooApp     *app,
                  char        cmd,
                  const char *data,
                  guint       len)
{
    switch (cmd)
    {
        case 'g':
            gap_session_execute (GAP_APP(app)->session, data, len);
            break;
        default:
            MOO_APP_CLASS(gap_app_parent_class)->exec_cmd (app, cmd, data, len);
    }
}


void
gap_app_send_intr (GapApp *app)
{
    g_return_if_fail (GAP_IS_APP (app) && MOO_IS_TERM (app->term));

    if (moo_term_child_alive (app->term))
        moo_term_ctrl_c (app->term);
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
    g_return_if_fail (MOO_IS_TERM (app->term));

    gap_app_stop_gap (app);
    g_usleep (100000);
    gap_app_start_gap_real (app, file);
}


static char *
saved_workspace_filename (void)
{
    return moo_get_user_data_file (WORKSPACE_FILE);
}


static GString *
make_command_line (const char *cmd_base,
                   const char *custom_wsp,
                   guint       session_id)
{
    gboolean init_pkg, save_workspace;
    gboolean wsp_already_saved = FALSE;
    char *wsp_file = NULL;
    const char *init_file = NULL;
    GString *cmd;

    init_pkg = moo_prefs_get_bool (APP_PREFS_GAP_INIT_PKG) &&
                gap_session_available ();
    save_workspace = moo_prefs_get_bool (APP_PREFS_GAP_SAVE_WORKSPACE);

    cmd = g_string_new (cmd_base);

    if (!custom_wsp && save_workspace)
    {
        wsp_file = saved_workspace_filename ();

        g_return_val_if_fail (wsp_file != NULL, cmd);

        wsp_already_saved = g_file_test (wsp_file, G_FILE_TEST_EXISTS);

        if (!wsp_already_saved)
        {
            char *gzipped = g_strdup_printf ("%s.gz", wsp_file);
            wsp_already_saved = g_file_test (gzipped, G_FILE_TEST_EXISTS);
            g_free (gzipped);
        }
    }

    if (custom_wsp)
    {
        g_string_append_printf (cmd, " -L \"%s\"", custom_wsp);
    }
    else if (save_workspace)
    {
        if (wsp_already_saved)
            g_string_append_printf (cmd, " -L \"%s\"", wsp_file);

        if (!wsp_already_saved && !moo_make_user_data_dir (NULL))
            g_critical ("%s: could not create user data dir", G_STRLOC);

        if (!wsp_already_saved || init_pkg)
            init_file = gap_init_file (wsp_already_saved ? NULL : wsp_file,
                                       init_pkg, session_id);
    }

    if (init_pkg && !init_file)
        init_file = gap_init_file (NULL, TRUE, session_id);

    if (init_pkg)
    {
        char **dirs;
        guint i, n_dirs = 0;

        dirs = moo_get_data_dirs (MOO_DATA_SHARE, &n_dirs);

        for (i = 0; i < n_dirs; ++i)
            g_string_append_printf (cmd, " -l \"%s\";", dirs[i]);

        g_strfreev (dirs);
    }

    if (init_file)
        g_string_append_printf (cmd, " \"%s\"", init_file);

    g_free (wsp_file);
    return cmd;
}


static void
gap_app_start_gap_real (GapApp     *app,
                        const char *workspace)
{
    static guint session_id;
    const char *cmd_base;
    GString *cmd;
    gboolean result;

    g_return_if_fail (GAP_IS_APP (app));

    gap_app_ensure_terminal (app);
    g_return_if_fail (!moo_term_child_alive (app->term));

    if (app->gap_cmd_line)
        cmd_base = app->gap_cmd_line;
    else
        cmd_base = moo_prefs_get_string (APP_PREFS_GAP_COMMAND);

    g_return_if_fail (cmd_base && cmd_base[0]);

#ifdef __WIN32__
    {
        static char *saved_bin_dir;
        static const char *saved_path;
        char *bin_dir;

        if (gap_parse_cmd_line (cmd_base, &bin_dir, NULL))
        {
            if (!saved_bin_dir || strcmp (saved_bin_dir, bin_dir))
            {
                char *path;

                if (!saved_path)
                    saved_path = g_getenv ("PATH");

                if (saved_path)
                    path = g_strdup_printf ("%s;%s", bin_dir, saved_path);
                else
                    path = g_strdup (bin_dir);

                g_setenv ("PATH", path, TRUE);
                g_free (path);

                saved_bin_dir = bin_dir;
                bin_dir = NULL;
            }

            g_free (bin_dir);
        }
        else
        {
            g_warning ("%s: could not parse command line `%s`",
                       G_STRLOC, cmd_base);
        }
    }
#endif

    if (session_id++ > 9999)
        session_id = 1;

    cmd = make_command_line (cmd_base, workspace, session_id);

    g_message ("starting GAP: %s", cmd->str);

    if (moo_prefs_get_bool (APP_PREFS_GAP_CLEAR_TERMINAL))
        moo_term_reset (app->term);

    result = moo_term_fork_command_line (app->term, cmd->str,
                                         moo_prefs_get_filename (APP_PREFS_GAP_WORKING_DIR),
                                         NULL, NULL);

    if (!result)
        g_critical ("%s: could not start gap", G_STRLOC);

    g_assert (app->session == NULL);
    app->session = gap_session_new (gap_app_output_get_name (), session_id);

    g_string_free (cmd, TRUE);
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
        cmd_base = moo_prefs_get_string (APP_PREFS_GAP_COMMAND);

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


void
gap_app_start_gap (GapApp *app)
{
    gap_app_start_gap_real (app, NULL);
}


void
gap_app_stop_gap (GapApp     *app)
{
    g_return_if_fail (GAP_IS_APP (app) && MOO_IS_TERM (app->term));

    if (moo_term_child_alive (app->term))
    {
        gap_app_feed_gap (app, "\n\4\4\4");
        g_usleep (100000);
        moo_term_kill_child (app->term);
    }
}


void
gap_app_feed_gap (GapApp     *app,
                  const char *text)
{
    g_return_if_fail (GAP_IS_APP (app) && text != NULL);
    gap_app_ensure_terminal (app);

    if (!moo_term_child_alive (app->term))
        gap_app_start_gap (app);

    g_return_if_fail (moo_term_child_alive (app->term));
    moo_term_feed_child (app->term, text, -1);
}


static void
remove_saved_workspace (void)
{
    char *wsp, *gzipped;

    wsp = saved_workspace_filename ();
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
    if (!moo_prefs_get_bool (APP_PREFS_GAP_SAVE_WORKSPACE))
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
    moo_plugin_attach_prefs (GTK_WIDGET (dialog));

    return GTK_WIDGET (dialog);
}


// static void
// gap_app_cmd_setup (MooApp     *app,
//                    MooCommand *cmd,
//                    GtkWindow  *window)
// {
//     MOO_APP_CLASS(gap_app_parent_class)->cmd_setup (app, cmd, window);
//
//     if (!GAP_APP_EDITOR_MODE)
//         gap_app_setup_command (cmd, window);
// }
