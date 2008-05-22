/*
 *   mdwindow.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MD_WINDOW_H
#define MD_WINDOW_H

#include <mooui/mddocument.h>
#include <mooutils/moowindow.h>


#define MD_WINDOW(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_WINDOW, MdWindow))
#define MD_WINDOW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_WINDOW, MdWindowClass))
#define MD_IS_WINDOW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_WINDOW))
#define MD_IS_WINDOW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_WINDOW))
#define MD_WINDOW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_WINDOW, MdWindowClass))

#define MD_WINDOW_PROP_HAS_OPEN_DOCUMENT "md-has-open-document"
#define MD_WINDOW_PROP_CAN_RELOAD "md-can-reload"

#define MD_WINDOW_TARGET_DOCUMENT_TAB "MD_DOCUMENT_TAB"

typedef struct MdWindowClass MdWindowClass;
typedef struct MdWindowPrivate MdWindowPrivate;

struct MdWindow {
    MooWindow base;
    MdWindowPrivate *priv;
    GtkWidget *notebook;
};

struct MdWindowClass {
    MooWindowClass base_class;

    void    (*active_doc_changed)   (MdWindow   *window);
    void    (*insert_doc)           (MdWindow   *window,
                                     MdDocument *doc);
    void    (*remove_doc)           (MdWindow   *window,
                                     MdDocument *doc);
};


MdManager   *md_window_get_manager      (MdWindow   *window);

MdDocument  *md_window_get_active_doc   (MdWindow   *window);
void         md_window_set_active_doc   (MdWindow   *window,
                                         MdDocument *doc);

GSList      *md_window_list_docs        (MdWindow   *window);


#endif /* MD_WINDOW_H */
