#ifndef MD_VIEW_PRIVATE_H
#define MD_VIEW_PRIVATE_H

#include "mdview.h"
#include <gtk/gtk.h>


void        _md_view_set_manager        (MdView     *view,
                                         MdManager  *mgr);
void        _md_view_set_window         (MdView     *view,
                                         MdWindow   *window);

GtkWidget  *_md_view_create_tab_label   (MdView     *view,
                                         GtkWidget **icon_evbox);
void        _md_view_apply_prefs        (MdView     *view);


#endif /* MD_VIEW_PRIVATE_H */
