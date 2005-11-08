/*
 *   gapapp.c
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

#include "config.h"
#include "gapapp.h"
#include "ggap-ui.h"
#include "gaptermwindow.h"
#include "gapeditwindow.h"
#include "gap.h"
#include "gapprefs-glade.h"
#include <mooutils/moostock.h>
#include <mooutils/mooprefsdialog.h>
#include <mooterm/mooterm-prefs.h>
#include <mooedit/mooeditprefs.h>
#include <mooedit/mooplugin.h>
#define WANT_MOO_APP_CMD_STRINGS
#include <mooapp/mooappinput.h>
#include <string.h>


#define APP_PREFS_PREFIX            "GAP"
#define APP_PREFS_GAP_COMMAND       APP_PREFS_PREFIX "/command"
#define APP_PREFS_GAP_INIT_PKG      APP_PREFS_PREFIX "/init_pkg"
#define APP_PREFS_GAP_WORKING_DIR   APP_PREFS_PREFIX "/working_dir"


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


G_DEFINE_TYPE(GapApp, gap_app, MOO_TYPE_APP)

enum {
    PROP_0,
    PROP_GAP_CMD_LINE,
    PROP_EDITOR_MODE,
    PROP_OPEN_FILES,
    PROP_NEW_APP,
    PROP_SIMPLE
};

static void
gap_app_class_init (GapAppClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    MooAppClass *app_class = MOO_APP_CLASS (klass);
    MooWindowClass *edit_class, *term_class;

    gobject_class->set_property = gap_app_set_property;
    gobject_class->get_property = gap_app_get_property;

    app_class->init = gap_app_init_real;
    app_class->run = gap_app_run;
    app_class->quit = gap_app_quit;
    app_class->try_quit = gap_app_try_quit;
    app_class->prefs_dialog = gap_app_prefs_dialog;
    app_class->exec_cmd = gap_app_exec_cmd;

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
                                     PROP_OPEN_FILES,
                                     g_param_spec_pointer ("open-files",
                                             "open-files",
                                             "open-files",
                                             G_PARAM_WRITABLE));

    g_object_class_install_property (gobject_class,
                                     PROP_NEW_APP,
                                     g_param_spec_boolean ("new-app",
                                             "new-app",
                                             "new-app",
                                             FALSE,
                                             G_PARAM_WRITABLE));

    g_object_class_install_property (gobject_class,
                                     PROP_SIMPLE,
                                     g_param_spec_boolean ("simple",
                                             "simple",
                                             "simple",
                                             FALSE,
                                             G_PARAM_WRITABLE));

    edit_class = g_type_class_ref (MOO_TYPE_EDIT_WINDOW);
    term_class = g_type_class_ref (GAP_TYPE_TERM_WINDOW);

//     moo_window_class_new_action (edit_class, "ExecuteSelection",
//                                  "name", "Execute Selection",
//                                  "label", "_Execute Selection",
//                                  "tooltip", "Execute Selection",
//                                  "icon-stock-id", GTK_STOCK_EXECUTE,
//                                  "accel", "<shift><alt>Return",
//                                  "closure::callback", execute_selection,
//                                  NULL);

    moo_window_class_new_action (term_class, "Restart",
                                 "name", "Restart",
                                 "label", "_Restart",
                                 "tooltip", "Restart",
                                 "icon-stock-id", MOO_STOCK_RESTART,
                                 "closure::callback", gap_app_restart_gap,
                                 "closure::proxy-func", moo_app_get_instance,
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

        case PROP_NEW_APP:
            app->new_app = g_value_get_boolean (value);
            break;

        case PROP_SIMPLE:
            app->simple = g_value_get_boolean (value);
            break;

        case PROP_OPEN_FILES:
            g_strfreev (app->open_files);
            app->open_files = g_strdupv (g_value_get_pointer (value));
            if (app->open_files && !*app->open_files)
            {
                g_strfreev (app->open_files);
                app->open_files = NULL;
            }
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}


static void
gap_app_init (GapApp *app)
{
    moo_app_set_terminal_type (MOO_APP (app), GAP_TYPE_TERM_WINDOW);
    app->term = NULL;

#ifdef __WIN32__
    moo_prefs_new_key_string (APP_PREFS_GAP_COMMAND, NULL);
#else
    moo_prefs_new_key_string (APP_PREFS_GAP_COMMAND, "gap");
#endif
    moo_prefs_new_key_bool (APP_PREFS_GAP_INIT_PKG, TRUE); /* XXX */
    moo_prefs_new_key_string (APP_PREFS_GAP_WORKING_DIR, NULL);
}


static gboolean
gap_app_init_real (MooApp *mapp)
{
    MooUIXML *xml;
    MooEditor *editor;
    GapApp *app = GAP_APP (mapp);

    if (!app->new_app)
    {
        char **p;
        GString *msg = g_string_new (NULL);

        if (!app->open_files || !*(app->open_files))
            g_string_append_len (msg, CMD_PRESENT, strlen (CMD_PRESENT) + 1);

        for (p = app->open_files; p && *p; ++p)
        {
            char *freeme = NULL;
            const char *basename, *filename;

            basename = *p;

            if (g_path_is_absolute (basename))
            {
                filename = basename;
            }
            else
            {
                char *dir = g_get_current_dir ();
                freeme = g_build_filename (dir, basename, NULL);
                filename = freeme;
                g_free (dir);
            }

            g_string_append_len (msg, CMD_OPEN_FILE, strlen (CMD_OPEN_FILE));
            g_string_append_len (msg, filename, strlen (filename) + 1);

            g_free (freeme);
        }

        if (moo_app_send_msg (mapp, msg->str, msg->len))
        {
            g_string_free (msg, TRUE);
            goto exit;
        }

        g_string_free (msg, TRUE);
    }

    xml = moo_app_get_ui_xml (mapp);
    moo_ui_xml_add_ui_from_string (xml, GGAP_UI, -1);

    app->new_app = TRUE;
    if (!MOO_APP_CLASS(gap_app_parent_class)->init (mapp))
        return FALSE;

    editor = moo_app_get_editor (mapp);
    moo_editor_set_window_type (editor, GAP_TYPE_EDIT_WINDOW);
    moo_editor_set_default_lang (editor, "GAP");

    return TRUE;

exit:
    app->new_app = FALSE;
    return TRUE;
}


static int
gap_app_run (MooApp     *mapp)
{
    GapApp *app;
    MooEditor *editor;
    MooEditWindow *edit_window;
    MooTermWindow *term_window;
    char **file;

    app = GAP_APP (mapp);

    if (!app->new_app)
        return 0;

    if (!app->editor_mode)
    {
        term_window = moo_app_get_terminal (mapp);
        app->term = moo_term_window_get_term (term_window);
        gap_app_start_gap (app);
    }

    if (app->open_files || app->editor_mode)
    {
        editor = moo_app_get_editor (mapp);
        edit_window = moo_editor_new_window (editor);

        for (file = app->open_files; file && *file; ++file)
            moo_editor_open_file (editor, edit_window, NULL,
                                  *file, NULL);
    }

    return MOO_APP_CLASS(gap_app_parent_class)->run (mapp);
}


static void
gap_app_quit (MooApp     *app)
{
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
    MOO_APP_CLASS(gap_app_parent_class)->exec_cmd (app, cmd, data, len);
}


void
gap_app_restart_gap (GapApp *app)
{
    gap_app_stop_gap (app);
    g_usleep (100000);
    gap_app_start_gap (app);
}


void
gap_app_start_gap (GapApp *app)
{
    const char *cmd_base;
    GString *cmd;
    gboolean result, init_pkg;

    g_return_if_fail (GAP_IS_APP (app));
    g_return_if_fail (!moo_term_child_alive (app->term));

    if (!app->term)
    {
        MooTermWindow *window = moo_app_get_terminal (MOO_APP (app));
        app->term = moo_term_window_get_term (window);
    }

    cmd_base = moo_prefs_get_string (APP_PREFS_GAP_COMMAND);
    g_return_if_fail (cmd_base && cmd_base[0]);

    cmd = g_string_new (cmd_base);

    init_pkg = moo_prefs_get_bool (APP_PREFS_GAP_INIT_PKG);

    if (init_pkg)
    {
#ifdef __WIN32__
        g_string_append_printf (cmd, " -l \"%s\";",
                                moo_app_get_application_dir (MOO_APP (app)));
#else
        g_string_append (cmd, " -l \"" GGAP_ROOT_DIR "\";");
#endif
    }

    result = moo_term_fork_command_line (app->term, cmd->str,
                                         moo_prefs_get_filename (APP_PREFS_GAP_WORKING_DIR),
                                         NULL, NULL);

    if (result)
    {
        if (init_pkg)
        {
            char *init_string = ggap_pkg_init_string ();
            gap_app_feed_gap (app, init_string);
            g_free (init_string);
        }
    }
    else
    {
        g_critical ("%s: could not start gap", G_STRLOC);
    }

    g_string_free (cmd, TRUE);
}


void
gap_app_stop_gap (GapApp     *app)
{
    g_return_if_fail (GAP_IS_APP (app) && MOO_IS_TERM (app->term));

    if (!moo_term_child_alive (app->term))
        return;

    gap_app_feed_gap (app, "\n\4\4\4");
    g_usleep (100000);
    moo_term_kill_child (app->term);
}


void
gap_app_feed_gap (GapApp     *app,
                  const char *text)
{
    g_return_if_fail (GAP_IS_APP (app) && text != NULL);
    g_return_if_fail (app->term != NULL && moo_term_child_alive (app->term));
    moo_term_feed_child (app->term, text, -1);
}


static GtkWidget *
gap_prefs_page_new (void)
{
    GtkWidget *page;

    page = moo_prefs_dialog_page_new_from_xml ("GAP",
                                               MOO_STOCK_GAP,
                                               GAP_PREFS_GLADE_UI,
                                               -1,
                                               "page",
                                               "");

    return page;
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

//     moo_prefs_dialog_append_page (dialog, moo_edit_prefs_page_new (moo_app_get_editor (app)));
    moo_prefs_dialog_append_page (dialog, moo_edit_colors_prefs_page_new (moo_app_get_editor (app)));
    _moo_plugin_attach_prefs (GTK_WIDGET (dialog));

    return GTK_WIDGET (dialog);
}
