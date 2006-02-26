/*
 *   gapapp-script.c
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

#include "gapapp.h"
#include "mooutils/mooscript/mooscript-context.h"


gpointer
gap_app_get_editor_context (MooWindow *window)
{
    return g_object_new (MS_TYPE_CONTEXT, NULL);
}


gpointer
gap_app_get_terminal_context (MooWindow *window)
{
    return g_object_new (MS_TYPE_CONTEXT, NULL);
}
