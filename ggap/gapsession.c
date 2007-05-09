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
#include "ggap-mod.h"
#include "gapoutput.h"
#include <mooutils/moopython.h>


struct _GapSessionPrivate {
    MooPyObject *py_session;
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
    gap_session_shutdown (GAP_SESSION (object));
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

static gboolean
init_ggap_module (void)
{
    static gboolean been_here;
    static gboolean success;
    MooPyObject *mod = NULL, *func = NULL;
    MooPyMethodDef meth;

    if (been_here)
        return success;

    been_here = TRUE;
    success = FALSE;

    mod = moo_py_import_exec ("ggap", GGAP_PY);

    if (!mod)
        goto out;

    meth.ml_name = "_app_output_write";
    meth.ml_meth = output_write_meth;
    meth.ml_flags = MOO_PY_METH_VARARGS;
    meth.ml_doc = NULL;
    func = moo_PyCFunction_New (&meth, NULL);

    if (!func)
    {
        moo_PyErr_Print ();
        goto out;
    }

    if (moo_PyModule_AddObject (mod, "_app_output_write", func) != 0)
    {
        moo_PyErr_Print ();
        goto out;
    }

    func = NULL;
    success = TRUE;

out:
    moo_Py_DECREF (mod);
    moo_Py_DECREF (func);
    return success;
}


gboolean
gap_session_available (void)
{
    return moo_python_running ();
}


GapSession *
gap_session_new (const char *output,
                 guint       session_id)
{
    GapSession *session;
    char *code;
    MooPyObject *py_session;

    g_return_val_if_fail (output != NULL, NULL);

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
    }
}
