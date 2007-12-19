/*
 *   gaptermwindow.h
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

#ifndef GAP_TERM_WINDOW_H
#define GAP_TERM_WINDOW_H

#include <mooui/mdappwindow.h>

G_BEGIN_DECLS


#define GAP_TYPE_TERM_WINDOW             (gap_term_window_get_type ())
#define GAP_TERM_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAP_TYPE_TERM_WINDOW, GapTermWindow))
#define GAP_TERM_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_TERM_WINDOW, GapTermWindowClass))
#define GAP_IS_TERM_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAP_TYPE_TERM_WINDOW))
#define GAP_IS_TERM_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_TERM_WINDOW))
#define GAP_TERM_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_TERM_WINDOW, GapTermWindowClass))


typedef struct GapTermWindow        GapTermWindow;
typedef struct GapTermWindowPrivate GapTermWindowPrivate;
typedef struct GapTermWindowClass   GapTermWindowClass;


struct GapTermWindow
{
    MdAppWindow base;
    GapTermWindowPrivate *priv;
};

struct GapTermWindowClass
{
    MdAppWindowClass base_class;
};


GType   gap_term_window_get_type    (void) G_GNUC_CONST;

void    gap_term_window_feed_gap    (GapTermWindow  *window,
                                     const char     *string);


G_END_DECLS

#endif /* GAP_TERM_WINDOW_H */
