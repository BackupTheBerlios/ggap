/*
 *   gapdoc.h
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

#ifndef __GAP_DOC_H__
#define __GAP_DOC_H__

#include <gtk/gtktreestore.h>

G_BEGIN_DECLS


#define GAP_TYPE_DOC                (gap_doc_get_type ())
#define GAP_DOC(object)             (G_TYPE_CHECK_INSTANCE_CAST ((object), GAP_TYPE_DOC, GapDoc))
#define GAP_DOC_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_DOC, GapDocClass))
#define GAP_IS_DOC(object)          (G_TYPE_CHECK_INSTANCE_TYPE ((object), GAP_TYPE_DOC))
#define GAP_IS_DOC_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_DOC))
#define GAP_DOC_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_DOC, GapDocClass))


typedef struct _GapDoc          GapDoc;
typedef struct _GapDocPrivate   GapDocPrivate;
typedef struct _GapDocClass     GapDocClass;

struct _GapDoc
{
    GObject base;

    GtkTreeStore *index_store;
};

struct _GapDocClass
{
    GObjectClass base_class;
};


GType   gap_doc_get_type    (void) G_GNUC_CONST;

GapDoc *gap_doc_instance    (void);


G_END_DECLS

#endif /* __GAP_DOC_H__ */
