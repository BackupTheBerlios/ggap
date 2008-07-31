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
#include "mooworksheet.h"
#include <mooutils/mooutils-misc.h>


struct MooWsTextBlockPrivate {
    char *text;
    char *indent;
    guint indent_width : 29;
    gboolean editable : 1;
    gboolean is_output : 1;
    guint output_type : 1;
};


G_DEFINE_TYPE (MooWsTextBlock, moo_ws_text_block, MOO_TYPE_WS_BLOCK)


static void
moo_ws_text_block_init (MooWsTextBlock *block)
{
    block->priv = G_TYPE_INSTANCE_GET_PRIVATE (block, MOO_TYPE_WS_TEXT_BLOCK,
                                               MooWsTextBlockPrivate);
    block->priv->text = NULL;
    block->priv->indent = NULL;
    block->priv->indent_width = 0;
}

static void
moo_ws_text_block_dispose (GObject *object)
{
    MooWsTextBlock *tb = MOO_WS_TEXT_BLOCK (object);

    if (tb->priv)
    {
        g_free (tb->priv->text);
        g_free (tb->priv->indent);
        tb->priv = NULL;
    }

    G_OBJECT_CLASS (moo_ws_text_block_parent_class)->dispose (object);
}

static void
moo_ws_text_block_add (MooWsBlock  *block,
                       MooWsBuffer *buffer,
                       MooWsBlock  *after,
                       MooWsBlock  *before)
{
    MooWsTextBlock *tb = MOO_WS_TEXT_BLOCK (block);

    MOO_WS_BLOCK_CLASS (moo_ws_text_block_parent_class)->
        add (block, buffer, after, before);

    if (tb->priv->text)
    {
        char *text = tb->priv->text;
        tb->priv->text = NULL;
        moo_ws_text_block_set_text (tb, text);
        g_free (text);
    }
}

static gboolean
moo_ws_text_block_insert_interactive (MooWsBlock  *block,
                                      GtkTextIter *where,
                                      const char  *text,
                                      gssize       len)
{
    MooWsTextBlock *tb = MOO_WS_TEXT_BLOCK (block);

    if (tb->priv->editable)
    {
        _moo_ws_block_insert (block, where, text, len);
        return TRUE;
    }
    else
    {
        _moo_ws_buffer_beep (block->buffer);
        return FALSE;
    }
}

static gboolean
moo_ws_text_block_delete_interactive (MooWsBlock  *block,
                                      GtkTextIter *start,
                                      GtkTextIter *end)
{
    MooWsTextBlock *tb = MOO_WS_TEXT_BLOCK (block);

    if (tb->priv->editable)
    {
        gtk_text_buffer_delete (GTK_TEXT_BUFFER (block->buffer), start, end);
        return TRUE;
    }
    else
    {
        _moo_ws_buffer_beep (block->buffer);
        return FALSE;
    }
}

static void
moo_ws_text_block_class_init (MooWsTextBlockClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MooWsBlockClass *block_class = MOO_WS_BLOCK_CLASS (klass);

    object_class->dispose = moo_ws_text_block_dispose;

    block_class->add = moo_ws_text_block_add;
    block_class->insert_interactive = moo_ws_text_block_insert_interactive;
    block_class->delete_interactive = moo_ws_text_block_delete_interactive;

    g_type_class_add_private (klass, sizeof (MooWsTextBlockPrivate));
}


MooWsTextBlock *
moo_ws_user_text_block_new (void)
{
    MooWsTextBlock *block = g_object_new (MOO_TYPE_WS_TEXT_BLOCK, NULL);
    block->priv->is_output = FALSE;
    block->priv->editable = TRUE;
    block->priv->indent_width = 0;
    block->priv->indent = NULL;
    return block;
}

MooWsTextBlock *
moo_ws_output_block_new (MooWsOutputType output_type)
{
    MooWsTextBlock *block = g_object_new (MOO_TYPE_WS_TEXT_BLOCK, NULL);
    block->priv->is_output = TRUE;
    block->priv->output_type = output_type;
    block->priv->editable = FALSE;
    block->priv->indent_width = MOO_WORKSHEET_OUTPUT_INDENT;
    block->priv->indent = g_strnfill (MOO_WORKSHEET_OUTPUT_INDENT, ' ');

    if (output_type == MOO_WS_OUTPUT_ERR)
        MOO_WS_BLOCK (block)->tag = MOO_WS_TAG_OUTPUT_ERR;

    return block;
}


gboolean
moo_ws_text_block_is_output (MooWsTextBlock *block)
{
    g_return_val_if_fail (MOO_IS_WS_TEXT_BLOCK (block), FALSE);
    return block->priv->is_output;
}

MooWsOutputType
moo_ws_output_block_get_output_type (MooWsTextBlock *block)
{
    g_return_val_if_fail (MOO_IS_WS_TEXT_BLOCK (block), MOO_WS_OUTPUT_OUT);
    g_return_val_if_fail (block->priv->is_output, MOO_WS_OUTPUT_OUT);
    return block->priv->output_type;
}


void
moo_ws_text_block_set_text (MooWsTextBlock *tb,
                            const char     *text)
{
    GtkTextIter start, end;
    MooWsBlock *block = MOO_WS_BLOCK (tb);

    if (!block->buffer)
    {
        g_free (tb->priv->text);
        tb->priv->text = g_strdup (text);
        return;
    }

    gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER (block->buffer),
                                      &start, block->start);
    gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER (block->buffer),
                                      &end, block->end);

    _moo_ws_buffer_start_edit (block->buffer);
    gtk_text_buffer_delete (GTK_TEXT_BUFFER (block->buffer), &start, &end);
    _moo_ws_buffer_end_edit (block->buffer);

    if (text)
        moo_ws_text_block_append (tb, text);
}

void
moo_ws_text_block_append (MooWsTextBlock *tb,
                          const char     *text)
{
    GtkTextIter iter;
    MooWsBlock *block;
    MooLineReader lr;
    const char *line;
    gsize line_len;

    g_return_if_fail (MOO_IS_WS_TEXT_BLOCK (tb));
    g_return_if_fail (text != NULL);

    block = MOO_WS_BLOCK (tb);
    g_return_if_fail (block->buffer != NULL);

    _moo_ws_block_get_end_iter (block, &iter);
    _moo_ws_buffer_start_edit (block->buffer);

    if (tb->priv->indent && gtk_text_iter_starts_line (&iter))
        _moo_ws_block_insert (block, &iter, tb->priv->indent, -1);

    for (moo_line_reader_init (&lr, text, -1);
         (line = moo_line_reader_get_line (&lr, &line_len, NULL)); )
    {
        if (line != text)
        {
            _moo_ws_block_insert (block, &iter, "\n", -1);
            if (tb->priv->indent)
                _moo_ws_block_insert (block, &iter, tb->priv->indent, -1);
        }

        _moo_ws_block_insert (block, &iter, line, line_len);
    }

    _moo_ws_buffer_end_edit (block->buffer);
}


char *
moo_ws_text_block_get_text (MooWsTextBlock *tb)
{
    GtkTextIter start, end;
    MooWsBlock *block;
    GString *string;

    g_return_val_if_fail (MOO_IS_WS_TEXT_BLOCK (tb), NULL);

    block = MOO_WS_BLOCK (tb);

    if (!block->buffer)
        return g_strdup (tb->priv->text);

    gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER (block->buffer),
                                      &start, block->start);
    gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER (block->buffer),
                                      &end, block->end);

    if (!tb->priv->indent_width)
        return gtk_text_buffer_get_slice (GTK_TEXT_BUFFER (block->buffer),
                                          &start, &end, TRUE);

    if (gtk_text_iter_get_line (&start) == gtk_text_iter_get_line (&end))
    {
        gtk_text_iter_forward_chars (&start, tb->priv->indent_width);
        return gtk_text_buffer_get_slice (GTK_TEXT_BUFFER (block->buffer),
                                          &start, &end, TRUE);
    }

    g_assert (gtk_text_iter_starts_line (&start));
    string = g_string_new (NULL);

    do
    {
        GtkTextIter line_end;

        if (string->len)
            g_string_append (string, "\n");

        line_end = start;
        if (!gtk_text_iter_ends_line (&line_end))
            gtk_text_iter_forward_to_line_end (&line_end);
        gtk_text_iter_forward_chars (&start, tb->priv->indent_width);

        if (gtk_text_iter_compare (&start, &line_end) < 0)
        {
            char *slice = gtk_text_buffer_get_slice (GTK_TEXT_BUFFER (block->buffer),
                                                     &start, &line_end, TRUE);
            g_string_append (string, slice);
            g_free (slice);
        }

        gtk_text_iter_forward_line (&start);
    }
    while (gtk_text_iter_compare (&start, &end) < 0);

    return g_string_free (string, FALSE);
}
