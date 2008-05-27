/*
 *   mddocument-private.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MD_DOCUMENT_PRIVATE_H
#define MD_DOCUMENT_PRIVATE_H

#include "mddocument.h"


gboolean    _md_document_need_save          (MdDocument     *doc);

void        _md_document_set_manager        (MdDocument     *doc,
                                             MdManager      *mgr);
void        _md_document_set_window         (MdDocument     *doc,
                                             MdWindow       *window);
void        _md_document_close              (MdDocument     *doc);

GdkPixbuf  *_md_document_get_icon           (MdDocument     *doc,
                                             GtkIconSize     size);
void        _md_document_apply_prefs        (MdDocument     *doc);
GtkWidget  *_md_document_create_tab_label   (MdDocument     *doc,
                                             GtkWidget     **icon_evbox);

void        _md_document_load_file          (MdDocument     *doc,
                                             MdFileInfo     *file_info,
                                             MdFileOpInfo   *op_info);
void        _md_document_save_file          (MdDocument     *doc,
                                             MdFileInfo     *file_info,
                                             MdFileOpInfo   *op_info);


#endif /* MD_DOCUMENT_PRIVATE_H */
