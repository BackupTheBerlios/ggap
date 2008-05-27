/*
 *   mdasyncop.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MD_ASYNC_OP_H
#define MD_ASYNC_OP_H

#include <mooui/mdtypes.h>


#define MD_ASYNC_OP(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_ASYNC_OP, MdAsyncOp))
#define MD_ASYNC_OP_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), MD_TYPE_ASYNC_OP, MdAsyncOpClass))
#define MD_IS_ASYNC_OP(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_ASYNC_OP))
#define MD_IS_ASYNC_OP_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), MD_TYPE_ASYNC_OP))
#define MD_ASYNC_OP_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MD_TYPE_ASYNC_OP, MdAsyncOpClass))

typedef struct MdAsyncOpClass MdAsyncOpClass;
typedef struct MdAsyncOpPrivate MdAsyncOpPrivate;

struct MdAsyncOp {
    GObject base;
    MdAsyncOpPrivate *priv;
};

struct MdAsyncOpClass {
    GObjectClass base_class;

    void     (*start)    (MdAsyncOp  *op);
    void     (*abort)    (MdAsyncOp  *op);

    gboolean (*cancel)   (MdAsyncOp  *op);

    void     (*progress) (MdAsyncOp  *op,
                          const char *text,
                          double      fraction);
    void     (*finished) (MdAsyncOp  *op);
};


void        md_async_op_start           (MdAsyncOp          *op);
void        md_async_op_abort           (MdAsyncOp          *op);
void        md_async_op_cancel          (MdAsyncOp          *op);

void        md_async_op_progress        (MdAsyncOp          *op,
                                         const char         *text,
                                         double              fraction);
void        md_async_op_finish          (MdAsyncOp          *op);

gboolean    md_async_op_started         (MdAsyncOp          *op);
gboolean    md_async_op_finished        (MdAsyncOp          *op);


#endif /* MD_ASYNC_OP_H */
