/*
 *   moocanvasitem.c
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

#include "moocanvas/moocanvas.h"
#include "mooutils/moomarshals.h"
#include <string.h>


static void     moo_canvas_item_set_property    (GObject        *object,
                                                 guint           property_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void     moo_canvas_item_get_property    (GObject        *object,
                                                 guint           property_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);

static void     moo_canvas_item_realize_real    (MooCanvasItem  *item);
static void     moo_canvas_item_unrealize_real  (MooCanvasItem  *item);
static gboolean moo_canvas_item_event_real      (MooCanvasItem  *item,
                                                 GdkEvent       *event);


G_DEFINE_ABSTRACT_TYPE (MooCanvasItem, moo_canvas_item, GTK_TYPE_OBJECT)


enum {
    PROP_0,
    PROP_LINE_WIDTH,
    PROP_FOREGROUND,
    PROP_BACKGROUND,
    PROP_FONT_NAME
};

enum {
    REALIZE,
    UNREALIZE,
    EVENT,
    DRAW,
    N_SIGNALS
};

static guint signals[N_SIGNALS];


static void
moo_canvas_item_init (MooCanvasItem *item)
{
    item->style = moo_canvas_style_new ();
}


inline static double
int_to_double (int x)
{
    return x;
}

inline static int
double_to_int (double x)
{
    int ix = x;

    if (x - ix > .5)
        ix += 1;
    else if (x - ix < -.5)
        ix -= 1;

    return ix;
}


static void
moo_canvas_item_destroy (GtkObject *object)
{
    MooCanvasItem *item = MOO_CANVAS_ITEM (object);

    if (MOO_CANVAS_ITEM_REALIZED (item))
        moo_canvas_item_unrealize (item);

    if (item->canvas)
        moo_canvas_remove (MOO_CANVAS (item->canvas), item);

    if (item->style)
    {
        moo_canvas_style_free (item->style);
        item->style = NULL;
    }

    GTK_OBJECT_CLASS (moo_canvas_item_parent_class)->destroy (object);
}


static void
moo_canvas_item_class_init (MooCanvasItemClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);

    gobject_class->set_property = moo_canvas_item_set_property;
    gobject_class->get_property = moo_canvas_item_get_property;
    gtkobject_class->destroy = moo_canvas_item_destroy;

    klass->realize = moo_canvas_item_realize_real;
    klass->unrealize = moo_canvas_item_unrealize_real;
    klass->event = moo_canvas_item_event_real;

    g_object_class_install_property (gobject_class, PROP_LINE_WIDTH,
                                     g_param_spec_int ("line-width", "line-width", "line-width",
                                                       0, G_MAXINT, 1,
                                                       G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class, PROP_FOREGROUND,
                                     g_param_spec_string ("foreground", "foreground", "foreground",
                                                          NULL, G_PARAM_WRITABLE));

    g_object_class_install_property (gobject_class, PROP_BACKGROUND,
                                     g_param_spec_string ("background", "background", "background",
                                                          NULL, G_PARAM_WRITABLE));

    g_object_class_install_property (gobject_class, PROP_FONT_NAME,
                                     g_param_spec_string ("font-name", "font-name", "font-name",
                                                          NULL, G_PARAM_WRITABLE));

    signals[REALIZE] =
            g_signal_new ("realize",
                          G_TYPE_FROM_CLASS (klass),
                          G_SIGNAL_RUN_FIRST,
                          G_STRUCT_OFFSET (MooCanvasItemClass, realize),
                          NULL, NULL,
                          _moo_marshal_VOID__VOID,
                          G_TYPE_NONE, 0);

    signals[UNREALIZE] =
            g_signal_new ("unrealize",
                          G_TYPE_FROM_CLASS (klass),
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (MooCanvasItemClass, unrealize),
                          NULL, NULL,
                          _moo_marshal_VOID__VOID,
                          G_TYPE_NONE, 0);

    signals[EVENT] =
            g_signal_new ("event",
                          G_TYPE_FROM_CLASS (klass),
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (MooCanvasItemClass, event),
                          g_signal_accumulator_true_handled, NULL,
                          _moo_marshal_BOOLEAN__BOXED,
                          G_TYPE_BOOLEAN, 1,
                          GDK_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE);

    signals[DRAW] =
            g_signal_new ("draw",
                          G_TYPE_FROM_CLASS (klass),
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (MooCanvasItemClass, draw),
                          g_signal_accumulator_true_handled, NULL,
                          _moo_marshal_BOOLEAN__BOXED_OBJECT,
                          G_TYPE_BOOLEAN, 2,
                          GDK_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE,
                          GDK_TYPE_DRAWABLE);
}


static void
moo_canvas_item_realize_real (MooCanvasItem *item)
{
    GdkGCValues values;
    GdkGCValuesMask mask;

    g_assert (item->canvas != NULL && item->canvas->window != NULL);

    MOO_CANVAS_ITEM_SET_FLAGS (item, MOO_CANVAS_REALIZED);

    gdk_colormap_alloc_color (gtk_widget_get_colormap (item->canvas),
                              &item->style->fg, TRUE, TRUE);
    gdk_colormap_alloc_color (gtk_widget_get_colormap (item->canvas),
                              &item->style->bg, TRUE, TRUE);

    mask = GDK_GC_LINE_WIDTH | GDK_GC_FOREGROUND | GDK_GC_BACKGROUND;
    values.line_width = item->style->line_width;
    values.foreground = item->style->fg;
    values.background = item->style->bg;

    item->gc = gdk_gc_new_with_values (item->canvas->window, &values, mask);
}


static void
moo_canvas_item_unrealize_real (MooCanvasItem *item)
{
    MOO_CANVAS_ITEM_UNSET_FLAGS (item, MOO_CANVAS_REALIZED);

    if (item->gc)
        g_object_unref (item->gc);
    item->gc = NULL;
}


static gboolean
moo_canvas_item_event_real (MooCanvasItem  *item,
                            GdkEvent       *event)
{
    g_return_val_if_fail (MOO_IS_CANVAS_ITEM (item), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    return FALSE;
}


static void
moo_canvas_item_set_property (GObject        *object,
                              guint           property_id,
                              const GValue   *value,
                              GParamSpec     *pspec)
{
    const char *sv;
    MooCanvasItem *item = MOO_CANVAS_ITEM (object);

    moo_canvas_item_invalidate (item);

    switch (property_id)
    {
        case PROP_LINE_WIDTH:
            if (item->style->line_width != g_value_get_int (value))
            {
                item->style->line_width = g_value_get_int (value);

                if (item->gc)
                    gdk_gc_set_line_attributes (item->gc,
                                                item->style->line_width,
                                                GDK_LINE_SOLID,
                                                GDK_CAP_BUTT,
                                                GDK_JOIN_MITER);

                g_object_notify (object, "line-width");
            }
            break;

        case PROP_FOREGROUND:
            sv = g_value_get_string (value);

            if (sv)
            {
                gdk_color_parse (sv, &item->style->fg);

                if (MOO_CANVAS_ITEM_REALIZED (item))
                    gdk_colormap_alloc_color (gtk_widget_get_colormap (item->canvas),
                                              &item->style->fg, TRUE, TRUE);

                if (item->gc)
                    gdk_gc_set_foreground (item->gc, &item->style->fg);

                g_object_notify (object, "foreground");
            }
            break;

        case PROP_BACKGROUND:
            sv = g_value_get_string (value);

            if (sv)
            {
                gdk_color_parse (sv, &item->style->bg);

                if (MOO_CANVAS_ITEM_REALIZED (item))
                    gdk_colormap_alloc_color (gtk_widget_get_colormap (item->canvas),
                                              &item->style->bg, TRUE, TRUE);

                if (item->gc)
                    gdk_gc_set_background (item->gc, &item->style->bg);

                g_object_notify (object, "background");
            }
            break;

        case PROP_FONT_NAME:
            sv = g_value_get_string (value);

            if (!sv)
            {
                if (item->style->font)
                    pango_font_description_free (item->style->font);
                item->style->font = NULL;
            }
            else
            {
                if (item->style->font)
                    pango_font_description_free (item->style->font);
                item->style->font = pango_font_description_from_string (sv);
            }

            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }

    moo_canvas_item_invalidate (item);
}


static void
moo_canvas_item_get_property (GObject        *object,
                              guint           property_id,
                              GValue         *value,
                              GParamSpec     *pspec)
{
    MooCanvasItem *item = MOO_CANVAS_ITEM (object);

    switch (property_id)
    {
        case PROP_LINE_WIDTH:
            g_value_set_int (value, item->style->line_width);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}


/*****************************************************************************/
/* Api for Container imlpementations
 */

void
moo_canvas_item_realize (MooCanvasItem *item)
{
    g_return_if_fail (MOO_IS_CANVAS_ITEM (item));
    g_return_if_fail (item->canvas != NULL);

    if (!MOO_CANVAS_ITEM_REALIZED (item))
        g_signal_emit (item, signals[REALIZE], 0);
}


void
moo_canvas_item_unrealize (MooCanvasItem *item)
{
    g_return_if_fail (MOO_IS_CANVAS_ITEM (item));

    if (MOO_CANVAS_ITEM_REALIZED (item))
    {
        g_object_ref (item);
        g_signal_emit (item, signals[UNREALIZE], 0);
        MOO_CANVAS_ITEM_UNSET_FLAGS (item, MOO_CANVAS_REALIZED);
        g_object_unref (item);
    }
}


void
_moo_canvas_item_set_parent (MooCanvasItem  *item,
                             GtkWidget      *widget)
{
    g_return_if_fail (MOO_IS_CANVAS_ITEM (item));
    g_return_if_fail (MOO_IS_CANVAS (widget));
    g_return_if_fail (item->canvas == NULL);
    item->canvas = widget;
}


void
_moo_canvas_item_unparent (MooCanvasItem *item)
{
    g_return_if_fail (MOO_IS_CANVAS_ITEM (item));
    item->canvas = NULL;
}


gboolean
_moo_canvas_item_event (MooCanvasItem  *item,
                        GdkEvent       *event)
{
    gboolean retval = FALSE;

    g_return_val_if_fail (MOO_IS_CANVAS_ITEM (item), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    g_signal_emit (item, signals[EVENT], 0, event, &retval);
    return retval;
}


gboolean
_moo_canvas_item_draw (MooCanvasItem  *item,
                       GdkEventExpose *event,
                       GdkDrawable    *dest)
{
    gboolean retval = FALSE;

    g_return_val_if_fail (MOO_IS_CANVAS_ITEM (item), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);
    g_return_val_if_fail (dest != NULL, FALSE);

    g_signal_emit (item, signals[DRAW], 0, event, dest, &retval);
    return retval;
}


/*****************************************************************************/
/* public api
 */

void
moo_canvas_item_move (MooCanvasItem  *item,
                      double          dx,
                      double          dy)
{
    g_return_if_fail (MOO_IS_CANVAS_ITEM (item));

    if (!MOO_CANVAS_ITEM_GET_CLASS (item)->move)
    {
        g_warning ("MooCanvasItem::move not implemented in %s",
                   g_type_name (G_OBJECT_TYPE (item)));
        return;
    }

    MOO_CANVAS_ITEM_GET_CLASS (item)->move (item, dx, dy);
}


void
moo_canvas_item_invalidate (MooCanvasItem *item)
{
    GdkRegion *region;

    g_return_if_fail (MOO_IS_CANVAS_ITEM (item));

    if (!MOO_CANVAS_ITEM_REALIZED (item))
        return;

    region = moo_canvas_item_get_region (item);
    g_return_if_fail (region != NULL);

    moo_canvas_invalidate (MOO_CANVAS (item->canvas), region);
    gdk_region_destroy (region);
}


GdkRegion *
moo_canvas_item_get_region (MooCanvasItem *item)
{
    g_return_val_if_fail (MOO_IS_CANVAS_ITEM (item), NULL);

    if (!MOO_CANVAS_ITEM_GET_CLASS (item)->get_region)
    {
        g_warning ("MooCanvasItem::get_region not implemented in %s",
                   g_type_name (G_OBJECT_TYPE (item)));
        return NULL;
    }

    return MOO_CANVAS_ITEM_GET_CLASS (item)->get_region (item);
}


void
moo_canvas_widget_to_item (G_GNUC_UNUSED MooCanvasItem *item,
                           int             wx,
                           int             wy,
                           double         *ix,
                           double         *iy)
{
    g_assert (MOO_IS_CANVAS_ITEM (item));
    g_assert (item->canvas);
    g_assert (ix && iy);
    *ix = int_to_double (wx);
    *iy = int_to_double (wy);
}


void
moo_canvas_item_to_widget (G_GNUC_UNUSED MooCanvasItem *item,
                           double          ix,
                           double          iy,
                           int            *wx,
                           int            *wy)
{
    g_assert (MOO_IS_CANVAS_ITEM (item));
    g_assert (wx && wy);
    g_assert (item->canvas != NULL);
    *wx = double_to_int (ix);
    *wy = double_to_int (iy);
}


void
moo_canvas_rect_item_to_widget (G_GNUC_UNUSED MooCanvasItem *item,
                                MooCanvasRect  *irect,
                                GdkRectangle   *wrect)
{
    int x1, y1, x2, y2;

    g_assert (MOO_IS_CANVAS_ITEM (item));
    g_assert (irect && wrect);
    g_assert (item->canvas != NULL);

    x1 = double_to_int (irect->x1);
    y1 = double_to_int (irect->y1);
    x2 = double_to_int (irect->x2);
    y2 = double_to_int (irect->y2);

    wrect->x = MIN (x1, x2);
    wrect->y = MIN (y1, y2);
    wrect->width = MAX (x1, x2) - MIN (x1, x2) + 1;
    wrect->height = MAX (y1, y2) - MIN (y1, y2) + 1;
}


MooCanvasStyle *
moo_canvas_style_new (void)
{
    MooCanvasStyle *style;

    style = g_new0 (MooCanvasStyle, 1);

    style->line_width = 1;

    return style;
}


MooCanvasStyle *
moo_canvas_style_copy (MooCanvasStyle *style)
{
    MooCanvasStyle *copy;

    g_return_val_if_fail (style != NULL, NULL);

    copy = g_new0 (MooCanvasStyle, 1);
    memcpy (copy, style, sizeof (MooCanvasStyle));

    if (style->font)
        copy->font = pango_font_description_copy (style->font);

    return copy;
}


void
moo_canvas_style_free (MooCanvasStyle *style)
{
    if (style)
    {
        if (style->font)
            pango_font_description_free (style->font);

        g_free (style);
    }
}
