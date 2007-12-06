/*
 *   moowsview.h
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

#ifndef MOO_WS_VIEW_H
#define MOO_WS_VIEW_H

#include <mooedit/mootextview.h>

G_BEGIN_DECLS


#define MOO_TYPE_WS_VIEW             (moo_ws_view_get_type ())
#define MOO_WS_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOO_TYPE_WS_VIEW, MooWsView))
#define MOO_WS_VIEW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_WS_VIEW, MooWsViewClass))
#define MOO_IS_WS_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOO_TYPE_WS_VIEW))
#define MOO_IS_WS_VIEW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_WS_VIEW))
#define MOO_WS_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_WS_VIEW, MooWsViewClass))

typedef struct _MooWsBlock MooWsBlock;
typedef struct _MooWsBlockClass MooWsBlockClass;
typedef struct _MooWsView MooWsView;
typedef struct _MooWsViewPrivate MooWsViewPrivate;
typedef struct _MooWsViewClass MooWsViewClass;

struct _MooWsView
{
    MooTextView base;
    MooWsViewPrivate *priv;
};

struct _MooWsViewClass
{
    MooTextViewClass base_class;
};


GType       moo_ws_view_get_type            (void) G_GNUC_CONST;

void        moo_ws_view_insert_block        (MooWsView      *view,
                                             MooWsBlock     *block,
                                             MooWsBlock     *after);
void        moo_ws_view_append_block        (MooWsView      *view,
                                             MooWsBlock     *block);
void        moo_ws_view_delete_block        (MooWsView      *view,
                                             MooWsBlock     *block);

MooWsBlock *_moo_ws_view_get_first_block    (MooWsView      *view);
MooWsBlock *_moo_ws_view_get_last_block     (MooWsView      *view);

void        _moo_ws_view_beep               (MooWsView      *view);
void        _moo_ws_view_start_edit         (MooWsView      *view);
void        _moo_ws_view_end_edit           (MooWsView      *view);

MooWsView  *_moo_ws_buffer_get_view         (GtkTextBuffer  *buffer);


G_END_DECLS

#endif /* MOO_WS_VIEW_H */
// -*- objc -*-
