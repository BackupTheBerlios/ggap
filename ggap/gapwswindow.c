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


G_DEFINE_TYPE (GapWsWindow, gap_ws_window, GAP_TYPE_WINDOW)


static void
gap_ws_window_class_init (GapWsWindowClass *klass)
{
    moo_window_class_set_id (MOO_WINDOW_CLASS (klass), "Worksheet", "Worksheet");
}


static void
gap_ws_window_init (GapWsWindow *window)
{
    GAP_WINDOW(window)->view_type = GAP_TYPE_WORKSHEET;
    moo_window_set_global_accels (MOO_WINDOW (window), FALSE);
}
