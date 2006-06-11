/*
 *   gapeditwindow.h
 *
 *   Copyright (C) 2004-2006 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef __GAP_EDIT_WINDOW_H__
#define __GAP_EDIT_WINDOW_H__

#include <mooedit/mooeditwindow.h>

G_BEGIN_DECLS


#define GAP_TYPE_EDIT_WINDOW             (gap_edit_window_get_type ())
#define GAP_EDIT_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAP_TYPE_EDIT_WINDOW, GapEditWindow))
#define GAP_EDIT_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_EDIT_WINDOW, GapEditWindowClass))
#define GAP_IS_EDIT_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAP_TYPE_EDIT_WINDOW))
#define GAP_IS_EDIT_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_EDIT_WINDOW))
#define GAP_EDIT_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_EDIT_WINDOW, GapEditWindowClass))


typedef struct _GapEditWindow        GapEditWindow;
typedef struct _GapEditWindowClass   GapEditWindowClass;


struct _GapEditWindow
{
    MooEditWindow base;
};

struct _GapEditWindowClass
{
    MooEditWindowClass base_class;
};


GType   gap_edit_window_get_type    (void) G_GNUC_CONST;

void    gap_edit_window_open_file   (const char *filename,
                                     int         line,
                                     GtkWidget  *widget);


G_END_DECLS

#endif /* __GAP_EDIT_WINDOW_H__ */
