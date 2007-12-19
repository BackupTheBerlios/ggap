#include "mdasyncop.h"
#include "mddocument.h"
#include "marshals.h"
#include "mdenums.h"


struct MdAsyncOpPrivate {
    MdDocument *doc;
    guint started : 1;
    guint finished : 1;
    MdAsyncOpResult result;
    char *title_text;
    char *progress_text;
    char *error;
    double progress_ratio;
};


static void     md_async_op_dispose         (GObject        *object);
static void     md_async_op_set_property    (GObject        *object,
                                             guint           prop_id,
                                             const GValue   *value,
                                             GParamSpec     *pspec);
static void     md_async_op_get_property    (GObject        *object,
                                             guint           prop_id,
                                             GValue         *value,
                                             GParamSpec     *pspec);


G_DEFINE_TYPE (MdAsyncOp, md_async_op, G_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_DOCUMENT
};

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
    object_class->set_property = md_async_op_set_property;
    object_class->get_property = md_async_op_get_property;

    g_type_class_add_private (klass, sizeof (MdAsyncOpPrivate));

    g_object_class_install_property (object_class, PROP_DOCUMENT,
        g_param_spec_object ("document", "document", "document",
                             MD_TYPE_DOCUMENT, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    signals[START] =
        g_signal_new ("start",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAsyncOpClass, start),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    signals[ABORT] =
        g_signal_new ("abort",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAsyncOpClass, abort),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    signals[CANCEL] =
        g_signal_new ("cancel",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAsyncOpClass, cancel),
                      g_signal_accumulator_true_handled, NULL,
                      _moo_ui_marshal_BOOL__VOID,
                      G_TYPE_BOOLEAN, 0);

    signals[PROGRESS] =
        g_signal_new ("progress",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAsyncOpClass, progress),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__STRING_STRING_DOUBLE,
                      G_TYPE_NONE, 3,
                      G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE,
                      G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE,
                      G_TYPE_DOUBLE);

    signals[FINISHED] =
        g_signal_new ("finished",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAsyncOpClass, finished),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__ENUM_STRING,
                      G_TYPE_NONE, 2,
                      MD_TYPE_ASYNC_OP_RESULT,
                      G_TYPE_STRING | G_SIGNAL_TYPE_STATIC_SCOPE);
}

static void
md_async_op_init (MdAsyncOp *op)
{
    op->priv = G_TYPE_INSTANCE_GET_PRIVATE (op, MD_TYPE_ASYNC_OP, MdAsyncOpPrivate);
    op->priv->doc = NULL;
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
        md_async_op_abort (op, FALSE);
    }

    g_free (op->priv->title_text);
    g_free (op->priv->progress_text);
    g_free (op->priv->error);
    op->priv->title_text = NULL;
    op->priv->progress_text = NULL;
    op->priv->error = NULL;

    G_OBJECT_CLASS (md_async_op_parent_class)->dispose (object);
}


static void
md_async_op_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
    MdAsyncOp *op = MD_ASYNC_OP (object);

    switch (prop_id)
    {
        case PROP_DOCUMENT:
            op->priv->doc = g_value_get_object (value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
md_async_op_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    MdAsyncOp *op = MD_ASYNC_OP (object);

    switch (prop_id)
    {
        case PROP_DOCUMENT:
            g_value_set_object (value, op->priv->doc);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

MdDocument *
md_async_op_get_doc (MdAsyncOp *op)
{
    g_return_val_if_fail (MD_IS_ASYNC_OP (op), NULL);
    return op->priv->doc;
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
md_async_op_abort (MdAsyncOp *op,
                   gboolean   interactive)
{
    gboolean do_abort = TRUE;

    g_return_if_fail (MD_IS_ASYNC_OP (op));
    g_return_if_fail (op->priv->started && !op->priv->finished);

    g_object_ref (op);

    if (interactive)
    {
        gboolean stopped = FALSE;
        g_signal_emit (op, signals[CANCEL], 0, &stopped);
        if (stopped)
            do_abort = FALSE;
    }

    if (do_abort && !op->priv->finished)
    {
        g_signal_emit (op, signals[ABORT], 0);

        if (!op->priv->finished)
            md_async_op_finish (op, MD_ASYNC_OP_CANCELLED, NULL);
    }

    g_object_unref (op);
}

void
md_async_op_finish (MdAsyncOp       *op,
                    MdAsyncOpResult  result,
                    const char      *error)
{
    g_return_if_fail (MD_IS_ASYNC_OP (op));
    g_return_if_fail (op->priv->started && !op->priv->finished);

    if (result == MD_ASYNC_OP_ERROR && !error)
    {
        g_critical ("%s: oops", G_STRFUNC);
        error = "Internal error";
    }
    else if (result != MD_ASYNC_OP_ERROR)
    {
        error = NULL;
    }

    op->priv->finished = TRUE;
    op->priv->result = result;
    op->priv->error = g_strdup (error);

    g_signal_emit (op, signals[FINISHED], 0, result, error);
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
