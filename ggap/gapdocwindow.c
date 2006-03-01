/*
 *   gapdocwindow.c
 *
 *   Copyright (C) 2004-2006 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "gapdocwindow.h"
#include "gapdoc-glade.h"
#include "gapdoc.h"
#include <gtk/gtk.h>


#define gap_doc (gap_doc_instance())


enum {
    IDX_COLUMN_NAME,
    IDX_COLUMN_REF
};


G_DEFINE_TYPE(GapDocWindow, gap_doc_window, MOO_TYPE_WINDOW)


static void
gap_doc_window_destroy (GtkObject *object)
{
    GapDocWindow *win = GAP_DOC_WINDOW (object);

    if (win->xml)
    {
        g_object_unref (win->xml);
        win->xml = NULL;
    }

    GTK_OBJECT_CLASS(gap_doc_window_parent_class)->destroy (object);
}


static void
gap_doc_window_class_init (GapDocWindowClass *klass)
{
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);

    moo_window_class_set_id (window_class, "GapDocWindow", "GapDocWindow");

    GTK_OBJECT_CLASS(klass)->destroy = gap_doc_window_destroy;
}


static void
gap_doc_window_init_index (GapDocWindow *win)
{
    GtkTreeView *index;
    GtkTreeViewColumn *column;
    GtkCellRenderer *cell;

    index = moo_glade_xml_get_widget (win->xml, "index");
    g_return_if_fail (index != NULL);

    gtk_tree_view_set_model (index, GTK_TREE_MODEL(gap_doc->index_store));

    cell = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (NULL, cell,
                                                       "text", IDX_COLUMN_NAME, NULL);
    gtk_tree_view_append_column (index, column);
}


static void
gap_doc_window_init_gui (GapDocWindow *win)
{
    gap_doc_window_init_index (win);
}


static void
gap_doc_window_init (GapDocWindow *win)
{
    win->xml = moo_glade_xml_new_empty ();
    moo_glade_xml_parse_memory (win->xml, GAP_DOC_GLADE_UI, -1, "hpaned");

    gtk_container_add (GTK_CONTAINER (MOO_WINDOW(win)->vbox),
                       moo_glade_xml_get_widget (win->xml, "hpaned"));
    gtk_widget_show (MOO_WINDOW(win)->vbox);

    g_signal_connect (win, "delete-event",
                      G_CALLBACK (gtk_widget_hide_on_delete), NULL);

    gap_doc_window_init_gui (win);
}


static GapDocWindow *
instance (void)
{
    static gpointer inst;

    if (!inst)
    {
        inst = g_object_new (GAP_TYPE_DOC_WINDOW, NULL);
        g_object_add_weak_pointer (G_OBJECT (inst), &inst);
    }

    return inst;
}


void
gap_doc_window_show (void)
{
    gtk_window_present (GTK_WINDOW (instance ()));
}
