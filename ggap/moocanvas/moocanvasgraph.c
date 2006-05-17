/*
 *   moocanvasgraph.c
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

#include "moocanvas/moocanvasgraph.h"
#include "moocanvas/moocanvasline.h"


typedef struct {
    MooCanvasVertex *start;
    MooCanvasVertex *end;
    MooCanvasItem *line;
} Edge;

static Edge    *edge_new    (void);
static void     edge_free   (Edge   *edge);

static void     moo_canvas_vertex_set_property  (GObject        *object,
                                                 guint           property_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void     moo_canvas_vertex_get_property  (GObject        *object,
                                                 guint           property_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);

static void     moo_canvas_vertex_destroy       (GtkObject      *object);

static void     moo_canvas_vertex_realize       (MooCanvasItem  *item);
static void     moo_canvas_vertex_unrealize     (MooCanvasItem  *item);
static GdkRegion *moo_canvas_vertex_get_region  (MooCanvasItem  *item);
static void     moo_canvas_vertex_move          (MooCanvasItem  *item,
                                                 double          dx,
                                                 double          dy);
static void     moo_canvas_vertex_draw          (MooCanvasItem  *item,
                                                 GdkEventExpose *event,
                                                 GdkDrawable    *dest);

static void     moo_canvas_vertex_set_coords    (MooCanvasVertex   *vertex,
                                                 double          x,
                                                 double          y);


G_DEFINE_TYPE (MooCanvasVertex, moo_canvas_vertex, MOO_TYPE_CANVAS_ITEM)


enum {
    VERTEX_PROP_0,
    VERTEX_PROP_X,
    VERTEX_PROP_Y
};


static void
moo_canvas_vertex_init (G_GNUC_UNUSED MooCanvasVertex *vertex)
{
}


static void
moo_canvas_vertex_class_init (MooCanvasVertexClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
    MooCanvasItemClass *item_class = MOO_CANVAS_ITEM_CLASS (klass);

    gobject_class->set_property = moo_canvas_vertex_set_property;
    gobject_class->get_property = moo_canvas_vertex_get_property;

    gtkobject_class->destroy = moo_canvas_vertex_destroy;

    item_class->realize = moo_canvas_vertex_realize;
    item_class->unrealize = moo_canvas_vertex_unrealize;
    item_class->get_region = moo_canvas_vertex_get_region;
    item_class->move = moo_canvas_vertex_move;
    item_class->draw = moo_canvas_vertex_draw;

    g_object_class_install_property (gobject_class, VERTEX_PROP_X,
                                     g_param_spec_int ("x", "x", "x",
                                                       G_MININT, G_MAXINT, 0,
                                                       G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class, VERTEX_PROP_Y,
                                     g_param_spec_int ("y", "y", "y",
                                                       G_MININT, G_MAXINT, 0,
                                                       G_PARAM_READWRITE));
}


static void
moo_canvas_vertex_destroy (GtkObject *object)
{
    GSList *list;
    MooCanvasVertex *vertex = MOO_CANVAS_VERTEX (object);

    list = g_slist_copy (vertex->edges);

    while (list)
    {
        Edge *edge = list->data;
        moo_canvas_vertex_disconnect (edge->start, edge->end);
        list = g_slist_delete_link (list, list);
    }

    if (vertex->box)
    {
        MooCanvasItem *tmp = vertex->box;
        vertex->box = NULL;
        gtk_object_destroy (GTK_OBJECT (tmp));
    }

    if (vertex->text)
    {
        MooCanvasItem *tmp = vertex->text;
        vertex->text = NULL;
        gtk_object_destroy (GTK_OBJECT (tmp));
    }

    GTK_OBJECT_CLASS (moo_canvas_vertex_parent_class)->destroy (object);
}


static void
moo_canvas_vertex_move (MooCanvasItem *item,
                        double         dx,
                        double         dy)
{
    GSList *l;
    MooCanvasVertex *vertex = MOO_CANVAS_VERTEX (item);

    moo_canvas_vertex_set_coords (vertex, vertex->x + dx, vertex->y + dy);

    for (l = vertex->edges; l != NULL; l = l->next)
    {
        Edge *edge = l->data;

        if (edge->start == vertex)
            moo_canvas_line_set_start (MOO_CANVAS_LINE (edge->line),
                                       MOO_CANVAS_LINE (edge->line)->x1 + dx,
                                       MOO_CANVAS_LINE (edge->line)->y1 + dy);
        else
            moo_canvas_line_set_end (MOO_CANVAS_LINE (edge->line),
                                     MOO_CANVAS_LINE (edge->line)->x2 + dx,
                                     MOO_CANVAS_LINE (edge->line)->y2 + dy);
    }
}


static void
moo_canvas_vertex_set_property (GObject        *object,
                                guint           property_id,
                                const GValue   *value,
                                GParamSpec     *pspec)
{
    MooCanvasVertex *vertex = MOO_CANVAS_VERTEX (object);

    switch (property_id)
    {
        case VERTEX_PROP_X:
            moo_canvas_vertex_set_coords (vertex, g_value_get_int (value), vertex->y);
            break;
        case VERTEX_PROP_Y:
            moo_canvas_vertex_set_coords (vertex, vertex->x, g_value_get_int (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}


static void
moo_canvas_vertex_get_property (GObject        *object,
                                guint           property_id,
                                GValue         *value,
                                GParamSpec     *pspec)
{
    MooCanvasVertex *vertex = MOO_CANVAS_VERTEX (object);

    switch (property_id)
    {
        case VERTEX_PROP_X:
            g_value_set_int (value, vertex->x);
            break;
        case VERTEX_PROP_Y:
            g_value_set_int (value, vertex->y);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}


static void
moo_canvas_vertex_set_coords (MooCanvasVertex *vertex,
                              double           x,
                              double           y)
{
    moo_canvas_item_invalidate (MOO_CANVAS_ITEM (vertex));

    vertex->x = x;
    vertex->y = y;

    moo_canvas_item_invalidate (MOO_CANVAS_ITEM (vertex));
}


static void
moo_canvas_vertex_realize (MooCanvasItem *item)
{
    GSList *l;
    MooCanvasVertex *vertex = MOO_CANVAS_VERTEX (item);

    MOO_CANVAS_ITEM_CLASS(moo_canvas_vertex_parent_class)->realize (item);

    for (l = vertex->edges; l != NULL; l = l->next)
    {
        Edge *edge = l->data;

        if (vertex != edge->start)
            continue;

        _moo_canvas_item_set_parent (edge->line, item->canvas);
        moo_canvas_item_realize (edge->line);
    }

    if (vertex->text)
    {
        _moo_canvas_item_set_parent (vertex->text, item->canvas);
        moo_canvas_item_realize (vertex->text);
    }

    if (vertex->box)
    {
        _moo_canvas_item_set_parent (vertex->box, item->canvas);
        moo_canvas_item_realize (vertex->box);
    }
}


static void
moo_canvas_vertex_unrealize (MooCanvasItem *item)
{
    GSList *l;
    MooCanvasVertex *vertex = MOO_CANVAS_VERTEX (item);

    for (l = vertex->edges; l != NULL; l = l->next)
    {
        Edge *edge = l->data;

        if (vertex != edge->start)
            continue;

        moo_canvas_item_unrealize (edge->line);
        _moo_canvas_item_unparent (edge->line);
    }

    if (vertex->text)
    {
        moo_canvas_item_unrealize (vertex->text);
        _moo_canvas_item_unparent (vertex->text);
    }

    if (vertex->box)
    {
        moo_canvas_item_unrealize (vertex->box);
        _moo_canvas_item_unparent (vertex->box);
    }

    MOO_CANVAS_ITEM_CLASS(moo_canvas_vertex_parent_class)->unrealize (item);
}


static GdkRegion *
moo_canvas_vertex_get_region (MooCanvasItem *item)
{
    GSList *l;
    GdkRegion *region1 = NULL, *region2 = NULL;
    GdkRegion *region;
    MooCanvasVertex *vertex = MOO_CANVAS_VERTEX (item);

    if (vertex->box)
        region1 = moo_canvas_item_get_region (vertex->box);
    if (vertex->text)
        region2 = moo_canvas_item_get_region (vertex->text);

    if (region1 && region2)
    {
        gdk_region_union (region1, region2);
        gdk_region_destroy (region2);
        region = region1;
    }
    else
    {
        region = region1 ? region1 : (region2 ? region2 : gdk_region_new ());
    }

    for (l = vertex->edges; l != NULL; l = l->next)
    {
        Edge *edge = l->data;

        if (edge->start == vertex)
        {
            GdkRegion *tmp = moo_canvas_item_get_region (edge->line);

            if (tmp)
            {
                gdk_region_union (region, tmp);
                gdk_region_destroy (tmp);
            }
        }
    }

    return region;
}


static void
moo_canvas_vertex_draw (MooCanvasItem  *item,
                        GdkEventExpose *event,
                        GdkDrawable    *dest)
{
    GSList *l;
    MooCanvasVertex *v = MOO_CANVAS_VERTEX (item);

    for (l = v->edges; l != NULL; l = l->next)
    {
        Edge *edge = l->data;

        if (edge->start == v)
            _moo_canvas_item_draw (edge->line, event, dest);
    }

    if (v->box)
        _moo_canvas_item_draw (v->box, event, dest);

    if (v->text)
        _moo_canvas_item_draw (v->text, event, dest);
}


static Edge *
edge_new (void)
{
    Edge *edge = g_new0 (Edge, 1);
    edge->line = g_object_new (MOO_TYPE_CANVAS_LINE, NULL);
    gtk_object_sink (g_object_ref (edge->line));
    return edge;
}


static void
edge_free (Edge *edge)
{
    if (edge)
    {
        g_object_unref (edge->line);
        g_free (edge);
    }
}


static Edge *
find_edge (MooCanvasVertex *v1,
           MooCanvasVertex *v2)
{
    GSList *l;

    for (l = v1->edges; l != NULL; l = l->next)
    {
        Edge *edge = l->data;
        g_assert (edge->start == v1 || edge->end == v1);
        if (edge->start == v2 || edge->end == v2)
            return edge;
    }

    return NULL;
}


void
moo_canvas_vertex_disconnect (MooCanvasVertex *v1,
                              MooCanvasVertex *v2)
{
    Edge *edge;

    g_return_if_fail (MOO_IS_CANVAS_VERTEX (v1));
    g_return_if_fail (MOO_IS_CANVAS_VERTEX (v2));

    if (v1 == v2)
        return;

    edge = find_edge (v1, v2);

    if (!edge)
        return;

    v1->edges = g_slist_remove (v1->edges, edge);
    v2->edges = g_slist_remove (v2->edges, edge);
    _moo_canvas_item_unparent (edge->line);
    gtk_object_destroy (GTK_OBJECT (edge->line));
    edge_free (edge);
}


void
moo_canvas_vertex_connect (MooCanvasVertex *v1,
                           MooCanvasVertex *v2)
{
    Edge *edge;

    g_return_if_fail (MOO_IS_CANVAS_VERTEX (v1));
    g_return_if_fail (MOO_IS_CANVAS_VERTEX (v2));
    g_return_if_fail (v1 != v2);

    if (find_edge (v1, v2))
        return;

    edge = edge_new ();
    edge->start = v1;
    edge->end = v2;

    v1->edges = g_slist_prepend (v1->edges, edge);
    v2->edges = g_slist_prepend (v2->edges, edge);

    moo_canvas_line_set_start (MOO_CANVAS_LINE (edge->line), v1->x, v1->y);
    moo_canvas_line_set_end (MOO_CANVAS_LINE (edge->line), v2->x, v2->y);

    if (MOO_CANVAS_ITEM_REALIZED (v1))
    {
        _moo_canvas_item_set_parent (edge->line, MOO_CANVAS_ITEM(v1)->canvas);
        moo_canvas_item_realize (edge->line);
    }
}
