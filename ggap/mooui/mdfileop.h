#ifndef MD_FILE_OP_H
#define MD_FILE_OP_H

#include <mooui/mdasyncop.h>


#define MD_TYPE_ASYNC_LOADING_OP             (_md_async_loading_op_get_type ())
#define MD_ASYNC_LOADING_OP(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_ASYNC_LOADING_OP, MdAsyncLoadingOp))
#define MD_ASYNC_LOADING_OP_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_ASYNC_LOADING_OP, MdAsyncLoadingOpClass))
#define MD_IS_ASYNC_LOADING_OP(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_ASYNC_LOADING_OP))
#define MD_IS_ASYNC_LOADING_OP_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_ASYNC_LOADING_OP))
#define MD_ASYNC_LOADING_OP_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_ASYNC_LOADING_OP, MdAsyncLoadingOpClass))

#define MD_TYPE_ASYNC_SAVING_OP              (_md_async_saving_op_get_type ())
#define MD_ASYNC_SAVING_OP(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_ASYNC_SAVING_OP, MdAsyncSavingOp))
#define MD_ASYNC_SAVING_OP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_ASYNC_SAVING_OP, MdAsyncSavingOpClass))
#define MD_IS_ASYNC_SAVING_OP(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_ASYNC_SAVING_OP))
#define MD_IS_ASYNC_SAVING_OP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_ASYNC_SAVING_OP))
#define MD_ASYNC_SAVING_OP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_ASYNC_SAVING_OP, MdAsyncSavingOpClass))

typedef struct MdAsyncLoadingOp MdAsyncLoadingOp;
typedef struct MdAsyncLoadingOpClass MdAsyncLoadingOpClass;
typedef struct MdAsyncLoadingOpPrivate MdAsyncLoadingOpPrivate;
typedef struct MdAsyncSavingOp MdAsyncSavingOp;
typedef struct MdAsyncSavingOpClass MdAsyncSavingOpClass;
typedef struct MdAsyncSavingOpPrivate MdAsyncSavingOpPrivate;

#define MD_FILE_ERROR (_md_file_error_quark ())

typedef enum {
    MD_FILE_ERROR_FAILED,
    MD_FILE_ERROR_NOT_IMPLEMENTED
} MdFileError;

struct MdAsyncLoadingOp {
    MdAsyncOp base;
    MdAsyncLoadingOpPrivate *priv;
};

struct MdAsyncLoadingOpClass {
    MdAsyncOpClass base_class;
};

struct MdAsyncSavingOp {
    MdAsyncOp base;
    MdAsyncSavingOpPrivate *priv;
};

struct MdAsyncSavingOpClass {
    MdAsyncOpClass base_class;
};


GType       _md_async_loading_op_get_type       (void) G_GNUC_CONST;
GType       _md_async_saving_op_get_type        (void) G_GNUC_CONST;
GQuark      _md_file_error_quark                (void) G_GNUC_CONST;

MdAsyncOp  *_md_async_loading_op_new            (MdDocument         *doc,
                                                 MdFileInfo         *file,
                                                 GError            **error);
const char *_md_async_loading_op_get_tmp_file   (MdAsyncLoadingOp   *op);
MdFileInfo *_md_async_loading_op_get_file_info  (MdAsyncLoadingOp   *op);

MdAsyncOp  *_md_async_saving_op_new             (MdDocument         *doc,
                                                 MdFileInfo         *file,
                                                 const char         *tmp_file,
                                                 GError            **error);


#endif /* MD_FILE_OP_H */
