/*
 *   moocanvasline.c
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

#include "moocanvas/moocanvasline.h"


static void     moo_canvas_line_set_property    (GObject        *object,
                                                 guint           property_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void     moo_canvas_line_get_property    (GObject        *object,
                                                 guint           property_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);

static void     moo_canvas_line_realize         (MooCanvasItem  *item);
static void     moo_canvas_line_move            (MooCanvasItem  *item,
                                                 double          dx,
                                                 double          dy);
static void     moo_canvas_line_draw            (MooCanvasItem  *item,
                                                 GdkEventExpose *event,
                                                 GdkDrawable    *dest);

static void     moo_canvas_line_set_ends        (MooCanvasLine  *line,
                                                 double          x1,
                                                 double          y1,
                                                 double          x2,
                                                 double          y2);


G_DEFINE_TYPE (MooCanvasLine, moo_canvas_line, MOO_TYPE_CANVAS_ITEM)


enum {
    PROP_0,
    PROP_X1,
    PROP_Y1,
    PROP_X2,
    PROP_Y2
};


static void
moo_canvas_line_init (G_GNUC_UNUSED MooCanvasLine *line)
{
}


static void
moo_canvas_line_class_init (MooCanvasLineClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    MooCanvasItemClass *item_class = MOO_CANVAS_ITEM_CLASS (klass);

    gobject_class->set_property = moo_canvas_line_set_property;
    gobject_class->get_property = moo_canvas_line_get_property;

    item_class->move = moo_canvas_line_move;
    item_class->draw = moo_canvas_line_draw;
    item_class->realize = moo_canvas_line_realize;

    g_object_class_install_property (gobject_class, PROP_X1,
                                     g_param_spec_int ("x1", "x1", "x1",
                                             G_MININT, G_MAXINT, 0,
                                             G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class, PROP_Y1,
                                     g_param_spec_int ("y1", "y1", "y1",
                                             G_MININT, G_MAXINT, 0,
                                             G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class, PROP_X2,
                                     g_param_spec_int ("x2", "x2", "x2",
                                             G_MININT, G_MAXINT, 0,
                                             G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class, PROP_Y2,
                                     g_param_spec_int ("y2", "y2", "y2",
                                             G_MININT, G_MAXINT, 0,
                                             G_PARAM_READWRITE));
}


static void
moo_canvas_line_move (MooCanvasItem *item,
                      double         dx,
                      double         dy)
{
    MooCanvasLine *line = MOO_CANVAS_LINE (item);
    moo_canvas_line_set_ends (line, line->x1 + dx, line->y1 + dy,
                              line->x2 + dx, line->y2 + dy);
}


static void
moo_canvas_line_realize (MooCanvasItem *item)
{
    MOO_CANVAS_ITEM_CLASS(moo_canvas_line_parent_class)->realize (item);
    gdk_gc_set_line_attributes (item->gc, item->style->line_width,
                                GDK_LINE_SOLID, GDK_CAP_PROJECTING,
                                GDK_JOIN_MITER);
}


static void
moo_canvas_line_set_property (GObject        *object,
                              guint           property_id,
                              const GValue   *value,
                              GParamSpec     *pspec)
{
    MooCanvasLine *line = MOO_CANVAS_LINE (object);

    switch (property_id)
    {
        case PROP_X1:
            moo_canvas_line_set_ends (line, g_value_get_int (value),
                                      line->y1, line->x2, line->y2);
            g_object_notify (object, "x1");
            break;
        case PROP_Y1:
            moo_canvas_line_set_ends (line, line->x1, g_value_get_int (value),
                                      line->x2, line->y2);
            g_object_notify (object, "y1");
            break;
        case PROP_X2:
            moo_canvas_line_set_ends (line, line->x1, line->y1,
                                      g_value_get_int (value), line->y2);
            g_object_notify (object, "x2");
            break;
        case PROP_Y2:
            moo_canvas_line_set_ends (line, line->x1, line->y1,
                                      line->x2, g_value_get_int (value));
            g_object_notify (object, "y2");
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}


static void
moo_canvas_line_get_property (GObject        *object,
                              guint           property_id,
                              GValue         *value,
                              GParamSpec     *pspec)
{
    MooCanvasLine *line = MOO_CANVAS_LINE (object);

    switch (property_id)
    {
        case PROP_X1:
            g_value_set_int (value, line->x1);
            break;
        case PROP_Y1:
            g_value_set_int (value, line->y1);
            break;
        case PROP_X2:
            g_value_set_int (value, line->x2);
            break;
        case PROP_Y2:
            g_value_set_int (value, line->y2);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}


static void
moo_canvas_line_set_ends (MooCanvasLine  *line,
                          double          x1,
                          double          y1,
                          double          x2,
                          double          y2)
{
    moo_canvas_item_invalidate (MOO_CANVAS_ITEM (line));
    line->x1 = x1;
    line->y1 = y1;
    line->x2 = x2;
    line->y2 = y2;
    moo_canvas_item_invalidate (MOO_CANVAS_ITEM (line));
}


void
moo_canvas_line_set_start (MooCanvasLine  *line,
                           double          x,
                           double          y)
{
    g_return_if_fail (MOO_IS_CANVAS_LINE (line));
    moo_canvas_line_set_ends (line, x, y, line->x2, line->y2);
}


void
moo_canvas_line_set_end (MooCanvasLine  *line,
                         double          x,
                         double          y)
{
    g_return_if_fail (MOO_IS_CANVAS_LINE (line));
    moo_canvas_line_set_ends (line, line->x1, line->y1, x, y);
}


static void
moo_canvas_line_draw (MooCanvasItem  *item,
                      GdkEventExpose *event,
                      GdkDrawable    *dest)
{
    int x1, y1, x2, y2;
    MooCanvasLine *line = MOO_CANVAS_LINE (item);
    gdk_gc_set_clip_region (item->gc, event->region);
    moo_canvas_item_to_widget (item, line->x1, line->y1, &x1, &y1);
    moo_canvas_item_to_widget (item, line->x2, line->y2, &x2, &y2);
    gdk_draw_line (dest, item->gc, x1, y1, x2, y2);
}
