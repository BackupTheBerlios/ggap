/*
 *   moocanvaswindow.c
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

#include "moocanvas/moocanvaswindow.h"
#include "moocanvas/moocanvas.h"
#include <gtk/gtk.h>


G_DEFINE_TYPE (MooCanvasWindow, moo_canvas_window, GTK_TYPE_WINDOW)

enum {
    PROP_0,
    PROP_SHOW_MENU,
    PROP_SHOW_STATUSBAR
};


static void
moo_canvas_window_init (MooCanvasWindow *window)
{
    GtkWidget *vbox, *swin;

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (window), vbox);

    window->menubar = gtk_menu_bar_new ();
    gtk_widget_set_no_show_all (window->menubar, TRUE);
    gtk_box_pack_start (GTK_BOX (vbox), window->menubar, FALSE, FALSE, 0);

    swin = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),
                                    GTK_POLICY_NEVER, GTK_POLICY_NEVER);
    window->canvas = moo_canvas_new ();
    gtk_container_add (GTK_CONTAINER (swin), window->canvas);
    gtk_widget_show_all (swin);
    gtk_box_pack_start (GTK_BOX (vbox), swin, TRUE, TRUE, 0);

    window->statusbar = gtk_statusbar_new ();
    gtk_widget_set_no_show_all (window->statusbar, TRUE);
    gtk_box_pack_end (GTK_BOX (vbox), window->statusbar, FALSE, FALSE, 0);
}


static void
moo_canvas_window_get_property (GObject *object,
                                guint property_id,
                                GValue *value,
                                GParamSpec *pspec)
{
    MooCanvasWindow *window = MOO_CANVAS_WINDOW (object);

    switch (property_id)
    {
        case PROP_SHOW_MENU:
            g_value_set_boolean (value, GTK_WIDGET_VISIBLE (window->menubar));
            break;

        case PROP_SHOW_STATUSBAR:
            g_value_set_boolean (value, GTK_WIDGET_VISIBLE (window->statusbar));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}


static void
moo_canvas_window_set_property (GObject *object,
                                guint property_id,
                                const GValue *value,
                                GParamSpec *pspec)
{
    MooCanvasWindow *window = MOO_CANVAS_WINDOW (object);

    switch (property_id)
    {
        case PROP_SHOW_MENU:
            g_object_set_property (G_OBJECT (window->menubar), "visible", value);
            break;

        case PROP_SHOW_STATUSBAR:
            g_object_set_property (G_OBJECT (window->statusbar), "visible", value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}


static void
moo_canvas_window_class_init (MooCanvasWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->set_property = moo_canvas_window_set_property;
    object_class->get_property = moo_canvas_window_get_property;

    g_object_class_install_property (object_class, PROP_SHOW_MENU,
                                     g_param_spec_boolean ("show-menu", "show-menu", "show-menu",
                                                           FALSE, G_PARAM_READWRITE));

    g_object_class_install_property (object_class, PROP_SHOW_STATUSBAR,
                                     g_param_spec_boolean ("show-statusbar", "show-statusbar", "show-statusbar",
                                                           FALSE, G_PARAM_READWRITE));
}


GtkWidget *
moo_canvas_window_new (void)
{
    return g_object_new (MOO_TYPE_CANVAS_WINDOW, NULL);
}


MooCanvas *
moo_canvas_window_get_canvas (MooCanvasWindow *window)
{
    g_return_val_if_fail (MOO_IS_CANVAS_WINDOW (window), NULL);
    return MOO_CANVAS (window->canvas);
}
