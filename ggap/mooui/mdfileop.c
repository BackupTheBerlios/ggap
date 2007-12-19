#include "mdfileop.h"
#include "mddocument.h"
#include <glib/gstdio.h>
#include <unistd.h>


GQuark
_md_file_error_quark (void)
{
    static GQuark q;

    if (G_UNLIKELY (!q))
        q = g_quark_from_static_string ("md-file-error");

    return q;
}


/*************************************************************************/
/* MdAsyncLoadingOp
 */

static void     md_async_loading_op_finalize    (GObject    *object);
static void     md_async_loading_op_start       (MdAsyncOp  *op);
static void     md_async_loading_op_abort       (MdAsyncOp  *op);

G_DEFINE_TYPE (MdAsyncLoadingOp, _md_async_loading_op, MD_TYPE_ASYNC_OP)

struct MdAsyncLoadingOpPrivate {
    char *tmp_file;
    MdFileInfo *file_info;
};

static void
_md_async_loading_op_class_init (MdAsyncLoadingOpClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MdAsyncOpClass *op_class = MD_ASYNC_OP_CLASS (klass);

    g_type_class_add_private (klass, sizeof (MdAsyncLoadingOpPrivate));

    object_class->finalize = md_async_loading_op_finalize;

    op_class->start = md_async_loading_op_start;
    op_class->abort = md_async_loading_op_abort;
}

static void
_md_async_loading_op_init (MdAsyncLoadingOp *op)
{
    op->priv = G_TYPE_INSTANCE_GET_PRIVATE (op, MD_TYPE_ASYNC_LOADING_OP,
                                            MdAsyncLoadingOpPrivate);
}

static void
md_async_loading_op_finalize (GObject *object)
{
    MdAsyncLoadingOp *op = MD_ASYNC_LOADING_OP (object);

    if (op->priv->tmp_file)
    {
        g_unlink (op->priv->tmp_file);
        g_free (op->priv->tmp_file);
    }

    md_file_info_free (op->priv->file_info);

    G_OBJECT_CLASS (_md_async_loading_op_parent_class)->finalize (object);
}

static void
md_async_loading_op_start (MdAsyncOp *aop)
{
    MdAsyncLoadingOp *op = MD_ASYNC_LOADING_OP (aop);

    g_return_if_fail (op->priv->tmp_file == NULL);
}

static void
md_async_loading_op_abort (MdAsyncOp *aop)
{
    MdAsyncLoadingOp *op = MD_ASYNC_LOADING_OP (aop);

    g_return_if_fail (op->priv->tmp_file != NULL);
}


MdAsyncOp *
_md_async_loading_op_new (MdDocument  *doc,
                          MdFileInfo  *file,
                          GError     **error)
{
    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);
    g_return_val_if_fail (file != NULL, NULL);
    g_return_val_if_fail (!error || !*error, NULL);

    g_set_error (error, MD_FILE_ERROR,
                 MD_FILE_ERROR_NOT_IMPLEMENTED,
                 "Loading not implemented");
    return NULL;
}

const char *
_md_async_loading_op_get_tmp_file (MdAsyncLoadingOp *op)
{
    g_return_val_if_fail (MD_IS_ASYNC_LOADING_OP (op), NULL);
    return op->priv->tmp_file;
}

MdFileInfo *
_md_async_loading_op_get_file_info (MdAsyncLoadingOp *op)
{
    g_return_val_if_fail (MD_IS_ASYNC_LOADING_OP (op), NULL);
    return op->priv->file_info;
}


/*************************************************************************/
/* MdAsyncSavingOp
 */

static void     md_async_saving_op_finalize     (GObject    *object);
static void     md_async_saving_op_start        (MdAsyncOp  *op);
static void     md_async_saving_op_abort        (MdAsyncOp  *op);

G_DEFINE_TYPE (MdAsyncSavingOp, _md_async_saving_op, MD_TYPE_ASYNC_OP)

struct MdAsyncSavingOpPrivate {
    char *tmp_file;
};

static void
_md_async_saving_op_class_init (MdAsyncSavingOpClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MdAsyncOpClass *op_class = MD_ASYNC_OP_CLASS (klass);

    g_type_class_add_private (klass, sizeof (MdAsyncSavingOpPrivate));

    object_class->finalize = md_async_saving_op_finalize;

    op_class->start = md_async_saving_op_start;
    op_class->abort = md_async_saving_op_abort;
}

static void
_md_async_saving_op_init (MdAsyncSavingOp *op)
{
    op->priv = G_TYPE_INSTANCE_GET_PRIVATE (op, MD_TYPE_ASYNC_SAVING_OP,
                                            MdAsyncSavingOpPrivate);
    op->priv->tmp_file = NULL;
}

static void
md_async_saving_op_finalize (GObject *object)
{
    MdAsyncSavingOp *op = MD_ASYNC_SAVING_OP (object);

    if (op->priv->tmp_file)
    {
        g_unlink (op->priv->tmp_file);
        g_free (op->priv->tmp_file);
    }

    G_OBJECT_CLASS (_md_async_saving_op_parent_class)->finalize (object);
}

static void
md_async_saving_op_start (MdAsyncOp *aop)
{
    MdAsyncSavingOp *op = MD_ASYNC_SAVING_OP (aop);

    g_return_if_fail (op->priv->tmp_file == NULL);
}

static void
md_async_saving_op_abort (MdAsyncOp *aop)
{
    MdAsyncSavingOp *op = MD_ASYNC_SAVING_OP (aop);

    g_return_if_fail (op->priv->tmp_file != NULL);
}


MdAsyncOp *
_md_async_saving_op_new (MdDocument  *doc,
                         MdFileInfo  *file,
                         const char  *tmp_file,
                         GError     **error)
{
    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);
    g_return_val_if_fail (file != NULL, NULL);
    g_return_val_if_fail (tmp_file != NULL, NULL);
    g_return_val_if_fail (!error || !*error, NULL);

    g_set_error (error, MD_FILE_ERROR,
                 MD_FILE_ERROR_NOT_IMPLEMENTED,
                 "Saving not implemented");
    return NULL;
}
