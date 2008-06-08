/*
 *   moowsbuffer.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MOO_WS_BUFFER_H
#define MOO_WS_BUFFER_H

#include <mooedit/mootextbuffer.h>
#include <moows/moowsblock.h>

G_BEGIN_DECLS


#define MOO_TYPE_WS_BUFFER             (moo_ws_buffer_get_type ())
#define MOO_WS_BUFFER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOO_TYPE_WS_BUFFER, MooWsBuffer))
#define MOO_WS_BUFFER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_WS_BUFFER, MooWsBufferClass))
#define MOO_IS_WS_BUFFER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOO_TYPE_WS_BUFFER))
#define MOO_IS_WS_BUFFER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_WS_BUFFER))
#define MOO_WS_BUFFER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_WS_BUFFER, MooWsBufferClass))

typedef struct MooWsBufferPrivate MooWsBufferPrivate;
typedef struct MooWsBufferClass MooWsBufferClass;

struct MooWsBuffer
{
    MooTextBuffer base;
    MooWsBufferPrivate *priv;
};

struct MooWsBufferClass
{
    MooTextBufferClass base_class;
};


GType       moo_ws_buffer_get_type          (void) G_GNUC_CONST;

void        moo_ws_buffer_insert_block      (MooWsBuffer    *buffer,
                                             MooWsBlock     *block,
                                             MooWsBlock     *after);
void        moo_ws_buffer_append_block      (MooWsBuffer    *buffer,
                                             MooWsBlock     *block);
void        moo_ws_buffer_delete_block      (MooWsBuffer    *buffer,
                                             MooWsBlock     *block);

MooWsBlock *moo_ws_buffer_get_first_block   (MooWsBuffer    *buffer);
MooWsBlock *moo_ws_buffer_get_last_block    (MooWsBuffer    *buffer);

void        _moo_ws_buffer_beep             (MooWsBuffer    *buffer);
void        _moo_ws_buffer_start_edit       (MooWsBuffer    *buffer);
void        _moo_ws_buffer_end_edit         (MooWsBuffer    *buffer);
void        _moo_ws_buffer_start_user_edit  (MooWsBuffer    *buffer);
void        _moo_ws_buffer_end_user_edit    (MooWsBuffer    *buffer);

G_END_DECLS

#endif /* MOO_WS_BUFFER_H */
