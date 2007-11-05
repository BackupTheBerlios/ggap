/*
 *   moowsblock.c
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

#include "moowsblock.h"


G_DEFINE_TYPE (MooWsBlock, moo_ws_block, GTK_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_VIEW
};

static void
moo_ws_block_init (G_GNUC_UNUSED MooWsBlock *block)
{
}

static void
moo_ws_block_add_real (MooWsBlock *block,
                       MooWsView  *view,
                       MooWsBlock *after,
                       MooWsBlock *before)
{
    GtkTextIter position;

    g_return_if_fail (block->view == NULL);
    g_return_if_fail (!before || before->prev == after);
    g_return_if_fail (!after || after->next == before);

    block->view = view;
    block->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));

    gtk_text_buffer_get_start_iter (block->buffer, &position);
    g_assert (gtk_text_iter_is_end (&position) || gtk_text_iter_is_cursor_position (&position));
    block->start = gtk_text_buffer_create_mark (block->buffer, NULL, &position, TRUE);
    g_object_set_data (G_OBJECT (block->start), "moo-ws-block", block);
    g_object_set_data (G_OBJECT (block->start), "moo-ws-block-start", GINT_TO_POINTER (TRUE));
    block->end = gtk_text_buffer_create_mark (block->buffer, NULL, &position, FALSE);
    g_object_set_data (G_OBJECT (block->end), "moo-ws-block", block);
    g_object_set_data (G_OBJECT (block->end), "moo-ws-block-end", GINT_TO_POINTER (TRUE));
    block->tag = gtk_text_buffer_create_tag (block->buffer, NULL, NULL);
    g_object_set_data (G_OBJECT (block->tag), "moo-ws-block", block);

    if (before)
    {
        gtk_text_buffer_get_iter_at_mark (block->buffer, &position, before->start);
        g_assert (gtk_text_iter_get_line_offset (&position) == 0);
        _moo_ws_block_insert (block, &position, "\n", -1);
        gtk_text_buffer_move_mark (block->buffer, before->start, &position);
        gtk_text_iter_backward_line (&position);
    }
    else
    {
        gtk_text_buffer_get_end_iter (block->buffer, &position);
        g_assert (gtk_text_iter_get_line_offset (&position) == 0);
        _moo_ws_block_insert (block, &position, "\n", -1);
        gtk_text_iter_backward_line (&position);
    }

    gtk_text_buffer_move_mark (block->buffer, block->start, &position);
    gtk_text_buffer_move_mark (block->buffer, block->end, &position);

    if (before)
        before->prev = block;
    if (after)
        after->next = block;
    block->prev = after;
    block->next = before;
}

static void
moo_ws_block_remove_real (MooWsBlock *block)
{
    GtkTextIter start_pos, end_pos;
    GtkTextTagTable *tag_table;

    g_return_if_fail (block->view != NULL);

    gtk_text_buffer_get_iter_at_mark (block->buffer, &start_pos, block->start);
    gtk_text_buffer_get_iter_at_mark (block->buffer, &end_pos, block->end);
    g_assert (gtk_text_iter_ends_line (&end_pos));
    gtk_text_iter_forward_line (&end_pos);
    gtk_text_buffer_delete (block->buffer, &start_pos, &end_pos);

    gtk_text_buffer_delete_mark (block->buffer, block->start);
    gtk_text_buffer_delete_mark (block->buffer, block->end);

    tag_table = gtk_text_buffer_get_tag_table (block->buffer);
    gtk_text_tag_table_remove (tag_table, block->tag);

    if (block->next)
        block->next->prev = block->prev;
    if (block->prev)
        block->prev->next = block->next;

    block->view = NULL;
    block->buffer = NULL;
    block->tag = NULL;
    block->start = NULL;
    block->end = NULL;
    block->prev = NULL;
    block->next = NULL;
}

// static void
// moo_ws_block_set_property (GObject      *object,
//                            guint         property_id,
//                            const GValue *value,
//                            GParamSpec   *pspec)
// {
//     MooWsView *view;
//     MooWsBlock *block = MOO_WS_BLOCK (object);
//
//     switch (property_id)
//     {
//         case PROP_VIEW:
//             view = g_value_get_object (value);
//             if (view)
//                 moo_ws_view_insert_block (view, block, _moo_ws_view_get_last_block (view));
//             else if (block->view)
//                 moo_ws_view_delete_block (MooWsView  *view,
//                                          MooWsBlock *block);
//
//         default:
//             G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
//     }
// }

static void
moo_ws_block_get_property (GObject    *object,
                           guint       property_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
    MooWsBlock *block = MOO_WS_BLOCK (object);

    switch (property_id)
    {
        case PROP_VIEW:
            g_value_set_object (value, block->view);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
moo_ws_block_class_init (MooWsBlockClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

//     object_class->set_property = moo_ws_block_set_property;
    object_class->get_property = moo_ws_block_get_property;

    klass->add = moo_ws_block_add_real;
    klass->remove = moo_ws_block_remove_real;

    g_object_class_install_property (object_class, PROP_VIEW,
        g_param_spec_object ("view", "view", "view",
                             MOO_TYPE_WS_VIEW,
                             G_PARAM_READABLE));
}


void
_moo_ws_block_add (MooWsBlock *block,
                   MooWsView  *view,
                   MooWsBlock *after,
                   MooWsBlock *before)
{
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (MOO_IS_WS_VIEW (view));
    g_return_if_fail (block->view == NULL);
    g_return_if_fail (!after || MOO_IS_WS_BLOCK (after));
    g_return_if_fail (!after || after->view == view);
    g_return_if_fail (!after || after->next == before);
    g_return_if_fail (!before || MOO_IS_WS_BLOCK (before));
    g_return_if_fail (!before || before->view == view);
    g_return_if_fail (!before || before->prev == after);

    MOO_WS_BLOCK_GET_CLASS(block)->add (block, view, after, before);
}

void
_moo_ws_block_remove (MooWsBlock *block)
{
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (block->view != NULL);
    MOO_WS_BLOCK_GET_CLASS(block)->remove (block);
}

void
_moo_ws_block_insert_text (MooWsBlock  *block,
                           GtkTextIter *where,
                           const char  *text,
                           gssize       len)
{
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (block->view != NULL);

    if (!MOO_WS_BLOCK_GET_CLASS (block)->insert_text)
    {
        _moo_ws_view_beep (block->view);
        return;
    }

    MOO_WS_BLOCK_GET_CLASS (block)->insert_text (block, where, text, len);
}

void
_moo_ws_block_delete_text (MooWsBlock  *block,
                           GtkTextIter *start,
                           GtkTextIter *end)
{
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (block->view != NULL);

    if (!MOO_WS_BLOCK_GET_CLASS (block)->delete_text)
    {
        _moo_ws_view_beep (block->view);
        return;
    }

    MOO_WS_BLOCK_GET_CLASS (block)->delete_text (block, start, end);
}


void
_moo_ws_block_insert (MooWsBlock  *block,
                      GtkTextIter *where,
                      const char  *text,
                      gssize       len)
{
    gtk_text_buffer_insert_with_tags (block->buffer, where, text, len,
                                      block->tag, NULL);
}

void
_moo_ws_block_insert_with_tags (MooWsBlock  *block,
                                GtkTextIter *where,
                                const char  *text,
                                gssize       len,
                                GtkTextTag  *tag,
                                ...)
{
    gint start_offset;
    GtkTextIter start;
    va_list args;

    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (block->buffer != NULL);
    g_return_if_fail (where != NULL);
    g_return_if_fail (text != NULL);

    start_offset = gtk_text_iter_get_offset (where);
    gtk_text_buffer_insert (block->buffer, where, text, len);
    gtk_text_buffer_get_iter_at_offset (block->buffer, &start, start_offset);

    gtk_text_buffer_apply_tag (block->buffer, block->tag, &start, where);

    va_start (args, tag);

    while (tag)
    {
        gtk_text_buffer_apply_tag (block->buffer, tag, &start, where);
        tag = va_arg (args, GtkTextTag*);
    }

    va_end (args);
}


MooWsBlock *
_moo_ws_iter_get_block (const GtkTextIter *pos)
{
    GSList *tags;

    g_return_val_if_fail (pos != NULL, NULL);

    tags = gtk_text_iter_get_tags (pos);

    while (tags)
    {
        MooWsBlock *block = g_object_get_data (tags->data, "moo-ws-block");

        if (block)
        {
            g_slist_free (tags);
            return block;
        }

        tags = g_slist_delete_link (tags, tags);
    }

    return NULL;
}

void
_moo_ws_block_get_start_iter (MooWsBlock  *block,
                              GtkTextIter *iter)
{
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (iter != NULL);
    gtk_text_buffer_get_iter_at_mark (block->buffer, iter, block->start);
}

void
_moo_ws_block_get_end_iter (MooWsBlock  *block,
                            GtkTextIter *iter)
{
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (iter != NULL);
    gtk_text_buffer_get_iter_at_mark (block->buffer, iter, block->end);
}

void
_moo_ws_block_get_iter_at_line (MooWsBlock  *block,
                                GtkTextIter *iter,
                                int          line,
                                int         *real_line)
{
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (block->buffer != NULL);

    if (line < 0)
    {
        GtkTextIter start_iter;

        if (block->next)
        {
            _moo_ws_block_get_start_iter (block->next, iter);
            gtk_text_iter_backward_line (iter);
        }
        else
        {
            gtk_text_buffer_get_end_iter (block->buffer, iter);
            gtk_text_iter_backward_line (iter);
        }

        _moo_ws_block_get_start_iter (block, &start_iter);
        *real_line = gtk_text_iter_get_line (iter) -
                        gtk_text_iter_get_line (&start_iter);
    }
    else
    {
        int i;

        _moo_ws_block_get_start_iter (block, iter);

        for (i = 0; i < line; ++i)
        {
            gtk_text_iter_forward_line (iter);

            if (_moo_ws_iter_get_block (iter) != block)
            {
                g_critical ("%s: line number %d too big", G_STRFUNC, line);
                gtk_text_iter_backward_line (iter);
                break;
            }
        }

        *real_line = i;
    }
}
