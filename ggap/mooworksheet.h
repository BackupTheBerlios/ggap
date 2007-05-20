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

#include <mooedit/mootextview.h>

G_BEGIN_DECLS


#define MOO_TYPE_WORKSHEET             (moo_worksheet_get_type ())
#define MOO_WORKSHEET(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOO_TYPE_WORKSHEET, MooWorksheet))
#define MOO_WORKSHEET_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_WORKSHEET, MooWorksheetClass))
#define MOO_IS_WORKSHEET(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOO_TYPE_WORKSHEET))
#define MOO_IS_WORKSHEET_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_WORKSHEET))
#define MOO_WORKSHEET_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_WORKSHEET, MooWorksheetClass))


typedef struct _MooWorksheet        MooWorksheet;
typedef struct _MooWorksheetPrivate MooWorksheetPrivate;
typedef struct _MooWorksheetClass   MooWorksheetClass;


struct _MooWorksheet
{
    MooTextView base;
    MooWorksheetPrivate *priv;
};

struct _MooWorksheetClass
{
    MooTextViewClass base_class;
    gboolean (*input_complete) (MooWorksheet *sheet,
                                const char   *input);
    void     (*process_input)  (MooWorksheet *sheet,
                                const char   *input);
};


GType       moo_worksheet_get_type          (void) G_GNUC_CONST;

void        moo_worksheet_set_ps1           (MooWorksheet   *sheet,
                                             const char     *prompt);
char       *moo_worksheet_get_ps1           (MooWorksheet   *sheet);
void        moo_worksheet_set_ps2           (MooWorksheet   *sheet,
                                             const char     *prompt);
char       *moo_worksheet_get_ps2           (MooWorksheet   *sheet);

void        moo_worksheet_reset             (MooWorksheet   *sheet);
void        moo_worksheet_start_input       (MooWorksheet   *sheet);
gboolean    moo_worksheet_accepting_input   (MooWorksheet   *sheet);
void        moo_worksheet_write_output      (MooWorksheet   *sheet,
                                             const char     *text);
void        moo_worksheet_write_error       (MooWorksheet   *sheet,
                                             const char     *text);


G_END_DECLS

#endif /* GAP_WORKSHEET_H */
