/*
 *   gapwswindow.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "gapwswindow.h"
#include "gapworksheet.h"
#include "gapapp.h"
#include "gapprocess.h"
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
static void     gap_ws_window_active_doc_changed    (MdWindow           *window);
static void     gap_ws_window_insert_doc            (MdWindow           *window,
                                                     MdDocument         *doc);
static void     gap_ws_window_remove_doc            (MdWindow           *window,
                                                     MdDocument         *doc);


static void
gap_ws_window_class_init (GapWsWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MdWindowClass *doc_window_class = MD_WINDOW_CLASS (klass);

    moo_window_class_set_id (MOO_WINDOW_CLASS (klass), "Worksheet", "Worksheet");
    g_type_class_add_private (klass, sizeof (GapWsWindowPrivate));

    object_class->constructor = gap_ws_window_constructor;

    doc_window_class->active_doc_changed = gap_ws_window_active_doc_changed;
    doc_window_class->insert_doc = gap_ws_window_insert_doc;
    doc_window_class->remove_doc = gap_ws_window_remove_doc;
}


static void
gap_ws_window_init (GapWsWindow *window)
{
    window->priv = G_TYPE_INSTANCE_GET_PRIVATE (window, GAP_TYPE_WS_WINDOW, GapWsWindowPrivate);

    g_object_set (G_OBJECT (window),
                  "menubar-ui-name", "Worksheet/Menubar",
                  "toolbar-ui-name", "Worksheet/Toolbar",
                  NULL);

    moo_window_set_global_accels (MOO_WINDOW (window), FALSE);
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
print_gap_state (GapWsWindow *window)
{
    MdDocument *doc = md_window_get_active_doc (MD_WINDOW (window));

    if (doc)
    {
        GapState state;

        g_object_get (doc, "gap-state", &state, NULL);

        switch (state)
        {
            case GAP_BUSY:
            case GAP_BUSY_INTERNAL:
                moo_window_message (MOO_WINDOW (window), "Busy");
                break;

            case GAP_DEAD:
                moo_window_message (MOO_WINDOW (window), "GAP not running");
                break;

            case GAP_IN_PROMPT:
                moo_window_message (MOO_WINDOW (window), NULL);
                break;

            case GAP_LOADING:
                moo_window_message (MOO_WINDOW (window), "Loading");
                break;

            default:
                g_critical ("%s: fixme", G_STRLOC);
                moo_window_message (MOO_WINDOW (window), NULL);
                break;
        }
    }
    else
    {
        moo_window_message (MOO_WINDOW (window), NULL);
    }
}


static void
gap_state_changed (MdDocument   *doc,
                   G_GNUC_UNUSED GParamSpec *pspec,
                   GapWsWindow  *window)
{
    if (doc == md_window_get_active_doc (MD_WINDOW (window)))
        print_gap_state (window);
}

static void
gap_ws_window_active_doc_changed (MdWindow *window)
{
    MD_WINDOW_CLASS (gap_ws_window_parent_class)->active_doc_changed (window);
    print_gap_state (GAP_WS_WINDOW (window));
}

static void
gap_ws_window_insert_doc (MdWindow   *window,
                          MdDocument *doc)
{
    GapWorksheet *ws;

    ws = GAP_WORKSHEET (doc);

    g_signal_connect (ws, "notify::gap-state",
                      G_CALLBACK (gap_state_changed),
                      window);

    MD_WINDOW_CLASS (gap_ws_window_parent_class)->insert_doc (window, doc);

    print_gap_state (GAP_WS_WINDOW (window));
}

static void
gap_ws_window_remove_doc (MdWindow   *window,
                          MdDocument *doc)
{
    GapWorksheet *ws;

    ws = GAP_WORKSHEET (doc);

    g_signal_handlers_disconnect_by_func (ws, (gpointer) gap_state_changed, window);

    MD_WINDOW_CLASS (gap_ws_window_parent_class)->remove_doc (window, doc);

    print_gap_state (GAP_WS_WINDOW (window));
}
