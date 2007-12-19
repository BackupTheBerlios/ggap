#ifndef MD_WINDOW_H
#define MD_WINDOW_H

#include <mooui/mdview.h>
#include <mooui/mdappwindow.h>


#define MD_TYPE_WINDOW              (md_window_get_type ())
#define MD_WINDOW(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_WINDOW, MdWindow))
#define MD_WINDOW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_WINDOW, MdWindowClass))
#define MD_IS_WINDOW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_WINDOW))
#define MD_IS_WINDOW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_WINDOW))
#define MD_WINDOW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_WINDOW, MdWindowClass))

typedef struct MdWindowClass MdWindowClass;
typedef struct MdWindowPrivate MdWindowPrivate;

struct MdWindow {
    MdAppWindow base;
    MdWindowPrivate *priv;
    GtkWidget *notebook;
};

struct MdWindowClass {
    MdAppWindowClass base_class;

    void    (*active_view_changed)  (MdWindow   *window);
    void    (*insert_view)          (MdWindow   *window,
                                     MdView     *view);
    void    (*remove_view)          (MdWindow   *window,
                                     MdView     *view);
};


GType        md_window_get_type         (void) G_GNUC_CONST;

MdManager   *md_window_get_manager      (MdWindow   *window);

MdDocument  *md_window_get_active_doc   (MdWindow   *window);
MdView      *md_window_get_active_view  (MdWindow   *window);
void         md_window_set_active_view  (MdWindow   *window,
                                         MdView     *view);

GSList      *md_window_list_views       (MdWindow   *window);


#endif /* MD_WINDOW_H */
