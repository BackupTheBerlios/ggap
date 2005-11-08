/*
 *   gap.c
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

#include "gap.h"
#include "gapapp.h"


static char *
gap_escape_filename (const char *filename)
{
    g_return_val_if_fail (filename != NULL, NULL);
    return g_strescape (filename, NULL);
}


char*
ggap_pkg_init_string (void)
{
    const char *in_name, *out_name;
    char *in_escaped, *out_escaped, *init_string;

    g_return_val_if_fail (GAP_IS_APP (moo_app_get_instance ()), NULL);

    in_name = moo_app_get_input_pipe_name (moo_app_get_instance ());
    out_name = moo_app_get_output_pipe_name (moo_app_get_instance ());
    g_return_val_if_fail (in_name != NULL && out_name != NULL, NULL);

    in_escaped = gap_escape_filename (in_name);
    out_escaped = gap_escape_filename (out_name);
    init_string = g_strdup_printf ("GGAP_API.INIT(\"%s\", \"%s\");\n",
                                   in_escaped, out_escaped);

    g_free (in_escaped);
    g_free (out_escaped);
    return init_string;
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
