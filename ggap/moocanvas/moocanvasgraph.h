/*
 *   moocanvasgraph.h
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

#ifndef __MOO_CANVAS_VERTEX_H__
#define __MOO_CANVAS_VERTEX_H__

#include <moocanvas/moocanvas.h>

G_BEGIN_DECLS


#define MOO_TYPE_CANVAS_VERTEX             (moo_canvas_vertex_get_type ())
#define MOO_CANVAS_VERTEX(object)          (G_TYPE_CHECK_INSTANCE_CAST ((object), MOO_TYPE_CANVAS_VERTEX, MooCanvasVertex))
#define MOO_CANVAS_VERTEX_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_CANVAS_VERTEX, MooCanvasVertexClass))
#define MOO_IS_CANVAS_VERTEX(object)       (G_TYPE_CHECK_INSTANCE_TYPE ((object), MOO_TYPE_CANVAS_VERTEX))
#define MOO_IS_CANVAS_VERTEX_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_CANVAS_VERTEX))
#define MOO_CANVAS_VERTEX_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_CANVAS_VERTEX, MooCanvasVertexClass))


typedef struct _MooCanvasVertex         MooCanvasVertex;
typedef struct _MooCanvasVertexClass    MooCanvasVertexClass;

struct _MooCanvasVertex
{
    MooCanvasItem base;

    double x, y;
    MooCanvasItem *box;
    MooCanvasItem *text;

    GSList *edges;
};

struct _MooCanvasVertexClass
{
    MooCanvasItemClass base_class;
};


GType   moo_canvas_vertex_get_type      (void) G_GNUC_CONST;

void    moo_canvas_vertex_connect       (MooCanvasVertex    *v1,
                                         MooCanvasVertex    *v2);
void    moo_canvas_vertex_disconnect    (MooCanvasVertex    *v1,
                                         MooCanvasVertex    *v2);


G_END_DECLS

#endif /* __MOO_CANVAS_VERTEX_H__ */
