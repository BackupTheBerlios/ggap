/*
 *   moows-input.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "moows-input.h"
#include "moows-private.h"
#include "moowspromptblock.h"
#include <gdk/gdkkeysyms.h>


static void     go_prev_block           (MooWorksheet   *ws);
static void     go_next_block           (MooWorksheet   *ws);
static void     go_first_block          (MooWorksheet   *ws);
static void     go_last_block           (MooWorksheet   *ws);
static void     go_home                 (MooWorksheet   *ws,
                                         gboolean        block_end);
static void     go_end                  (MooWorksheet   *ws,
                                         gboolean        block_end);

static void     scroll_insert_onscreen  (MooWorksheet   *ws);


static GtkTextBuffer *
get_buffer (MooWorksheet *ws)
{
    return gtk_text_view_get_buffer (GTK_TEXT_VIEW (ws));
}

static MooWsBuffer *
get_ws_buffer (MooWorksheet *ws)
{
    return MOO_WS_BUFFER (get_buffer (ws));
}


static void
scroll_insert_onscreen (MooWorksheet *ws)
{
    gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (ws),
        gtk_text_buffer_get_insert (get_buffer (ws)));
}


static MooWsBlock *
find_block (MooWorksheet *ws,
            MooWsBlock   *block,
            int           steps)
{
    if (!block)
    {
        if (steps > 0)
            block = moo_ws_buffer_get_first_block (get_ws_buffer (ws));
        else
            block = moo_ws_buffer_get_last_block (get_ws_buffer (ws));
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
go_to_block (MooWorksheet *ws,
             int           steps)
{
    MooWsBlock *block;
    MooWsBlock *go_to = NULL;
    GtkTextIter iter;

    moo_text_view_get_cursor (MOO_TEXT_VIEW (ws), &iter);
    block = _moo_ws_iter_get_block (&iter);
    go_to = find_block (ws, block, steps);

    if (go_to && go_to != block)
    {
        moo_ws_prompt_block_place_cursor (MOO_WS_PROMPT_BLOCK (go_to), -1, -1);
        scroll_insert_onscreen (ws);
    }
}

static void
go_prev_block (MooWorksheet *ws)
{
    go_to_block (ws, -1);
}

static void
go_next_block (MooWorksheet *ws)
{
    go_to_block (ws, 1);
}

static void
go_first_block (MooWorksheet *ws)
{
    go_to_block (ws, G_MININT);
}

static void
go_last_block (MooWorksheet *ws)
{
    go_to_block (ws, G_MAXINT);
}

static void
go_to_iter (MooWorksheet      *ws,
            const GtkTextIter *iter)
{
    gtk_text_buffer_place_cursor (get_buffer (ws), iter);
    scroll_insert_onscreen (ws);
}

static void
go_home (MooWorksheet *ws,
         gboolean      block_start)
{
    GtkTextIter iter;
    MooWsBlock *block;

    moo_text_view_get_cursor (MOO_TEXT_VIEW (ws), &iter);
    block = _moo_ws_iter_get_block (&iter);

    if (block && block_start)
        _moo_ws_block_get_start_iter (block, &iter);

    if (MOO_IS_WS_PROMPT_BLOCK (block))
        moo_ws_prompt_block_iter_set_line_offset (MOO_WS_PROMPT_BLOCK (block), &iter, 0);
    else
        gtk_text_iter_set_line_offset (&iter, 0);

    go_to_iter (ws, &iter);
}

static void
go_end (MooWorksheet *ws,
        gboolean      block_end)
{
    GtkTextIter iter;

    moo_text_view_get_cursor (MOO_TEXT_VIEW (ws), &iter);

    if (block_end)
    {
        MooWsBlock *block = _moo_ws_iter_get_block (&iter);

        if (block)
            _moo_ws_block_get_end_iter (block, &iter);
    }

    if (!gtk_text_iter_ends_line (&iter))
        gtk_text_iter_forward_to_line_end (&iter);

    go_to_iter (ws, &iter);
}

static gboolean
steal_navigation_keys (MooWorksheet *ws,
                       GdkEventKey  *event)
{
    GdkModifierType mods = event->state & (GDK_CONTROL_MASK | GDK_MOD1_MASK | GDK_SHIFT_MASK);

    switch (event->keyval)
    {
        case GDK_Up:
            if (mods == GDK_MOD1_MASK)
            {
                go_prev_block (ws);
                return TRUE;
            }
            else if (mods == GDK_CONTROL_MASK)
            {
                _moo_worksheet_history_prev (ws);
                return TRUE;
            }
            break;

        case GDK_Down:
            if (mods == GDK_MOD1_MASK)
            {
                go_next_block (ws);
                return TRUE;
            }
            else if (mods == GDK_CONTROL_MASK)
            {
                _moo_worksheet_history_next (ws);
                return TRUE;
            }
            break;

        case GDK_Home:
        case GDK_KP_Home:
            switch (mods)
            {
                case GDK_MOD1_MASK | GDK_CONTROL_MASK:
                    go_first_block (ws);
                    return TRUE;
                case GDK_MOD1_MASK:
                    go_home (ws, TRUE);
                    return TRUE;
                case 0:
                    go_home (ws, FALSE);
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
                    go_last_block (ws);
                    return TRUE;
                case GDK_MOD1_MASK:
                    go_end (ws, TRUE);
                    return TRUE;
                case 0:
                    go_end (ws, FALSE);
                    return TRUE;
                default:
                    return FALSE;
            }
            break;
    }

    return FALSE;
}


gboolean
_moo_worksheet_key_press (GtkWidget   *widget,
                          GdkEventKey *event)
{
    gboolean retval;
    MooWorksheet *ws = MOO_WORKSHEET (widget);

    if (steal_navigation_keys (ws, event))
        return TRUE;

    switch (event->keyval)
    {
        case GDK_Return:
            if (!(_moo_worksheet_get_allow_multiline (ws) &&
                  (event->state & GDK_SHIFT_MASK)) &&
                _moo_worksheet_commit_input (ws))
                    return TRUE;
    }

    _moo_ws_buffer_start_user_edit (get_ws_buffer (ws));
    retval = GTK_WIDGET_CLASS (_moo_worksheet_parent_class)->key_press_event (widget, event);
    _moo_ws_buffer_end_user_edit (get_ws_buffer (ws));

    return retval;
}

void
_moo_worksheet_cut_clipboard (GtkTextView *text_view)
{
    MooWorksheet *ws = MOO_WORKSHEET (text_view);
    _moo_ws_buffer_start_user_edit (get_ws_buffer (ws));
    GTK_TEXT_VIEW_CLASS (_moo_worksheet_parent_class)->cut_clipboard (text_view);
    _moo_ws_buffer_end_user_edit (get_ws_buffer (ws));
}

void
_moo_worksheet_paste_clipboard (GtkTextView *text_view)
{
    MooWorksheet *ws = MOO_WORKSHEET (text_view);
    _moo_ws_buffer_start_user_edit (get_ws_buffer (ws));
    GTK_TEXT_VIEW_CLASS (_moo_worksheet_parent_class)->paste_clipboard (text_view);
    _moo_ws_buffer_end_user_edit (get_ws_buffer (ws));
}

void
_moo_worksheet_move_cursor (GtkTextView    *text_view,
                            GtkMovementStep step,
                            int             count,
                            gboolean        extend_selection)
{
    MooWorksheet *ws = MOO_WORKSHEET (text_view);
    GtkTextIter iter;
    MooWsBlock *block;

    GTK_TEXT_VIEW_CLASS (_moo_worksheet_parent_class)->
        move_cursor (text_view, step, count, extend_selection);

    moo_text_view_get_cursor (MOO_TEXT_VIEW (ws), &iter);
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
