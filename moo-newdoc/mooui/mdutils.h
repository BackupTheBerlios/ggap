/*
 *   mdutils.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MD_UTILS_H
#define MD_UTILS_H

#include <gtk/gtk.h>


gboolean    _md_image_attach_throbber   (GtkImage       *image,
                                         GtkIconSize     size);
void        _md_widget_beep             (GtkWidget      *widget);

char       *_md_uri_get_dirname         (const char     *uri);


#endif /* MD_UTILS_H */
