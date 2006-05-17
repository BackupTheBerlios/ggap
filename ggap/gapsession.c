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

#include "gapapp-script.h"
#include "gapapp.h"
#include "mooutils/moomarshals.h"
#include <gtk/gtk.h>
#include <string.h>


typedef struct {
    gulong id;
    char *gap_id;
} Callback;


static Callback    *callback_new        (const char *gap_id,
                                         gulong      id);
static void         callback_free       (Callback   *cb);

static GapObject   *gap_object_new      (gpointer    object,
                                         const char *id,
                                         gboolean    toplevel);
static void         object_died         (GapObject  *wrapper,
                                         gpointer    object);
static gboolean     close_window        (GapObject  *wrapper);
static void         disconnect_wrapper  (GapObject  *wrapper);


G_DEFINE_TYPE(GapObject, gap_object, G_TYPE_OBJECT)
G_DEFINE_TYPE(GapSession, gap_session, G_TYPE_OBJECT)

enum {
    OBJECT_DIED,
    N_OBJECT_SIGNALS
};

static guint object_signals[N_OBJECT_SIGNALS];


static void
gap_object_init (GapObject *obj)
{
    obj->dead = FALSE;
}


static void
disconnect_wrapper (GapObject *wrapper)
{
    gpointer object;

    g_return_if_fail (GTK_IS_OBJECT (wrapper->obj));

    wrapper->dead = TRUE;
    object = wrapper->obj;
    wrapper->obj = NULL;

    g_signal_handlers_disconnect_by_func (object, (gpointer) object_died, wrapper);

    if (GTK_IS_WINDOW (object))
        g_signal_handlers_disconnect_by_func (object, (gpointer) close_window, wrapper);

    while (wrapper->callbacks)
    {
        Callback *cb = wrapper->callbacks->data;
        wrapper->callbacks = g_slist_delete_link (wrapper->callbacks, wrapper->callbacks);
        g_signal_handler_disconnect (object, cb->id);
        callback_free (cb);
    }
}


static void
gap_object_finalize (GObject *object)
{
    GapObject *gap_obj = GAP_OBJECT (object);

    g_free (gap_obj->id);

    if (gap_obj->obj)
        disconnect_wrapper (gap_obj);

    G_OBJECT_CLASS (gap_object_parent_class)->finalize (object);
}


static void
gap_object_class_init (GapObjectClass *klass)
{
    G_OBJECT_CLASS(klass)->finalize = gap_object_finalize;

    object_signals[OBJECT_DIED] =
            g_signal_new ("object-died",
                          G_TYPE_FROM_CLASS (klass),
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (GapObjectClass, object_died),
                          NULL, NULL,
                          _moo_marshal_VOID__VOID,
                          G_TYPE_NONE, 0);
}


static void
gap_session_init (GapSession *session)
{
    session->objects = g_hash_table_new_full (g_str_hash, g_str_equal,
                                              g_free, g_object_unref);
}


static void
gap_session_finalize (GObject *object)
{
    GapSession *session = GAP_SESSION (object);

    g_hash_table_destroy (session->objects);

    G_OBJECT_CLASS (gap_session_parent_class)->finalize (object);
}


static void
gap_session_class_init (GapSessionClass *klass)
{
    G_OBJECT_CLASS(klass)->finalize = gap_session_finalize;
}


GapSession *
gap_session_new (void)
{
    return g_object_new (GAP_TYPE_SESSION, NULL);
}


static void
session_object_died (GapSession *session,
                     GapObject  *wrapper)
{
    g_return_if_fail (GAP_IS_SESSION (session));
    g_return_if_fail (GAP_IS_OBJECT (wrapper));
    g_return_if_fail (g_hash_table_lookup (session->objects, wrapper->id) == wrapper);

    g_object_ref (wrapper);

    g_signal_handlers_disconnect_by_func (wrapper, (gpointer) session_object_died, session);
    g_hash_table_remove (session->objects, wrapper->id);

    if (!wrapper->destroyed && wrapper->toplevel)
    {
        GString *data = g_string_new (NULL);
        gap_data_add_command_object_died (data, wrapper->id);
        gap_data_send (data);
        g_string_free (data, TRUE);;
    }

    g_object_unref (wrapper);
}


static char *
generate_id (gpointer object)
{
#ifdef MOO_DEBUG
    return g_strdup_printf ("%s-%p-%08x", g_type_name (G_OBJECT_TYPE (object)),
                            object, g_random_int ());
#else
    id = g_strdup_printf ("%08x", g_random_int ());
#endif
}


GapObject *
gap_session_add_object (GapSession *session,
                        gpointer    object,
                        gboolean    toplevel)
{
    GapObject *wrapper;
    char *id;

    g_return_val_if_fail (GAP_IS_SESSION (session), NULL);
    g_return_val_if_fail (GTK_IS_OBJECT (object), NULL);

    wrapper = g_object_get_data (object, "gap-object");

    if (wrapper)
        return wrapper;

    id = generate_id (object);

    if (g_hash_table_lookup (session->objects, id))
    {
        g_critical ("object with id '%s' already registered", id);
        return NULL;
    }

    wrapper = gap_object_new (object, id, toplevel);
    g_hash_table_insert (session->objects, g_strdup (id), wrapper);

    g_signal_connect_swapped (wrapper, "object-died",
                              G_CALLBACK (session_object_died),
                              session);

    g_free (id);
    return wrapper;
}


static void
object_died (GapObject *wrapper,
             gpointer   object)
{
    g_return_if_fail (GAP_IS_OBJECT (wrapper));
    g_return_if_fail (!wrapper->dead);
    g_return_if_fail (wrapper->obj == object);

    disconnect_wrapper (wrapper);

    g_signal_emit (wrapper, object_signals[OBJECT_DIED], 0);
}


static GapObject *
gap_object_new (gpointer    object,
                const char *id,
                gboolean    toplevel)
{
    GapObject *wrapper;

    g_return_val_if_fail (GTK_IS_OBJECT (object), NULL);
    g_return_val_if_fail (id && id[0], NULL);

    wrapper = g_object_new (GAP_TYPE_OBJECT, NULL);
    wrapper->obj = object;
    wrapper->id = g_strdup (id);
    wrapper->toplevel = toplevel != 0;

    g_signal_connect_swapped (object, "destroy",
                              G_CALLBACK (object_died),
                              wrapper);

    if (GTK_IS_WINDOW (object))
        g_signal_connect_swapped (object, "delete-event",
                                  G_CALLBACK (close_window),
                                  wrapper);

    return wrapper;
}


void
gap_object_destroy (GapObject *object)
{
    g_return_if_fail (GAP_IS_OBJECT (object));
    g_return_if_fail (!object->dead);
    g_return_if_fail (GTK_IS_OBJECT (object->obj));

    g_object_ref (object);

    object->destroyed = TRUE;
    gtk_object_destroy (object->obj);

    g_object_unref (object);
}


GapObject *
gap_session_find_object (GapSession  *session,
                         const char  *id)
{
    g_return_val_if_fail (GAP_IS_SESSION (session), NULL);
    g_return_val_if_fail (id != NULL, NULL);
    return g_hash_table_lookup (session->objects, id);
}


static gboolean
close_window (GapObject *wrapper)
{
    GString *data;

    g_return_val_if_fail (GAP_IS_OBJECT (wrapper) && !wrapper->dead, FALSE);

    data = g_string_new (NULL);
    gap_data_add_command_close_window  (data, wrapper->id);
    gap_data_send (data);

    g_string_free (data, TRUE);
    return TRUE;
}


static Callback *
callback_new (const char *gap_id,
              gulong      id)
{
    Callback *cb;

    g_return_val_if_fail (gap_id != NULL, NULL);
    g_return_val_if_fail (id != 0, NULL);

    cb = g_new0 (Callback, 1);

    cb->gap_id = g_strdup (gap_id);
    cb->id = id;

    return cb;
}


static void
callback_free (Callback *cb)
{
    if (cb)
    {
        g_free (cb->gap_id);
        g_free (cb);
    }
}


void
gap_object_connect (GapObject  *object,
                    const char *gap_id,
                    gulong      handler)
{
    Callback *cb;

    g_return_if_fail (GAP_IS_OBJECT (object));
    g_return_if_fail (gap_id != NULL);
    g_return_if_fail (handler != 0);

    cb = callback_new (gap_id, handler);
    object->callbacks = g_slist_prepend (object->callbacks, cb);
}


void
gap_object_disconnect (GapObject  *object,
                       const char *gap_id)
{
    GSList *l;

    g_return_if_fail (GAP_IS_OBJECT (object));
    g_return_if_fail (gap_id != NULL);

    for (l = object->callbacks; l != NULL; l = l->next)
    {
        Callback *cb = l->data;

        if (!strcmp (cb->gap_id, gap_id))
        {
            object->callbacks = g_slist_delete_link (object->callbacks, l);
            g_signal_handler_disconnect (object->obj, cb->id);
            callback_free (cb);
            return;
        }
    }
}
