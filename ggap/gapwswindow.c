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
#include <gtk/gtk.h>

#define PULSE_TIMEOUT 100

struct _GapWsWindowPrivate {
    GtkStatusbar *statusbar;
};

G_DEFINE_TYPE (GapWsWindow, gap_ws_window, GAP_TYPE_WINDOW)

static GObject *gap_ws_window_constructor   (GType   type,
                                             guint   n_props,
                                             GObjectConstructParam *props);

static void
gap_ws_window_class_init (GapWsWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);

    object_class->constructor = gap_ws_window_constructor;

    moo_window_class_set_id (MOO_WINDOW_CLASS (klass), "Worksheet", "Worksheet");
    g_type_class_add_private (klass, sizeof (GapWsWindowPrivate));

//     moo_window_class_new_action (window_class, "Cut", NULL,
//                                  "display-name", GTK_STOCK_CUT,
//                                  "label", GTK_STOCK_CUT,
//                                  "tooltip", GTK_STOCK_CUT,
//                                  "stock-id", GTK_STOCK_COPY,
//                                  "accel", "<Ctrl>C",
//                                  "closure-callback", copy_clipboard,
//                                  "closure-proxy-func", gap_term_window_get_terminal,
//                                  NULL);
//
//     moo_window_class_new_action (window_class, "Copy", NULL,
//                                  "display-name", GTK_STOCK_COPY,
//                                  "label", GTK_STOCK_COPY,
//                                  "tooltip", GTK_STOCK_COPY,
//                                  "stock-id", GTK_STOCK_COPY,
//                                  "accel", "<alt>C",
//                                  "closure-callback", copy_clipboard,
//                                  "closure-proxy-func", gap_term_window_get_terminal,
//                                  NULL);
//
//     moo_window_class_new_action (window_class, "Paste", NULL,
//                                  "display-name", GTK_STOCK_PASTE,
//                                  "label", GTK_STOCK_PASTE,
//                                  "tooltip", GTK_STOCK_PASTE,
//                                  "stock-id", GTK_STOCK_PASTE,
//                                  "accel", "<alt>V",
//                                  "closure-callback", paste_clipboard,
//                                  "closure-proxy-func", gap_term_window_get_terminal,
//                                  NULL);
//
//     moo_window_class_new_action (window_class, "SelectAll", NULL,
//                                  "display-name", GTK_STOCK_SELECT_ALL,
//                                  "label", GTK_STOCK_SELECT_ALL,
//                                  "tooltip", GTK_STOCK_SELECT_ALL,
//                                  "accel", "<alt>A",
//                                  "closure-callback", moo_term_select_all,
//                                  "closure-proxy-func", gap_term_window_get_terminal,
//                                  NULL);

    moo_window_class_new_action (window_class, "Interrupt", NULL,
                                 "display-name", "Interrupt",
                                 "label", "Inte_rrupt",
                                 "tooltip", "Interrupt",
                                 "accel", "<Ctrl>Break",
                                 "closure-callback", gap_view_send_intr,
                                 "closure-proxy-func", gap_window_get_terminal,
                                 NULL);
}


static void
gap_ws_window_init (GapWsWindow *window)
{
    GAP_WINDOW(window)->view_type = GAP_TYPE_WORKSHEET;
    moo_window_set_global_accels (MOO_WINDOW (window), FALSE);
    window->priv = G_TYPE_INSTANCE_GET_PRIVATE (window, GAP_TYPE_WS_WINDOW, GapWsWindowPrivate);
}


static void
set_statusbar_text (GapWsWindow *window,
                    const char  *text)
{
    gtk_statusbar_pop (window->priv->statusbar, 0);
    if (text && text[0])
        gtk_statusbar_push (window->priv->statusbar, 0, text);
}

static void
gap_state_changed (GapWorksheet *ws,
                   G_GNUC_UNUSED GParamSpec *pspec,
                   GapWsWindow  *window)
{
    GapState state;

    g_object_get (ws, "gap-state", &state, NULL);

    switch (state)
    {
        case GAP_BUSY:
            set_statusbar_text (window, "Busy");
            break;

        case GAP_DEAD:
        case GAP_IN_PROMPT:
            set_statusbar_text (window, NULL);
            break;

        case GAP_LOADING:
            set_statusbar_text (window, "Loading");
            break;
    }
}

static GObject *
gap_ws_window_constructor (GType type,
                           guint n_props,
                           GObjectConstructParam *props)
{
    GapWsWindow *window;
    GapWindow *gwindow;
    GtkWidget *statusbar;

    GObject *object = G_OBJECT_CLASS(gap_ws_window_parent_class)->constructor (type, n_props, props);
    window = GAP_WS_WINDOW (object);
    gwindow = GAP_WINDOW (object);

    statusbar = gtk_statusbar_new ();
    gtk_widget_show (statusbar);
    gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (statusbar), TRUE);
    gtk_box_pack_start (GTK_BOX (MOO_WINDOW (window)->vbox), statusbar, FALSE, FALSE, 0);
    window->priv->statusbar = GTK_STATUSBAR (statusbar);

    g_signal_connect (gwindow->terminal, "notify::gap-state",
                      G_CALLBACK (gap_state_changed), gwindow);

    return object;
}
