/*
 *   ggapfile.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef GAP_FILE_H
#define GAP_FILE_H

#include "gapworksheet.h"

G_BEGIN_DECLS


enum {
    GGAP_FILE_ERROR_FAILED,
    GGAP_FILE_ERROR_BAD_VERSION,
    GGAP_FILE_ERROR_BAD_DATA,
    GGAP_FILE_ERROR_NOMEM
};

#define GGAP_FILE_ERROR (ggap_file_error_quark())

GQuark      ggap_file_error_quark   (void) G_GNUC_CONST;

gboolean    ggap_file_save_xml      (const char     *text,
                                     const char     *binary_file,
                                     const char     *filename,
                                     GError        **error);
gboolean    ggap_file_load          (const char     *filename,
                                     GapFileType    *type,
                                     char          **text,
                                     gsize          *text_len,
                                     char          **binary_file,
                                     GError        **error);


G_END_DECLS

#endif /* GAP_FILE_H */
