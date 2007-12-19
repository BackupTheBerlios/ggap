/*
 *   ggap.c
 *
 *   Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
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
#include "ggap-credits.h"
#include "ggap-i18n.h"
#include "ggapfile.h"
#include "mooutils/mooutils-fs.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/moostock.h"
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>


// /********************************************************
//  * command line parsing code generated by Opag
//  * http://www.zero-based.org/software/opag/
//  */
// #line 43 "ggap.c"
// %%
// g  gap-cmd      "=COMMAND GAP command line"                     reqarg
// n  new-app      "Run new instance of application"
//    fancy        "Fancy mode"
// x  extract      "=FILE Unpack saved worksheet file"
// l  log          "[=FILE] Show debug output or write it to FILE" optarg
//    version      "Display version information and exit"          return
// h  help         "Display this help text and exit"               return
// %%
// #line 55 "../../../ggap/ggap.opag"
// /* end of generated code
//  ********************************************************/


static void
push_appdir_to_path (void)
{
#ifdef __WIN32__
    char *appdir;
    const char *path;
    char *new_path;

    appdir = moo_win32_get_app_dir ();
    g_return_if_fail (appdir != NULL);

    path = g_getenv ("Path");

    if (path)
        new_path = g_strdup_printf ("%s;%s", appdir, path);
    else
        new_path = g_strdup (appdir);

    g_setenv ("Path", new_path, TRUE);

    g_free (new_path);
    g_free (appdir);
#endif
}

static void
extract_file (const char *filename)
{
    GError *error = NULL;
    char *text = NULL;
    gsize text_len = 0;
    char *workspace = NULL;

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

    if (!ggap_file_unpack (filename, &text, &text_len, &workspace, &error))
    {
        if (error)
            g_printerr ("%s\n", error->message);
        else
            g_printerr ("Failed\n");
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

int main (int argc, char *argv[])
{
    MdApp *app;
    GError *error = NULL;
    GOptionContext *context;

#ifdef ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, moo_get_locale_dir ());
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif
#endif

    g_thread_init (NULL);
    gdk_threads_init ();
    gdk_threads_enter ();
    g_type_init ();

    push_appdir_to_path ();

    app = g_object_new (GAP_TYPE_APP,
                        "short-name", "ggap",
                        "full-name", "GGAP",
                        "version", VERSION,
                        "description", "GGAP is a front end for GAP",
                        "icon-name", "ggap",
                        "credits", THANKS,
                        NULL);

    if (!app)
        return EXIT_FAILURE;

    context = g_option_context_new (NULL);
    md_app_setup_option_context (app, context);
    g_option_context_add_group (context, gtk_get_option_group (FALSE));

    if (!g_option_context_parse (context, &argc, &argv, &error))
    {
        g_printerr ("%s\n", error->message);
        exit (EXIT_FAILURE);
    }

    md_app_run (app, argc, argv);

    /* never reached */
    return EXIT_SUCCESS;
}


#if defined(__WIN32__) && !defined(__GNUC__)

#include <windows.h>

int __stdcall
WinMain (HINSTANCE hInstance,
         HINSTANCE hPrevInstance,
         char     *lpszCmdLine,
         int       nCmdShow)
{
	return main (__argc, __argv);
}

#endif
