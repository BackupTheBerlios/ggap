/*
 *   mdfileops.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MD_FILE_OPS_H
#define MD_FILE_OPS_H

#include <mooui/mdtypes.h>


#define MD_FILE_ERROR           (md_file_error_quark ())

#define MD_FILE_OP(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_FILE_OP, MdFileOp))
#define MD_IS_FILE_OP(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_FILE_OP))
#define MD_FILE_LOADER(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_FILE_LOADER, MdFileLoader))
#define MD_IS_FILE_LOADER(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_FILE_LOADER))
#define MD_FILE_SAVER(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_FILE_SAVER, MdFileSaver))
#define MD_IS_FILE_SAVER(obj)   (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_FILE_SAVER))

typedef enum {
    MD_FILE_ERROR_INVAL = 1,
    MD_FILE_ERROR_UNSUPPORTED,
    MD_FILE_ERROR_FAILED
} MdFileError;

GQuark           md_file_error_quark            (void) G_GNUC_CONST;

MdFileOpStatus   md_file_op_get_status          (MdFileOp       *op);
GError          *md_file_op_get_error           (MdFileOp       *op);
MdFileInfo      *md_file_op_get_file_info       (MdFileOp       *op);

MdFileLoader    *md_file_loader_new             (MdFileInfo     *file,
                                                 GError        **error);
const char      *md_file_loader_get_filename    (MdFileLoader   *loader);

typedef enum {
    MD_FILE_SAVER_CREATE_BACKUP = 1 << 0
} MdFileSaverFlags;

MdFileSaver     *md_file_saver_new              (MdFileInfo     *file,
                                                 MdFileSaverFlags flags,
                                                 GError        **error);
void             md_file_saver_write            (MdFileSaver    *saver,
                                                 const char     *data,
                                                 gsize           len);
void             md_file_saver_rewind           (MdFileSaver    *saver);
void             md_file_saver_finish           (MdFileSaver    *saver);
void             md_file_saver_error            (MdFileSaver    *saver,
                                                 GQuark          err_domain,
                                                 int             err_code,
                                                 const char     *fmt,
                                                 ...);

int              md_open_file_for_reading       (const char     *filename,
                                                 GError        **error);
int              md_open_file_for_writing       (const char     *filename,
                                                 GError        **error);


#endif /* MD_FILE_OPS_H */
