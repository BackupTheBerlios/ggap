/*
 *   gapdoc.c
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

#include "gapdoc.h"
#include "gapdoc-glade.h"
#include <gtk/gtk.h>


enum {
    IDX_COLUMN_NAME,
    IDX_COLUMN_REF
};


G_DEFINE_TYPE(GapDoc, gap_doc, MOO_TYPE_WINDOW)


static void
gap_doc_destroy (GtkObject *object)
{
    GapDoc *win = GAP_DOC (object);

    if (win->xml)
    {
        g_object_unref (win->xml);
        win->xml = NULL;
    }

    GTK_OBJECT_CLASS(gap_doc_parent_class)->destroy (object);
}


static void
gap_doc_class_init (GapDocClass *klass)
{
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);

    moo_window_class_set_id (window_class, "GapDoc", "GapDoc");

    GTK_OBJECT_CLASS(klass)->destroy = gap_doc_destroy;
}


static void
index_populate (GtkTreeStore *store)
{
    GtkTreeIter node, iter;
    char *file;
    GError *error = NULL;
    char **lines, **p;
    gboolean has_node = FALSE;

    if (!g_file_get_contents ("/tmp/idx", &file, NULL, &error))
    {
        g_critical ("Could not read index: %s", error->message);
        g_error_free (error);
        return;
    }

    lines = g_strsplit (file, "\n", 0);

    for (p = lines; p && *p; ++p)
    {
        char **entry;
        char *line = *p;

        if (!*line)
            continue;

        if (!line[1])
        {
            gtk_tree_store_append (store, &node, NULL);
            gtk_tree_store_set (store, &node,
                                IDX_COLUMN_NAME, line, -1);
            has_node = TRUE;
            continue;
        }
        else if (!has_node)
        {
            g_critical ("%s: bad index file", G_STRLOC);
            gtk_tree_store_append (store, &node, NULL);
            gtk_tree_store_set (store, &node,
                                IDX_COLUMN_NAME, " ", -1);
            has_node = TRUE;
        }

        entry = g_strsplit (*p, " ", 2);

        if (!entry || !entry[0] || !entry[1])
        {
            g_critical ("%s: bad line: %s", G_STRLOC, *p);
            g_strfreev (entry);
            continue;
        }

        gtk_tree_store_append (store, &iter, &node);
        gtk_tree_store_set (store, &iter,
                            IDX_COLUMN_NAME, entry[1],
                            IDX_COLUMN_REF, entry[0], -1);

        g_strfreev (entry);
    }

    g_strfreev (lines);
    g_free (file);
}


static void
gap_doc_init_index (GapDoc *win)
{
    GtkTreeView *index;
    GtkTreeViewColumn *column;
    GtkCellRenderer *cell;
    GtkTreeStore *store;

    index = moo_glade_xml_get_widget (win->xml, "index");
    g_return_if_fail (index != NULL);

    store = gtk_tree_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
    index_populate (store);
    gtk_tree_view_set_model (index, GTK_TREE_MODEL (store));

    cell = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (NULL, cell,
                                                       "text", IDX_COLUMN_NAME, NULL);
    gtk_tree_view_append_column (index, column);
}


static void
gap_doc_init_gui (GapDoc *win)
{
    gap_doc_init_index (win);
}


static void
gap_doc_init (GapDoc *win)
{
    win->xml = moo_glade_xml_new_empty ();
    moo_glade_xml_parse_memory (win->xml, GAP_DOC_GLADE_UI, -1, "hpaned");

    gtk_container_add (GTK_CONTAINER (MOO_WINDOW(win)->vbox),
                       moo_glade_xml_get_widget (win->xml, "hpaned"));
    gtk_widget_show (MOO_WINDOW(win)->vbox);

    g_signal_connect (win, "delete-event",
                      G_CALLBACK (gtk_widget_hide_on_delete), NULL);

    gap_doc_init_gui (win);
}


static GapDoc *
instance (void)
{
    static gpointer inst;

    if (!inst)
    {
        inst = g_object_new (GAP_TYPE_DOC, NULL);
        g_object_add_weak_pointer (G_OBJECT (inst), &inst);
    }

    return inst;
}


void
gap_doc_show (void)
{
    gtk_window_present (GTK_WINDOW (instance ()));
}
