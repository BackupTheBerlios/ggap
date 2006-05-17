/*
 *   moocanvasbox.c
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

#include "moocanvas/moocanvasbox.h"


static void     moo_canvas_box_set_property     (GObject        *object,
                                                 guint           property_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void     moo_canvas_box_get_property     (GObject        *object,
                                                 guint           property_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);

static void     moo_canvas_box_realize          (MooCanvasItem  *item);
static GdkRegion *moo_canvas_box_get_region     (MooCanvasItem  *item);
static void     moo_canvas_box_move             (MooCanvasItem  *item,
                                                 double          dx,
                                                 double          dy);
static void     moo_canvas_box_draw             (MooCanvasItem  *item,
                                                 GdkEventExpose *event,
                                                 GdkDrawable    *dest);

static void     moo_canvas_box_set_coords       (MooCanvasBox   *box,
                                                 double          x1,
                                                 double          y1,
                                                 double          x2,
                                                 double          y2);
static void     moo_canvas_box_set_filled       (MooCanvasBox   *box,
                                                 gboolean        filled);


G_DEFINE_TYPE (MooCanvasBox, moo_canvas_box, MOO_TYPE_CANVAS_ITEM)


enum {
    BOX_PROP_0,
    BOX_PROP_X1,
    BOX_PROP_Y1,
    BOX_PROP_X2,
    BOX_PROP_Y2,
    BOX_PROP_FILLED,
    BOX_PROP_SHAPE
};


static void
moo_canvas_box_init (G_GNUC_UNUSED MooCanvasBox *box)
{
}


static void
moo_canvas_box_class_init (MooCanvasBoxClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    MooCanvasItemClass *item_class = MOO_CANVAS_ITEM_CLASS (klass);

    gobject_class->set_property = moo_canvas_box_set_property;
    gobject_class->get_property = moo_canvas_box_get_property;

    item_class->realize = moo_canvas_box_realize;
    item_class->get_region = moo_canvas_box_get_region;
    item_class->move = moo_canvas_box_move;
    item_class->draw = moo_canvas_box_draw;

    g_object_class_install_property (gobject_class, BOX_PROP_SHAPE,
                                     g_param_spec_enum ("shape", "shape", "shape",
                                                        MOO_TYPE_CANVAS_BOX_SHAPE,
                                                        MOO_CANVAS_BOX_RECT,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (gobject_class, BOX_PROP_X1,
                                     g_param_spec_int ("x1", "x1", "x1",
                                                       G_MININT, G_MAXINT, 0,
                                                       G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class, BOX_PROP_Y1,
                                     g_param_spec_int ("y1", "y1", "y1",
                                                       G_MININT, G_MAXINT, 0,
                                                       G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class, BOX_PROP_X2,
                                     g_param_spec_int ("x2", "x2", "x2",
                                                       G_MININT, G_MAXINT, 0,
                                                       G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class, BOX_PROP_Y2,
                                     g_param_spec_int ("y2", "y2", "y2",
                                                       G_MININT, G_MAXINT, 0,
                                                       G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class, BOX_PROP_FILLED,
                                     g_param_spec_boolean ("filled", "filled", "filled",
                                                           FALSE, G_PARAM_READWRITE));
}


static void
moo_canvas_box_move (MooCanvasItem *item,
                      double        dx,
                      double        dy)
{
    MooCanvasBox *box = MOO_CANVAS_BOX (item);
    moo_canvas_box_set_coords (box, box->x1 + dx, box->y1 + dy,
                               box->x2 + dx, box->y2 + dy);
}


static void
moo_canvas_box_set_property (GObject        *object,
                             guint           property_id,
                             const GValue   *value,
                             GParamSpec     *pspec)
{
    MooCanvasBox *box = MOO_CANVAS_BOX (object);

    switch (property_id)
    {
        case BOX_PROP_SHAPE:
            box->shape = g_value_get_enum (value);
            break;
        case BOX_PROP_X1:
            moo_canvas_box_set_coords (box, g_value_get_int (value),
                                       box->y1, box->x2, box->y2);
            break;
        case BOX_PROP_Y1:
            moo_canvas_box_set_coords (box, box->x1, g_value_get_int (value),
                                       box->x2, box->y2);
            break;
        case BOX_PROP_X2:
            moo_canvas_box_set_coords (box, box->x1, box->y1,
                                       g_value_get_int (value), box->y2);
            break;
        case BOX_PROP_Y2:
            moo_canvas_box_set_coords (box, box->x1, box->y1,
                                       box->x2, g_value_get_int (value));
            break;
        case BOX_PROP_FILLED:
            moo_canvas_box_set_filled (box, g_value_get_boolean (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}


static void
moo_canvas_box_get_property (GObject        *object,
                              guint           property_id,
                              GValue         *value,
                              GParamSpec     *pspec)
{
    MooCanvasBox *box = MOO_CANVAS_BOX (object);

    switch (property_id)
    {
        case BOX_PROP_X1:
            g_value_set_int (value, box->x1);
            break;
        case BOX_PROP_Y1:
            g_value_set_int (value, box->y1);
            break;
        case BOX_PROP_X2:
            g_value_set_int (value, box->x2);
            break;
        case BOX_PROP_Y2:
            g_value_set_int (value, box->y2);
            break;
        case BOX_PROP_FILLED:
            g_value_set_int (value, box->y2);
            break;
        case BOX_PROP_SHAPE:
            g_value_set_enum (value, box->shape);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}


static void
moo_canvas_box_set_coords (MooCanvasBox   *box,
                           double          x1,
                           double          y1,
                           double          x2,
                           double          y2)
{
    moo_canvas_item_invalidate (MOO_CANVAS_ITEM (box));

    box->x1 = x1;
    box->y1 = y1;
    box->x2 = x2;
    box->y2 = y2;

    moo_canvas_item_invalidate (MOO_CANVAS_ITEM (box));
}


static void
moo_canvas_box_set_filled (MooCanvasBox *box,
                           gboolean      filled)
{
    if (!box->filled != !filled)
    {
        box->filled = filled != 0;
        moo_canvas_item_invalidate (MOO_CANVAS_ITEM (box));
    }
}


static void
moo_canvas_box_realize (MooCanvasItem *item)
{
    MOO_CANVAS_ITEM_CLASS(moo_canvas_box_parent_class)->realize (item);

    if (MOO_CANVAS_BOX(item)->shape == MOO_CANVAS_BOX_DIAMOND)
    {
        GdkGCValues values;
        values.join_style = GDK_JOIN_MITER;
        gdk_gc_set_values (item->gc, &values, GDK_GC_JOIN_STYLE);
    }
}


static GdkRegion *
rect_get_region (MooCanvasBox *box)
{
    MooCanvasRect irect = {box->x1, box->y1, box->x2, box->y2};
    GdkRectangle wrect;
    moo_canvas_rect_item_to_widget (MOO_CANVAS_ITEM (box), &irect, &wrect);
    return gdk_region_rectangle (&wrect);
}

static GdkRegion *
ellipse_get_region (MooCanvasBox *box)
{
    MooCanvasRect irect = {box->x1, box->y1, box->x2, box->y2};
    GdkRectangle wrect;
    moo_canvas_rect_item_to_widget (MOO_CANVAS_ITEM (box), &irect, &wrect);
    return gdk_region_rectangle (&wrect);
}

static GdkRegion *
diamond_get_region (MooCanvasBox *box)
{
    MooCanvasRect irect = {box->x1, box->y1, box->x2, box->y2};
    GdkRectangle wrect;
    moo_canvas_rect_item_to_widget (MOO_CANVAS_ITEM (box), &irect, &wrect);
    return gdk_region_rectangle (&wrect);
}

static GdkRegion *
moo_canvas_box_get_region (MooCanvasItem *item)
{
    MooCanvasBox *box = MOO_CANVAS_BOX (item);

    switch (box->shape)
    {
        case MOO_CANVAS_BOX_RECT:
            return rect_get_region (box);
        case MOO_CANVAS_BOX_ELLIPSE:
            return ellipse_get_region (box);
        case MOO_CANVAS_BOX_DIAMOND:
            return diamond_get_region (box);
    }

    g_return_val_if_reached (NULL);
}


static void
rect_draw (MooCanvasItem *item,
           GdkDrawable   *dest)
{
    MooCanvasBox *box = MOO_CANVAS_BOX (item);
    MooCanvasRect irect = {box->x1, box->y1, box->x2, box->y2};
    GdkRectangle wrect;

    moo_canvas_rect_item_to_widget (MOO_CANVAS_ITEM (box), &irect, &wrect);

    if (box->filled)
    {
        gdk_gc_set_foreground (item->gc, &item->style->fg);
        gdk_draw_rectangle (dest, item->gc, TRUE,
                            wrect.x, wrect.y, wrect.width, wrect.height);

        if (MAX (wrect.width, wrect.height) > item->style->line_width * 2)
        {
            gdk_gc_set_foreground (item->gc, &item->style->bg);
            gdk_draw_rectangle (dest, item->gc, TRUE,
                                wrect.x + item->style->line_width,
                                wrect.y + item->style->line_width,
                                wrect.width - 2 * item->style->line_width,
                                wrect.height - 2 * item->style->line_width);
        }
    }
    else if (item->style->line_width < 2)
    {
        gdk_draw_rectangle (dest, item->gc, FALSE,
                            wrect.x, wrect.y, wrect.width - 1, wrect.height - 1);
    }
    else
    {
        gdk_draw_rectangle (dest, item->gc, TRUE,
                            wrect.x, wrect.y, item->style->line_width, wrect.height);
        gdk_draw_rectangle (dest, item->gc, TRUE,
                            wrect.x, wrect.y, wrect.width, item->style->line_width);
        gdk_draw_rectangle (dest, item->gc, TRUE,
                            wrect.x, wrect.y + wrect.height - item->style->line_width,
                            wrect.width, item->style->line_width);
        gdk_draw_rectangle (dest, item->gc, TRUE,
                            wrect.x + wrect.width - item->style->line_width, wrect.y,
                            item->style->line_width, wrect.height);
    }
}

static void
ellipse_draw (MooCanvasItem *item,
              GdkDrawable   *dest)
{
    MooCanvasBox *box = MOO_CANVAS_BOX (item);
    MooCanvasRect irect = {box->x1, box->y1, box->x2, box->y2};
    GdkRectangle wrect;

    moo_canvas_rect_item_to_widget (MOO_CANVAS_ITEM (box), &irect, &wrect);

    if (box->filled)
    {
        gdk_gc_set_foreground (item->gc, &item->style->fg);
        gdk_draw_arc (dest, item->gc, TRUE,
                      wrect.x, wrect.y, wrect.width, wrect.height,
                      0, 360 * 64);

        if (MAX (wrect.width, wrect.height) > item->style->line_width * 2)
        {
            gdk_gc_set_foreground (item->gc, &item->style->bg);
            gdk_draw_arc (dest, item->gc, TRUE,
                          wrect.x + item->style->line_width,
                          wrect.y + item->style->line_width,
                          wrect.width - 2 * item->style->line_width,
                          wrect.height - 2 * item->style->line_width,
                          0, 360 * 64);
        }
    }
    else if (item->style->line_width < 2)
    {
        gdk_draw_arc (dest, item->gc, FALSE,
                      wrect.x, wrect.y, wrect.width, wrect.height,
                      0, 360 * 64);
    }
    else
    {
        gdk_draw_arc (dest, item->gc, FALSE,
                      wrect.x, wrect.y, wrect.width, wrect.height,
                      0, 360 * 64);
    }
}

static void
diamond_draw (MooCanvasItem *item,
              GdkDrawable   *dest)
{
    MooCanvasBox *box = MOO_CANVAS_BOX (item);
    MooCanvasRect irect = {box->x1, box->y1, box->x2, box->y2};
    GdkRectangle wrect;
    GdkPoint points[4];

    moo_canvas_rect_item_to_widget (MOO_CANVAS_ITEM (box), &irect, &wrect);

    points[0].x = wrect.x;
    points[0].y = wrect.y + wrect.height / 2;
    points[1].x = wrect.x + wrect.width / 2;
    points[1].y = wrect.y;
    points[2].x = wrect.x + wrect.width - 1;
    points[2].y = wrect.y + wrect.height / 2;
    points[3].x = wrect.x + wrect.width / 2;
    points[3].y = wrect.y + wrect.height - 1;

    gdk_gc_set_foreground (item->gc, &item->style->fg);
    gdk_draw_polygon (dest, item->gc, box->filled,
                      points, 4);


    if (box->filled && MAX (wrect.width, wrect.height) > item->style->line_width * 2)
    {
        points[0].x += item->style->line_width;
        points[1].y += item->style->line_width;
        points[2].x -= item->style->line_width;
        points[3].y -= item->style->line_width;

        gdk_gc_set_foreground (item->gc, &item->style->bg);
        gdk_draw_polygon (dest, item->gc, TRUE,
                          points, 4);
    }
}

static void
moo_canvas_box_draw (MooCanvasItem  *item,
                     GdkEventExpose *event,
                     GdkDrawable    *dest)
{
    gdk_gc_set_clip_region (item->gc, event->region);

    switch (MOO_CANVAS_BOX(item)->shape)
    {
        case MOO_CANVAS_BOX_RECT:
            rect_draw (item, dest);
            break;
        case MOO_CANVAS_BOX_ELLIPSE:
            ellipse_draw (item, dest);
            break;
        case MOO_CANVAS_BOX_DIAMOND:
            diamond_draw (item, dest);
            break;
    }
}


GType
moo_canvas_box_shape_get_type (void)
{
    static GType type;

    if (!type)
    {
        static GEnumValue values[] = {
            { MOO_CANVAS_BOX_RECT, (char*) "MOO_CANVAS_BOX_RECT", (char*) "rect" },
            { MOO_CANVAS_BOX_ELLIPSE, (char*) "MOO_CANVAS_BOX_ELLIPSE", (char*) "ellipse" },
            { MOO_CANVAS_BOX_DIAMOND, (char*) "MOO_CANVAS_BOX_DIAMOND", (char*) "diamond" },
            { 0, NULL, NULL }
        };

        type = g_enum_register_static ("MooCanvasBoxShape", values);
    }

    return type;
}
