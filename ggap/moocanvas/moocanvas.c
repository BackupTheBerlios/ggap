/*
 *   moocanvas.c
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


struct _MooCanvasPrivate {
    GQueue *items;
};


#define ITEMS_HEAD(c) (c)->priv->items->head->data
#define ITEMS_TAIL(c) (c)->priv->items->tail->data

#define BIN_WINDOW(c) (GTK_LAYOUT(c)->bin_window)


static void     moo_canvas_destroy          (GtkObject      *object);
static gboolean moo_canvas_expose           (GtkWidget      *widget,
                                             GdkEventExpose *event);
static void     moo_canvas_realize          (GtkWidget      *widget);
static void     moo_canvas_unrealize        (GtkWidget      *widget);
static void     moo_canvas_style_set        (GtkWidget      *widget,
                                             GtkStyle       *prev);
static void     moo_canvas_state_changed    (GtkWidget      *widget,
                                             GtkStateType    prev);


G_DEFINE_TYPE (MooCanvas, moo_canvas, GTK_TYPE_LAYOUT)

// enum {
//     SET_SCROLL_ADJUSTMENTS,
//     N_SIGNALS
// };
//
// static guint signals[N_SIGNALS];


static void
moo_canvas_init (MooCanvas *canvas)
{
    GTK_WIDGET_UNSET_FLAGS (canvas, GTK_NO_WINDOW);
    canvas->priv = g_new0 (MooCanvasPrivate, 1);
    canvas->priv->items = g_queue_new ();
}


static void
moo_canvas_class_init (MooCanvasClass *klass)
{
    GtkObjectClass *object_class = GTK_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    object_class->destroy = moo_canvas_destroy;

    widget_class->expose_event = moo_canvas_expose;
    widget_class->realize = moo_canvas_realize;
    widget_class->unrealize = moo_canvas_unrealize;
    widget_class->style_set = moo_canvas_style_set;
    widget_class->state_changed = moo_canvas_state_changed;
}


static void
moo_canvas_destroy (GtkObject *object)
{
    MooCanvas *canvas = MOO_CANVAS (object);

    if (canvas->priv)
    {
        while (canvas->priv->items->length)
            gtk_object_destroy (ITEMS_HEAD (canvas));
        g_queue_free (canvas->priv->items);
        g_free (canvas->priv);
        canvas->priv = NULL;
    }

    GTK_OBJECT_CLASS(moo_canvas_parent_class)->destroy (object);
}


static void
moo_canvas_set_background (GtkWidget *widget)
{
    if (GTK_WIDGET_REALIZED (widget))
        gdk_window_set_background (BIN_WINDOW (widget),
                                   &widget->style->base[GTK_WIDGET_STATE (widget)]);
}


static void
moo_canvas_realize (GtkWidget *widget)
{
    MooCanvas *canvas = MOO_CANVAS (widget);

    GTK_WIDGET_CLASS(moo_canvas_parent_class)->realize (widget);

    moo_canvas_set_background (widget);
    moo_canvas_forall (canvas, (MooCanvasCallback) moo_canvas_item_realize, NULL);
}


static void
moo_canvas_unrealize (GtkWidget *widget)
{
    MooCanvas *canvas = MOO_CANVAS (widget);
    moo_canvas_forall (canvas, (MooCanvasCallback) moo_canvas_item_unrealize, NULL);
    GTK_WIDGET_CLASS(moo_canvas_parent_class)->unrealize (widget);
}


static void
moo_canvas_style_set (GtkWidget      *widget,
                      G_GNUC_UNUSED GtkStyle *prev)
{
    moo_canvas_set_background (widget);
}


static void
moo_canvas_state_changed (GtkWidget      *widget,
                          G_GNUC_UNUSED GtkStateType    prev)
{
    moo_canvas_set_background (widget);
}


static gboolean
moo_canvas_expose (GtkWidget      *widget,
                   GdkEventExpose *event)
{
    GList *l;
    MooCanvas *canvas = MOO_CANVAS (widget);

    if (event->window != BIN_WINDOW (widget))
        return FALSE;

    for (l = canvas->priv->items->tail; l != NULL; l = l->prev)
        _moo_canvas_item_draw (l->data, event, BIN_WINDOW (widget));

    return GTK_WIDGET_CLASS(moo_canvas_parent_class)->expose_event (widget, event);
}


/**************************************************************************/
/* Public api
 */

GtkWidget *
moo_canvas_new (void)
{
    return g_object_new (MOO_TYPE_CANVAS, NULL);
}


void
moo_canvas_forall (MooCanvas        *canvas,
                   MooCanvasCallback func,
                   gpointer          data)
{
    g_return_if_fail (MOO_IS_CANVAS (canvas));
    g_return_if_fail (func != NULL);
    g_queue_foreach (canvas->priv->items, (GFunc) func, data);
}


void
moo_canvas_add_item (MooCanvas     *canvas,
                     MooCanvasItem *item)
{
    g_return_if_fail (MOO_IS_CANVAS (canvas));
    g_return_if_fail (MOO_IS_CANVAS_ITEM (item));
    g_return_if_fail (item->canvas == NULL);

    gtk_object_sink (g_object_ref (item));
    g_queue_push_head (canvas->priv->items, item);

    _moo_canvas_item_set_parent (item, GTK_WIDGET (canvas));

    if (GTK_WIDGET_REALIZED (canvas))
        moo_canvas_item_realize (item);
}


void
moo_canvas_remove (MooCanvas     *canvas,
                   MooCanvasItem *item)
{
    g_return_if_fail (MOO_IS_CANVAS (canvas));
    g_return_if_fail (MOO_IS_CANVAS_ITEM (item));
    g_return_if_fail (item->canvas == GTK_WIDGET (canvas));

    if (MOO_CANVAS_ITEM_REALIZED (item))
        moo_canvas_item_unrealize (item);

    _moo_canvas_item_unparent (item);
    g_queue_remove (canvas->priv->items, item);

    g_object_unref (item);
}


void
moo_canvas_invalidate (MooCanvas *canvas,
                       GdkRegion *region)
{
    g_return_if_fail (MOO_IS_CANVAS (canvas));
    g_return_if_fail (region != NULL);

    if (GTK_WIDGET_REALIZED (canvas))
        gdk_window_invalidate_region (BIN_WINDOW (canvas), region, TRUE);
}


gpointer
moo_canvas_add (MooCanvas      *canvas,
                GType           item_type,
                const char     *first_prop,
                ...)
{
    MooCanvasItem *item;
    va_list var_args;

    g_return_val_if_fail (MOO_IS_CANVAS (canvas), NULL);
    g_return_val_if_fail (g_type_is_a (item_type, MOO_TYPE_CANVAS_ITEM), NULL);

    va_start (var_args, first_prop);
    item = (MooCanvasItem*) g_object_new_valist (item_type, first_prop, var_args);
    va_end (var_args);

    g_return_val_if_fail (item != NULL, NULL);

    moo_canvas_add_item (canvas, item);
    return item;
}
