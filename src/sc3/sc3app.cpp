//
//   @(#)$Id: sc3app.cpp,v 1.4 2005/05/30 15:09:46 emuntyan Exp $
//
//   Copyright (C) 2004-2005 by Yevgen Muntyan <muntyan@math.tamu.edu>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   See COPYING file that comes with this distribution.
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sc3/sc3app.h"
#include "sc3/sc3termwindow.h"
#include "sc3/sc3prefs.h"
#include "mooutils/moocompat.h"
#include "mooutils/mooprefs.h"
#include <string.h>


namespace {
struct Sc3CmdlineOptions {
    Sc3CmdlineOptions ()
    {
    }
    ~Sc3CmdlineOptions ()
    {
    }
};
}
static Sc3CmdlineOptions sc3_cmdline_options;


struct _Sc3AppPrivate {
    _Sc3AppPrivate () : sclang_alive (false) {}
    ~_Sc3AppPrivate () {}

    bool sclang_alive;
};


static void sc3_app_class_init          (Sc3AppClass    *klass);

static void sc3_app_init                (Sc3App         *app);
static void sc3_app_finalize            (GObject        *object);

static int  sc3_app_run                 (MooApp         *app);
static gboolean sc3_app_quit            (MooApp         *app);

static void on_sc3_exit                 (Sc3App *app);

static void sc3_app_set_defaults        ();


inline MooTerm *term (Sc3App *app)
{
    MooTermWindow *win =
        MOO_TERM_WINDOW (moo_app_get_term_window (MOO_APP (app)));
    return win ? win->terminal : NULL;
}


/// SC3_TYPE_APP
G_DEFINE_TYPE (Sc3App, sc3_app, MOO_TYPE_APP)


static void sc3_app_class_init (Sc3AppClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    MooAppClass *app_class = MOO_APP_CLASS (klass);

    gobject_class->finalize = sc3_app_finalize;

    app_class->run = sc3_app_run;
    app_class->quit = sc3_app_quit;
}


static void sc3_app_init (Sc3App *app)
{
    app->priv = new Sc3AppPrivate;
}


static void sc3_app_finalize       (GObject      *object)
{
    delete SC3_APP(object)->priv;
    G_OBJECT_CLASS (sc3_app_parent_class)->finalize (object);
}


MooApp          *sc3_app_new                    (int     ,
                                                 char  **argv)
{
    return MOO_APP (g_object_new (SC3_TYPE_APP, "argv", argv, NULL));
}


Sc3App          *sc3_app                        (void)
{
    return SC3_APP (moo_app_get_instance ());
}


GOptionGroup    *sc3_app_get_option_group       (void)
{
    static GOptionEntry entries[] = {
        { 0, 0, 0, G_OPTION_ARG_NONE, 0, 0, 0 }
    };

    GOptionGroup *group = g_option_group_new ("sc3",
                                              "sc3 options",
                                              "sc3 options",
                                              NULL, NULL);
    g_option_group_add_entries (group, entries);
    return group;
}


const char      *sc3_app_get_profile_name       (void)
{
    return "sc3";
}


static int  sc3_app_run                 (MooApp         *app)
{
    Sc3App *sapp = SC3_APP (app);

    sc3_app_set_defaults ();
    moo_app_init_default (app);

    moo_app_set_term_window (app, GTK_WINDOW (sc3_term_window_new ()));
    sapp->priv->sclang_alive = false;
    sc3_app_start_sclang (sapp);

    gtk_main ();
    return moo_app_get_exit_code (app);
}


static gboolean sc3_app_quit            (MooApp         *app)
{
    if (!moo_editor_close_all (moo_app_get_editor (app)))
        return false;
    moo_prefs_save (moo_app_get_rc_file_name (app));
    return true;
}


static void sc3_app_set_defaults        ()
{
    moo_prefs_set_if_not_set_ignore_change (SC3_PREFS_SCLANG_COMMAND, "sclang");
    moo_prefs_set_if_not_set_ignore_change (SC3_PREFS_SCLANG_WORKING_DIR, "");
}


void             sc3_app_start_sclang              (Sc3App     *app)
{
    g_return_if_fail (!app->priv->sclang_alive);
    const char *c = moo_prefs_get (SC3_PREFS_SCLANG_COMMAND);
    g_return_if_fail (c && strlen(c));

    g_message ("%s: starting '%s'", G_STRLOC, c);

    if (moo_term_fork_command (term (app), c,
                               moo_prefs_get (SC3_PREFS_SCLANG_WORKING_DIR), NULL))
    {
        app->priv->sclang_alive = true;
        g_signal_connect_swapped (term (app), "child-exited",
                                  (GCallback)on_sc3_exit, app);
    }
    else
        g_critical ("%s: could not start sclang", G_STRLOC);
}


void             sc3_app_stop_sclang               (Sc3App     *app)
{
    g_return_if_fail (app->priv->sclang_alive);
    moo_term_feed_child (term (app), "\4\4\4", -1);
    g_usleep (100000);
    moo_term_kill_child (term (app));
    app->priv->sclang_alive = false;
}


void             sc3_app_restart_sclang            (Sc3App     *app)
{
    sc3_app_stop_sclang (app);
    sc3_app_start_sclang (app);
}


static void on_sc3_exit (Sc3App *app)
{
    app->priv->sclang_alive = false;
    if (term (app))
        g_signal_handlers_disconnect_by_func (G_OBJECT (term (app)),
                                              (gpointer)on_sc3_exit, app);
}


void             sc3_app_feed_sclang            (Sc3App     *app,
                                                 const char *text,
                                                 gssize      len)
{
    g_return_if_fail (SC3_IS_APP (app) && text != NULL);
    MooTerm *t = term (app);
    g_return_if_fail (t != NULL);
    moo_term_feed_child (t, text, len);
}


void             sc3_app_eval_all               (Sc3App     *app)
{
    g_return_if_fail (SC3_IS_APP (app));
    sc3_app_feed_sclang (app, "\33\n", -1);
}


void             sc3_app_stop_server            (Sc3App     *app)
{
    g_return_if_fail (SC3_IS_APP (app));
    sc3_app_feed_sclang (app, "s.freeAll; thisProcess.stop\14\n", -1);
}
