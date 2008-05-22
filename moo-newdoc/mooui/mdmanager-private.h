/*
 *   mdmanager-private.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MOO_DOCUMENT_MANAGER_PRIVATE_H
#define MOO_DOCUMENT_MANAGER_PRIVATE_H

#include <mooui/mdmanager.h>
#include <mooutils/moofilewatch.h>


void         _md_manager_move_doc               (MdManager      *mgr,
                                                 MdDocument     *doc,
                                                 MdWindow       *dest);
const char  *_md_manager_get_app_name           (MdManager      *mgr);
char        *_md_manager_add_untitled           (MdManager      *mgr,
                                                 MdDocument     *doc);
void         _md_manager_remove_untitled        (MdManager      *mgr,
                                                 MdDocument     *doc);

void         _md_manager_action_close_window    (MdManager      *mgr,
                                                 MdWindow       *window);
void         _md_manager_action_new_doc         (MdManager      *mgr,
                                                 MdWindow       *window);
void         _md_manager_action_new_window      (MdManager      *mgr);
void         _md_manager_action_open            (MdManager      *mgr,
                                                 MdWindow       *window);
void         _md_manager_action_open_uri        (MdManager      *mgr,
                                                 MdWindow       *window);
void         _md_manager_action_reload          (MdManager      *mgr,
                                                 MdDocument     *doc);
void         _md_manager_action_save            (MdManager      *mgr,
                                                 MdDocument     *doc);
void         _md_manager_action_save_as         (MdManager      *mgr,
                                                 MdDocument     *doc);
void         _md_manager_action_close_docs      (MdManager      *mgr,
                                                 GSList         *list);
void         _md_manager_action_open_files      (MdManager      *mgr,
                                                 MdFileInfo    **files,
                                                 MdWindow       *window,
                                                 GtkWidget      *widget);

void         _md_manager_quit                   (MdManager      *mgr);

MooFileWatch *_md_manager_get_file_watch        (MdManager      *mgr);
void          _md_manager_add_recent            (MdManager      *mgr,
                                                 MdDocument     *doc);


#endif /* MOO_DOCUMENT_MANAGER_PRIVATE_H */
