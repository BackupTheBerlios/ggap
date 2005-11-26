/*
 *   gaptermwindow.c
 *
 *   Copyright (C) 2004-2005 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "gaptermwindow.h"
#include <mooutils/eggregex.h>
#include <mooapp/mooapp.h>
#include <gtk/gtkstock.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


struct _GapTermWindowPrivate {
};


static void     gap_term_window_class_init  (GapTermWindowClass *klass);
static void     gap_term_window_init        (GapTermWindow      *window);
static void     gap_term_window_destroy     (GtkObject          *object);

static void     switch_to_editor            (void);
static void     open_file                   (GapTermWindow      *window);


/* GAP_TYPE_TERM_WINDOW */
G_DEFINE_TYPE (GapTermWindow, gap_term_window, MOO_TYPE_TERM_WINDOW)


static void gap_term_window_class_init (GapTermWindowClass *klass)
{
    GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);

    moo_window_class_set_id (window_class, "Terminal", "Terminal");

    gtkobject_class->destroy = gap_term_window_destroy;

    moo_window_class_new_action (window_class, "SwitchToEditor",
                                 "name", "Switch to Editor",
                                 "label", "Switch to Editor",
                                 "tooltip", "Switch to Editor",
                                 "icon-stock-id", GTK_STOCK_EDIT,
                                 "closure-callback", switch_to_editor,
                                 NULL);

    moo_window_class_new_action (window_class, "OpenFile",
                                 "name", "Open File",
                                 "label", "Open File",
                                 "tooltip", "Open File",
                                 "icon-stock-id", GTK_STOCK_OPEN,
                                 "closure-callback", open_file,
                                 NULL);
}


static void gap_term_window_init (GapTermWindow *window)
{
    window->priv = g_new0 (GapTermWindowPrivate, 1);
    moo_term_window_set_term_type (MOO_TERM_WINDOW (window), GAP_TYPE_TERM);
}


static void
gap_term_window_destroy (GtkObject          *object)
{
    GapTermWindow *window = GAP_TERM_WINDOW (object);

    g_free (window->priv);
    window->priv = NULL;

    GTK_OBJECT_CLASS(gap_term_window_parent_class)->destroy (object);
}


static void
switch_to_editor (void)
{
    MooApp *app = moo_app_get_instance ();
    MooEditor *editor = moo_app_get_editor (app);
    moo_editor_present (editor);
}


static void
open_file (GapTermWindow *window)
{
    MooApp *app = moo_app_get_instance ();
    MooEditor *editor = moo_app_get_editor (app);
    moo_editor_open_file (editor, NULL, GTK_WIDGET (window), NULL, NULL);
}
