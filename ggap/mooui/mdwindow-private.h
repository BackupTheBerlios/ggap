#ifndef MD_WINDOW_PRIVATE_H
#define MD_WINDOW_PRIVATE_H

#include "mdwindow.h"


void        _md_window_insert_view  (MdWindow   *window,
                                     MdView     *view);
void        _md_window_remove_view  (MdWindow   *window,
                                     MdView     *view);

gboolean    _md_window_destroyed    (MdWindow   *window);


#endif /* MD_WINDOW_PRIVATE_H */
