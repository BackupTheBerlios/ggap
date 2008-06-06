/*
 *   moowsview.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "ggap-i18n.h"
#include "moowsview.h"
#include "moows-input.h"
#include "mooworksheet.h"
#include "moowspromptblock.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/mooutils-gobject.h"
#include <gdk/gdkkeysyms.h>
#include <string.h>


struct MooWsViewPrivate {
    int dummy;
};

gpointer _moo_ws_view_parent_class;
G_DEFINE_TYPE (MooWsView, moo_ws_view, MOO_TYPE_TEXT_VIEW)

static void
moo_ws_view_init (MooWsView *view)
{
    view->priv = G_TYPE_INSTANCE_GET_PRIVATE (view, MOO_TYPE_WS_VIEW, MooWsViewPrivate);
    moo_text_view_set_buffer_type (MOO_TEXT_VIEW (view), MOO_TYPE_WS_BUFFER);
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

    return object;
}


static void
moo_ws_view_class_init (MooWsViewClass *klass)
{
    GtkTextViewClass *textview_class = GTK_TEXT_VIEW_CLASS (klass);

    G_OBJECT_CLASS (klass)->constructor = moo_ws_view_constructor;
    GTK_WIDGET_CLASS (klass)->key_press_event = _moo_worksheet_key_press;

    textview_class->move_cursor = _moo_worksheet_move_cursor;
    textview_class->cut_clipboard = _moo_worksheet_cut_clipboard;
    textview_class->paste_clipboard = _moo_worksheet_paste_clipboard;

    _moo_ws_view_parent_class = moo_ws_view_parent_class;

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
