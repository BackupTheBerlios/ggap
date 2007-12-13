/*
 *   gapwswindow.h
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

#ifndef GAP_WS_WINDOW_H
#define GAP_WS_WINDOW_H

#include <mooui/mdwindow.h>
#include <gtk/gtkstatusbar.h>
#include <gtk/gtkprogressbar.h>

G_BEGIN_DECLS


#define GAP_TYPE_WS_WINDOW             (gap_ws_window_get_type ())
#define GAP_WS_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAP_TYPE_WS_WINDOW, GapWsWindow))
#define GAP_WS_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_WS_WINDOW, GapWsWindowClass))
#define GAP_IS_WS_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAP_TYPE_WS_WINDOW))
#define GAP_IS_WS_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_WS_WINDOW))
#define GAP_WS_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_WS_WINDOW, GapWsWindowClass))


typedef struct GapWsWindow        GapWsWindow;
typedef struct GapWsWindowPrivate GapWsWindowPrivate;
typedef struct GapWsWindowClass   GapWsWindowClass;


struct GapWsWindow
{
    MdWindow base;
    GapWsWindowPrivate *priv;
};

struct GapWsWindowClass
{
    MdWindowClass base_class;
};


GType   gap_ws_window_get_type  (void) G_GNUC_CONST;


G_END_DECLS

#endif /* GAP_WS_WINDOW_H */
