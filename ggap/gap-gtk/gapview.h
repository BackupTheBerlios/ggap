/*
 *   gapview.h
 *
 *   Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef GAP_VIEW_H
#define GAP_VIEW_H

#include "gapsession.h"

G_BEGIN_DECLS


#define GAP_TYPE_VIEW             (gap_view_get_type ())
#define GAP_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GAP_TYPE_VIEW, GapView))
#define GAP_IS_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GAP_TYPE_VIEW))
#define GAP_VIEW_GET_IFACE(obj)   (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GAP_TYPE_VIEW, GapViewIface))


typedef struct _GapView        GapView;
typedef struct _GapViewIface   GapViewIface;


struct _GapViewIface
{
    GTypeInterface base;

    /* signals */
    void        (*gap_exited)    (GapView    *view);

    /* vfuncs */
    gboolean    (*start_gap)     (GapView    *view,
                                  const char *cmd_line);
    void        (*stop_gap)      (GapView    *view);
    void        (*feed_gap)      (GapView    *view,
                                  const char *text);
    gboolean    (*child_alive)   (GapView    *view);
    void        (*send_intr)     (GapView    *view);

    void        (*get_gap_flags) (GapView    *view,
                                  char      **cmd_line_flags,
                                  gboolean   *fancy);

    void        (*display_graph) (GapView    *view,
                                  GObject    *obj);
};


GType       gap_view_get_type       (void) G_GNUC_CONST;

gboolean    gap_view_child_alive    (GapView    *view);
void        gap_view_send_intr      (GapView    *view);

void        gap_view_start_gap      (GapView    *view,
                                     const char *cmd_base,
                                     const char *workspace);
void        gap_view_stop_gap       (GapView    *view);
void        gap_view_feed_gap       (GapView    *view,
                                     const char *text);

GapSession *gap_view_get_session    (GapView    *view);

void        gap_view_display_graph  (GapView    *view,
                                     GObject    *obj);


G_END_DECLS

#endif /* GAP_VIEW_H */
