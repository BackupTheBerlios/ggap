/*
 *   @(#)$Id: sc3app.h,v 1.4 2005/06/06 14:45:55 emuntyan Exp $
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

#ifndef SC3_SC3APP_H
#define SC3_SC3APP_H

#include "mooapp/mooapp.h"
#include "mooutils/moocompat.h"

G_BEGIN_DECLS


#define SC3_TYPE_APP              (sc3_app_get_type ())
#define SC3_APP(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), SC3_TYPE_APP, Sc3App))
#define SC3_APP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), SC3_TYPE_APP, Sc3AppClass))
#define SC3_IS_APP(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), SC3_TYPE_APP))
#define SC3_IS_APP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), SC3_TYPE_APP))
#define SC3_APP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), SC3_TYPE_APP, Sc3AppClass))


typedef struct _Sc3App        Sc3App;
typedef struct _Sc3AppPrivate Sc3AppPrivate;
typedef struct _Sc3AppClass   Sc3AppClass;

struct _Sc3App
{
    MooApp           parent;

    Sc3AppPrivate   *priv;
};

struct _Sc3AppClass
{
    MooAppClass      parent_class;
};


GType            sc3_app_get_type               (void) G_GNUC_CONST;

Sc3App          *sc3_app                        (void);

MooApp          *sc3_app_new                    (int     argc,
                                                 char  **argv);

GOptionGroup    *sc3_app_get_option_group       (void);
const char      *sc3_app_get_profile_name       (void);

void             sc3_app_feed_sclang            (Sc3App     *app,
                                                 const char *text,
                                                 gssize      len);

void             sc3_app_start_sclang           (Sc3App     *app);
void             sc3_app_stop_sclang            (Sc3App     *app);
void             sc3_app_restart_sclang         (Sc3App     *app);

void             sc3_app_eval_all               (Sc3App     *app);
void             sc3_app_stop_server            (Sc3App     *app);


G_END_DECLS

#endif /* SC3_SC3APP_H */
