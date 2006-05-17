/*
 *   moocanvas.h
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

#ifndef __MOO_CANVAS_H__
#define __MOO_CANVAS_H__

#include <moocanvas/moocanvasitem.h>
#include <gtk/gtklayout.h>

G_BEGIN_DECLS


#define MOO_TYPE_CANVAS             (moo_canvas_get_type ())
#define MOO_CANVAS(object)          (G_TYPE_CHECK_INSTANCE_CAST ((object), MOO_TYPE_CANVAS, MooCanvas))
#define MOO_CANVAS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_CANVAS, MooCanvasClass))
#define MOO_IS_CANVAS(object)       (G_TYPE_CHECK_INSTANCE_TYPE ((object), MOO_TYPE_CANVAS))
#define MOO_IS_CANVAS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_CANVAS))
#define MOO_CANVAS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_CANVAS, MooCanvasClass))


typedef struct _MooCanvas         MooCanvas;
typedef struct _MooCanvasPrivate  MooCanvasPrivate;
typedef struct _MooCanvasClass    MooCanvasClass;

struct _MooCanvas
{
    GtkLayout parent;
    MooCanvasPrivate *priv;
};

struct _MooCanvasClass
{
    GtkLayoutClass parent_class;
};


typedef void (*MooCanvasCallback)   (MooCanvasItem  *item,
                                     gpointer        data);

GType            moo_canvas_get_type    (void) G_GNUC_CONST;

GtkWidget       *moo_canvas_new         (void);

gpointer         moo_canvas_add         (MooCanvas      *canvas,
                                         GType           item_type,
                                         const char     *first_prop,
                                         ...);
void             moo_canvas_add_item    (MooCanvas      *canvas,
                                         MooCanvasItem  *item);
void             moo_canvas_remove      (MooCanvas      *canvas,
                                         MooCanvasItem  *item);

void             moo_canvas_forall      (MooCanvas      *canvas,
                                         MooCanvasCallback func,
                                         gpointer        data);
GSList          *moo_canvas_list_items  (MooCanvas      *canvas);

void             moo_canvas_invalidate  (MooCanvas      *canvas,
                                         GdkRegion      *region);


G_END_DECLS

#endif /* __MOO_CANVAS_H__ */
