/*
 *   moocanvastext.c
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

#include "moocanvas/moocanvastext.h"
#include <string.h>


static void     moo_canvas_text_set_property    (GObject        *object,
                                                 guint           property_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void     moo_canvas_text_get_property    (GObject        *object,
                                                 guint           property_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);

static void     moo_canvas_text_realize         (MooCanvasItem  *item);
static GdkRegion *moo_canvas_text_get_region    (MooCanvasItem  *item);
static void     moo_canvas_text_move            (MooCanvasItem  *item,
                                                 double          dx,
                                                 double          dy);
static void     moo_canvas_text_draw            (MooCanvasItem  *item,
                                                 GdkEventExpose *event,
                                                 GdkDrawable    *dest);

static void     moo_canvas_text_set_coords      (MooCanvasText  *text,
                                                 double          x,
                                                 double          y);
static void     moo_canvas_text_set_text        (MooCanvasText  *text,
                                                 const char     *string);


G_DEFINE_TYPE (MooCanvasText, moo_canvas_text, MOO_TYPE_CANVAS_ITEM)


enum {
    TEXT_PROP_0,
    TEXT_PROP_X,
    TEXT_PROP_Y,
    TEXT_PROP_TEXT
};


static void
moo_canvas_text_init (MooCanvasText *text)
{
    text->text = g_strdup ("");
}


static void
moo_canvas_text_class_init (MooCanvasTextClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    MooCanvasItemClass *item_class = MOO_CANVAS_ITEM_CLASS (klass);

    gobject_class->set_property = moo_canvas_text_set_property;
    gobject_class->get_property = moo_canvas_text_get_property;

    item_class->realize = moo_canvas_text_realize;
    item_class->get_region = moo_canvas_text_get_region;
    item_class->move = moo_canvas_text_move;
    item_class->draw = moo_canvas_text_draw;

    g_object_class_install_property (gobject_class, TEXT_PROP_TEXT,
                                     g_param_spec_string ("text", "text", "text", "",
                                                          G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class, TEXT_PROP_X,
                                     g_param_spec_int ("x", "x", "x",
                                                       G_MININT, G_MAXINT, 0,
                                                       G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class, TEXT_PROP_Y,
                                     g_param_spec_int ("y", "y", "y",
                                                       G_MININT, G_MAXINT, 0,
                                                       G_PARAM_READWRITE));
}


static void
moo_canvas_text_move (MooCanvasItem *item,
                      double         dx,
                      double         dy)
{
    MooCanvasText *text = MOO_CANVAS_TEXT (item);
    moo_canvas_text_set_coords (text, text->x + dx, text->y + dy);
}


static void
moo_canvas_text_set_property (GObject        *object,
                              guint           property_id,
                              const GValue   *value,
                              GParamSpec     *pspec)
{
    MooCanvasText *text = MOO_CANVAS_TEXT (object);

    switch (property_id)
    {
        case TEXT_PROP_TEXT:
            moo_canvas_text_set_text (text, g_value_get_string (value));
            break;
        case TEXT_PROP_X:
            moo_canvas_text_set_coords (text, g_value_get_int (value), text->y);
            break;
        case TEXT_PROP_Y:
            moo_canvas_text_set_coords (text, text->x, g_value_get_int (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}


static void
moo_canvas_text_get_property (GObject        *object,
                              guint           property_id,
                              GValue         *value,
                              GParamSpec     *pspec)
{
    MooCanvasText *text = MOO_CANVAS_TEXT (object);

    switch (property_id)
    {
        case TEXT_PROP_TEXT:
            g_value_set_string (value, text->text);
            break;
        case TEXT_PROP_X:
            g_value_set_int (value, text->x);
            break;
        case TEXT_PROP_Y:
            g_value_set_int (value, text->y);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}


static void
moo_canvas_text_set_coords (MooCanvasText   *text,
                            double           x,
                            double           y)
{
    moo_canvas_item_invalidate (MOO_CANVAS_ITEM (text));

    text->x = x;
    text->y = y;

    moo_canvas_item_invalidate (MOO_CANVAS_ITEM (text));
}


static void
moo_canvas_text_set_text (MooCanvasText *text,
                          const char    *string)
{
    char *tmp;

    if (!string)
        string = "";

    if (!strcmp (string, text->text))
        return;

    tmp = text->text;
    text->text = g_strdup (string);
    g_free (tmp);

    if (text->layout)
        pango_layout_set_text (text->layout, text->text, -1);

    moo_canvas_item_invalidate (MOO_CANVAS_ITEM (text));
}


static void
moo_canvas_text_realize (MooCanvasItem *item)
{
    MooCanvasText *text = MOO_CANVAS_TEXT (item);

    MOO_CANVAS_ITEM_CLASS(moo_canvas_text_parent_class)->realize (item);

    text->layout = pango_layout_new (gtk_widget_get_pango_context (item->canvas));
    pango_layout_set_text (text->layout, text->text, -1);

    if (item->style->font)
        pango_layout_set_font_description (text->layout, item->style->font);
}


static GdkRegion *
moo_canvas_text_get_region (MooCanvasItem *item)
{
    GdkRectangle rect;
    PangoRectangle prect;
    MooCanvasText *text = MOO_CANVAS_TEXT (item);

    g_return_val_if_fail (text->layout != NULL, NULL);

    moo_canvas_item_to_widget (item, text->x, text->y,
                               &rect.x, &rect.y);
    pango_layout_get_pixel_extents (text->layout, &prect, NULL);
    rect.x += prect.x;
    rect.y += prect.y;
    rect.width = prect.width;
    rect.height = prect.height;

    return gdk_region_rectangle (&rect);
}


static void
moo_canvas_text_draw (MooCanvasItem  *item,
                      GdkEventExpose *event,
                      GdkDrawable    *dest)
{
    int x, y;
    MooCanvasText *text = MOO_CANVAS_TEXT (item);

    moo_canvas_item_to_widget (item, text->x, text->y, &x, &y);
    gdk_gc_set_clip_region (item->gc, event->region);
    gdk_draw_layout (dest, item->gc, x, y, text->layout);
}
