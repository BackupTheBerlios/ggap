/*
 *   moowspromptblock.h
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

#ifndef MOO_WS_PROMPT_BLOCK_H
#define MOO_WS_PROMPT_BLOCK_H

#include <moows/moowsblock.h>

G_BEGIN_DECLS


#define MOO_TYPE_WS_PROMPT_BLOCK             (moo_ws_prompt_block_get_type ())
#define MOO_WS_PROMPT_BLOCK(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOO_TYPE_WS_PROMPT_BLOCK, MooWsPromptBlock))
#define MOO_WS_PROMPT_BLOCK_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MOO_TYPE_WS_PROMPT_BLOCK, MooWsPromptBlockClass))
#define MOO_IS_WS_PROMPT_BLOCK(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOO_TYPE_WS_PROMPT_BLOCK))
#define MOO_IS_WS_PROMPT_BLOCK_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MOO_TYPE_WS_PROMPT_BLOCK))
#define MOO_WS_PROMPT_BLOCK_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MOO_TYPE_WS_PROMPT_BLOCK, MooWsPromptBlockClass))

typedef struct MooWsPromptBlock MooWsPromptBlock;
typedef struct MooWsPromptBlockPrivate MooWsPromptBlockPrivate;
typedef struct MooWsPromptBlockClass MooWsPromptBlockClass;

struct MooWsPromptBlock
{
    MooWsBlock base;
    MooWsPromptBlockPrivate *priv;
};

struct MooWsPromptBlockClass
{
    MooWsBlockClass base_class;
};


GType        moo_ws_prompt_block_get_type       (void) G_GNUC_CONST;

MooWsBlock  *moo_ws_prompt_block_new            (const char         *ps,
                                                 const char         *ps2);

GtkTextTag  *moo_ws_prompt_block_get_ps_tag     (MooWsPromptBlock   *pb);
GtkTextTag  *moo_ws_prompt_block_get_text_tag   (MooWsPromptBlock   *pb);

char        *moo_ws_prompt_block_get_text       (MooWsPromptBlock   *pb);
char       **moo_ws_prompt_block_get_lines      (MooWsPromptBlock   *pb);
void         moo_ws_prompt_block_set_text       (MooWsPromptBlock   *pb,
                                                 const char         *text);
const char  *moo_ws_prompt_block_get_ps         (MooWsPromptBlock   *pb);
const char  *moo_ws_prompt_block_get_ps2        (MooWsPromptBlock   *pb);

void         moo_ws_prompt_block_iter_set_line_offset
                                                (MooWsPromptBlock   *pb,
                                                 GtkTextIter        *iter,
                                                 int                 offset);
void         moo_ws_prompt_block_place_cursor   (MooWsPromptBlock   *pb,
                                                 int                 line,
                                                 int                 column);
void         moo_ws_prompt_block_highlight_error(MooWsPromptBlock   *pb,
                                                 int                 line,
                                                 int                 start_column,
                                                 int                 end_column);
void         moo_ws_prompt_block_clear_errors   (MooWsPromptBlock   *pb);
void         moo_ws_prompt_block_new_line       (MooWsPromptBlock   *pb);


G_END_DECLS

#endif /* MOO_WS_PROMPT_BLOCK_H */
