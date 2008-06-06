/*
 *   moows-input.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MOO_WS_INPUT_H
#define MOO_WS_INPUT_H

#include <moows/mooworksheet.h>

G_BEGIN_DECLS


gboolean    _moo_worksheet_key_press        (GtkWidget      *widget,
                                             GdkEventKey    *event);
void        _moo_worksheet_move_cursor      (GtkTextView    *text_view,
                                             GtkMovementStep step,
                                             int             count,
                                             gboolean        extend_selection);
void        _moo_worksheet_cut_clipboard    (GtkTextView    *text_view);
void        _moo_worksheet_paste_clipboard  (GtkTextView    *text_view);


G_END_DECLS

#endif /* MOO_WS_INPUT_H */
