/*
 *   mooworksheet.h
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

#ifndef MOO_WORKSHEET_H
#define MOO_WORKSHEET_H

#include "moows/moowsview.h"

G_BEGIN_DECLS


#define MOO_TYPE_WORKSHEET             (moo_worksheet_get_type ())
#define MOO_WORKSHEET(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOO_TYPE_WORKSHEET, MooWorksheet))
#define MOO_WORKSHEET_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_WORKSHEET, MooWorksheetClass))
#define MOO_IS_WORKSHEET(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOO_TYPE_WORKSHEET))
#define MOO_IS_WORKSHEET_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_WORKSHEET))
#define MOO_WORKSHEET_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_WORKSHEET, MooWorksheetClass))

typedef struct _MooWorksheet MooWorksheet;
typedef struct _MooWorksheetPrivate MooWorksheetPrivate;
typedef struct _MooWorksheetClass MooWorksheetClass;

struct _MooWorksheet
{
    MooWsView base;
    MooWorksheetPrivate *priv;
};

struct _MooWorksheetClass
{
    MooWsViewClass base_class;

    void (*process_input) (MooWorksheet  *sheet,
                           char         **lines);
};


GType       moo_worksheet_get_type          (void) G_GNUC_CONST;

void        moo_worksheet_reset             (MooWorksheet   *ws);
gboolean    moo_worksheet_accepting_input   (MooWorksheet   *ws);
void        moo_worksheet_resume_input      (MooWorksheet   *ws,
                                             int             line,
                                             int             column);
void        moo_worksheet_continue_input    (MooWorksheet   *ws);
void        moo_worksheet_start_input       (MooWorksheet   *ws,
                                             const char     *ps,
                                             const char     *ps2);

void        moo_worksheet_add_history       (MooWorksheet   *ws,
                                             const char     *string);
void        moo_worksheet_reset_history     (MooWorksheet   *ws);

void        moo_worksheet_write_output      (MooWorksheet   *ws,
                                             const char     *string,
                                             gssize          len);
void        moo_worksheet_write_error       (MooWorksheet   *ws,
                                             const char     *format,
                                             ...);
void        moo_worksheet_write_error_len   (MooWorksheet   *ws,
                                             const char     *string,
                                             gssize          len);


G_END_DECLS

#endif /* MOO_WORKSHEET_H */
