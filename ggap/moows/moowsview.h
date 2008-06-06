/*
 *   moowsview.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MOO_WS_VIEW_H
#define MOO_WS_VIEW_H

#include <mooedit/mootextview.h>
#include <moows/moowsbuffer.h>

G_BEGIN_DECLS


#define MOO_TYPE_WS_VIEW             (moo_ws_view_get_type ())
#define MOO_WS_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOO_TYPE_WS_VIEW, MooWsView))
#define MOO_WS_VIEW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_WS_VIEW, MooWsViewClass))
#define MOO_IS_WS_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOO_TYPE_WS_VIEW))
#define MOO_IS_WS_VIEW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_WS_VIEW))
#define MOO_WS_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_WS_VIEW, MooWsViewClass))

typedef struct MooWsViewPrivate MooWsViewPrivate;
typedef struct MooWsViewClass MooWsViewClass;

struct MooWsView
{
    MooTextView base;
    MooWsViewPrivate *priv;
};

struct MooWsViewClass
{
    MooTextViewClass base_class;
};


GType       moo_ws_view_get_type            (void) G_GNUC_CONST;

void        _moo_ws_view_beep               (MooWsView      *view);
void        _moo_ws_view_start_edit         (MooWsView      *view);
void        _moo_ws_view_end_edit           (MooWsView      *view);


G_END_DECLS

#endif /* MOO_WS_VIEW_H */
