/*
 *   gapsession.c
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

#include "config.h"
#include "gapsession.h"
#include "gapview.h"
#include "gapoutput.h"


static GHashTable *sessions;


struct _GapSessionPrivate {
    GapView *view;
    guint id;
};


G_DEFINE_TYPE(GapSession, gap_session, G_TYPE_OBJECT)


static void
gap_session_init (GapSession *session)
{
    session->priv = G_TYPE_INSTANCE_GET_PRIVATE (session, GAP_TYPE_SESSION,
                                                 GapSessionPrivate);
}


static void
gap_session_finalize (GObject *object)
{
    GapSession *session = GAP_SESSION (object);
    gap_session_shutdown (session);
    G_OBJECT_CLASS (gap_session_parent_class)->finalize (object);
}


static void
gap_session_class_init (GapSessionClass *klass)
{
    G_OBJECT_CLASS(klass)->finalize = gap_session_finalize;
    g_type_class_add_private (klass, sizeof (GapSessionPrivate));
}


GapSession *
gap_session_new (const char *output,
                 guint       session_id,
                 gpointer    view)
{
    GapSession *session;

    g_return_val_if_fail (output != NULL, NULL);
    g_return_val_if_fail (GAP_IS_VIEW (view), NULL);

    if (sessions && g_hash_table_lookup (sessions, GUINT_TO_POINTER (session_id)))
    {
        g_critical ("%s: session with id %u already exists", G_STRLOC, session_id);
        return NULL;
    }

    session = g_object_new (GAP_TYPE_SESSION, NULL);
    session->priv->view = view;
    session->priv->id = session_id;

    if (!sessions)
        sessions = g_hash_table_new (g_direct_hash, g_direct_equal);
    g_hash_table_insert (sessions, GUINT_TO_POINTER (session_id), session);

    return session;
}


void
gap_session_shutdown (GapSession *session)
{
    g_return_if_fail (GAP_IS_SESSION (session));
    g_hash_table_remove (sessions, GUINT_TO_POINTER (session->priv->id));
}
