#ifndef MD_MANAGER_H
#define MD_MANAGER_H

#include <mooui/mdwindow.h>
#include <mooui/mddocument.h>
#include <mooui/mdview.h>


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
    void         (*new_doc)                 (MdManager  *mgr,
                                             MdDocument *doc);
    void         (*new_view)                (MdManager  *mgr,
                                             MdView     *view);
    void         (*new_window)              (MdManager  *mgr,
                                             MdWindow   *window);
    void         (*close_doc)               (MdManager  *mgr,
                                             MdDocument *doc);
    void         (*close_view)              (MdManager  *mgr,
                                             MdView     *view);
    void         (*close_window)            (MdManager  *mgr,
                                             MdWindow   *window);
    MdFileInfo** (*ask_open)                (MdManager  *mgr,
                                             MdWindow   *window);
    MdFileInfo*  (*ask_save_as)             (MdManager  *mgr,
                                             MdView     *view);

    MdCloseAllResult (*close_all)           (MdManager  *mgr);

    /* Action signals, may safely be emitted and stopped */
    void         (*action_close_views)      (MdManager  *mgr,
                                             GSList     *views);
    void         (*action_close_windows)    (MdManager  *mgr,
                                             GSList     *windows);

    void         (*action_open_files)       (MdManager  *mgr,
                                             MdWindow   *window,
                                             MdFileInfo **files); /* NULL-terminated */
};


GType       md_manager_get_type             (void) G_GNUC_CONST;

void        md_manager_set_doc_type         (MdManager  *mgr,
                                             GType       type);
void        md_manager_set_view_type        (MdManager  *mgr,
                                             GType       type);
void        md_manager_set_window_type      (MdManager  *mgr,
                                             GType       type);
void        md_manager_set_ui_xml           (MdManager  *mgr,
                                             MooUIXML   *xml);

void        md_manager_set_active_window    (MdManager  *mgr,
                                             MdWindow   *window);
MdWindow   *md_manager_get_active_window    (MdManager  *mgr);


#endif /* MD_MANAGER_H */
