/*
 *   gapworksheet.c
 *
 *   Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "ggap-i18n.h"
#include "gapwsview.h"
#include "gapapp.h"
#include "gapparser.h"
#include "ggapfile.h"
#include "gap.h"
#include "mooedit/mootextcompletion.h"
#include "mooterm/mootermpt.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/mootype-macros.h"
#include "moows/moowsblock.h"
#include <glib/gregex.h>
#include <gdk/gdkkeysyms.h>
#include <errno.h>
#include <stdlib.h>

struct GapWsViewPrivate {
    guint resize_idle;
    int width;
    int height;
    MooTextCompletion *completion;
};


static void     gap_ws_view_destroy         (GtkObject      *object);

static void     gap_ws_view_size_allocate   (GtkWidget      *widget,
                                             GtkAllocation  *allocation);
static void     gap_ws_view_style_set       (GtkWidget      *widget,
                                             GtkStyle       *prev_style);
static void     gap_ws_view_realize         (GtkWidget      *widget);
static gboolean gap_ws_view_key_press       (GtkWidget      *widget,
                                             GdkEventKey    *event);

static void     gap_ws_view_update_size     (GapWsView      *view);

static void     gap_ws_view_complete        (GapWsView      *view);


/* GAP_TYPE_WS_VIEW */
G_DEFINE_TYPE_WITH_CODE (GapWsView, gap_ws_view, MOO_TYPE_WS_VIEW,
                         G_IMPLEMENT_INTERFACE (MD_TYPE_VIEW, view_iface_init))

enum {
    PROP_0,
    PROP_MD_DOCUMENT
};

static void
gap_ws_view_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    GapWsView *ws = GAP_WS_VIEW (object);

    switch (prop_id)
    {
        case PROP_MD_DOCUMENT:
            g_object_set (ws, "buffer", g_value_get_object (value), NULL);
            md_view_set_doc (MD_VIEW (ws), g_value_get_object (value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
gap_ws_view_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    GapWsView *ws = GAP_WS_VIEW (object);

    switch (prop_id)
    {
        case PROP_MD_DOCUMENT:
            g_value_set_object (value, md_view_get_doc (MD_VIEW (ws)));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
gap_ws_view_class_init (GapWsViewClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkObjectClass *gtk_object_class = GTK_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    object_class->get_property = gap_ws_view_get_property;
    object_class->set_property = gap_ws_view_set_property;
    gtk_object_class->destroy = gap_ws_view_destroy;

    widget_class->size_allocate = gap_ws_view_size_allocate;
    widget_class->style_set = gap_ws_view_style_set;
    widget_class->realize = gap_ws_view_realize;
    widget_class->key_press_event = gap_ws_view_key_press;

    g_object_class_override_property (object_class, PROP_MD_DOCUMENT, "md-document");

    g_type_class_add_private (klass, sizeof (GapWsViewPrivate));
}


static void
gap_ws_view_init (GapWsView *ws)
{
    ws->priv = G_TYPE_INSTANCE_GET_PRIVATE (ws, GAP_TYPE_WS_VIEW, GapWsViewPrivate);
    ws->priv->width = -1;
    ws->priv->height = -1;
    moo_text_view_set_buffer_type (MOO_TEXT_VIEW (ws), GAP_TYPE_WORKSHEET);
}


static void
gap_ws_view_destroy (GtkObject *object)
{
    GapWsView *ws = GAP_WS_VIEW (object);

    if (ws->priv)
    {
        if (ws->priv->resize_idle)
            g_source_remove (ws->priv->resize_idle);
        ws->priv = NULL;
    }

    GTK_OBJECT_CLASS (gap_ws_view_parent_class)->destroy (object);
}


static void
view_iface_init (G_GNUC_UNUSED MdViewIface *iface)
{
}


static GapWorksheet *
get_worksheet (GapWsView *view)
{
    return GAP_WORKSHEET (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));
}

static void
gap_ws_view_update_size (GapWsView *view)
{
    _gap_worksheet_set_size (get_worksheet (view),
                             view->priv->width,
                             view->priv->height);
}

static gboolean
resize_idle (GapWsView *view)
{
    GtkAllocation *allocation = &GTK_WIDGET(view)->allocation;

    if (!GTK_WIDGET_REALIZED (view) || allocation->width <= 1 || allocation->height <= 1)
    {
        view->priv->width = -1;
        view->priv->height = -1;
    }
    else
    {
        PangoLayout *layout;
        int width, height;

        layout = gtk_widget_create_pango_layout (GTK_WIDGET (view), " ");
        pango_layout_get_pixel_size (layout, &width, &height);

#define HOW_MANY(x__,y__) (((x__) + (y__) - 1) / (y__))
        view->priv->width = HOW_MANY (allocation->width, width);
        view->priv->height = HOW_MANY (allocation->height, height);
#undef HOW_MANY

        /* leave one char to make sure horizontal scrollbar won't appear */
        view->priv->width = MAX (view->priv->width - 1, 10);
        view->priv->height = MAX (view->priv->height, 10);
        g_object_unref (layout);
    }

    gap_ws_view_update_size (view);

    view->priv->resize_idle = 0;
    return FALSE;
}

static void
queue_resize (GapWsView *view)
{
    if (!view->priv->resize_idle)
        view->priv->resize_idle = g_idle_add ((GSourceFunc) resize_idle, view);
}

static void
gap_ws_view_size_allocate (GtkWidget     *widget,
                           GtkAllocation *allocation)
{
    GTK_WIDGET_CLASS (gap_ws_view_parent_class)->size_allocate (widget, allocation);
    queue_resize (GAP_WS_VIEW (widget));
}

static void
gap_ws_view_style_set (GtkWidget *widget,
                       GtkStyle  *prev_style)
{
    GTK_WIDGET_CLASS (gap_ws_view_parent_class)->style_set (widget, prev_style);
    queue_resize (GAP_WS_VIEW (widget));
}

static void
gap_ws_view_realize (GtkWidget *widget)
{
    GTK_WIDGET_CLASS (gap_ws_view_parent_class)->realize (widget);
    queue_resize (GAP_WS_VIEW (widget));
}


static gboolean
gap_ws_view_key_press (GtkWidget   *widget,
                       GdkEventKey *event)
{
    if (event->keyval == GDK_Tab)
    {
        gap_ws_view_complete (GAP_WS_VIEW (widget));
        return TRUE;
    }

    return GTK_WIDGET_CLASS(gap_ws_view_parent_class)->key_press_event (widget, event);
}


static void
gap_ws_view_ensure_completion (GapWsView *view)
{
    if (!view->priv->completion)
    {
        view->priv->completion = _gap_worksheet_get_completion (get_worksheet (view));
        moo_text_completion_set_doc (view->priv->completion, GTK_TEXT_VIEW (view));
    }
}

static void
gap_ws_view_complete (GapWsView *view)
{
    gap_ws_view_ensure_completion (view);
    _gap_worksheet_ask_for_completions (get_worksheet (view));
    moo_text_completion_try_complete (view->priv->completion, FALSE);
}
