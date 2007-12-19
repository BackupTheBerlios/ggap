#ifndef MD_APP_H
#define MD_APP_H

#include <mooui/mdmanager.h>


#define MD_TYPE_APP             (md_app_get_type ())
#define MD_APP(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_APP, MdApp))
#define MD_APP_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_APP, MdAppClass))
#define MD_IS_APP(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_APP))
#define MD_IS_APP_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_APP))
#define MD_APP_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_APP, MdAppClass))

typedef struct MdAppClass MdAppClass;
typedef struct MdAppPrivate MdAppPrivate;

struct MdApp {
    GObject base;
    MdAppPrivate *priv;
};

struct MdAppClass {
    GObjectClass base_class;

    gboolean (*initialize)           (MdApp          *app);
    int      (*run)                  (MdApp          *app,
                                      int             argc,
                                      char          **argv);
    void     (*quit)                 (MdApp          *app);

    void     (*setup_option_context) (MdApp          *app,
                                      GOptionContext *ctx);

    void     (*quit_requested)       (MdApp          *app);
    void     (*quit_cancelled)       (MdApp          *app);
};


GType           md_app_get_type             (void) G_GNUC_CONST;

MdApp          *md_app_instance             (void);

void            md_app_setup_option_context (MdApp          *app,
                                             GOptionContext *ctx);

void            md_app_set_need_display     (MdApp          *app,
                                             gboolean        need_display);
void            md_app_set_need_ui          (MdApp          *app,
                                             gboolean        need_ui);
void            md_app_set_need_run         (MdApp          *app,
                                             gboolean        need_run);

int             md_app_run                  (MdApp          *app,
                                             int             argc,
                                             char          **argv) G_GNUC_NORETURN;

void            md_app_open_files           (MdApp          *app,
                                             MdFileInfo    **files,
                                             guint32         stamp);

void            md_app_set_main_window      (MdApp          *app,
                                             MdAppWindow    *window);
void            md_app_set_document_manager (MdApp          *app,
                                             MdManager      *mgr);

void            md_app_delay_quit           (MdApp          *app);
void            md_app_resume_quit          (MdApp          *app);
void            md_app_cancel_quit          (MdApp          *app);


#endif /* MD_APP_H */
