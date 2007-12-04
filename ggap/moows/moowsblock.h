/*
 *   moowsblock.h
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

#ifndef MOO_WS_BLOCK_H
#define MOO_WS_BLOCK_H

#include "moows/moowsview.h"

G_BEGIN_DECLS


#define MOO_TYPE_WS_BLOCK             (moo_ws_block_get_type ())
#define MOO_WS_BLOCK(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOO_TYPE_WS_BLOCK, MooWsBlock))
#define MOO_WS_BLOCK_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_WS_BLOCK, MooWsBlockClass))
#define MOO_IS_WS_BLOCK(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOO_TYPE_WS_BLOCK))
#define MOO_IS_WS_BLOCK_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_WS_BLOCK))
#define MOO_WS_BLOCK_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_WS_BLOCK, MooWsBlockClass))

struct _MooWsBlock
{
    GtkObject base;

    MooWsBlock *prev;
    MooWsBlock *next;
    GtkTextMark *start;
    GtkTextMark *end;
    GtkTextTag *tag;

    MooWsView *view;
    GtkTextBuffer *buffer;
};

struct _MooWsBlockClass
{
    GtkObjectClass base_class;

    void (*add)    (MooWsBlock *block,
                    MooWsView  *view,
                    MooWsBlock *after,
                    MooWsBlock *before);
    void (*remove) (MooWsBlock *block);

    void (*insert_text) (MooWsBlock  *block,
                         GtkTextIter *where,
                         const char  *text,
                         gssize       len);
    void (*delete_text) (MooWsBlock  *block,
                         GtkTextIter *start,
                         GtkTextIter *end);
};


GType        moo_ws_block_get_type          (void) G_GNUC_CONST;

void        _moo_ws_block_add               (MooWsBlock     *block,
                                             MooWsView      *view,
                                             MooWsBlock     *after,
                                             MooWsBlock     *before);
void        _moo_ws_block_remove            (MooWsBlock     *block);

void        _moo_ws_block_insert_text       (MooWsBlock     *block,
                                             GtkTextIter    *where,
                                             const char     *text,
                                             gssize          len);
void        _moo_ws_block_delete_text       (MooWsBlock     *block,
                                             GtkTextIter    *start,
                                             GtkTextIter    *end);

void        _moo_ws_block_insert            (MooWsBlock         *block,
                                             GtkTextIter        *where,
                                             const char         *text,
                                             gssize              len);
void        _moo_ws_block_insert_with_tags  (MooWsBlock         *block,
                                             GtkTextIter        *where,
                                             const char         *text,
                                             gssize              len,
                                             GtkTextTag         *first_tag,
                                             ...) G_GNUC_NULL_TERMINATED;

MooWsBlock *_moo_ws_iter_get_block          (const GtkTextIter  *pos);
void        _moo_ws_block_get_start_iter    (MooWsBlock         *block,
                                             GtkTextIter        *iter);
void        _moo_ws_block_get_end_iter      (MooWsBlock         *block,
                                             GtkTextIter        *iter);
void        _moo_ws_block_get_iter_at_line  (MooWsBlock         *block,
                                             GtkTextIter        *iter,
                                             int                 line,
                                             int                *real_line);


G_END_DECLS

#endif /* MOO_WS_BLOCK_H */
