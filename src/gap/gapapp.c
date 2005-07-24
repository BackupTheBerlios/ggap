/*
 *   @(#)$Id: gapapp.c,v 1.4 2005/05/31 23:57:22 emuntyan Exp $
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gap/gap.h"
#include "gap/gapapp.h"
#include "gap/gaptermwindow.h"
#include "gap/gapprefs.h"
#include "mooutils/moocompat.h"
#include "mooutils/mooprefs.h"


typedef struct {
    char *gap_command;
} GapCmdlineOptions;
static GapCmdlineOptions gap_cmdline_options = {0};


struct _GapAppPrivate {
    gboolean        gap_alive;
    GapOutChannel  *out;
};


static void gap_app_class_init      (GapAppClass    *klass);
static void gap_app_init            (GapApp     *app);
static void gap_app_finalize        (GObject    *object);

static int  gap_app_run             (MooApp     *app);
static gboolean gap_app_quit        (MooApp     *app);

static void on_gap_exit             (GapApp     *app);

static void gap_app_set_defaults    (void);


inline static MooTerm *term (GapApp *app)
{
    MooTermWindow *win =
        MOO_TERM_WINDOW (moo_app_get_term_window (MOO_APP (app)));
    return win ? win->terminal : NULL;
}


/* GAP_TYPE_APP */
G_DEFINE_TYPE (GapApp, gap_app, MOO_TYPE_APP)


static void gap_app_class_init (GapAppClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    MooAppClass *app_class = MOO_APP_CLASS (klass);

    gobject_class->finalize = gap_app_finalize;

    app_class->run = gap_app_run;
    app_class->quit = gap_app_quit;
}


static void gap_app_init (GapApp *app)
{
    app->priv = g_new0 (GapAppPrivate, 1);
    app->priv->gap_alive = FALSE;
    app->priv->out = gap_out_channel_new (NULL);
}


static void gap_app_finalize       (GObject      *object)
{
    GapApp *app = GAP_APP (object);
    if (app->priv->out) {
        gap_out_channel_shutdown (app->priv->out);
        g_object_unref (app->priv->out);
    }
    g_free (app->priv);
    G_OBJECT_CLASS (gap_app_parent_class)->finalize (object);
}


MooApp          *gap_app_new                    (G_GNUC_UNUSED int     argc,
                                                 char  **argv)
{
    return MOO_APP (g_object_new (GAP_TYPE_APP,
                                  "argv", argv,
                                  NULL));
}


GapApp          *gap_app                        (void)
{
    return GAP_APP (moo_app_get_instance ());
}


GOptionGroup    *gap_app_get_option_group       (void)
{
    static GOptionEntry entries[] = {
        { "gap-command", 'g', 0, G_OPTION_ARG_STRING,
            &(gap_cmdline_options.gap_command),
            "Override GAP command application setting", "COMMAND" },
        { 0, 0, 0, G_OPTION_ARG_NONE, 0, 0, 0 }
    };

    GOptionGroup *group = g_option_group_new ("GAP",
                                              "GAP options",
                                              "GAP options",
                                              NULL, NULL);
    g_option_group_add_entries (group, entries);
    return group;
}


const char      *gap_app_get_profile_name       (void)
{
    return "GAP";
}


static int  gap_app_run                 (MooApp         *app)
{
    GapApp *gapp = GAP_APP (app);

    gap_out_channel_start (gapp->priv->out);

    gap_app_set_defaults ();
    moo_app_init_default (app);

    moo_app_set_term_window (app, GTK_WINDOW (gap_term_window_new ()));
    gapp->priv->gap_alive = FALSE;
    gap_app_start_gap (gapp);

    gtk_main ();
    
    gap_out_channel_shutdown (gapp->priv->out);

    return moo_app_get_exit_code (app);
}


static gboolean gap_app_quit            (MooApp         *app)
{
    if (!moo_editor_close_all (moo_app_get_editor (app)))
        return FALSE;
    moo_prefs_save (moo_app_get_rc_file_name (app));
    return TRUE;
}


#define set_default(key,val)                                \
    moo_prefs_set_if_not_set_ignore_change (key, val)
#define set_bool_default(key,val)                           \
    moo_prefs_set_bool_if_not_set_ignore_change (key, val)

static void gap_app_set_defaults        (void)
{
#ifdef __WIN32__
    set_default (GAP_PREFS_GAP_COMMAND, "C:\\GAP4r4\\bin\\gapw95.exe");
#else /* !__WIN32__ */
    set_default (GAP_PREFS_GAP_COMMAND, "gap");
#endif /* !__WIN32__ */
    set_default (GAP_PREFS_GAP_WORKING_DIR, "");
    set_bool_default (GAP_PREFS_GAP_INIT_PKG, FALSE);
    set_bool_default (GAP_PREFS_GAP_LOAD_WORKSPACE, FALSE);
    set_bool_default (GAP_PREFS_GAP_SAVE_WORKSPACE, FALSE);
    set_default (GAP_PREFS_GAP_WORKSPACE, "");
}


void             gap_app_start_gap              (GapApp     *app)
{
    const char *c;
    char *cmd = NULL;

    g_return_if_fail (!app->priv->gap_alive);
    c = moo_prefs_get (GAP_PREFS_GAP_COMMAND);
    g_return_if_fail (c && c[0]);

#ifdef USE_PYTHON
    if (moo_prefs_get_bool(GAP_PREFS_GAP_INIT_PKG))
        cmd = g_strdup_printf ("%s -l \"%s\";",
                               c,
#ifdef __WIN32__
                               moo_app_get_application_dir (MOO_APP (app)));
#else /* ! __WIN32__ */
                               GGAP_ROOT_DIR);
#endif /* ! __WIN32__ */
    else
        cmd = g_strdup (c);
#else /* !USE_PYTHON */
    cmd = g_strdup (c);
#endif /* !USE_PYTHON */

    g_message ("%s: starting '%s'", G_STRLOC, cmd);

    if (moo_term_fork_command (term (app), cmd,
                               moo_prefs_get (GAP_PREFS_GAP_WORKING_DIR), NULL))
    {
        app->priv->gap_alive = TRUE;
#ifdef USE_PYTHON
        if (moo_prefs_get_bool (GAP_PREFS_GAP_INIT_PKG))
            gap_app_feed_gap (app, gap_pkg_init_string (moo_app_get_input_pipe_name (MOO_APP (app))), -1);
#endif /* USE_PYTHON */

        g_signal_connect_swapped (term (app), "child-exited",
                                  (GCallback) on_gap_exit, app);
    }
    else
        g_critical ("%s: could not start gap", G_STRLOC);

    g_free (cmd);
}


void             gap_app_stop_gap               (GapApp     *app)
{
    g_return_if_fail (app->priv->gap_alive);
    moo_term_feed_child (term (app), "\n\4\4\4", -1);
    g_usleep (100000);
    moo_term_kill_child (term (app));
    app->priv->gap_alive = FALSE;
}


void             gap_app_restart_gap            (GapApp     *app)
{
    gap_app_stop_gap (app);
#ifdef OS_DARWIN
    /* without this gap can't be restarted for some reasons */
    g_usleep (100000);
#endif /* OS_DARWIN */
    gap_app_start_gap (app);
}


static void on_gap_exit (GapApp *app)
{
    app->priv->gap_alive = FALSE;
    if (term (app))
        g_signal_handlers_disconnect_by_func (G_OBJECT (term (app)),
                                              (gpointer) on_gap_exit, app);
}


void             gap_app_feed_gap               (GapApp     *app,
                                                 const char *text,
                                                 gssize      len)
{
    MooTerm *t;
    g_return_if_fail (GAP_IS_APP (app) && text != NULL);
    t = term (app);
    g_return_if_fail (t != NULL);
    moo_term_feed_child (t, text, len);
}


GapOutChannel   *gap_app_get_out_channel        (GapApp     *app)
{
    g_return_val_if_fail (GAP_IS_APP (app), NULL);
    return app->priv->out;
}


gboolean         gap_app_write_out              (GapApp     *app,
                                                 const char *data,
                                                 gssize      len)
{
    g_return_val_if_fail (GAP_IS_APP (app) && data != NULL, FALSE);
    return gap_out_channel_write (app->priv->out, data, len);
}
