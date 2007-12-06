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
moo_ws_block_init (MooWsBlock *block)
{
    block->tag = gtk_text_tag_new (NULL);
    g_object_set_data (G_OBJECT (block->tag), "moo-ws-block", block);
}

static void
moo_ws_block_finalize (GObject *object)
{
    MooWsBlock *block = MOO_WS_BLOCK (object);
    g_object_unref (block->tag);
    G_OBJECT_CLASS (moo_ws_block_parent_class)->finalize (object);
}

#if 0
static void
pb (const char *text, MooWsBlock *block)
{
    if (block)
    {
        GtkTextIter s, e;
        _moo_ws_block_get_start_iter (block, &s);
        _moo_ws_block_get_end_iter (block, &e);
        if (text)
            g_print ("%s: start: %d, %d; end: %d, %d\n", text,
                     gtk_text_iter_get_line (&s),
                     gtk_text_iter_get_line_offset (&s),
                     gtk_text_iter_get_line (&e),
                     gtk_text_iter_get_line_offset (&e));
        else
            g_print ("start: %d, %d; end: %d, %d\n",
                     gtk_text_iter_get_line (&s),
                     gtk_text_iter_get_line_offset (&s),
                     gtk_text_iter_get_line (&e),
                     gtk_text_iter_get_line_offset (&e));
    }
    else
    {
        if (text)
            g_print ("%s: null\n", text);
        else
            g_print ("null\n");
    }
}
#endif

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

    gtk_text_tag_table_add (gtk_text_buffer_get_tag_table (block->buffer), block->tag);

#if 0
    g_print ("--- before insert\n");
    pb ("prev", after);
    pb ("next", before);
    g_print ("------------\n");
#endif

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

        if (after)
        {
            _moo_ws_block_insert (after, &position, "\n", -1);
#if 0
            g_print ("after nl: %d, %d\n",
                     gtk_text_iter_get_line (&position),
                     gtk_text_iter_get_line_offset (&position));
#endif
            gtk_text_iter_backward_cursor_position (&position);
            gtk_text_buffer_move_mark (block->buffer, after->end, &position);
#if 0
            g_print ("moved end mark to %d, %d\n",
                     gtk_text_iter_get_line (&position),
                     gtk_text_iter_get_line_offset (&position));
#endif
            gtk_text_iter_forward_line (&position);
        }
    }

    g_assert (gtk_text_iter_get_line_offset (&position) == 0);
    gtk_text_buffer_move_mark (block->buffer, block->start, &position);
    gtk_text_buffer_move_mark (block->buffer, block->end, &position);

    if (before)
        before->prev = block;
    if (after)
        after->next = block;
    block->prev = after;
    block->next = before;

#if 0
    g_print ("--- after insert\n");
    if (after)
        pb ("prev", after);
    pb ("this", block);
    if (before)
        pb ("next", before);
    g_print ("------------\n");
#endif
}

static void
moo_ws_block_remove_real (MooWsBlock *block)
{
    GtkTextIter start_pos, end_pos;
    GtkTextTagTable *tag_table;

    g_return_if_fail (block->view != NULL);

#if 0
    g_print ("--- before remove\n");
    if (block->prev)
        pb ("prev", block->prev);
    pb ("this", block);
    if (block->next)
        pb ("next", block->next);
    g_print ("-------------\n");
#endif

    gtk_text_buffer_get_iter_at_mark (block->buffer, &start_pos, block->start);
    if (!block->next)
        gtk_text_iter_backward_cursor_position (&start_pos);

    gtk_text_buffer_get_iter_at_mark (block->buffer, &end_pos, block->end);
    g_assert (gtk_text_iter_ends_line (&end_pos));
    gtk_text_iter_forward_line (&end_pos);

    gtk_text_buffer_delete (block->buffer, &start_pos, &end_pos);

    gtk_text_buffer_delete_mark (block->buffer, block->start);
    gtk_text_buffer_delete_mark (block->buffer, block->end);

    tag_table = gtk_text_buffer_get_tag_table (block->buffer);
    gtk_text_tag_table_remove (tag_table, block->tag);

#if 0
    g_print ("--- after remove\n");
    if (block->prev)
        pb ("prev", block->prev);
    if (block->next)
        pb ("next", block->next);
    g_print ("-------------\n");
#endif

    if (block->next)
        block->next->prev = block->prev;
    if (block->prev)
        block->prev->next = block->next;

    block->view = NULL;
    block->buffer = NULL;
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
    object_class->finalize = moo_ws_block_finalize;

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

gboolean
_moo_ws_block_insert_interactive (MooWsBlock  *block,
                                  GtkTextIter *where,
                                  const char  *text,
                                  gssize       len)
{
    gboolean retval;

    g_return_val_if_fail (MOO_IS_WS_BLOCK (block), FALSE);
    g_return_val_if_fail (block->view != NULL, FALSE);

    if (!MOO_WS_BLOCK_GET_CLASS (block)->insert_interactive)
    {
        _moo_ws_view_beep (block->view);
        return FALSE;
    }

#if 0
    g_print ("--- before insert_interactive\n");
    pb (NULL, block);
    g_print ("-------------\n");
#endif

    _moo_ws_view_start_edit (block->view);
    retval = MOO_WS_BLOCK_GET_CLASS (block)->insert_interactive (block, where, text, len);
    _moo_ws_view_end_edit (block->view);

#if 0
    g_print ("--- after insert_interactive\n");
    pb (NULL, block);
    g_print ("-------------\n");
#endif

    return retval;
}

gboolean
_moo_ws_block_delete_interactive (MooWsBlock  *block,
                                  GtkTextIter *start,
                                  GtkTextIter *end)
{
    gboolean retval;

    g_return_val_if_fail (MOO_IS_WS_BLOCK (block), FALSE);
    g_return_val_if_fail (block->view != NULL, FALSE);

    if (!MOO_WS_BLOCK_GET_CLASS (block)->delete_interactive)
    {
        _moo_ws_view_beep (block->view);
        return FALSE;
    }

#if 0
    g_print ("--- before delete_interactive\n");
    pb (NULL, block);
    g_print ("-------------\n");
#endif

    _moo_ws_view_start_edit (block->view);
    retval = MOO_WS_BLOCK_GET_CLASS (block)->delete_interactive (block, start, end);
    _moo_ws_view_end_edit (block->view);

#if 0
    g_print ("--- before delete_interactive\n");
    pb (NULL, block);
    g_print ("-------------\n");
#endif

    return retval;
}

gboolean
_moo_ws_block_check_move_cursor (MooWsBlock     *block,
                                 GtkTextIter    *pos,
                                 GtkMovementStep step,
                                 int             count,
                                 gboolean        extend_selection)
{
    g_return_val_if_fail (MOO_IS_WS_BLOCK (block), FALSE);
    g_return_val_if_fail (block->view != NULL, FALSE);

    if (!MOO_WS_BLOCK_GET_CLASS (block)->check_move_cursor)
        return FALSE;
    else
        return MOO_WS_BLOCK_GET_CLASS (block)->
            check_move_cursor (block, pos, step, count, extend_selection);
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

#if 0
    g_print ("--- after insert_with_tags\n");
    pb (NULL, block);
    g_print ("-------------\n");
#endif
}


MooWsBlock *
_moo_ws_iter_get_block (const GtkTextIter *pos)
{
    GSList *tags;

    g_return_val_if_fail (pos != NULL, NULL);

    if (gtk_text_iter_is_end (pos))
    {
        MooWsView *view;

        view = _moo_ws_iter_get_view (pos);
        g_return_val_if_fail (view != NULL, NULL);

        return _moo_ws_view_get_last_block (view);
    }

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

MooWsView *
_moo_ws_iter_get_view (const GtkTextIter *iter)
{
    GtkTextBuffer *buffer;

    buffer = gtk_text_iter_get_buffer (iter);
    g_return_val_if_fail (buffer != NULL, NULL);

    return _moo_ws_buffer_get_view (buffer);
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
            gtk_text_iter_set_line_offset (iter, 0);
        }

        _moo_ws_block_get_start_iter (block, &start_iter);
        *real_line = gtk_text_iter_get_line (iter) -
                        gtk_text_iter_get_line (&start_iter);
    }
    else
    {
        _moo_ws_block_get_start_iter (block, iter);

        if (line > 0)
        {
            int first_line, last_line;
            GtkTextIter end;

            _moo_ws_block_get_end_iter (block, &end);
            first_line = gtk_text_iter_get_line (iter);
            last_line = gtk_text_iter_get_line (&end);

            if (line > last_line - first_line)
            {
                g_critical ("%s: line number %d too big", G_STRFUNC, line);
                line = last_line - first_line;
            }
        }

        if (line > 0)
            gtk_text_iter_forward_lines (iter, line);

        *real_line = line;
    }
}
