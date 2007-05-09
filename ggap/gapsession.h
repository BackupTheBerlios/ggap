/*
 *   gapsession.h
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

#ifndef __GAP_SESSION_H__
#define __GAP_SESSION_H__

#include <glib-object.h>

G_BEGIN_DECLS


#define GAP_TYPE_SESSION                (gap_session_get_type ())
#define GAP_SESSION(object)             (G_TYPE_CHECK_INSTANCE_CAST ((object), GAP_TYPE_SESSION, GapSession))
#define GAP_SESSION_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_SESSION, GapSessionClass))
#define GAP_IS_SESSION(object)          (G_TYPE_CHECK_INSTANCE_TYPE ((object), GAP_TYPE_SESSION))
#define GAP_IS_SESSION_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_SESSION))
#define GAP_SESSION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_SESSION, GapSessionClass))

typedef struct _GapSession GapSession;
typedef struct _GapSessionPrivate GapSessionPrivate;
typedef struct _GapSessionClass GapSessionClass;

struct _GapSession {
    GObject parent;
    GapSessionPrivate *priv;
};

struct _GapSessionClass {
    GObjectClass parent_class;
};


GType            gap_session_get_type       (void) G_GNUC_CONST;

gboolean         gap_session_available      (void);
GapSession      *gap_session_new            (const char *output,
                                             guint       session_id);

void             gap_session_execute        (GapSession *session,
                                             const char *data,
                                             guint       len);
void             gap_session_shutdown       (GapSession *session);


G_END_DECLS

#endif /* __GAP_SESSION_H__ */
