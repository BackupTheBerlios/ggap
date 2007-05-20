/*
 *   gapapp.h
 *
 *   Copyright (C) 2004-2006 by Yevgen Muntyan <muntyan@math.tamu.edu>
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
#include "gapwindow.h"
#include "gapsession.h"

G_BEGIN_DECLS


extern gboolean GAP_APP_EDITOR_MODE;

#define GGAP_PREFS_PREFIX               "ggap"
#define GGAP_PREFS_GAP_COMMAND          GGAP_PREFS_PREFIX "/command"
#define GGAP_PREFS_GAP_INIT_PKG         GGAP_PREFS_PREFIX "/init_pkg"
#define GGAP_PREFS_GAP_WORKING_DIR      GGAP_PREFS_PREFIX "/working_dir"
#define GGAP_PREFS_GAP_SAVE_WORKSPACE   GGAP_PREFS_PREFIX "/save_workspace"
#define GGAP_PREFS_GAP_CLEAR_TERMINAL   GGAP_PREFS_PREFIX "/clear_terminal"
#define GGAP_WORKSPACE_FILE             "workspace"

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

    GapView *terminal;
    GapWindow *gap_window;

    char *gap_cmd_line;
    gboolean editor_mode;
    gboolean fancy;
};

struct _GapAppClass
{
    MooAppClass base_class;
};


#define GAP_APP_INSTANCE (GAP_APP (moo_app_get_instance ()))

GType       gap_app_get_type                (void) G_GNUC_CONST;

void        gap_app_restart_gap             (GapApp     *app);
void        gap_app_start_gap               (GapApp     *app);
void        gap_app_stop_gap                (GapApp     *app);
void        gap_app_send_intr               (GapApp     *app);

void        gap_app_open_workspace          (GapApp     *app,
                                             const char *file);
char       *gap_saved_workspace_filename    (void);

void        gap_app_feed_gap                (GapApp     *app,
                                             const char *text);


G_END_DECLS

#endif /* __GAP_APP_H__ */
