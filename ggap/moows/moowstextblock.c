/*
 *   moowstextblock.c
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

#include "moowstextblock.h"


struct _MooWsTextBlockPrivate {
    char *text;
    gboolean editable;
};


G_DEFINE_TYPE (MooWsTextBlock, moo_ws_text_block, MOO_TYPE_WS_BLOCK)


static void
moo_ws_text_block_init (MooWsTextBlock *block)
{
    block->priv = g_new0 (MooWsTextBlockPrivate, 1);
    block->priv->text = NULL;
}

static void
moo_ws_text_block_dispose (GObject *object)
{
    MooWsTextBlock *tb = MOO_WS_TEXT_BLOCK (object);

    if (tb->priv)
    {
        g_free (tb->priv->text);
        g_free (tb->priv);
        tb->priv = NULL;
    }

    G_OBJECT_CLASS (moo_ws_text_block_parent_class)->dispose (object);
}

static void
moo_ws_text_block_add (MooWsBlock *block,
                       MooWsView  *view,
                       MooWsBlock *after,
                       MooWsBlock *before)
{
    MooWsTextBlock *tb = MOO_WS_TEXT_BLOCK (block);

    MOO_WS_BLOCK_CLASS (moo_ws_text_block_parent_class)->add (block, view, after, before);

    if (tb->priv->text)
    {
        char *text = tb->priv->text;
        tb->priv->text = NULL;
        moo_ws_text_block_set_text (tb, text);
        g_free (text);
    }
}

static void
moo_ws_text_block_insert_text (MooWsBlock  *block,
                               GtkTextIter *where,
                               const char  *text,
                               gssize       len)
{
    MooWsTextBlock *tb = MOO_WS_TEXT_BLOCK (block);

    if (tb->priv->editable)
        _moo_ws_block_insert (block, where, text, len);
    else
        _moo_ws_view_beep (block->view);
}

static void
moo_ws_text_block_delete_text (MooWsBlock  *block,
                               GtkTextIter *start,
                               GtkTextIter *end)
{
    MooWsTextBlock *tb = MOO_WS_TEXT_BLOCK (block);

    if (tb->priv->editable)
        gtk_text_buffer_delete (block->buffer, start, end);
    else
        _moo_ws_view_beep (block->view);
}

static void
moo_ws_text_block_class_init (MooWsTextBlockClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MooWsBlockClass *block_class = MOO_WS_BLOCK_CLASS (klass);

    object_class->dispose = moo_ws_text_block_dispose;

    block_class->add = moo_ws_text_block_add;
    block_class->insert_text = moo_ws_text_block_insert_text;
    block_class->delete_text = moo_ws_text_block_delete_text;
}


MooWsTextBlock *
moo_ws_text_block_new (void)
{
    return g_object_new (MOO_TYPE_WS_TEXT_BLOCK, NULL);
}


void
moo_ws_text_block_set_text (MooWsTextBlock *tb,
                            const char     *text)
{
    GtkTextIter start, end;
    MooWsBlock *block = MOO_WS_BLOCK (tb);

    if (!block->view)
    {
        g_free (tb->priv->text);
        tb->priv->text = g_strdup (text);
        return;
    }

    gtk_text_buffer_get_iter_at_mark (block->buffer, &start, block->start);
    gtk_text_buffer_get_iter_at_mark (block->buffer, &end, block->end);

    _moo_ws_view_start_edit (block->view);

    gtk_text_buffer_delete (block->buffer, &start, &end);

    if (text)
        _moo_ws_block_insert (block, &start, text, -1);

    _moo_ws_view_end_edit (block->view);
}

void
moo_ws_text_block_append (MooWsTextBlock *tb,
                          const char     *text)
{
    GtkTextIter iter;
    MooWsBlock *block;

    g_return_if_fail (MOO_IS_WS_TEXT_BLOCK (tb));
    g_return_if_fail (text != NULL);

    block = MOO_WS_BLOCK (tb);
    g_return_if_fail (block->view != NULL);

    _moo_ws_block_get_end_iter (block, &iter);

    _moo_ws_view_start_edit (block->view);
    _moo_ws_block_insert (block, &iter, text, -1);
    _moo_ws_view_end_edit (block->view);
}
