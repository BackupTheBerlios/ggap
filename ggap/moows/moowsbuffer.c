/*
 *   moowsbuffer.c
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

#include "ggap-i18n.h"
#include "moowsbuffer.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/mooutils-gobject.h"
#include <gdk/gdkkeysyms.h>
#include <string.h>


struct MooWsBufferPrivate {
    MooWsBlock *first_block;
    MooWsBlock *last_block;
    MooWsBlock *last_edited;
    guint modifying_text;
    guint user_edit;
};

static void     moo_ws_buffer_check             (MooWsBuffer    *buffer);

static void     moo_ws_buffer_insert_text       (GtkTextBuffer  *buffer,
                                                 GtkTextIter    *where,
                                                 const char     *text,
                                                 int             len);
static void     moo_ws_buffer_delete_range      (GtkTextBuffer  *buffer,
                                                 GtkTextIter    *start,
                                                 GtkTextIter    *end);

static void     moo_ws_buffer_delete_block_priv (MooWsBuffer    *buffer,
                                                 MooWsBlock     *block,
                                                 gboolean        destroying);

G_DEFINE_TYPE (MooWsBuffer, moo_ws_buffer, MOO_TYPE_TEXT_BUFFER)

enum {
    BEEP,
    N_SIGNALS
};

static guint signals[N_SIGNALS];


static void
moo_ws_buffer_init (MooWsBuffer *buffer)
{
    buffer->priv = G_TYPE_INSTANCE_GET_PRIVATE (buffer,
                                                MOO_TYPE_WS_BUFFER,
                                                MooWsBufferPrivate);

    buffer->priv->first_block = NULL;
    buffer->priv->last_block = NULL;
    buffer->priv->last_edited = NULL;
}

static void
moo_ws_buffer_dispose (GObject *object)
{
    MooWsBuffer *buffer = MOO_WS_BUFFER (object);

    if (buffer->priv)
    {
        while (buffer->priv->first_block)
            moo_ws_buffer_delete_block_priv (buffer, buffer->priv->first_block, TRUE);

        buffer->priv = NULL;
    }

    G_OBJECT_CLASS (moo_ws_buffer_parent_class)->dispose (object);
}

static void
moo_ws_buffer_class_init (MooWsBufferClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkTextBufferClass *buf_class = GTK_TEXT_BUFFER_CLASS (klass);

    object_class->dispose = moo_ws_buffer_dispose;

    buf_class->insert_text = moo_ws_buffer_insert_text;
    buf_class->delete_range = moo_ws_buffer_delete_range;

    g_type_class_add_private (klass, sizeof (MooWsBufferPrivate));

    signals[BEEP] = g_signal_new ("beep",
                                  MOO_TYPE_WS_BUFFER,
                                  G_SIGNAL_RUN_LAST,
                                  0, NULL, NULL,
                                  g_cclosure_marshal_VOID__VOID,
                                  G_TYPE_NONE, 0);
}


void
moo_ws_buffer_insert_block (MooWsBuffer *buffer,
                            MooWsBlock  *block,
                            MooWsBlock  *after)
{
    g_return_if_fail (MOO_IS_WS_BUFFER (buffer));
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (block->buffer == NULL);
    g_return_if_fail (!after || after->buffer == buffer);

    MOO_OBJECT_REF_SINK (block);

    moo_text_buffer_begin_not_undoable_action (MOO_TEXT_BUFFER (buffer));
    _moo_ws_buffer_start_edit (buffer);
    _moo_ws_block_add (block, buffer, after,
                       after ? after->next : buffer->priv->first_block);
    _moo_ws_buffer_end_edit (buffer);
    moo_text_buffer_end_not_undoable_action (MOO_TEXT_BUFFER (buffer));
    buffer->priv->last_edited = NULL;

    g_return_if_fail (block->buffer == buffer);

    if (after == buffer->priv->last_block)
        buffer->priv->last_block = block;
    if (!after)
        buffer->priv->first_block = block;

    moo_ws_buffer_check (buffer);
}

void
moo_ws_buffer_append_block (MooWsBuffer *buffer,
                            MooWsBlock  *block)
{
    g_return_if_fail (MOO_IS_WS_BUFFER (buffer));
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    moo_ws_buffer_insert_block (buffer, block, buffer->priv->last_block);
}

static void
moo_ws_buffer_delete_block_priv (MooWsBuffer *buffer,
                                 MooWsBlock  *block,
                                 gboolean     destroying)
{
    g_return_if_fail (MOO_IS_WS_BUFFER (buffer));
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (block->buffer == buffer);

    if (block == buffer->priv->first_block)
        buffer->priv->first_block = block->next;
    if (block == buffer->priv->last_block)
        buffer->priv->last_block = block->prev;

    if (!destroying)
    {
        moo_text_buffer_begin_not_undoable_action (MOO_TEXT_BUFFER (buffer));
        _moo_ws_buffer_start_edit (buffer);
        _moo_ws_block_remove (block);
        _moo_ws_buffer_end_edit (buffer);
        moo_text_buffer_end_not_undoable_action (MOO_TEXT_BUFFER (buffer));
        buffer->priv->last_edited = NULL;
        moo_ws_buffer_check (buffer);
    }

    gtk_object_destroy (GTK_OBJECT (block));
    g_object_unref (block);
}

void
moo_ws_buffer_delete_block (MooWsBuffer *buffer,
                            MooWsBlock  *block)
{
    g_return_if_fail (MOO_IS_WS_BUFFER (buffer));
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (block->buffer == buffer);
    moo_ws_buffer_delete_block_priv (buffer, block, FALSE);
}


MooWsBlock *
moo_ws_buffer_get_first_block (MooWsBuffer *buffer)
{
    g_return_val_if_fail (MOO_IS_WS_BUFFER (buffer), NULL);
    return buffer->priv->first_block;
}

MooWsBlock *
moo_ws_buffer_get_last_block (MooWsBuffer *buffer)
{
    g_return_val_if_fail (MOO_IS_WS_BUFFER (buffer), NULL);
    return buffer->priv->last_block;
}


void
_moo_ws_buffer_beep (MooWsBuffer *buffer)
{
    g_signal_emit (buffer, signals[BEEP], 0);
}


void
_moo_ws_buffer_start_edit (MooWsBuffer *buffer)
{
    g_return_if_fail (MOO_IS_WS_BUFFER (buffer));
    buffer->priv->modifying_text += 1;
}

void
_moo_ws_buffer_end_edit (MooWsBuffer *buffer)
{
    g_return_if_fail (MOO_IS_WS_BUFFER (buffer));
    g_return_if_fail (buffer->priv->modifying_text != 0);
    buffer->priv->modifying_text -= 1;
}

void
_moo_ws_buffer_start_user_edit (MooWsBuffer *buffer)
{
    g_return_if_fail (MOO_IS_WS_BUFFER (buffer));
    buffer->priv->user_edit += 1;
}

void
_moo_ws_buffer_end_user_edit (MooWsBuffer *buffer)
{
    g_return_if_fail (MOO_IS_WS_BUFFER (buffer));
    g_return_if_fail (buffer->priv->user_edit != 0);
    buffer->priv->user_edit -= 1;
}


static void
clear_undo (MooWsBuffer *buffer)
{
    moo_text_buffer_begin_not_undoable_action (MOO_TEXT_BUFFER (buffer));
    moo_text_buffer_end_not_undoable_action (MOO_TEXT_BUFFER (buffer));
}

static void
moo_ws_buffer_insert_text (GtkTextBuffer *text_buffer,
                           GtkTextIter   *where,
                           const char    *text,
                           int            len)
{
    MooWsBuffer *buffer = MOO_WS_BUFFER (text_buffer);
    MooWsBlock *block;
    gboolean inserted;

    if (!buffer->priv->modifying_text && !buffer->priv->user_edit)
    {
        g_critical ("%s: oops", G_STRFUNC);
        return;
    }

    if (buffer->priv->modifying_text)
    {
        GTK_TEXT_BUFFER_CLASS (moo_ws_buffer_parent_class)->
            insert_text (text_buffer, where, text, len);
        return;
    }

    g_assert (gtk_text_iter_is_end (where) ||
                gtk_text_iter_is_cursor_position (where));

    block = _moo_ws_iter_get_block (where);

    if (!block)
    {
        _moo_ws_buffer_beep (buffer);
        return;
    }

    if (block != buffer->priv->last_edited)
        clear_undo (buffer);

    inserted = _moo_ws_block_insert_interactive (block, where, text, len);

    if (buffer->priv->user_edit && inserted)
    {
        GtkTextIter cursor;
        gtk_text_buffer_get_iter_at_mark (text_buffer, &cursor,
                                          gtk_text_buffer_get_insert (text_buffer));
        if (!gtk_text_iter_equal (where, &cursor))
            gtk_text_buffer_place_cursor (text_buffer, where);
    }

    buffer->priv->last_edited = block;

    moo_ws_buffer_check (buffer);
}

static void
moo_ws_buffer_delete_range (GtkTextBuffer *text_buffer,
                            GtkTextIter   *start,
                            GtkTextIter   *end)
{
    MooWsBuffer *buffer = MOO_WS_BUFFER (text_buffer);
    MooWsBlock *start_block, *end_block, *block;
    gboolean delete_from_start = FALSE;
    gboolean delete_to_end = FALSE;
    GtkTextMark *start_mark, *end_mark;
    GtkTextIter iter;

    if (!buffer->priv->modifying_text && !buffer->priv->user_edit)
    {
        g_critical ("%s: oops", G_STRFUNC);
        return;
    }

    if (buffer->priv->modifying_text)
    {
        GTK_TEXT_BUFFER_CLASS (moo_ws_buffer_parent_class)->
            delete_range (text_buffer, start, end);
        return;
    }

    gtk_text_iter_order (start, end);
    start_block = _moo_ws_iter_get_block (start);
    end_block = _moo_ws_iter_get_block (end);
    g_assert (start_block != NULL);

    if (!end_block || gtk_text_iter_is_end (end))
    {
        end_block = buffer->priv->last_block;
        delete_to_end = TRUE;
    }
    else if (gtk_text_iter_starts_line (end))
    {
        _moo_ws_block_get_start_iter (end_block, &iter);

        if (gtk_text_iter_equal (end, &iter))
        {
            end_block = end_block->prev;
            delete_to_end = TRUE;
        }
    }

    if (gtk_text_iter_starts_line (start))
    {
        _moo_ws_block_get_start_iter (start_block, &iter);

        if (gtk_text_iter_equal (start, &iter))
            delete_from_start = TRUE;
    }

    start_mark = gtk_text_buffer_create_mark (text_buffer, NULL, start, TRUE);
    end_mark = gtk_text_buffer_create_mark (text_buffer, NULL, end, FALSE);

    if (start_block == end_block)
    {
        if (delete_from_start && delete_to_end)
        {
            moo_ws_buffer_delete_block (buffer, start_block);
        }
        else
        {
            if (start_block != buffer->priv->last_edited)
                clear_undo (buffer);

            _moo_ws_block_delete_interactive (start_block, start, end);
            gtk_text_buffer_move_mark (text_buffer, start_mark, start);

            buffer->priv->last_edited = start_block;
        }

        goto out;
    }

    buffer->priv->last_edited = NULL;
    moo_text_buffer_begin_not_undoable_action (MOO_TEXT_BUFFER (buffer));

    block = start_block->next;
    g_assert (block != NULL);

    if (delete_from_start)
    {
        moo_ws_buffer_delete_block (buffer, start_block);
    }
    else
    {
        _moo_ws_block_get_end_iter (start_block, end);
        _moo_ws_block_delete_interactive (start_block, start, end);
        gtk_text_buffer_move_mark (text_buffer, start_mark, start);
    }

    while (block != end_block)
    {
        MooWsBlock *next = block->next;
        moo_ws_buffer_delete_block (buffer, block);
        block = next;
    }

    if (delete_to_end)
    {
        moo_ws_buffer_delete_block (buffer, end_block);
    }
    else
    {
        _moo_ws_block_get_start_iter (end_block, start);
        gtk_text_buffer_get_iter_at_mark (text_buffer, end, end_mark);
        _moo_ws_block_delete_interactive (end_block, start, end);
    }

    moo_text_buffer_end_not_undoable_action (MOO_TEXT_BUFFER (buffer));

out:
    gtk_text_buffer_get_iter_at_mark (text_buffer, start, start_mark);
    gtk_text_buffer_get_iter_at_mark (text_buffer, end, end_mark);
    gtk_text_buffer_delete_mark (text_buffer, start_mark);
    gtk_text_buffer_delete_mark (text_buffer, end_mark);

    gtk_text_buffer_get_iter_at_mark (text_buffer, &iter,
                                      gtk_text_buffer_get_insert (text_buffer));
    gtk_text_buffer_place_cursor (text_buffer, &iter);

    moo_ws_buffer_check (buffer);
}


// static void
// moo_ws_buffer_check_tags (MooWsBuffer *buffer)
// {
//     GtkTextIter iter;
//
//     gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (buffer), &iter);
//
//     while (!gtk_text_iter_is_end (&iter))
//     {
//         GSList *tags;
//         GtkTextTag *block_tag = NULL;
//
//         tags = gtk_text_iter_get_tags (&iter);
//
//         while (tags)
//         {
//             if (g_object_get_data (tags->data, "moo-ws-block"))
//             {
//                 if (block_tag)
//                     g_assert (g_object_get_data (tags->data, "moo-ws-block") ==
//                                 g_object_get_data (G_OBJECT (block_tag), "moo-ws-block"));
//                 else
//                     block_tag = tags->data;
//             }
//
//             tags = g_slist_delete_link (tags, tags);
//         }
//
//         g_assert (block_tag != NULL);
//
//         gtk_text_iter_forward_to_tag_toggle (&iter, NULL);
//     }
// }

static void
moo_ws_buffer_check (G_GNUC_UNUSED MooWsBuffer *buffer)
{
//     moo_ws_buffer_check_tags (buffer);
}
