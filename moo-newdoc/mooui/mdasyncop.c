/*
 *   mdasyncop.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "mooui/mdasyncop.h"
#include "mooui/marshals.h"
#include "mooui/mdenums.h"
#include "mooutils/mooutils-misc.h"


struct MdAsyncOpPrivate {
    guint started : 1;
    guint finished : 1;
    char *title_text;
    char *progress_text;
    char *error;
    double progress_ratio;
};


static void     md_async_op_dispose         (GObject        *object);

G_DEFINE_TYPE (MdAsyncOp, md_async_op, G_TYPE_OBJECT)

enum {
    START,
    ABORT,
    CANCEL,
    PROGRESS,
    FINISHED,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

static void
md_async_op_class_init (MdAsyncOpClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = md_async_op_dispose;

    g_type_class_add_private (klass, sizeof (MdAsyncOpPrivate));

    signals[START] =
        g_signal_new ("start",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAsyncOpClass, start),
                      NULL, NULL,
                      _moo_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    signals[ABORT] =
        g_signal_new ("abort",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAsyncOpClass, abort),
                      NULL, NULL,
                      _moo_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    signals[CANCEL] =
        g_signal_new ("cancel",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAsyncOpClass, cancel),
                      g_signal_accumulator_true_handled, NULL,
                      _moo_marshal_BOOL__VOID,
                      G_TYPE_BOOLEAN, 0);

    signals[PROGRESS] =
        g_signal_new ("progress",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAsyncOpClass, progress),
                      NULL, NULL,
                      _moo_marshal_VOID__STRING_DOUBLE,
                      G_TYPE_NONE, 2,
                      G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE,
                      G_TYPE_DOUBLE);

    signals[FINISHED] =
        g_signal_new ("finished",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAsyncOpClass, finished),
                      NULL, NULL,
                      _moo_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);
}

static void
md_async_op_init (MdAsyncOp *op)
{
    op->priv = G_TYPE_INSTANCE_GET_PRIVATE (op, MD_TYPE_ASYNC_OP, MdAsyncOpPrivate);
    op->priv->title_text = NULL;
    op->priv->progress_text = NULL;
    op->priv->error = NULL;
    op->priv->progress_ratio = 0.;
}

static void
md_async_op_dispose (GObject *object)
{
    MdAsyncOp *op = MD_ASYNC_OP (object);

    if (!op->priv->finished && op->priv->started)
    {
        g_critical ("%s: operation not finished", G_STRFUNC);
        md_async_op_abort (op);
    }

    g_free (op->priv->title_text);
    g_free (op->priv->progress_text);
    g_free (op->priv->error);
    op->priv->title_text = NULL;
    op->priv->progress_text = NULL;
    op->priv->error = NULL;

    G_OBJECT_CLASS (md_async_op_parent_class)->dispose (object);
}


void
md_async_op_start (MdAsyncOp *op)
{
    g_return_if_fail (MD_IS_ASYNC_OP (op));
    g_return_if_fail (!op->priv->started);

    op->priv->started = TRUE;
    g_signal_emit (op, signals[START], 0);
}

void
md_async_op_abort (MdAsyncOp *op)
{
    g_return_if_fail (MD_IS_ASYNC_OP (op));
    g_return_if_fail (op->priv->started && !op->priv->finished);

    g_object_ref (op);

    g_signal_emit (op, signals[ABORT], 0);
    if (!op->priv->finished)
        md_async_op_finish (op);

    g_object_unref (op);
}

void
md_async_op_cancel (MdAsyncOp *op)
{
    gboolean stopped = FALSE;

    g_return_if_fail (MD_IS_ASYNC_OP (op));
    g_return_if_fail (op->priv->started && !op->priv->finished);

    g_object_ref (op);

    g_signal_emit (op, signals[CANCEL], 0, &stopped);

    if (!stopped && !op->priv->finished)
        md_async_op_abort (op);

    g_object_unref (op);
}

void
md_async_op_finish (MdAsyncOp *op)
{
    g_return_if_fail (MD_IS_ASYNC_OP (op));
    g_return_if_fail (op->priv->started && !op->priv->finished);
    op->priv->finished = TRUE;
    g_signal_emit (op, signals[FINISHED], 0);
}

void
md_async_op_progress (MdAsyncOp  *op,
                      const char *text,
                      double      fraction)
{
    g_return_if_fail (MD_IS_ASYNC_OP (op));
    g_return_if_fail (op->priv->started && !op->priv->finished);

    if (!text)
        text = "";

    MOO_ASSIGN_STRING (op->priv->progress_text, text);
    op->priv->progress_ratio = fraction;

    g_print ("progress: %f, %s\n", fraction, text);

    g_signal_emit (op, signals[PROGRESS], 0, text, fraction);
}

gboolean
md_async_op_started (MdAsyncOp *op)
{
    g_return_val_if_fail (MD_IS_ASYNC_OP (op), FALSE);
    return op->priv->started;
}

gboolean
md_async_op_finished (MdAsyncOp *op)
{
    g_return_val_if_fail (MD_IS_ASYNC_OP (op), FALSE);
    return op->priv->finished;
}
