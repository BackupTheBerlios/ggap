/*
 *   usermenu.c
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

#include "usermenu.h"
#include "mooutils/mooprefsdialog.h"
#include "mooutils/moostock.h"


void
user_menu_init (void)
{
}


GtkWidget *
user_menu_prefs_page_new (void)
{
    return moo_prefs_dialog_page_new ("User menu", MOO_STOCK_MENU);
}
