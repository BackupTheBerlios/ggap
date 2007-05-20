/*
 *   gapwindow.h
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

#ifndef GAP_WINDOW_H
#define GAP_WINDOW_H

#include <mooutils/moowindow.h>
#include "gapview.h"

G_BEGIN_DECLS


#define GAP_TYPE_WINDOW             (gap_window_get_type ())
#define GAP_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAP_TYPE_WINDOW, GapWindow))
#define GAP_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_WINDOW, GapWindowClass))
#define GAP_IS_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAP_TYPE_WINDOW))
#define GAP_IS_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_WINDOW))
#define GAP_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_WINDOW, GapWindowClass))


typedef struct _GapWindow        GapWindow;
typedef struct _GapWindowPrivate GapWindowPrivate;
typedef struct _GapWindowClass   GapWindowClass;


struct _GapWindow
{
    MooWindow base;
    GType view_type;
    GapView *terminal;
};

struct _GapWindowClass
{
    MooWindowClass  base_class;
};


GType       gap_window_get_type         (void) G_GNUC_CONST;

GapView    *gap_window_get_terminal     (GapWindow  *window);


G_END_DECLS

#endif /* GAP_WINDOW_H */
