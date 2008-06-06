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

G_BEGIN_DECLS

typedef enum {
    MOO_WS_OUTPUT_OUT,
    MOO_WS_OUTPUT_ERR
} MooWsOutputType;

extern gpointer _moo_ws_view_parent_class;

G_END_DECLS

#endif /* MOO_WS_PRIVATE_H */
