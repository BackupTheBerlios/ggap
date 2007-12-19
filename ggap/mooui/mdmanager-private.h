#ifndef MOO_DOCUMENT_MANAGER_PRIVATE_H
#define MOO_DOCUMENT_MANAGER_PRIVATE_H

#include <mooui/mdmanager.h>


void         _md_manager_move_view              (MdManager      *mgr,
                                                 MdView         *view,
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
void         _md_manager_action_reload          (MdManager      *mgr,
                                                 MdView         *view);
void         _md_manager_action_save            (MdManager      *mgr,
                                                 MdView         *view);
void         _md_manager_action_save_as         (MdManager      *mgr,
                                                 MdView         *view);
void         _md_manager_action_close_views     (MdManager      *mgr,
                                                 GSList         *list);
void         _md_manager_action_open_files      (MdManager      *mgr,
                                                 MdWindow       *window,
                                                 MdFileInfo    **files);


#endif /* MOO_DOCUMENT_MANAGER_PRIVATE_H */
