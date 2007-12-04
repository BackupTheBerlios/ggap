/*
 *   moowstextblock.h
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

#ifndef MOO_WS_TEXT_BLOCK_H
#define MOO_WS_TEXT_BLOCK_H

#include "moows/moowsblock.h"

G_BEGIN_DECLS


#define MOO_TYPE_WS_TEXT_BLOCK             (moo_ws_text_block_get_type ())
#define MOO_WS_TEXT_BLOCK(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOO_TYPE_WS_TEXT_BLOCK, MooWsTextBlock))
#define MOO_WS_TEXT_BLOCK_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_WS_TEXT_BLOCK, MooWsTextBlockClass))
#define MOO_IS_WS_TEXT_BLOCK(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOO_TYPE_WS_TEXT_BLOCK))
#define MOO_IS_WS_TEXT_BLOCK_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_WS_TEXT_BLOCK))
#define MOO_WS_TEXT_BLOCK_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_WS_TEXT_BLOCK, MooWsTextBlockClass))

typedef struct _MooWsTextBlock MooWsTextBlock;
typedef struct _MooWsTextBlockPrivate MooWsTextBlockPrivate;
typedef struct _MooWsTextBlockClass MooWsTextBlockClass;

struct _MooWsTextBlock
{
    MooWsBlock base;
    MooWsTextBlockPrivate *priv;
};

struct _MooWsTextBlockClass
{
    MooWsBlockClass base_class;
};


GType           moo_ws_text_block_get_type  (void) G_GNUC_CONST;

MooWsTextBlock *moo_ws_text_block_new       (void);
void            moo_ws_text_block_set_text  (MooWsTextBlock *block,
                                             const char     *text);
char           *moo_ws_text_block_get_text  (MooWsTextBlock *block);
void            moo_ws_text_block_append    (MooWsTextBlock *block,
                                             const char     *text);


G_END_DECLS

#endif /* MOO_WS_TEXT_BLOCK_H */
