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
#include "ggap-mod.h"
#include "gap-mod.h"
#include "gap-cb-mod.h"
#include "gap-canvas-mod.h"
#include "gap-types-mod.h"
#include "gapoutput.h"
#include <mooutils/moopython.h>


static GHashTable *sessions;


struct _GapSessionPrivate {
    MooPyObject *py_session;
    GapView *view;
    guint id;
};


G_DEFINE_TYPE(GapSession, gap_session, G_TYPE_OBJECT)


static void
gap_session_init (GapSession *session)
{
    session->priv = g_new0 (GapSessionPrivate, 1);
}


static void
gap_session_finalize (GObject *object)
{
    GapSession *session = GAP_SESSION (object);
    gap_session_shutdown (session);
    g_free (session->priv);
    G_OBJECT_CLASS (gap_session_parent_class)->finalize (object);
}


static void
gap_session_class_init (GapSessionClass *klass)
{
    G_OBJECT_CLASS(klass)->finalize = gap_session_finalize;
}


static MooPyObject *
output_write_meth (G_GNUC_UNUSED MooPyObject *self,
                   MooPyObject *args)
{
    const char *data;
    int len = -1;

    if (!moo_PyArg_ParseTuple (args, "s#", &data, &len))
        return NULL;

    gap_app_output_write (data, len);

    moo_Py_INCREF (moo_Py_None);
    return moo_Py_None;
}

static MooPyObject *
display_graph_meth (G_GNUC_UNUSED MooPyObject *self,
                    MooPyObject *args)
{
    MooPyObject *py_obj;
    guint session_id;
    GObject *obj;
    GapSession *session;

    if (!moo_PyArg_ParseTuple (args, "IO", &session_id, &py_obj))
        return NULL;

    if (!sessions || !(session = g_hash_table_lookup (sessions, GUINT_TO_POINTER (session_id))))
    {
        moo_py_set_error (MOO_PY_RUNTIME_ERROR, "no session with id %u", session_id);
        return NULL;
    }

    if (!(obj = moo_gobject_from_py_object (py_obj)))
    {
        moo_py_set_error (MOO_PY_TYPE_ERROR, "second argument must be a GObject");
        return NULL;
    }

    g_return_val_if_fail (session->priv != NULL && GAP_IS_VIEW (session->priv->view), NULL);
    gap_view_display_graph (session->priv->view, obj);

    moo_Py_INCREF (moo_Py_None);
    return moo_Py_None;
}

static gboolean
import_module (const char *name,
               const char *code)
{
    MooPyObject *mod;

    if (!(mod = moo_py_import_exec (name, code)))
    {
        g_critical ("could not import module '%s'", name);
        moo_PyErr_Print ();
        return FALSE;
    }

    moo_Py_DECREF (mod);
    return TRUE;
}

static gboolean
add_method (MooPyObject   *mod,
            const char    *name,
            MooPyCFunction cfunc)
{
    MooPyObject *func;
    MooPyMethodDef meth;
    gboolean success = FALSE;

    meth.ml_name = name;
    meth.ml_meth = cfunc;
    meth.ml_flags = MOO_PY_METH_VARARGS;
    meth.ml_doc = NULL;
    func = moo_PyCFunction_New (&meth, NULL);

    if (!func || moo_PyModule_AddObject (mod, name, func) != 0)
    {
        moo_PyErr_Print ();
        moo_Py_DECREF (func);
    }
    else
    {
        success = TRUE;
    }

    return success;
}

static gboolean
init_ggap_module (void)
{
    static gboolean been_here;
    static gboolean success;
    MooPyObject *mod = NULL;

    if (been_here)
        return success;

    been_here = TRUE;
    success = FALSE;

    if (!import_module ("gap_canvas", GAP_CANVAS_PY) ||
        !import_module ("gap_types", GAP_TYPES_PY) ||
        !import_module ("gap_cb", GAP_CB_PY) ||
        !import_module ("gap", GAP_PY))
            goto out;

    if (!(mod = moo_py_import_exec ("ggap", GGAP_PY)))
    {
        g_critical ("could not import module '%s'", "ggap");
        moo_PyErr_Print ();
        goto out;
    }

    if (!add_method (mod, "_app_output_write", output_write_meth) ||
        !add_method (mod, "_display_graph", display_graph_meth))
        goto out;

    success = TRUE;

out:
    moo_Py_DECREF (mod);
    return success;
}


gboolean
gap_session_available (void)
{
    return moo_python_running ();
}


GapSession *
gap_session_new (const char *output,
                 guint       session_id,
                 gpointer    view)
{
    GapSession *session;
    char *code;
    MooPyObject *py_session;

    g_return_val_if_fail (output != NULL, NULL);
    g_return_val_if_fail (GAP_IS_VIEW (view), NULL);

    if (sessions && g_hash_table_lookup (sessions, GUINT_TO_POINTER (session_id)))
    {
        g_critical ("%s: session with id %u already exists", G_STRLOC, session_id);
        return NULL;
    }

    if (!moo_python_running ())
        return NULL;

    if (!init_ggap_module ())
        return NULL;

    code = g_strdup_printf ("import ggap\n"
                            "import sys\n"
                            "__retval__ = ggap.Session(%d, log=sys.stdout)",
                            session_id);
    py_session = moo_python_run_code (code, NULL, NULL);
    g_free (code);

    if (!py_session)
    {
        moo_PyErr_Print ();
        return NULL;
    }

    session = g_object_new (GAP_TYPE_SESSION, NULL);
    session->priv->py_session = py_session;
    session->priv->view = view;
    session->priv->id = session_id;

    if (!sessions)
        sessions = g_hash_table_new (g_direct_hash, g_direct_equal);
    g_hash_table_insert (sessions, GUINT_TO_POINTER (session_id), session);

    return session;
}


void
gap_session_execute (GapSession *session,
                     const char *data,
                     guint       len)
{
    MooPyObject *ret;

    g_return_if_fail (GAP_IS_SESSION (session));
    g_return_if_fail (session->priv->py_session != NULL);
    g_return_if_fail (data != NULL);

    if (!len)
        return;

    ret = moo_PyObject_CallMethod (session->priv->py_session, "execute", "s", data);

    if (!ret)
    {
        moo_PyErr_Print ();
        return;
    }

    moo_Py_DECREF (ret);
}


void
gap_session_shutdown (GapSession *session)
{
    g_return_if_fail (GAP_IS_SESSION (session));

    if (session->priv->py_session)
    {
        MooPyObject *ret;

        ret = moo_PyObject_CallMethod (session->priv->py_session, "shutdown", NULL);

        if (!ret)
            moo_PyErr_Print ();
        else
            moo_Py_DECREF (ret);

        moo_Py_DECREF (session->priv->py_session);
        session->priv->py_session = NULL;

        g_hash_table_remove (sessions, GUINT_TO_POINTER (session->priv->id));
    }
}
