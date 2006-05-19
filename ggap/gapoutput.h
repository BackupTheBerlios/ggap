/*
 *   gapoutput.h
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

#ifndef __GAP_APP_OUTPUT__
#define __GAP_APP_OUTPUT__

#ifdef __WIN32__
#include <windows.h>
#endif
#include <glib.h>

G_BEGIN_DECLS


const char     *gap_app_output_get_name (void);

gboolean        gap_app_output_start    (void);
void            gap_app_output_shutdown (void);
gboolean        gap_app_output_restart  (void);

void            gap_app_output_write    (const char     *data,
                                         gssize          len);


G_END_DECLS

#endif /* __GAP_APP_OUTPUT__ */
