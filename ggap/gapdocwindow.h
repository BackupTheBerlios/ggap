/*
 *   gapdoc_windowwindow.h
 *
 *   Copyright (C) 2004-2006 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef __GAP_DOC_WINDOW_H__
#define __GAP_DOC_WINDOW_H__

#include "mooutils/moowindow.h"
#include "mooutils/mooglade.h"

G_BEGIN_DECLS


#define GAP_TYPE_DOC_WINDOW                (gap_doc_window_get_type ())
#define GAP_DOC_WINDOW(object)             (G_TYPE_CHECK_INSTANCE_CAST ((object), GAP_TYPE_DOC_WINDOW, GapDocWindow))
#define GAP_DOC_WINDOW_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_DOC_WINDOW, GapDocWindowClass))
#define GAP_IS_DOC_WINDOW(object)          (G_TYPE_CHECK_INSTANCE_TYPE ((object), GAP_TYPE_DOC_WINDOW))
#define GAP_IS_DOC_WINDOW_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_DOC_WINDOW))
#define GAP_DOC_WINDOW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_DOC_WINDOW, GapDocWindowClass))


typedef struct _GapDocWindow          GapDocWindow;
typedef struct _GapDocWindowPrivate   GapDocWindowPrivate;
typedef struct _GapDocWindowClass     GapDocWindowClass;

struct _GapDocWindow
{
    MooWindow base;
    MooGladeXML *xml;
};

struct _GapDocWindowClass
{
    MooWindowClass base_class;
};


GType   gap_doc_window_get_type     (void) G_GNUC_CONST;

void    gap_doc_window_show         (void);


G_END_DECLS

#endif /* __GAP_DOC_WINDOW_H__ */
