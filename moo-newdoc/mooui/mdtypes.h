/*
 *   mdtypes.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MD_TYPES_H
#define MD_TYPES_H

#include <gtk/gtk.h>
#include <mooui/mdenums.h>
#include <mooui/mooapp.h>
#include <mooui/mooui-enums.h>
#include <mooutils/mdhistorymgr.h>


typedef struct MdAsyncOp MdAsyncOp;
typedef struct MdDocument MdDocument;
typedef struct MdFileInfo MdFileInfo;
typedef struct MdFileLoader MdFileLoader;
typedef struct MdFileOp MdFileOp;
typedef struct MdFileOpInfo MdFileOpInfo;
typedef struct MdFileSaver MdFileSaver;
typedef struct MdManager MdManager;
typedef struct MdWindow MdWindow;

#define MD_TYPE_ASYNC_OP            (md_async_op_get_type ())
#define MD_TYPE_DOCUMENT            (md_document_get_type ())
#define MD_TYPE_FILE_INFO_ARRAY     (md_file_info_array_get_type ())
#define MD_TYPE_FILE_INFO           (md_file_info_get_type ())
#define MD_TYPE_FILE_LOADER         (md_file_loader_get_type ())
#define MD_TYPE_FILE_OP             (md_file_op_get_type ())
#define MD_TYPE_FILE_OP_INFO        (md_file_op_info_get_type ())
#define MD_TYPE_FILE_SAVER          (md_file_saver_get_type ())
#define MD_TYPE_WINDOW              (md_window_get_type ())

GType   md_async_op_get_type        (void) G_GNUC_CONST;
GType   md_document_get_type        (void) G_GNUC_CONST;
GType   md_file_info_array_get_type (void) G_GNUC_CONST;
GType   md_file_info_get_type       (void) G_GNUC_CONST;
GType   md_file_loader_get_type     (void) G_GNUC_CONST;
GType   md_file_op_get_type         (void) G_GNUC_CONST;
GType   md_file_op_info_get_type    (void) G_GNUC_CONST;
GType   md_file_saver_get_type      (void) G_GNUC_CONST;
GType   md_window_get_type          (void) G_GNUC_CONST;


typedef enum
{
    MD_FILE_OP_STATUS_IN_PROGRESS,
    MD_FILE_OP_STATUS_ERROR,
    MD_FILE_OP_STATUS_SUCCESS,
    MD_FILE_OP_STATUS_CANCELLED
} MdFileOpStatus;


#endif /* MD_TYPES_H */
