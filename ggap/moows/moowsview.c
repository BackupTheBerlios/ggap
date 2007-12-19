/*
 *   moowsview.c
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
#include "moowsview.h"
#include "mooworksheet.h"
#include "moowspromptblock.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/mooutils-gobject.h"
#include <gdk/gdkkeysyms.h>
#include <string.h>


struct MooWsViewPrivate {
    int dummy;
};

static gboolean moo_ws_view_key_press   (GtkWidget      *widget,
                                         GdkEventKey    *event);
static void     moo_ws_view_move_cursor (GtkTextView    *text_view,
                                         GtkMovementStep step,
                                         int             count,
                                         gboolean        extend_selection);
static void     moo_ws_view_cut_clipboard (GtkTextView  *text_view);
static void     moo_ws_view_paste_clipboard (GtkTextView *text_view);

static void     go_prev_block           (MooWsView      *view);
static void     go_next_block           (MooWsView      *view);
static void     go_first_block          (MooWsView      *view);
static void     go_last_block           (MooWsView      *view);
static void     go_home                 (MooWsView      *view,
                                         gboolean        block_end);
static void     go_end                  (MooWsView      *view,
                                         gboolean        block_end);

static void     scroll_insert_onscreen  (MooWsView      *view);


G_DEFINE_TYPE (MooWsView, moo_ws_view, MOO_TYPE_TEXT_VIEW)

static void
moo_ws_view_init (MooWsView *view)
{
    view->priv = G_TYPE_INSTANCE_GET_PRIVATE (view, MOO_TYPE_WS_VIEW, MooWsViewPrivate);
    moo_text_view_set_buffer_type (MOO_TEXT_VIEW (view), MOO_TYPE_WORKSHEET);
    moo_text_view_set_font_from_string (MOO_TEXT_VIEW (view), "Monospace");
}

static GtkTextBuffer *
get_buffer (MooWsView *view)
{
    return gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
}

static MooWsBuffer *
get_ws_buffer (MooWsView *view)
{
    return MOO_WS_BUFFER (get_buffer (view));
}

static MooWorksheet *
get_worksheet (MooWsView *view)
{
    return MOO_WORKSHEET (get_buffer (view));
}

static GObject *
moo_ws_view_constructor (GType                  type,
                         guint                  n_props,
                         GObjectConstructParam *props)
{
    GObject *object;
    MooWsView *view;
    MooWsBuffer *buffer;

    object = G_OBJECT_CLASS (moo_ws_view_parent_class)->constructor (type, n_props, props);
    view = MOO_WS_VIEW (object);

    buffer = get_ws_buffer (view);
    g_object_set_data (G_OBJECT (buffer), "moo-ws-view", view);
    g_signal_connect_swapped (buffer, "beep",
                              G_CALLBACK (_moo_ws_view_beep),
                              view);
    g_signal_connect_swapped (buffer, "scroll-insert-onscreen",
                              G_CALLBACK (scroll_insert_onscreen),
                              view);

    return object;
}


static void
moo_ws_view_class_init (MooWsViewClass *klass)
{
    GtkTextViewClass *textview_class = GTK_TEXT_VIEW_CLASS (klass);

    G_OBJECT_CLASS (klass)->constructor = moo_ws_view_constructor;
    GTK_WIDGET_CLASS (klass)->key_press_event = moo_ws_view_key_press;

    textview_class->move_cursor = moo_ws_view_move_cursor;
    textview_class->cut_clipboard = moo_ws_view_cut_clipboard;
    textview_class->paste_clipboard = moo_ws_view_paste_clipboard;

    g_type_class_add_private (klass, sizeof (MooWsViewPrivate));
}


void
_moo_ws_view_beep (MooWsView *view)
{
    g_return_if_fail (MOO_IS_WS_VIEW (view));

#if GTK_CHECK_VERSION(2,12,0)
    gdk_window_beep (GTK_WIDGET (view)->window);
#else
    gdk_display_beep (gtk_widget_get_display (GTK_WIDGET (view)));
#endif
}


static void
scroll_insert_onscreen (MooWsView *view)
{
    gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (view),
                                        gtk_text_buffer_get_insert (get_buffer (view)));
}


static MooWsBlock *
find_block (MooWsView  *view,
            MooWsBlock *block,
            int         steps)
{
    if (!block)
    {
        if (steps > 0)
            block = _moo_ws_buffer_get_first_block (get_ws_buffer (view));
        else
            block = _moo_ws_buffer_get_last_block (get_ws_buffer (view));
    }

    if (!block)
        return NULL;

    while (steps != 0)
    {
        if (steps > 0)
        {
            MooWsBlock *next;

            for (next = block->next;
                 next != NULL && !MOO_IS_WS_PROMPT_BLOCK (next);
                 next = next->next);

            if (next)
            {
                block = next;
                steps -= 1;
            }
            else
            {
                break;
            }
        }
        else
        {
            MooWsBlock *prev;

            for (prev = block->prev;
                 prev != NULL && !MOO_IS_WS_PROMPT_BLOCK (prev);
                 prev = prev->prev);

            if (prev)
            {
                block = prev;
                steps += 1;
            }
            else
            {
                break;
            }
        }
    }

    return block;
}

static void
go_to_block (MooWsView *view,
             int        steps)
{
    MooWsBlock *block;
    MooWsBlock *go_to = NULL;
    GtkTextIter iter;

    moo_text_view_get_cursor (MOO_TEXT_VIEW (view), &iter);
    block = _moo_ws_iter_get_block (&iter);
    go_to = find_block (view, block, steps);

    if (go_to && go_to != block)
    {
        moo_ws_prompt_block_place_cursor (MOO_WS_PROMPT_BLOCK (go_to), -1, -1);
        scroll_insert_onscreen (view);
    }
}

static void
go_prev_block (MooWsView *view)
{
    go_to_block (view, -1);
}

static void
go_next_block (MooWsView *view)
{
    go_to_block (view, 1);
}

static void
go_first_block (MooWsView *view)
{
    go_to_block (view, G_MININT);
}

static void
go_last_block (MooWsView *view)
{
    go_to_block (view, G_MAXINT);
}

static void
go_to_iter (MooWsView         *view,
            const GtkTextIter *iter)
{
    gtk_text_buffer_place_cursor (get_buffer (view), iter);
    scroll_insert_onscreen (view);
}

static void
go_home (MooWsView *view,
         gboolean   block_start)
{
    GtkTextIter iter;
    MooWsBlock *block;

    moo_text_view_get_cursor (MOO_TEXT_VIEW (view), &iter);
    block = _moo_ws_iter_get_block (&iter);

    if (block && block_start)
        _moo_ws_block_get_start_iter (block, &iter);

    if (MOO_IS_WS_PROMPT_BLOCK (block))
        moo_ws_prompt_block_iter_set_line_offset (MOO_WS_PROMPT_BLOCK (block), &iter, 0);
    else
        gtk_text_iter_set_line_offset (&iter, 0);

    go_to_iter (view, &iter);
}

static void
go_end (MooWsView *view,
        gboolean   block_end)
{
    GtkTextIter iter;

    moo_text_view_get_cursor (MOO_TEXT_VIEW (view), &iter);

    if (block_end)
    {
        MooWsBlock *block = _moo_ws_iter_get_block (&iter);

        if (block)
            _moo_ws_block_get_end_iter (block, &iter);
    }

    if (!gtk_text_iter_ends_line (&iter))
        gtk_text_iter_forward_to_line_end (&iter);

    go_to_iter (view, &iter);
}

static gboolean
steal_navigation_keys (MooWsView   *view,
                       GdkEventKey *event)
{
    GdkModifierType mods = event->state & (GDK_CONTROL_MASK | GDK_MOD1_MASK | GDK_SHIFT_MASK);

    switch (event->keyval)
    {
        case GDK_Up:
            if (mods == GDK_MOD1_MASK)
            {
                go_prev_block (view);
                return TRUE;
            }
            else if (mods == GDK_CONTROL_MASK)
            {
                _moo_worksheet_history_prev (get_worksheet (view));
                return TRUE;
            }
            break;

        case GDK_Down:
            if (mods == GDK_MOD1_MASK)
            {
                go_next_block (view);
                return TRUE;
            }
            else if (mods == GDK_CONTROL_MASK)
            {
                _moo_worksheet_history_next (get_worksheet (view));
                return TRUE;
            }
            break;

        case GDK_Home:
        case GDK_KP_Home:
            switch (mods)
            {
                case GDK_MOD1_MASK | GDK_CONTROL_MASK:
                    go_first_block (view);
                    return TRUE;
                case GDK_MOD1_MASK:
                    go_home (view, TRUE);
                    return TRUE;
                case 0:
                    go_home (view, FALSE);
                    return TRUE;
                default:
                    return FALSE;
            }
            break;

        case GDK_End:
        case GDK_KP_End:
            switch (mods)
            {
                case GDK_MOD1_MASK | GDK_CONTROL_MASK:
                    go_last_block (view);
                    return TRUE;
                case GDK_MOD1_MASK:
                    go_end (view, TRUE);
                    return TRUE;
                case 0:
                    go_end (view, FALSE);
                    return TRUE;
                default:
                    return FALSE;
            }
            break;
    }

    return FALSE;
}


static gboolean
moo_ws_view_key_press (GtkWidget   *widget,
                       GdkEventKey *event)
{
    gboolean retval;
    MooWsView *view = MOO_WS_VIEW (widget);

    if (steal_navigation_keys (view, event))
        return TRUE;

    switch (event->keyval)
    {
        case GDK_Return:
            if (!(_moo_worksheet_get_allow_multiline (get_worksheet (view)) &&
                  (event->state & GDK_SHIFT_MASK)) &&
                _moo_worksheet_commit_input (get_worksheet (view)))
                    return TRUE;
    }

    _moo_ws_buffer_start_user_edit (get_ws_buffer (view));
    retval = GTK_WIDGET_CLASS (moo_ws_view_parent_class)->key_press_event (widget, event);
    _moo_ws_buffer_end_user_edit (get_ws_buffer (view));

    return retval;
}

static void
moo_ws_view_cut_clipboard (GtkTextView *text_view)
{
    MooWsView *view = MOO_WS_VIEW (text_view);
    _moo_ws_buffer_start_user_edit (get_ws_buffer (view));
    GTK_TEXT_VIEW_CLASS (moo_ws_view_parent_class)->cut_clipboard (text_view);
    _moo_ws_buffer_end_user_edit (get_ws_buffer (view));
}

static void
moo_ws_view_paste_clipboard (GtkTextView *text_view)
{
    MooWsView *view = MOO_WS_VIEW (text_view);
    _moo_ws_buffer_start_user_edit (get_ws_buffer (view));
    GTK_TEXT_VIEW_CLASS (moo_ws_view_parent_class)->paste_clipboard (text_view);
    _moo_ws_buffer_end_user_edit (get_ws_buffer (view));
}

static void
moo_ws_view_move_cursor (GtkTextView    *text_view,
                         GtkMovementStep step,
                         int             count,
                         gboolean        extend_selection)
{
    MooWsView *view = MOO_WS_VIEW (text_view);
    GtkTextIter iter;
    MooWsBlock *block;

    GTK_TEXT_VIEW_CLASS (moo_ws_view_parent_class)->
        move_cursor (text_view, step, count, extend_selection);

    moo_text_view_get_cursor (MOO_TEXT_VIEW (view), &iter);
    block = _moo_ws_iter_get_block (&iter);

    if (block && _moo_ws_block_check_move_cursor (block, &iter, step,
                                                  count, extend_selection))
    {
        if (extend_selection)
            gtk_text_buffer_move_mark_by_name (gtk_text_view_get_buffer (text_view),
                                               "insert", &iter);
        else
            gtk_text_buffer_place_cursor (gtk_text_view_get_buffer (text_view),
                                          &iter);
    }
}

void
_moo_ws_view_start_edit (MooWsView *view)
{
    g_return_if_fail (MOO_IS_WS_VIEW (view));
    _moo_ws_buffer_start_edit (get_ws_buffer (view));
}

void
_moo_ws_view_end_edit (MooWsView *view)
{
    g_return_if_fail (MOO_IS_WS_VIEW (view));
    _moo_ws_buffer_end_edit (get_ws_buffer (view));
}
