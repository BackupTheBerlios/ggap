#ifndef MD_DOCUMENT_PRIVATE_H
#define MD_DOCUMENT_PRIVATE_H

#include "mddocument.h"
#include <gtk/gtk.h>


void                _md_document_undo           (MdDocument     *doc);
void                _md_document_redo           (MdDocument     *doc);
gboolean            _md_document_can_undo       (MdDocument     *doc);
gboolean            _md_document_can_redo       (MdDocument     *doc);

gboolean            _md_document_need_save      (MdDocument     *doc);
gboolean            _md_document_is_untitled    (MdDocument     *doc);
gboolean            _md_document_is_empty       (MdDocument     *doc);

void                _md_document_set_manager    (MdDocument     *doc,
                                                 MdManager      *mgr);
void                _md_document_add_view       (MdDocument     *doc,
                                                 MdView         *view);
void                _md_document_remove_view    (MdDocument     *doc,
                                                 MdView         *view);
void                _md_document_close          (MdDocument     *doc);

GdkPixbuf          *_md_document_get_icon       (MdDocument     *doc,
                                                 GtkWidget      *widget,
                                                 GtkIconSize     size);

MdFileOpStatus      _md_document_load_file      (MdDocument     *doc,
                                                 MdFileInfo     *file_info,
                                                 GError        **error);
MdFileOpStatus      _md_document_save_file      (MdDocument     *doc,
                                                 MdFileInfo     *file_info,
                                                 GError        **error);


#endif /* MD_DOCUMENT_PRIVATE_H */
