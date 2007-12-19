#ifndef MD_VIEW_H
#define MD_VIEW_H

#include <mooui/mddocument.h>


#define MD_TYPE_VIEW            (md_view_get_type ())
#define MD_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_VIEW, MdView))
#define MD_IS_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_VIEW))
#define MD_VIEW_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), MD_TYPE_VIEW, MdViewIface))

typedef struct MdViewIface MdViewIface;

struct MdViewIface {
    GTypeInterface g_iface;

    void (*set_document)    (MdView     *view,
                             MdDocument *doc);

    void (*attach_async_op) (MdView     *view,
                             MdAsyncOp  *op);
    void (*detach_async_op) (MdView     *view,
                             MdAsyncOp  *op);
};


GType               md_view_get_type                (void) G_GNUC_CONST;

MdViewIface        *md_view_interface_peek_parent   (MdViewIface *iface);

MdDocument         *md_view_get_doc                 (MdView     *view);
MdWindow           *md_view_get_window              (MdView     *view);
MdManager          *md_view_get_manager             (MdView     *view);

void                md_view_set_doc                 (MdView     *view,
                                                     MdDocument *doc);


#endif /* MD_VIEW_H */
