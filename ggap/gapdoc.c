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
#include "mooutils/mooglade.h"


G_DEFINE_TYPE(GapDoc, gap_doc, MOO_TYPE_WINDOW)


static void
gap_doc_class_init (GapDocClass *klass)
{
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);
    moo_window_class_set_id (window_class, "GapDoc", "GapDoc");
}


static void
gap_doc_init (GapDoc *win)
{
    MooGladeXML *xml;

    xml = moo_glade_xml_new_empty ();
    moo_glade_xml_parse_memory (xml, GAP_DOC_GLADE_UI, -1, "hpaned");

    gtk_container_add (GTK_CONTAINER (MOO_WINDOW(win)->vbox),
                       moo_glade_xml_get_widget (xml, "hpaned"));
    gtk_widget_show (MOO_WINDOW(win)->vbox);

    g_signal_connect (win, "delete-event",
                      G_CALLBACK (gtk_widget_hide_on_delete), NULL);

    g_object_unref (xml);
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
