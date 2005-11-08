/*
 *   gap.h
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

#ifndef __GAP_H__
#define __GAP_H__

#include <glib.h>

G_BEGIN_DECLS


char    *ggap_pkg_init_string   (void);
char    *gap_read_file_string   (const char *filename);
char    *gap_reread_file_string (const char *filename);


G_END_DECLS

#endif /* __GAP_H__ */
