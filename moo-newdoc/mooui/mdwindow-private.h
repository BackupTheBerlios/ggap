/*
 *   mdwindow-private.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MD_WINDOW_PRIVATE_H
#define MD_WINDOW_PRIVATE_H

#include "mdwindow.h"


void        _md_window_insert_doc   (MdWindow   *window,
                                     MdDocument *doc);
void        _md_window_remove_doc   (MdWindow   *window,
                                     MdDocument *doc);

gboolean    _md_window_destroyed    (MdWindow   *window);


#endif /* MD_WINDOW_PRIVATE_H */
