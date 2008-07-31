/*
 *   moows-private.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MOO_WS_PRIVATE_H
#define MOO_WS_PRIVATE_H

#include <moows/mooworksheet.h>
#include <moows/moowsbuffer.h>

G_BEGIN_DECLS

extern gpointer _moo_worksheet_parent_class;

void        _moo_worksheet_history_next         (MooWorksheet   *ws);
void        _moo_worksheet_history_prev         (MooWorksheet   *ws);
gboolean    _moo_worksheet_commit_input         (MooWorksheet   *ws);
gboolean    _moo_worksheet_get_allow_multiline  (MooWorksheet   *ws);

G_END_DECLS

#endif /* MOO_WS_PRIVATE_H */
