/*
 *   @(#)$Id: sc3editwindow.h,v 1.2 2005/05/29 01:52:39 emuntyan Exp $
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

#ifndef SC3_SC3EDITWINDOW_H
#define SC3_SC3EDITWINDOW_H

#include "mooedit/mooeditwindow.h"

G_BEGIN_DECLS


#define SC3_TYPE_EDIT_WINDOW             (sc3_edit_window_get_type ())
#define SC3_EDIT_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), SC3_TYPE_EDIT_WINDOW, Sc3EditWindow))
#define SC3_EDIT_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), SC3_TYPE_EDIT_WINDOW, Sc3EditWindowClass))
#define SC3_IS_EDIT_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SC3_TYPE_EDIT_WINDOW))
#define SC3_IS_EDIT_WINDOW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), SC3_TYPE_EDIT_WINDOW))
#define SC3_EDIT_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), SC3_TYPE_EDIT_WINDOW, Sc3EditWindowClass))


typedef struct _Sc3EditWindow        Sc3EditWindow;
typedef struct _Sc3EditWindowClass   Sc3EditWindowClass;

struct _Sc3EditWindow
{
    MooEditWindow       parent;
};

struct _Sc3EditWindowClass
{
    MooEditWindowClass  parent_class;
};


GType       sc3_edit_window_get_type           (void) G_GNUC_CONST;
GtkWidget  *sc3_edit_window_new                (void);


G_END_DECLS

#endif /* SC3_SC3EDITWINDOW_H */
