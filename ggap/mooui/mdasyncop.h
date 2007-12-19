#ifndef MD_ASYNC_OP_H
#define MD_ASYNC_OP_H

#include <mooui/mdtypes.h>


#define MD_TYPE_ASYNC_OP             (md_async_op_get_type ())
#define MD_ASYNC_OP(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_ASYNC_OP, MdAsyncOp))
#define MD_ASYNC_OP_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_ASYNC_OP, MdAsyncOpClass))
#define MD_IS_ASYNC_OP(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_ASYNC_OP))
#define MD_IS_ASYNC_OP_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_ASYNC_OP))
#define MD_ASYNC_OP_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_ASYNC_OP, MdAsyncOpClass))

typedef struct MdAsyncOpClass MdAsyncOpClass;
typedef struct MdAsyncOpPrivate MdAsyncOpPrivate;

typedef enum
{
    MD_ASYNC_OP_ERROR,
    MD_ASYNC_OP_SUCCESS,
    MD_ASYNC_OP_CANCELLED
} MdAsyncOpResult;

struct MdAsyncOp {
    GObject base;
    MdAsyncOpPrivate *priv;
};

struct MdAsyncOpClass {
    GObjectClass base_class;

    void     (*start)    (MdAsyncOp         *op);
    void     (*abort)    (MdAsyncOp         *op);

    gboolean (*cancel)   (MdAsyncOp         *op);

    void     (*progress) (MdAsyncOp         *op,
                          const char        *title,
                          const char        *text,
                          double             fraction);
    void     (*finished) (MdAsyncOp         *op,
                          MdAsyncOpResult    result,
                          const char        *error);
};


GType       md_async_op_get_type        (void) G_GNUC_CONST;

void        md_async_op_start           (MdAsyncOp          *op);
void        md_async_op_abort           (MdAsyncOp          *op,
                                         gboolean            interactive);

void        md_async_op_finish          (MdAsyncOp          *op,
                                         MdAsyncOpResult     result,
                                         const char         *error);

gboolean    md_async_op_started         (MdAsyncOp          *op);
gboolean    md_async_op_finished        (MdAsyncOp          *op);

MdDocument *md_async_op_get_doc         (MdAsyncOp          *op);


#endif /* MD_ASYNC_OP_H */
