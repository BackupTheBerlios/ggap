/*
 *   moocanvasline.h
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

#ifndef __MOO_CANVAS_LINE_H__
#define __MOO_CANVAS_LINE_H__

#include <moocanvas/moocanvas.h>

G_BEGIN_DECLS


#define MOO_TYPE_CANVAS_LINE             (moo_canvas_line_get_type ())
#define MOO_CANVAS_LINE(object)          (G_TYPE_CHECK_INSTANCE_CAST ((object), MOO_TYPE_CANVAS_LINE, MooCanvasLine))
#define MOO_CANVAS_LINE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_CANVAS_LINE, MooCanvasLineClass))
#define MOO_IS_CANVAS_LINE(object)       (G_TYPE_CHECK_INSTANCE_TYPE ((object), MOO_TYPE_CANVAS_LINE))
#define MOO_IS_CANVAS_LINE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_CANVAS_LINE))
#define MOO_CANVAS_LINE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_CANVAS_LINE, MooCanvasLineClass))


typedef struct _MooCanvasLine         MooCanvasLine;
typedef struct _MooCanvasLineClass    MooCanvasLineClass;

struct _MooCanvasLine
{
    MooCanvasItem base;

    double x1;
    double y1;
    double x2;
    double y2;
};

struct _MooCanvasLineClass
{
    MooCanvasItemClass base_class;
};


GType       moo_canvas_line_get_type    (void) G_GNUC_CONST;

void        moo_canvas_line_set_start   (MooCanvasLine  *line,
                                         double          x,
                                         double          y);
void        moo_canvas_line_set_end     (MooCanvasLine  *line,
                                         double          x,
                                         double          y);


G_END_DECLS

#endif /* __MOO_CANVAS_LINE_H__ */
