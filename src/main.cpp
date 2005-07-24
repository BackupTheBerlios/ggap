//
//   @(#)$Id: main.cpp,v 1.24 2005/06/06 14:45:54 emuntyan Exp $
//
//   Copyright (C) 2004 by Yevgen Muntyan <muntyan@math.tamu.edu>
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

#include "mooapp/mooapp.h"
#include "mooapp/mooapp-private.h"
#include "mooutils/moolog.h"
#include "mooutils/moocompat.h"
#include "mooutils/moostock.h"
#include <string>

#ifdef USE_PYTHON
#include "mooapp/mooappmod.h"
#endif

#ifdef USE_GAP_PROFILE
#include "gap/gapapp.h"
#endif
#ifdef USE_SC3_PROFILE
#include "sc3/sc3app.h"
#endif
#ifdef USE_GENERIC_PROFILE
#include "generic/genericapp.h"
#endif

#ifndef DEBUG
#define DEBUG 0
#else
#undef DEBUG
#define DEBUG 1
#endif

static void version ();
static void detailed_version ();

int main (int argc, char *argv[])
{
    // 1. Create a GOption group with default parameters

    bool show_debug = DEBUG;
    bool show_debug_console = false;
    char *log_file = NULL;
    char *rc_file = NULL;
    bool show_version = false;
    bool show_detailed_version = false;
    char *profile = NULL;

    static GOptionEntry entries[] = {
        { "profile", 'p', 0, G_OPTION_ARG_STRING, &profile, "Use application profile PROFILE", "PROFILE" },
        { "rc-file", 'f', 0, G_OPTION_ARG_FILENAME, &rc_file, "Alternate rc file", "FILE" },
        { "version", 'v', 0, G_OPTION_ARG_NONE, &show_version, "Show application version and quit", NULL },
        { "detailed-version", 'V', 0, G_OPTION_ARG_NONE, &show_detailed_version, "Show application version and compilation parameters, and quit", NULL },
        { "show-debug", 'd', 0, G_OPTION_ARG_NONE, &show_debug, "Show debug output", NULL },
        { "show-debug-console", 'D', 0, G_OPTION_ARG_NONE, &show_debug_console, "Show debug console", NULL },
        { "log-file", 'l', 0, G_OPTION_ARG_FILENAME, &log_file, "Write debug output to file", "FILE" },
        { 0, 0, 0, G_OPTION_ARG_NONE, 0, 0, 0 }
    };

    GOptionGroup *general = g_option_group_new ("general",
                                                "General options",
                                                "General options",
                                                NULL, NULL);
    g_option_group_add_entries (general, entries);

    GOptionContext *context = g_option_context_new ("");
    g_option_context_set_ignore_unknown_options (context, true);
    g_option_context_set_main_group (context, general);

    // gtk_get_option_group is a macro returning NULL if gtk < 2.6
    GOptionGroup *gtkgroup = gtk_get_option_group (false);
    if (gtkgroup) g_option_context_add_group (context, gtkgroup);

    // 2. Ask available profiles to attach their options

    std::string gap_profile, sc3_profile, gen_profile;

#ifdef USE_GAP_PROFILE
#define ANY_PROFILE
    g_option_context_add_group (context,
                                gap_app_get_option_group ());
    gap_profile = gap_app_get_profile_name ();
#endif

#ifdef USE_SC3_PROFILE
#define ANY_PROFILE
    g_option_context_add_group (context,
                                sc3_app_get_option_group ());
    sc3_profile = sc3_app_get_profile_name ();
#endif

#ifdef USE_GENERIC_PROFILE
#define ANY_PROFILE
    g_option_context_add_group (context,
                                generic_app_get_option_group ());
    gen_profile = generic_app_get_profile_name ();
#endif

#ifndef ANY_PROFILE
#error "No application profiles defined"
#endif

    // 3. Parse command line options and perform initialization

    GError *error = NULL;
    g_option_context_parse (context, &argc, &argv, &error);

    if (show_detailed_version) {
        detailed_version ();
        return 0;
    }
    if (show_version) {
        version ();
        return 0;
    }

    if (show_debug_console) {
        gtk_init (&argc, &argv);
        moo_set_log_func_window (true);
    }
    else {
        if (log_file)
            moo_set_log_func_file (log_file);
        else
            moo_set_log_func (show_debug);
        gtk_init (&argc, &argv);
    }

    moo_create_stock_items ();

    // 4. Start python interpreter

#ifdef USE_PYTHON
    MooPython *python = moo_python_new ();
    ggap_mod_set_python (python);
    moo_python_start (python, argc, argv);
    MooAppInput *input = moo_app_input_new (python);
    moo_app_input_start (input);
    moo_app_set_python (python, input);
#endif // USE_PYTHON

    // 5. Create appropriate application

#if defined(USE_GAP_PROFILE)
    if (!profile) profile = g_strdup (gap_app_get_profile_name ());
#elif defined(USE_SC3_PROFILE)
    if (!profile) profile = g_strdup (sc3_app_get_profile_name ());
#elif defined(USE_GENERIC_PROFILE)
    if (!profile) profile = g_strdup (generic_app_get_profile_name ());
#endif

    MooApp *app = NULL;
#ifdef USE_GAP_PROFILE
    if (!app && gap_profile == profile)
        app = gap_app_new (argc, argv);
#endif // USE_GAP_PROFILE
#ifdef USE_SC3_PROFILE
    if (!app && sc3_profile == profile)
        app = sc3_app_new (argc, argv);
#endif // USE_SC3_PROFILE
#ifdef USE_GENERIC_PROFILE
    if (!app)
        app = generic_app_new (profile, argc, argv);
#endif // USE_GENERIC_PROFILE

    if (!app) {
        g_printerr ("'%s' is not a valid application profile\n", profile);
        exit (1);
    }

    // 6. run application

    if (rc_file) moo_app_set_rc_file_name (app, rc_file);
    moo_app_run (app);

    // 7. Clean up

    g_free (rc_file);
    g_free (log_file);
    g_free (profile);
    g_object_unref (G_OBJECT (app));
    g_option_context_free (context);
#ifdef USE_PYTHON
    moo_app_input_shutdown (input);
    moo_python_shutdown (python);
    g_object_unref (input);
    g_object_unref (python);
#endif // USE_PYTHON
}


static void version ()
{
#ifdef GGAP_VERSION
    g_print ("ggap %s\n", GGAP_VERSION);
#else
    g_print ("ggap\n");
#endif
}


static void detailed_version ()
{
#ifdef GGAP_VERSION
    g_print ("ggap %s\n", GGAP_VERSION);
#else
    g_print ("ggap\n");
#endif
}
