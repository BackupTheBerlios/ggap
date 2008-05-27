/*
 *   gapapp.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef GAP_APP_H
#define GAP_APP_H

#include <mooui/mooapp.h>

G_BEGIN_DECLS


#define GGAP_PREFS_PREFIX               "ggap"
#define GGAP_PREFS_GAP_COMMAND          GGAP_PREFS_PREFIX "/command"
#define GGAP_PREFS_GAP_WORKING_DIR      GGAP_PREFS_PREFIX "/working_dir"
#define GGAP_PREFS_GAP_SAVE_WORKSPACE   GGAP_PREFS_PREFIX "/save_workspace"
#define GGAP_PREFS_GAP_CLEAR_TERMINAL   GGAP_PREFS_PREFIX "/clear_terminal"

#define GAP_TYPE_APP                (gap_app_get_type ())
#define GAP_APP(object)             (G_TYPE_CHECK_INSTANCE_CAST ((object), GAP_TYPE_APP, GapApp))
#define GAP_APP_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_APP, GapAppClass))
#define GAP_IS_APP(object)          (G_TYPE_CHECK_INSTANCE_TYPE ((object), GAP_TYPE_APP))
#define GAP_IS_APP_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_APP))
#define GAP_APP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_APP, GapAppClass))


typedef struct GapApp        GapApp;
typedef struct GapAppPrivate GapAppPrivate;
typedef struct GapAppClass   GapAppClass;

struct GapApp
{
    MooApp base;
    GapAppPrivate *priv;
};

struct GapAppClass
{
    MooAppClass base_class;
};


#define GAP_APP_INSTANCE (GAP_APP (moo_app_instance ()))

GType       gap_app_get_type                (void) G_GNUC_CONST;

void        gap_app_send_and_bring_to_front (const char *string);


G_END_DECLS

#endif /* GAP_APP_H */
