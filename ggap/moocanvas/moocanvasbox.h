/*
 *   moocanvasbox.h
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

#ifndef __MOO_CANVAS_BOX_H__
#define __MOO_CANVAS_BOX_H__

#include <moocanvas/moocanvas.h>

G_BEGIN_DECLS


#define MOO_TYPE_CANVAS_BOX             (moo_canvas_box_get_type ())
#define MOO_CANVAS_BOX(object)          (G_TYPE_CHECK_INSTANCE_CAST ((object), MOO_TYPE_CANVAS_BOX, MooCanvasBox))
#define MOO_CANVAS_BOX_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_CANVAS_BOX, MooCanvasBoxClass))
#define MOO_IS_CANVAS_BOX(object)       (G_TYPE_CHECK_INSTANCE_TYPE ((object), MOO_TYPE_CANVAS_BOX))
#define MOO_IS_CANVAS_BOX_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_CANVAS_BOX))
#define MOO_CANVAS_BOX_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_CANVAS_BOX, MooCanvasBoxClass))


#define MOO_TYPE_CANVAS_BOX_SHAPE       (moo_canvas_box_shape_get_type ())

typedef enum {
    MOO_CANVAS_BOX_RECT,
    MOO_CANVAS_BOX_ELLIPSE,
    MOO_CANVAS_BOX_DIAMOND
} MooCanvasBoxShape;

typedef struct _MooCanvasBox         MooCanvasBox;
typedef struct _MooCanvasBoxClass    MooCanvasBoxClass;

struct _MooCanvasBox
{
    MooCanvasItem base;

    MooCanvasBoxShape shape;
    double x1, y1, x2, y2;
    guint filled : 1;
};

struct _MooCanvasBoxClass
{
    MooCanvasItemClass base_class;
};


GType   moo_canvas_box_get_type         (void) G_GNUC_CONST;
GType   moo_canvas_box_shape_get_type   (void) G_GNUC_CONST;


G_END_DECLS

#endif /* __MOO_CANVAS_BOX_H__ */
