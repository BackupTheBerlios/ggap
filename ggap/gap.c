/*
 *   gap.c
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

#include "gap.h"
#include "gapapp.h"
#include "mooutils/mooutils-misc.h"
#include <string.h>


static char *
gap_escape_filename (const char *filename)
{
    g_return_val_if_fail (filename != NULL, NULL);
    return g_strescape (filename, NULL);
}


char*
gap_pkg_init_file (void)
{
    const char *in_name, *out_name;
    char *in_escaped, *out_escaped, *init_string;
    char *filename;
    MooApp *app;
    GError *error = NULL;

    app = moo_app_get_instance ();
    g_return_val_if_fail (GAP_IS_APP (app), NULL);

    filename = moo_app_tempnam (app);
    g_return_val_if_fail (filename != NULL, NULL);

    in_name = moo_app_get_input_pipe_name (moo_app_get_instance ());
    out_name = moo_app_get_output_pipe_name (moo_app_get_instance ());

    in_escaped = gap_escape_filename (in_name ? in_name : "");
    out_escaped = gap_escape_filename (out_name ? out_name : "");
    init_string = g_strdup_printf ("GGAP_API.INIT(\"%s\", \"%s\");\n",
                                   in_escaped, out_escaped);

    if (!moo_save_file_utf8 (filename, init_string, -1, &error))
    {
        g_critical ("%s: %s", G_STRLOC, error->message);
        g_error_free (error);
        g_free (filename);
        filename = NULL;
    }

    g_free (in_escaped);
    g_free (out_escaped);
    g_free (init_string);

    return filename;
}


static char *
gap_file_func_string (const char *filename,
                      const char *func)
{
    char *escaped, *string;

    g_return_val_if_fail (filename != NULL, NULL);
    g_return_val_if_fail (func != NULL, NULL);

    escaped = gap_escape_filename (filename);
    string = g_strdup_printf ("%s(\"%s\");\n", func, escaped);

    g_free (escaped);
    return string;
}


char*
gap_read_file_string (const char *filename)
{
    return gap_file_func_string (filename, "Read");
}


char*
gap_reread_file_string (const char *filename)
{
    return gap_file_func_string (filename, "Reread");
}


#ifdef __WIN32__
gboolean
gap_parse_cmd_line (const char *command_line,
                    char      **bin_dir_p,
                    char      **root_dir_p)
{
    char *bin_dir = NULL, *root_dir = NULL, *exe = NULL;

    g_return_val_if_fail (command_line && *command_line, FALSE);

    if (*command_line == '"')
    {
        char *second = strchr (command_line + 1, '"');

        if (!second || second == command_line + 1)
            return FALSE;

        exe = g_strndup (command_line, second - command_line - 1);
    }
    else
    {
        char **comps = g_strsplit_set (command_line, " \t", 2);

        if (!comps)
            return FALSE;

        exe = g_strdup (comps[0]);

        g_strfreev (comps);
    }

    if (!g_path_is_absolute (exe))
    {
        g_free (exe);
        return FALSE;
    }

    bin_dir = g_path_get_dirname (exe);
    root_dir = g_path_get_dirname (bin_dir);

    if (bin_dir_p)
        *bin_dir_p = bin_dir;
    else
        g_free (bin_dir);

    if (root_dir_p)
        *root_dir_p = root_dir;
    else
        g_free (root_dir);

    g_free (exe);

    return TRUE;
}
#endif
