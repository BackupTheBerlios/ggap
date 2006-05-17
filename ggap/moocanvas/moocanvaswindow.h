/*
 *   moocanvaswindow.h
 *
 *   Copyright (C) 2006 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef __MOO_CANVAS_WINDOW_H__
#define __MOO_CANVAS_WINDOW_H__

#include <gtk/gtkwindow.h>
#include <moocanvas/moocanvas.h>

G_BEGIN_DECLS


#define MOO_TYPE_CANVAS_WINDOW             (moo_canvas_window_get_type ())
#define MOO_CANVAS_WINDOW(object)          (G_TYPE_CHECK_INSTANCE_CAST ((object), MOO_TYPE_CANVAS_WINDOW, MooCanvasWindow))
#define MOO_CANVAS_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_CANVAS_WINDOW, MooCanvasWindowClass))
#define MOO_IS_CANVAS_WINDOW(object)       (G_TYPE_CHECK_INSTANCE_TYPE ((object), MOO_TYPE_CANVAS_WINDOW))
#define MOO_IS_CANVAS_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_CANVAS_WINDOW))
#define MOO_CANVAS_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_CANVAS_WINDOW, MooCanvasWindowClass))


typedef struct _MooCanvasWindow         MooCanvasWindow;
typedef struct _MooCanvasWindowClass    MooCanvasWindowClass;

struct _MooCanvasWindow
{
    GtkWindow parent;

    GtkWidget *menubar;
    GtkWidget *canvas;
    GtkWidget *statusbar;
};

struct _MooCanvasWindowClass
{
    GtkWindowClass parent_class;
};


GType        moo_canvas_window_get_type     (void) G_GNUC_CONST;

GtkWidget   *moo_canvas_window_new          (void);

MooCanvas   *moo_canvas_window_get_canvas   (MooCanvasWindow    *window);


G_END_DECLS

#endif /* __MOO_CANVAS_WINDOW_H__ */
