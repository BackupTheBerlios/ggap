/*
 *   @(#)$Id: gap.c,v 1.1 2005/04/22 08:49:55 emuntyan Exp $
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

#include <glib.h>
#include "gap/gap.h"


const char *gap_pkg_init_string (const char *pipe_name)
{
    static char *s = NULL;
    char *pipe = NULL;

    g_return_val_if_fail (pipe_name != NULL, NULL);

    g_free (s);
    pipe = g_strescape (pipe_name, NULL);
    s = g_strdup_printf ("GGAP_INIT(\"%s\");\n", pipe);
    g_free (pipe);
    return s;
}
