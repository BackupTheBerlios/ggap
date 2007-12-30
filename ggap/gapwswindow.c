/*
 *   gapwswindow.c
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

#include "gapwswindow.h"
#include "gapworksheet.h"
#include "gapapp.h"
#include "ggap-i18n.h"
#include "mooutils/moodialogs.h"
#include "mooutils/moofiledialog.h"
#include <gtk/gtk.h>

struct GapWsWindowPrivate {
    int dummy;
};

G_DEFINE_TYPE (GapWsWindow, gap_ws_window, MD_TYPE_WINDOW)

static GObject *gap_ws_window_constructor           (GType               type,
                                                     guint               n_props,
                                                     GObjectConstructParam *props);
static void     gap_ws_window_active_view_changed   (MdWindow           *window);
static void     gap_ws_window_insert_view           (MdWindow           *window,
                                                     MdView             *view);
static void     gap_ws_window_remove_view           (MdWindow           *window,
                                                     MdView             *view);


static void
gap_ws_window_class_init (GapWsWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MdWindowClass *doc_window_class = MD_WINDOW_CLASS (klass);

    md_app_window_class_set_id (MD_APP_WINDOW_CLASS (klass), "Worksheet", "Worksheet");
    g_type_class_add_private (klass, sizeof (GapWsWindowPrivate));

    object_class->constructor = gap_ws_window_constructor;

    doc_window_class->active_view_changed = gap_ws_window_active_view_changed;
    doc_window_class->insert_view = gap_ws_window_insert_view;
    doc_window_class->remove_view = gap_ws_window_remove_view;
}


static void
gap_ws_window_init (GapWsWindow *window)
{
    window->priv = G_TYPE_INSTANCE_GET_PRIVATE (window, GAP_TYPE_WS_WINDOW, GapWsWindowPrivate);

    g_object_set (G_OBJECT (window),
                  "menubar-ui-name", "Worksheet/Menubar",
                  "toolbar-ui-name", "Worksheet/Toolbar",
                  NULL);

    md_app_window_set_global_accels (MD_APP_WINDOW (window), FALSE);
}


static GObject *
gap_ws_window_constructor (GType type,
                           guint n_props,
                           GObjectConstructParam *props)
{
    GapWsWindow *window;

    GObject *object = G_OBJECT_CLASS(gap_ws_window_parent_class)->constructor (type, n_props, props);
    window = GAP_WS_WINDOW (object);

    return object;
}


static void
gap_state_changed (MdDocument   *doc,
                   G_GNUC_UNUSED GParamSpec *pspec,
                   GapWsWindow  *window)
{
    GapState state;

    if (doc != md_window_get_active_doc (MD_WINDOW (window)))
        return;

    g_object_get (doc, "gap-state", &state, NULL);

    switch (state)
    {
        case GAP_BUSY:
        case GAP_BUSY_INTERNAL:
            md_app_window_message (MD_APP_WINDOW (window), "Busy");
            break;

        case GAP_DEAD:
        case GAP_IN_PROMPT:
            md_app_window_message (MD_APP_WINDOW (window), NULL);
            break;

        case GAP_LOADING:
            md_app_window_message (MD_APP_WINDOW (window), "Loading");
            break;
    }
}

static void
gap_ws_window_active_view_changed (MdWindow *window)
{
    MD_WINDOW_CLASS (gap_ws_window_parent_class)->active_view_changed (window);
    md_app_window_message (MD_APP_WINDOW (window), NULL);
}

static void
gap_ws_window_insert_view (MdWindow *window,
                           MdView   *view)
{
    GapWorksheet *ws;

    ws = GAP_WORKSHEET (md_view_get_doc (view));

    g_signal_connect (ws, "notify::gap-state",
                      G_CALLBACK (gap_state_changed),
                      window);

    MD_WINDOW_CLASS (gap_ws_window_parent_class)->insert_view (window, view);

    gap_state_changed (MD_DOCUMENT (ws), NULL, GAP_WS_WINDOW (window));
}

static void
gap_ws_window_remove_view (MdWindow *window,
                           MdView   *view)
{
    GapWorksheet *ws;

    ws = GAP_WORKSHEET (md_view_get_doc (view));

    g_signal_handlers_disconnect_by_func (ws, (gpointer) gap_state_changed, window);

    MD_WINDOW_CLASS (gap_ws_window_parent_class)->remove_view (window, view);
}
