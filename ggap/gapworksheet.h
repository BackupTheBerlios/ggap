/*
 *   gapworksheet.h
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

#ifndef GAP_WORKSHEET_H
#define GAP_WORKSHEET_H

#include "moows/mooworksheet.h"

G_BEGIN_DECLS


#define GAP_TYPE_WORKSHEET             (gap_worksheet_get_type ())
#define GAP_WORKSHEET(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAP_TYPE_WORKSHEET, GapWorksheet))
#define GAP_WORKSHEET_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_WORKSHEET, GapWorksheetClass))
#define GAP_IS_WORKSHEET(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAP_TYPE_WORKSHEET))
#define GAP_IS_WORKSHEET_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_WORKSHEET))
#define GAP_WORKSHEET_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_WORKSHEET, GapWorksheetClass))

#define GAP_TYPE_STATE                 (gap_state_get_type ())

typedef struct _GapWorksheet        GapWorksheet;
typedef struct _GapWorksheetPrivate GapWorksheetPrivate;
typedef struct _GapWorksheetClass   GapWorksheetClass;

typedef enum {
    GAP_DEAD = 0,
    GAP_IN_PROMPT,
    GAP_LOADING,
    GAP_BUSY,
    GAP_BUSY_INTERNAL
} GapState;

struct _GapWorksheet
{
    MooWorksheet base;
    GapWorksheetPrivate *priv;
};

struct _GapWorksheetClass
{
    MooWorksheetClass base_class;
};


GType       gap_worksheet_get_type      (void) G_GNUC_CONST;
GType       gap_state_get_type          (void) G_GNUC_CONST;


G_END_DECLS

#endif /* GAP_WORKSHEET_H */
