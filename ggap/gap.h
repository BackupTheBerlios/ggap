/*
 *   gap.h
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

#ifndef GAP_H
#define GAP_H

#include <glib.h>

G_BEGIN_DECLS


const char  *gap_init_file          (const char *workspace,
                                     gboolean    init_pkg,
                                     guint       session_id,
                                     gboolean    fancy);

char        *gap_read_file_string   (const char *filename);
char        *gap_reread_file_string (const char *filename);

#ifdef __WIN32__
gboolean     gap_parse_cmd_line     (const char *command_line,
                                     char      **bin_dir,
                                     char      **root_dir);
#endif


G_END_DECLS

#endif /* GAP_H */
