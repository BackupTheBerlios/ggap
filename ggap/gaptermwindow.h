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

#ifndef __GAP_TERM_WINDOW_H__
#define __GAP_TERM_WINDOW_H__

#include "gapwindow.h"

G_BEGIN_DECLS


#define GAP_TYPE_TERM_WINDOW             (gap_term_window_get_type ())
#define GAP_TERM_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAP_TYPE_TERM_WINDOW, GapTermWindow))
#define GAP_TERM_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_TERM_WINDOW, GapTermWindowClass))
#define GAP_IS_TERM_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAP_TYPE_TERM_WINDOW))
#define GAP_IS_TERM_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_TERM_WINDOW))
#define GAP_TERM_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_TERM_WINDOW, GapTermWindowClass))


typedef struct _GapTermWindow        GapTermWindow;
typedef struct _GapTermWindowPrivate GapTermWindowPrivate;
typedef struct _GapTermWindowClass   GapTermWindowClass;


struct _GapTermWindow
{
    GapWindow base;
    GapTermWindowPrivate *priv;
};

struct _GapTermWindowClass
{
    GapWindowClass  base_class;
};


GType   gap_term_window_get_type    (void) G_GNUC_CONST;


G_END_DECLS

#endif /* __GAP_TERM_WINDOW_H__ */
