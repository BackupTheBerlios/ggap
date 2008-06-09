/*
 *   moows-draw.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MOO_WS_DRAW_H
#define MOO_WS_DRAW_H

#include <moows/mooworksheet.h>

G_BEGIN_DECLS


#define MOO_WORKSHEET_BLOCK_GUIDE_WIDTH 3
#define MOO_WORKSHEET_MARGIN_WIDTH (MOO_WORKSHEET_BLOCK_GUIDE_WIDTH + 2)

gboolean    _moo_worksheet_expose_event     (GtkWidget      *widget,
                                             GdkEventExpose *event);


G_END_DECLS

#endif /* MOO_WS_DRAW_H */
