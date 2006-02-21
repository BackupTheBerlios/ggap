/*
 *   gapapp.h
 *
 *   Copyright (C) 2004-2005 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef __GAP_APP_H__
#define __GAP_APP_H__

#include <mooapp/mooapp.h>
#include "gaptermwindow.h"

G_BEGIN_DECLS


#define GAP_TYPE_APP                (gap_app_get_type ())
#define GAP_APP(object)             (G_TYPE_CHECK_INSTANCE_CAST ((object), GAP_TYPE_APP, GapApp))
#define GAP_APP_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_APP, GapAppClass))
#define GAP_IS_APP(object)          (G_TYPE_CHECK_INSTANCE_TYPE ((object), GAP_TYPE_APP))
#define GAP_IS_APP_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_APP))
#define GAP_APP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_APP, GapAppClass))


typedef struct _GapApp              GapApp;
typedef struct _GapAppPrivate       GapAppPrivate;
typedef struct _GapAppClass         GapAppClass;

struct _GapApp
{
    MooApp base;

    MooTerm *term;
    MooTermWindow *term_window;

    char *gap_cmd_line;
    gboolean editor_mode;
    gboolean simple;
};

struct _GapAppClass
{
    MooAppClass base_class;
};


#define GAP_APP_INSTANCE (GAP_APP (moo_app_get_instance ()))

GType   gap_app_get_type    (void) G_GNUC_CONST;

void    gap_app_restart_gap (GapApp     *app);
void    gap_app_start_gap   (GapApp     *app);
void    gap_app_stop_gap    (GapApp     *app);

void    gap_app_feed_gap    (GapApp     *app,
                             const char *text);


G_END_DECLS

#endif /* __GAP_APP_H__ */
