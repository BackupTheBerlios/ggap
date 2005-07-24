/*
 *   @(#)$Id: gapapp.h,v 1.5 2005/06/06 14:45:55 emuntyan Exp $
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

#ifndef GAPAPP_H
#define GAPAPP_H

#include "mooapp/mooapp.h"
#include "gap/gapoutchannel.h"
#include "mooutils/moocompat.h"

G_BEGIN_DECLS


#define GAP_TYPE_APP              (gap_app_get_type ())
#define GAP_APP(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), GAP_TYPE_APP, GapApp))
#define GAP_APP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_APP, GapAppClass))
#define GAP_IS_APP(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), GAP_TYPE_APP))
#define GAP_IS_APP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_APP))
#define GAP_APP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_APP, GapAppClass))


typedef struct _GapApp        GapApp;
typedef struct _GapAppPrivate GapAppPrivate;
typedef struct _GapAppClass   GapAppClass;

struct _GapApp
{
    MooApp           parent;

    GapAppPrivate   *priv;
};

struct _GapAppClass
{
    MooAppClass      parent_class;
};


GType            gap_app_get_type               (void) G_GNUC_CONST;

GapApp          *gap_app                        (void);

MooApp          *gap_app_new                    (int     argc,
                                                 char  **argv);

GOptionGroup    *gap_app_get_option_group       (void);
const char      *gap_app_get_profile_name       (void);

void             gap_app_feed_gap               (GapApp     *app,
                                                 const char *text,
                                                 gssize      len);

void             gap_app_start_gap              (GapApp     *app);
void             gap_app_stop_gap               (GapApp     *app);
void             gap_app_restart_gap            (GapApp     *app);

GapOutChannel   *gap_app_get_out_channel        (GapApp     *app);
gboolean         gap_app_write_out              (GapApp     *app,
                                                 const char *data,
                                                 gssize      len);


G_END_DECLS

#endif /* MOOAPP_H */
