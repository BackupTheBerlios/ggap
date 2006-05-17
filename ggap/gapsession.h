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


#define GAP_TYPE_OBJECT                 (gap_object_get_type ())
#define GAP_OBJECT(object)              (G_TYPE_CHECK_INSTANCE_CAST ((object), GAP_TYPE_OBJECT, GapObject))
#define GAP_OBJECT_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_OBJECT, GapObjectClass))
#define GAP_IS_OBJECT(object)           (G_TYPE_CHECK_INSTANCE_TYPE ((object), GAP_TYPE_OBJECT))
#define GAP_IS_OBJECT_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_OBJECT))
#define GAP_OBJECT_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_OBJECT, GapObjectClass))

#define GAP_TYPE_SESSION                (gap_session_get_type ())
#define GAP_SESSION(object)             (G_TYPE_CHECK_INSTANCE_CAST ((object), GAP_TYPE_SESSION, GapSession))
#define GAP_SESSION_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_SESSION, GapSessionClass))
#define GAP_IS_SESSION(object)          (G_TYPE_CHECK_INSTANCE_TYPE ((object), GAP_TYPE_SESSION))
#define GAP_IS_SESSION_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_SESSION))
#define GAP_SESSION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_SESSION, GapSessionClass))

typedef struct _GapObject GapObject;
typedef struct _GapObjectClass GapObjectClass;
typedef struct _GapSession GapSession;
typedef struct _GapSessionClass GapSessionClass;

struct _GapObject {
    GObject parent;

    gpointer obj;
    char *id;

    GSList *callbacks;

    guint dead : 1;
    guint destroyed : 1;
    guint toplevel : 1;
};

struct _GapObjectClass {
    GObjectClass parent_class;

    void (*object_died) (GapObject *object);
};

struct _GapSession {
    GObject parent;
    GHashTable *objects;
};

struct _GapSessionClass {
    GObjectClass parent_class;
};


GType            gap_object_get_type        (void) G_GNUC_CONST;
GType            gap_session_get_type       (void) G_GNUC_CONST;

GapSession      *gap_session_new            (void);

GapObject       *gap_session_add_object     (GapSession *session,
                                             gpointer    object,
                                             gboolean    toplevel);
GapObject       *gap_session_find_object    (GapSession *session,
                                             const char *id);

void             gap_object_destroy         (GapObject  *object);
void             gap_object_connect         (GapObject  *object,
                                             const char *gap_id,
                                             gulong      handler);
void             gap_object_disconnect      (GapObject  *object,
                                             const char *gap_id);


G_END_DECLS

#endif /* __GAP_SESSION_H__ */
