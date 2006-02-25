/*
 *   usermenu.h
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

#ifndef __USER_MENU_H__
#define __USER_MENU_H__

#include <gtk/gtkwidget.h>


void         user_menu_init             (void);
GtkWidget   *user_menu_prefs_page_new   (void);


#endif /* __USER_MENU_H__ */
