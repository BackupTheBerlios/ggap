/*
 *   @(#)$Id: sc3termwindow.h,v 1.2 2005/05/29 01:52:39 emuntyan Exp $
 *
 *   Copyright (C) 2004-2005 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef SC3_SC3TERMWINDOW_H
#define SC3_SC3TERMWINDOW_H

#include <mooterm/mootermwindow.h>

G_BEGIN_DECLS


#define SC3_TYPE_TERM_WINDOW             (sc3_term_window_get_type ())
#define SC3_TERM_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SC3_TYPE_TERM_WINDOW, Sc3TermWindow))
#define SC3_TERM_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SC3_TYPE_TERM_WINDOW, Sc3TermWindowClass))
#define SC3_IS_TERM_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC3_TYPE_TERM_WINDOW))
#define SC3_IS_TERM_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SC3_TYPE_TERM_WINDOW))
#define SC3_TERM_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SC3_TYPE_TERM_WINDOW, Sc3TermWindowClass))


typedef struct _Sc3TermWindow        Sc3TermWindow;
typedef struct _Sc3TermWindowClass   Sc3TermWindowClass;


struct _Sc3TermWindow
{
    MooTermWindow   parent;
};

struct _Sc3TermWindowClass
{
    MooTermWindowClass  parent;
};


GType            sc3_term_window_get_type        (void) G_GNUC_CONST;
Sc3TermWindow   *sc3_term_window_new             (void);

void             sc3_term_window_apply_settings  (Sc3TermWindow     *window);


G_END_DECLS

#endif /* SC3_SC3TERMWINDOW_H */
