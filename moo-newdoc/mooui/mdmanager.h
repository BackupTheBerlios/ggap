/*
 *   mdmanager.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MD_MANAGER_H
#define MD_MANAGER_H

#include <mooui/mdwindow.h>
#include <mooui/mddocument.h>
#include <mooutils/mdhistorymgr.h>
#include <mooutils/moofiledialog.h>
#include <mooutils/moofiltermgr.h>

G_BEGIN_DECLS

#define MD_TYPE_MANAGER             (md_manager_get_type ())
#define MD_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_MANAGER, MdManager))
#define MD_MANAGER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_MANAGER, MdManagerClass))
#define MD_IS_MANAGER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_MANAGER))
#define MD_IS_MANAGER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_MANAGER))
#define MD_MANAGER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_MANAGER, MdManagerClass))

typedef struct MdManagerClass MdManagerClass;
typedef struct MdManagerPrivate MdManagerPrivate;

typedef enum {
    MD_CLOSE_ALL_DONE,
    MD_CLOSE_ALL_CANCELLED,
    MD_CLOSE_ALL_IN_PROGRESS
} MdCloseAllResult;

struct MdManager {
    GObject base;
    MdManagerPrivate *priv;
};

struct MdManagerClass {
    GObjectClass base_class;

    /* Signals which do the job, may not be stopped */
    void         (*new_doc)                 (MdManager      *mgr,
                                             MdDocument     *doc);
    void         (*new_window)              (MdManager      *mgr,
                                             MdWindow       *window);
    void         (*close_doc)               (MdManager      *mgr,
                                             MdDocument     *doc);
    void         (*close_window)            (MdManager      *mgr,
                                             MdWindow       *window);

    MdFileInfo** (*ask_open)                (MdManager      *mgr,
                                             MdWindow       *window);
    MdFileInfo** (*run_open_dialog)         (MdManager      *mgr,
                                             MooFileDialog  *dialog);
    MdFileInfo** (*ask_open_uri)            (MdManager      *mgr,
                                             MdWindow       *window);
    MdFileInfo*  (*ask_save_as)             (MdManager      *mgr,
                                             MdDocument     *doc);
    MdFileInfo*  (*run_save_dialog)         (MdManager      *mgr,
                                             MdDocument     *doc,
                                             MooFileDialog  *dialog);
    void         (*add_recent)              (MdManager      *mgr,
                                             MdDocument     *doc);

    MdCloseAllResult (*close_all)           (MdManager      *mgr);
    void         (*apply_prefs)             (MdManager      *mgr);

    /* not a signal */
    void         (*quit)                    (MdManager      *mgr);

    /* Action signals, may safely be emitted and stopped */
    void         (*action_close_docs)       (MdManager      *mgr,
                                             GSList         *docs);
    void         (*action_close_windows)    (MdManager      *mgr,
                                             GSList         *windows);
};


GType       md_manager_get_type             (void) G_GNUC_CONST;

void        md_manager_set_doc_type         (MdManager  *mgr,
                                             GType       type);
void        md_manager_set_window_type      (MdManager  *mgr,
                                             GType       type);

void        md_manager_set_ui_xml           (MdManager  *mgr,
                                             MooUIXML   *xml);
MooUIXML   *md_manager_get_ui_xml           (MdManager  *mgr);

void        md_manager_set_active_window    (MdManager  *mgr,
                                             MdWindow   *window);
MdWindow   *md_manager_get_active_window    (MdManager  *mgr);

GSList     *md_manager_list_windows         (MdManager  *mgr);
GSList     *md_manager_list_docs            (MdManager  *mgr);

MdDocument *md_manager_get_doc              (MdManager  *mgr,
                                             const char *uri);
void        md_manager_present_doc          (MdManager  *mgr,
                                             MdDocument *doc);

MdWindow   *md_manager_ui_new_window        (MdManager  *mgr);
MdDocument *md_manager_ui_new_doc           (MdManager  *mgr,
                                             MdWindow   *window);

MdDocument *md_manager_ui_new_file          (MdManager  *mgr,
                                             MdFileInfo *file_info,
                                             MdWindow   *window);
MdDocument *md_manager_ui_open_file         (MdManager  *mgr,
                                             MdFileInfo *file_info,
                                             MdWindow   *window,
                                             GtkWidget  *widget);
MdFileOpStatus md_manager_ui_save           (MdManager  *mgr,
                                             MdDocument *doc);
MdFileOpStatus md_manager_ui_save_as        (MdManager  *mgr,
                                             MdDocument *doc,
                                             MdFileInfo *file_info);
gboolean    md_manager_ui_close_doc         (MdManager  *mgr,
                                             MdDocument *doc);
gboolean    md_manager_ui_close_window      (MdManager  *mgr,
                                             MdWindow   *window);
gboolean    md_manager_ui_close_all         (MdManager  *mgr);

MdHistoryMgr *md_manager_get_history_mgr    (MdManager  *mgr);
MooFilterMgr *md_manager_get_filter_mgr     (MdManager  *mgr);

void        md_manager_apply_prefs          (MdManager  *mgr);

MooAppQuitReply md_manager_quit_requested   (MdManager  *mgr);


G_END_DECLS

#endif /* MD_MANAGER_H */
