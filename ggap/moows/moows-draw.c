/*
 *   moows-draw.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "moows-draw.h"
#include "moows-private.h"
#include "moowspromptblock.h"
#include "moowstextblock.h"


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
draw_block_guides (GtkWidget      *widget,
                   GdkEventExpose *event)
{
    GtkTextView *text_view = GTK_TEXT_VIEW (widget);
    GdkRectangle buf_rect, win_rect;
    int ymin, ymax;
    GtkTextIter start, end;

    gtk_text_view_get_visible_rect (text_view, &buf_rect);
    gtk_text_view_buffer_to_window_coords (text_view, GTK_TEXT_WINDOW_TEXT,
                                           buf_rect.x, buf_rect.y,
                                           &win_rect.x, &win_rect.y);

    ymin = MAX (win_rect.y, event->area.y);
    ymax = MIN (win_rect.y + win_rect.height - 1, event->area.y + event->area.height - 1);

    if (ymin > ymax)
        return;

    win_rect.y = ymin;
    win_rect.height = ymax - ymin + 1;
    gtk_text_view_window_to_buffer_coords (text_view, GTK_TEXT_WINDOW_TEXT,
                                           win_rect.x, win_rect.y,
                                           &buf_rect.x, &buf_rect.y);

    gtk_text_view_get_line_at_y (text_view, &start, buf_rect.y, NULL);
    gtk_text_view_get_line_at_y (text_view, &end, buf_rect.y + buf_rect.height - 1, NULL);

    while (gtk_text_iter_compare (&start, &end) <= 0)
    {
        MooWsBlock *block;
        GtkTextIter block_end;
        int y1, y2;
        int y, height;
        int x, width;

        if (!(block = _moo_ws_iter_get_block (&start)))
            break;

        _moo_ws_block_get_start_iter (block, &start);
        _moo_ws_block_get_end_iter (block, &block_end);

        if (MOO_IS_WS_PROMPT_BLOCK (block))
        {
            MooWsBlock *next = moo_ws_block_next (block);
            if (MOO_IS_WS_TEXT_BLOCK (next) &&
                moo_ws_text_block_is_output (MOO_WS_TEXT_BLOCK (next)))
                    _moo_ws_block_get_end_iter (next, &block_end);
        }
        else if (MOO_IS_WS_TEXT_BLOCK (block) &&
                 moo_ws_text_block_is_output (MOO_WS_TEXT_BLOCK (block)))
        {
            MooWsBlock *prev = moo_ws_block_prev (block);
            if (MOO_IS_WS_PROMPT_BLOCK (prev))
                _moo_ws_block_get_start_iter (prev, &start);
        }

        gtk_text_view_get_line_yrange (text_view, &start, &y, &height);
        y1 = y + 1;
        gtk_text_view_get_line_yrange (text_view, &block_end, &y, &height);
        y2 = y + height - 1;

        x = 1;
        width = MOO_WORKSHEET_BLOCK_GUIDE_WIDTH;
        if (text_view->hadjustment)
            x -= (int) text_view->hadjustment->value;

        gtk_text_view_buffer_to_window_coords (text_view, GTK_TEXT_WINDOW_TEXT,
                                               0, y1, NULL, &y1);
        gtk_text_view_buffer_to_window_coords (text_view, GTK_TEXT_WINDOW_TEXT,
                                               0, y2, NULL, &y2);

        gdk_draw_line (event->window,
                       widget->style->text_gc[GTK_WIDGET_STATE (widget)],
                       x, y1, x + width - 1, y1);
        gdk_draw_line (event->window,
                       widget->style->text_gc[GTK_WIDGET_STATE (widget)],
                       x, y1, x, y2);
        gdk_draw_line (event->window,
                       widget->style->text_gc[GTK_WIDGET_STATE (widget)],
                       x, y2, x + width - 1, y2);

        start = block_end;

        if (!gtk_text_iter_forward_line (&start))
            break;
    }
}

gboolean
_moo_worksheet_expose_event (GtkWidget      *widget,
                             GdkEventExpose *event)
{
    GTK_WIDGET_CLASS (_moo_worksheet_parent_class)->expose_event (widget, event);

    if (event->window == gtk_text_view_get_window (GTK_TEXT_VIEW (widget), GTK_TEXT_WINDOW_TEXT))
        draw_block_guides (widget, event);

    return FALSE;
}
