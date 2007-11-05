/*
 *   moowsview.m
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
#include "moowsblock.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/mooutils-gobject.h"
#include <gdk/gdkkeysyms.h>
#include <string.h>


G_DEFINE_TYPE (MooWsView, moo_ws_view, MOO_TYPE_TEXT_VIEW)


struct _MooWsViewPrivate {
    MooWsBlock *first_block;
    MooWsBlock *last_block;
    MooWsBlock *last_edited;
    GtkTextBuffer *buffer;
    guint modifying_text;
    guint in_key_press : 1;
};

enum {
    BUFFER_INSERT_TEXT,
    BUFFER_DELETE_RANGE,
    N_BUFFER_SIGNALS
};

static guint buffer_signals[N_BUFFER_SIGNALS];

static void     moo_ws_view_check               (MooWsView      *view);

static gboolean moo_ws_view_key_press           (GtkWidget      *widget,
                                                 GdkEventKey    *event);
static void     moo_ws_buffer_insert_text       (GtkTextBuffer  *buffer,
                                                 GtkTextIter    *where,
                                                 char           *text,
                                                 int             len,
                                                 MooWsView      *view);
static void     moo_ws_buffer_delete_range      (GtkTextBuffer  *buffer,
                                                 GtkTextIter    *start,
                                                 GtkTextIter    *end,
                                                 MooWsView      *view);

static void     moo_ws_view_delete_block_priv   (MooWsView      *view,
                                                 MooWsBlock     *block,
                                                 gboolean        destroying);

static void
moo_ws_view_init (MooWsView *view)
{
    view->priv = g_new0 (MooWsViewPrivate, 1);
    view->priv->buffer = NULL;
    view->priv->first_block = NULL;
    view->priv->last_block = NULL;
    view->priv->last_edited = NULL;
    moo_text_view_set_font_from_string (MOO_TEXT_VIEW (view), "Monospace");
}

static GObject *
moo_ws_view_constructor (GType                  type,
                         guint                  n_props,
                         GObjectConstructParam *props)
{
    GObject *object;
    MooWsView *view;

    object = G_OBJECT_CLASS (moo_ws_view_parent_class)->constructor (type, n_props, props);
    view = MOO_WS_VIEW (object);

    view->priv->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
    g_signal_connect (view->priv->buffer, "insert-text",
                      G_CALLBACK (moo_ws_buffer_insert_text), view);
    g_signal_connect (view->priv->buffer, "delete-range",
                      G_CALLBACK (moo_ws_buffer_delete_range), view);

    return object;
}

static void
moo_ws_view_dispose (GObject *object)
{
    MooWsView *view = MOO_WS_VIEW (object);

    if (view->priv)
    {
        while (view->priv->first_block)
            moo_ws_view_delete_block_priv (view, view->priv->first_block, TRUE);
        g_free (view->priv);
        view->priv = NULL;
    }

    G_OBJECT_CLASS (moo_ws_view_parent_class)->dispose (object);
}

static void
moo_ws_view_class_init (MooWsViewClass *klass)
{
    GtkTextBufferClass *buf_class;
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    object_class->constructor = moo_ws_view_constructor;
    object_class->dispose = moo_ws_view_dispose;

    widget_class->key_press_event = moo_ws_view_key_press;

    buf_class = g_type_class_ref (GTK_TYPE_TEXT_BUFFER);
    buffer_signals[BUFFER_INSERT_TEXT] =
        g_signal_lookup ("insert-text", GTK_TYPE_TEXT_BUFFER);
    buffer_signals[BUFFER_DELETE_RANGE] =
        g_signal_lookup ("delete-range", GTK_TYPE_TEXT_BUFFER);
    g_type_class_unref (buf_class);
}


void
moo_ws_view_insert_block (MooWsView  *view,
                          MooWsBlock *block,
                          MooWsBlock *after)
{
    g_return_if_fail (MOO_IS_WS_VIEW (view));
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (block->view == NULL);
    g_return_if_fail (!after || after->view == view);

    MOO_OBJECT_REF_SINK (block);

    moo_text_view_begin_not_undoable_action (MOO_TEXT_VIEW (view));
    _moo_ws_view_start_edit (view);
    _moo_ws_block_add (block, view, after,
                       after ? after->next : view->priv->first_block);
    _moo_ws_view_end_edit (view);
    moo_text_view_end_not_undoable_action (MOO_TEXT_VIEW (view));
    view->priv->last_edited = NULL;

    g_return_if_fail (block->view == view);

    if (after == view->priv->last_block)
        view->priv->last_block = block;
    if (!after)
        view->priv->first_block = block;

    moo_ws_view_check (view);
}

void
moo_ws_view_append_block (MooWsView  *view,
                          MooWsBlock *block)
{
    g_return_if_fail (MOO_IS_WS_VIEW (view));
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    moo_ws_view_insert_block (view, block, view->priv->last_block);
}

static void
moo_ws_view_delete_block_priv (MooWsView  *view,
                               MooWsBlock *block,
                               gboolean    destroying)
{
    g_return_if_fail (MOO_IS_WS_VIEW (view));
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (block->view == view);

    if (block == view->priv->first_block)
        view->priv->first_block = block->next;
    if (block == view->priv->last_block)
        view->priv->last_block = block->prev;

    if (!destroying)
    {
        moo_text_view_begin_not_undoable_action (MOO_TEXT_VIEW (view));
        _moo_ws_view_start_edit (view);
        _moo_ws_block_remove (block);
        _moo_ws_view_end_edit (view);
        moo_text_view_end_not_undoable_action (MOO_TEXT_VIEW (view));
        view->priv->last_edited = NULL;
        moo_ws_view_check (view);
    }

    gtk_object_destroy (GTK_OBJECT (block));
    g_object_unref (block);
}

void
moo_ws_view_delete_block (MooWsView  *view,
                          MooWsBlock *block)
{
    g_return_if_fail (MOO_IS_WS_VIEW (view));
    g_return_if_fail (MOO_IS_WS_BLOCK (block));
    g_return_if_fail (block->view == view);
    moo_ws_view_delete_block_priv (view, block, FALSE);
}


MooWsBlock *
_moo_ws_view_get_first_block (MooWsView *view)
{
    g_return_val_if_fail (MOO_IS_WS_VIEW (view), NULL);
    return view->priv->first_block;
}

MooWsBlock *
_moo_ws_view_get_last_block (MooWsView *view)
{
    g_return_val_if_fail (MOO_IS_WS_VIEW (view), NULL);
    return view->priv->last_block;
}


void
_moo_ws_view_beep (MooWsView *view)
{
    g_return_if_fail (MOO_IS_WS_VIEW (view));
    gdk_display_beep (gtk_widget_get_display (GTK_WIDGET (view)));
}


static gboolean
do_special_keys (G_GNUC_UNUSED MooWsView   *view,
                 G_GNUC_UNUSED GdkEventKey *event)
{
    // handle Enter here
    return FALSE;
}

static gboolean
moo_ws_view_key_press (GtkWidget   *widget,
                       GdkEventKey *event)
{
    gboolean retval;
    MooWsView *view = MOO_WS_VIEW (widget);

    if (do_special_keys (view, event))
        return TRUE;

    view->priv->in_key_press = TRUE;
    retval = GTK_WIDGET_CLASS (moo_ws_view_parent_class)->key_press_event (widget, event);
    view->priv->in_key_press = FALSE;

    return retval;
}

void
_moo_ws_view_start_edit (MooWsView *view)
{
    g_return_if_fail (MOO_IS_WS_VIEW (view));
    view->priv->modifying_text += 1;
}

void
_moo_ws_view_end_edit (MooWsView *view)
{
    g_return_if_fail (MOO_IS_WS_VIEW (view));
    g_return_if_fail (view->priv->modifying_text != 0);
    view->priv->modifying_text -= 1;
}

static void
clear_undo (MooWsView *view)
{
    moo_text_view_begin_not_undoable_action (MOO_TEXT_VIEW (view));
    moo_text_view_end_not_undoable_action (MOO_TEXT_VIEW (view));
}

static void
moo_ws_buffer_insert_text (GtkTextBuffer *buffer,
                           GtkTextIter   *where,
                           char          *text,
                           int            len,
                           MooWsView     *view)
{
    MooWsBlock *block;

    if (!view->priv->modifying_text && !view->priv->in_key_press)
    {
        g_critical ("%s: oops", G_STRFUNC);
        g_signal_stop_emission (buffer, buffer_signals[BUFFER_INSERT_TEXT], 0);
        return;
    }

    if (view->priv->modifying_text)
        return;

    g_signal_stop_emission (buffer, buffer_signals[BUFFER_INSERT_TEXT], 0);

    g_assert (gtk_text_iter_is_end (where) || gtk_text_iter_is_cursor_position (where));

    block = _moo_ws_iter_get_block (where);

    if (!block)
    {
        _moo_ws_view_beep (view);
        return;
    }

    if (block != view->priv->last_edited)
        clear_undo (view);

    _moo_ws_view_start_edit (block->view);
    _moo_ws_block_insert_text (block, where, text, len);
    _moo_ws_view_end_edit (block->view);

    view->priv->last_edited = block;

    moo_ws_view_check (view);
}

static void
moo_ws_buffer_delete_range (GtkTextBuffer *buffer,
                            GtkTextIter   *start,
                            GtkTextIter   *end,
                            MooWsView     *view)
{
    MooWsBlock *start_block, *end_block, *block;
    gboolean delete_from_start = FALSE;
    gboolean delete_to_end = FALSE;
    GtkTextMark *start_mark, *end_mark;
    GtkTextIter iter;

    if (!view->priv->modifying_text && !view->priv->in_key_press)
    {
        g_critical ("%s: oops", G_STRFUNC);
        g_signal_stop_emission (buffer, buffer_signals[BUFFER_DELETE_RANGE], 0);
        return;
    }

    if (view->priv->modifying_text)
        return;

    g_signal_stop_emission (buffer, buffer_signals[BUFFER_DELETE_RANGE], 0);

    gtk_text_iter_order (start, end);
    start_block = _moo_ws_iter_get_block (start);
    end_block = _moo_ws_iter_get_block (end);
    g_assert (start_block != NULL);

    if (!end_block)
    {
        end_block = view->priv->last_block;
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

    start_mark = gtk_text_buffer_create_mark (buffer, NULL, start, TRUE);
    end_mark = gtk_text_buffer_create_mark (buffer, NULL, end, FALSE);

    if (start_block == end_block)
    {
        if (delete_from_start && delete_to_end)
        {
            moo_ws_view_delete_block (view, start_block);
        }
        else
        {
            if (delete_to_end)
            {
                if (gtk_text_iter_starts_line (start))
                {
                    gtk_text_iter_backward_line (start);
                    if (!gtk_text_iter_ends_line (start))
                        gtk_text_iter_forward_to_line_end (start);
                }

                gtk_text_iter_backward_line (end);
                if (!gtk_text_iter_ends_line (end))
                    gtk_text_iter_forward_to_line_end (end);
            }

            if (start_block != view->priv->last_edited)
                clear_undo (view);

            _moo_ws_view_start_edit (view);
            _moo_ws_block_delete_text (start_block, start, end);
            gtk_text_buffer_move_mark (buffer, start_mark, start);
            _moo_ws_view_end_edit (view);

            view->priv->last_edited = start_block;
        }

        goto out;
    }

    view->priv->last_edited = NULL;
    moo_text_view_begin_not_undoable_action (MOO_TEXT_VIEW (view));

    block = start_block->next;
    g_assert (block != NULL);

    if (delete_from_start)
    {
        moo_ws_view_delete_block (view, start_block);
    }
    else
    {
        _moo_ws_view_start_edit (view);
        _moo_ws_block_get_end_iter (start_block, end);
        _moo_ws_block_delete_text (start_block, start, end);
        gtk_text_buffer_move_mark (buffer, start_mark, start);
        _moo_ws_view_end_edit (view);
    }

    while (block != end_block)
    {
        MooWsBlock *next = block->next;
        moo_ws_view_delete_block (view, block);
        block = next;
    }

    if (delete_to_end)
    {
        moo_ws_view_delete_block (view, end_block);
    }
    else
    {
        _moo_ws_block_get_start_iter (end_block, start);
        gtk_text_buffer_get_iter_at_mark (buffer, end, end_mark);
        _moo_ws_view_start_edit (view);
        _moo_ws_block_delete_text (end_block, start, end);
        _moo_ws_view_end_edit (view);
    }

    moo_text_view_end_not_undoable_action (MOO_TEXT_VIEW (view));

out:
    gtk_text_buffer_get_iter_at_mark (buffer, start, start_mark);
    gtk_text_buffer_get_iter_at_mark (buffer, end, end_mark);
    gtk_text_buffer_delete_mark (buffer, start_mark);
    gtk_text_buffer_delete_mark (buffer, end_mark);

    gtk_text_buffer_get_iter_at_mark (buffer, &iter, gtk_text_buffer_get_insert (buffer));
    gtk_text_buffer_place_cursor (buffer, &iter);

    moo_ws_view_check (view);
}


static void
moo_ws_view_check_tags (MooWsView *view)
{
    GtkTextIter iter;

    gtk_text_buffer_get_start_iter (view->priv->buffer, &iter);

    while (!gtk_text_iter_is_end (&iter))
    {
        GSList *tags;
        GtkTextTag *block_tag = NULL;

        tags = gtk_text_iter_get_tags (&iter);

        while (tags)
        {
            if (g_object_get_data (tags->data, "moo-ws-block"))
            {
                if (block_tag)
                    g_assert (g_object_get_data (tags->data, "moo-ws-block") ==
                                g_object_get_data (G_OBJECT (block_tag), "moo-ws-block"));
                else
                    block_tag = tags->data;
            }

            tags = g_slist_delete_link (tags, tags);
        }

        g_assert (block_tag != NULL);

        gtk_text_iter_forward_to_tag_toggle (&iter, NULL);
    }
}

static void
moo_ws_view_check (MooWsView *view)
{
    if (!view->priv->buffer)
        return;

    moo_ws_view_check_tags (view);
}
