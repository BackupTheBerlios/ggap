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
#include <gtk/gtk.h>


enum {
    IDX_COLUMN_NAME,
    IDX_COLUMN_REF
};


G_DEFINE_TYPE(GapDoc, gap_doc, G_TYPE_OBJECT)


static void
gap_doc_dispose (GObject *object)
{
    GapDoc *doc = GAP_DOC (object);

    if (doc->index_store)
    {
        g_object_unref (doc->index_store);
        doc->index_store = NULL;
    }

    G_OBJECT_CLASS(gap_doc_parent_class)->dispose (object);
}


static void
gap_doc_class_init (GapDocClass *klass)
{
    G_OBJECT_CLASS(klass)->dispose = gap_doc_dispose;
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
gap_doc_init_index (GapDoc *doc)
{
    doc->index_store = gtk_tree_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
    index_populate (doc->index_store);
}


static void
gap_doc_init (GapDoc *doc)
{
    gap_doc_init_index (doc);
}


GapDoc *
gap_doc_instance (void)
{
    static gpointer instance;

    if (!instance)
    {
        instance = g_object_new (GAP_TYPE_DOC, NULL);
        g_object_add_weak_pointer (G_OBJECT (instance), &instance);
    }

    return instance;
}
