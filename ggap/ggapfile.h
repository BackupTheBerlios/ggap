/*
 *   ggapfile.h
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

#ifndef GAP_FILE_H
#define GAP_FILE_H

#include <glib.h>

G_BEGIN_DECLS


enum {
    GGAP_FILE_ERROR_FAILED,
    GGAP_FILE_ERROR_BAD_VERSION,
    GGAP_FILE_ERROR_BAD_DATA,
    GGAP_FILE_ERROR_NOMEM
};

#define GGAP_FILE_ERROR (ggap_file_error_quark())

GQuark      ggap_file_error_quark   (void) G_GNUC_CONST;

gboolean    ggap_file_pack          (const char *text,
                                     const char *binary_file,
                                     int        *fd,
                                     GError    **error);
gboolean    ggap_file_unpack        (int        *fd,
                                     char      **text,
                                     gsize      *text_len,
                                     char      **binary_file,
                                     GError    **error);


G_END_DECLS

#endif /* GAP_FILE_H */
