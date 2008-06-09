/*
 *   gapworksheet.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef GAP_WORKSHEET_H
#define GAP_WORKSHEET_H

#include "moows/mooworksheet.h"
#include <mooui/mddocument.h>

G_BEGIN_DECLS


#define GAP_TYPE_WORKSHEET             (gap_worksheet_get_type ())
#define GAP_WORKSHEET(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAP_TYPE_WORKSHEET, GapWorksheet))
#define GAP_WORKSHEET_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_WORKSHEET, GapWorksheetClass))
#define GAP_IS_WORKSHEET(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAP_TYPE_WORKSHEET))
#define GAP_IS_WORKSHEET_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_WORKSHEET))
#define GAP_WORKSHEET_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_WORKSHEET, GapWorksheetClass))

typedef struct GapWorksheet        GapWorksheet;
typedef struct GapWorksheetPrivate GapWorksheetPrivate;
typedef struct GapWorksheetClass   GapWorksheetClass;

/* keep in sync with save dialog code */
typedef enum {
    GAP_FILE_WORKSPACE,
    GAP_FILE_WORKSHEET,
    GAP_FILE_TEXT
} GapFileType;

struct GapWorksheet
{
    MooWorksheet base;
    GapWorksheetPrivate *priv;
};

struct GapWorksheetClass
{
    MooWorksheetClass base_class;
};


GType       gap_worksheet_get_type              (void) G_GNUC_CONST;

GapFileType gap_worksheet_get_file_type         (GapWorksheet   *ws);

void        gap_file_info_set_file_type         (MdFileInfo     *file_info,
                                                 GapFileType     type);
GapFileType gap_file_info_get_file_type         (MdFileInfo     *file_info,
                                                 GapFileType     dflt);


G_END_DECLS

#endif /* GAP_WORKSHEET_H */
