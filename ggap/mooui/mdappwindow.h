#ifndef MD_APP_WINDOW_H
#define MD_APP_WINDOW_H

#include <mooui/mdtypes.h>
#include <mooutils/moouixml.h>


#define MD_TYPE_APP_WINDOW              (md_app_window_get_type ())
#define MD_APP_WINDOW(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_APP_WINDOW, MdAppWindow))
#define MD_APP_WINDOW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_APP_WINDOW, MdAppWindowClass))
#define MD_IS_APP_WINDOW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_APP_WINDOW))
#define MD_IS_APP_WINDOW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_APP_WINDOW))
#define MD_APP_WINDOW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_APP_WINDOW, MdAppWindowClass))

typedef struct MdAppWindowClass MdAppWindowClass;
typedef struct MdAppWindowPrivate MdAppWindowPrivate;

struct MdAppWindow {
    GtkWindow base;

    MdAppWindowPrivate  *priv;

    GtkWidget           *menubar;
    GtkWidget           *toolbar;
    GtkWidget           *vbox;
};

struct MdAppWindowClass {
    GtkWindowClass base_class;

    gboolean (*close) (MdAppWindow *window);
};

typedef GtkAction *(*MdAppWindowActionFunc) (MdAppWindow *window,
                                             gpointer     data);


GType       md_app_window_get_type                  (void) G_GNUC_CONST;

gboolean    md_app_window_close                     (MdAppWindow        *window);

void        md_app_window_class_set_id              (MdAppWindowClass   *klass,
                                                     const char         *id,
                                                     const char         *name);

void        md_app_window_class_new_action          (MdAppWindowClass   *klass,
                                                     const char         *id,
                                                     const char         *group,
                                                     ...) G_GNUC_NULL_TERMINATED;
void        md_app_window_class_new_action_custom   (MdAppWindowClass  *klass,
                                                     const char         *id,
                                                     const char         *group,
                                                     MdAppWindowActionFunc func,
                                                     gpointer            data,
                                                     GDestroyNotify      notify);
void       _md_app_window_class_new_action_callback (MdAppWindowClass   *klass,
                                                     const char         *id,
                                                     const char         *group,
                                                     GCallback           callback,
                                                     GSignalCMarshaller  marshal,
                                                     GType               return_type,
                                                     guint               n_args,
                                                     ...) G_GNUC_NULL_TERMINATED;

gboolean    md_app_window_class_find_action         (MdAppWindowClass   *klass,
                                                     const char         *id);
void        md_app_window_class_remove_action       (MdAppWindowClass   *klass,
                                                     const char         *id);

void        md_app_window_class_new_group           (MdAppWindowClass   *klass,
                                                     const char         *name,
                                                     const char         *display_name);
gboolean    md_app_window_class_find_group          (MdAppWindowClass   *klass,
                                                     const char         *name);
void        md_app_window_class_remove_group        (MdAppWindowClass   *klass,
                                                     const char         *name);

MooUIXML   *md_app_window_get_ui_xml                (MdAppWindow        *window);
void        md_app_window_set_ui_xml                (MdAppWindow        *window,
                                                     MooUIXML           *xml);

MooActionCollection *md_app_window_get_actions      (MdAppWindow        *window);
GtkAction  *md_app_window_get_action                (MdAppWindow        *window,
                                                     const char         *action);

void        md_app_window_set_global_accels         (MdAppWindow        *window,
                                                     gboolean            global);

void        md_app_window_set_edit_ops_widget       (MdAppWindow        *window,
                                                     GtkWidget          *widget);


#endif /* MD_APP_WINDOW_H */
