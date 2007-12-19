/*
 *   gapwsview.h
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

#ifndef GAP_WS_VIEW_H
#define GAP_WS_VIEW_H

#include "moows/moowsview.h"
#include "gapworksheet.h"

G_BEGIN_DECLS


#define GAP_TYPE_WS_VIEW             (gap_ws_view_get_type ())
#define GAP_WS_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAP_TYPE_WS_VIEW, GapWsView))
#define GAP_WS_VIEW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_WS_VIEW, GapWsViewClass))
#define GAP_IS_WS_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAP_TYPE_WS_VIEW))
#define GAP_IS_WS_VIEW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_WS_VIEW))
#define GAP_WS_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_WS_VIEW, GapWsViewClass))

typedef struct GapWsView        GapWsView;
typedef struct GapWsViewPrivate GapWsViewPrivate;
typedef struct GapWsViewClass   GapWsViewClass;

struct GapWsView
{
    MooWsView base;
    GapWsViewPrivate *priv;
};

struct GapWsViewClass
{
    MooWsViewClass base_class;
};


GType       gap_ws_view_get_type        (void) G_GNUC_CONST;


G_END_DECLS

#endif /* GAP_WS_VIEW_H */
