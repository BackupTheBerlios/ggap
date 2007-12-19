#include "mdview-private.h"
#include "mddocument-private.h"
#include "mdmanager-private.h"
#include "mdasyncop.h"
#include "marshals.h"
#include <mooutils/mooutils-misc.h>
#include <gtk/gtk.h>
#include <math.h>


typedef struct {
    MdManager *mgr;
    MdWindow *window;

    MdDocument *doc;
    MdAsyncOp *async_op;

    GtkWidget *label;
    GtkWidget *icon;
    GtkWidget *icon_evbox;
} MdViewData;


static GQuark md_view_data_quark;

static MdViewData   *md_view_get_data           (MdView     *view);

static void          md_view_set_document_real  (MdView     *view,
                                                 MdDocument *doc);

static void          md_view_detach_tab_label   (MdView     *view);
static void          md_view_update_tab_label   (MdView     *view);

static void          md_view_async_op_start     (MdView     *view,
                                                 MdAsyncOp  *op);
static void          md_view_async_op_end       (MdView     *view,
                                                 MdAsyncOp  *op);
static void          md_view_detach_async_op    (MdView     *view);


static void
md_view_base_init (G_GNUC_UNUSED MdViewIface *iface)
{
}

static void
md_view_class_init (MdViewIface *iface)
{
    md_view_data_quark = g_quark_from_static_string ("md-view-data");

    iface->set_document = md_view_set_document_real;

    g_object_interface_install_property (iface,
        g_param_spec_object ("md-document", "md-document", "md-document",
                             MD_TYPE_DOCUMENT, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

    g_signal_new ("set-document",
                  MD_TYPE_VIEW,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdViewIface, set_document),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  MD_TYPE_DOCUMENT);

    g_signal_new ("attach-async-op",
                  MD_TYPE_VIEW,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdViewIface, attach_async_op),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  MD_TYPE_ASYNC_OP);

    g_signal_new ("detach-async-op",
                  MD_TYPE_VIEW,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdViewIface, detach_async_op),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  MD_TYPE_ASYNC_OP);
}

GType
md_view_get_type (void)
{
    static GType type;

    if (G_UNLIKELY (!type))
    {
        GTypeInfo type_info = {
            sizeof (MdViewIface), /* class_size */
            (GBaseInitFunc) md_view_base_init, /* base_init */
            NULL, /* base_finalize */
            (GClassInitFunc) md_view_class_init, /* class_init */
            NULL
        };

        type = g_type_register_static (G_TYPE_INTERFACE, "MdView",
                                       &type_info, 0);

        g_type_interface_add_prerequisite (type, GTK_TYPE_WIDGET);
    }

    return type;
}

MdViewIface *
md_view_interface_peek_parent (MdViewIface *iface)
{
    MdViewIface *piface;

    g_return_val_if_fail (iface != NULL, NULL);

    piface = g_type_interface_peek_parent (iface);
    if (!piface)
        piface = g_type_default_interface_peek (MD_TYPE_VIEW);

    return piface;
}


static void
md_view_data_free (MdViewData *data)
{
    if (data)
    {
        g_free (data);
    }
}

static void
md_view_check_iface (MdView *view)
{
    MdViewIface *iface = MD_VIEW_GET_IFACE (view);

    if (!iface->set_document)
        g_critical ("type '%s' does not provide MdViewIface:set_document() method",
                    g_type_name (G_OBJECT_TYPE (view)));
}

static MdViewData *
md_view_get_data (MdView *view)
{
    MdViewData *data;

    data = g_object_get_qdata (G_OBJECT (view), md_view_data_quark);

    if (!data)
    {
        md_view_check_iface (view);

        data = g_new0 (MdViewData, 1);
        g_object_set_qdata_full (G_OBJECT (view), md_view_data_quark, data,
                                 (GDestroyNotify) md_view_data_free);

        data->mgr = NULL;
        data->window = NULL;
        data->doc = NULL;
    }

    return data;
}


void
_md_view_set_window (MdView   *view,
                     MdWindow *window)
{
    MdViewData *data;

    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (!window || MD_IS_WINDOW (window));

    data = md_view_get_data (view);

    if (data->window != window)
    {
        md_view_detach_tab_label (view);
        data->window = window;
    }
}

MdWindow *
md_view_get_window (MdView *view)
{
    MdViewData *data;

    g_return_val_if_fail (MD_IS_VIEW (view), NULL);

    data = md_view_get_data (view);
    return data->window;
}


void
_md_view_set_manager (MdView    *view,
                      MdManager *mgr)
{
    MdViewData *data;

    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (MD_IS_MANAGER (mgr));

    data = md_view_get_data (view);
    g_return_if_fail (data != NULL && data->mgr == NULL);

    data->mgr = mgr;
}

MdManager *
md_view_get_manager (MdView *view)
{
    MdViewData *data;

    g_return_val_if_fail (MD_IS_VIEW (view), NULL);

    data = md_view_get_data (view);
    g_return_val_if_fail (data != NULL, NULL);
    g_return_val_if_fail (data->mgr != NULL, NULL);

    return data->mgr;
}


void
md_view_set_doc (MdView     *view,
                 MdDocument *doc)
{
    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (!doc || MD_IS_DOCUMENT (doc));
    g_signal_emit_by_name (view, "set-document", doc);
}

MdDocument *
md_view_get_doc (MdView *view)
{
    MdViewData *data;

    g_return_val_if_fail (MD_IS_VIEW (view), NULL);

    data = md_view_get_data (view);
    g_return_val_if_fail (data != NULL, NULL);
    g_return_val_if_fail (data->doc != NULL, NULL);

    return data->doc;
}


static void
doc_status_notify (MdView *view)
{
    md_view_update_tab_label (view);
}

static void
doc_file_info_notify (MdView *view)
{
    md_view_update_tab_label (view);
}

static void
md_view_set_document_real (MdView     *view,
                           MdDocument *doc)
{
    MdViewData *data;

    data = md_view_get_data (view);

    if (data->doc == doc)
        return;

    md_view_detach_async_op (view);

    if (data->doc)
    {
        g_signal_handlers_disconnect_by_func (data->doc,
                                              (gpointer) doc_status_notify,
                                              view);
        g_signal_handlers_disconnect_by_func (data->doc,
                                              (gpointer) doc_file_info_notify,
                                              view);
        g_signal_handlers_disconnect_by_func (data->doc,
                                              (gpointer) md_view_async_op_start,
                                              view);
        g_signal_handlers_disconnect_by_func (data->doc,
                                              (gpointer) md_view_async_op_end,
                                              view);
    }

    data->doc = doc;

    if (data->doc)
    {
        g_signal_connect_swapped (data->doc, "notify::md-doc-status",
                                  G_CALLBACK (doc_status_notify), view);
        g_signal_connect_swapped (data->doc, "notify::md-doc-file-info",
                                  G_CALLBACK (doc_file_info_notify), view);
        g_signal_connect_swapped (data->doc, "async-op-start",
                                  G_CALLBACK (md_view_async_op_start), view);
        g_signal_connect_swapped (data->doc, "async-op-end",
                                  G_CALLBACK (md_view_async_op_end), view);
    }

    md_view_update_tab_label (view);
    g_object_notify (G_OBJECT (view), "md-document");
}


/***************************************************************************/
/* blocking operations
 */

static void
md_view_async_op_start (MdView    *view,
                        MdAsyncOp *op)
{
    MdViewData *data;

    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (MD_IS_ASYNC_OP (op));

    data = md_view_get_data (view);

    if (data->async_op)
    {
        g_critical ("%s: another operation not finished yet",
                    G_STRFUNC);
        md_view_detach_async_op (view);
        return;
    }

    data->async_op = op;
    g_signal_emit_by_name (view, "attach-async-op", op);
}

static void
md_view_async_op_end (MdView    *view,
                      MdAsyncOp *op)
{
    MdViewData *data;

    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (MD_IS_ASYNC_OP (op));

    data = md_view_get_data (view);

    if (data->async_op != op)
    {
        g_critical ("%s: oops", G_STRFUNC);
        md_view_detach_async_op (view);
        return;
    }

    g_signal_emit_by_name (view, "detach-async-op", op);
    data->async_op = NULL;
}

static void
md_view_detach_async_op (MdView *view)
{
    MdViewData *data;

    data = md_view_get_data (view);

    if (data->async_op)
    {
        g_print ("%s\n", G_STRFUNC);
        g_signal_emit_by_name (view, "detach-async-op", data->async_op);
        data->async_op = NULL;
    }
}


/***************************************************************************/
/* Tab label
 */

static void
update_evbox_shape (GtkWidget *image,
                    GtkWidget *evbox)
{
    GtkMisc *misc;
    GdkPixbuf *pixbuf;
    GdkBitmap *mask;
    int width, height;
    int x, y;

    g_return_if_fail (GTK_IS_EVENT_BOX (evbox));
    g_return_if_fail (GTK_IS_IMAGE (image));

    if (!GTK_WIDGET_REALIZED (image) || !(pixbuf = gtk_image_get_pixbuf (GTK_IMAGE (image))))
        return;

    width = gdk_pixbuf_get_width (pixbuf);
    height = gdk_pixbuf_get_height (pixbuf);
    g_return_if_fail (width < 2000 && height < 2000);

    gdk_pixbuf_render_pixmap_and_mask (pixbuf, NULL, &mask, 1);
    g_return_if_fail (mask != NULL);

    misc = GTK_MISC (image);
    x = floor (image->allocation.x + misc->xpad
             + ((image->allocation.width - image->requisition.width) * misc->xalign));
    y = floor (image->allocation.y + misc->ypad
             + ((image->allocation.height - image->requisition.height) * misc->yalign));

    gtk_widget_shape_combine_mask (evbox, NULL, 0, 0);
    gtk_widget_shape_combine_mask (evbox, mask, x, y);

    g_object_unref (mask);
}

static void
icon_size_allocate (GtkWidget     *image,
                    GtkAllocation *allocation,
                    GtkWidget     *evbox)
{
    GtkAllocation *old_allocation;

    old_allocation = g_object_get_data (G_OBJECT (image), "md-view-icon-allocation");

    if (!old_allocation ||
        old_allocation->x != allocation->x ||
        old_allocation->y != allocation->y ||
        old_allocation->width != allocation->width ||
        old_allocation->height != allocation->height)
    {
        GtkAllocation *copy = g_memdup (allocation, sizeof *allocation);
        g_object_set_data_full (G_OBJECT (image), "md-view-icon-allocation", copy, g_free);
        update_evbox_shape (image, evbox);
    }
}

GtkWidget *
_md_view_create_tab_label (MdView     *view,
                           GtkWidget **icon_evbox)
{
    MdViewData *data;
    GtkWidget *hbox, *icon, *label, *evbox;
    GtkSizeGroup *group;

    g_return_val_if_fail (MD_IS_VIEW (view), NULL);
    g_return_val_if_fail (icon_evbox != NULL, NULL);

    data = md_view_get_data (view);
    g_return_val_if_fail (data != NULL, NULL);
    g_return_val_if_fail (data->label == NULL, NULL);

    group = gtk_size_group_new (GTK_SIZE_GROUP_VERTICAL);

    hbox = gtk_hbox_new (FALSE, 3);
    gtk_widget_show (hbox);

    *icon_evbox = evbox = gtk_event_box_new ();
    gtk_box_pack_start (GTK_BOX (hbox), evbox, FALSE, FALSE, 0);

    icon = gtk_image_new ();
    gtk_container_add (GTK_CONTAINER (evbox), icon);
    gtk_widget_show_all (evbox);

    g_signal_connect (icon, "realize", G_CALLBACK (update_evbox_shape), evbox);
    g_signal_connect (icon, "size-allocate", G_CALLBACK (icon_size_allocate), evbox);

    label = gtk_label_new (NULL);
    gtk_label_set_single_line_mode (GTK_LABEL (label), TRUE);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

    gtk_size_group_add_widget (group, evbox);
    gtk_size_group_add_widget (group, label);

    data->label = label;
    data->icon = icon;
    data->icon_evbox = evbox;

    md_view_update_tab_label (view);

    g_object_unref (group);

    return hbox;
}

static void
md_view_detach_tab_label (MdView *view)
{
    MdViewData *data;

    data = md_view_get_data (view);

    data->label = NULL;
    data->icon = NULL;
    data->icon_evbox = NULL;
}


static void
set_tab_icon (GtkWidget *image,
              GtkWidget *evbox,
              GdkPixbuf *pixbuf)
{
    GdkPixbuf *old_pixbuf;

    /* file icons are cached, so it's likely the same pixbuf
     * object as before (and it happens every time you switch tabs) */
    old_pixbuf = gtk_image_get_pixbuf (GTK_IMAGE (image));

    if (old_pixbuf != pixbuf)
    {
        gtk_image_set_from_pixbuf (GTK_IMAGE (image), pixbuf);

        if (GTK_WIDGET_REALIZED (evbox))
            update_evbox_shape (image, evbox);
    }
}

static void
md_view_update_tab_label (MdView *view)
{
    MdViewData *data;
    MdDocumentStatus status;
    char *label_text;
    gboolean modified, deleted;
    GdkPixbuf *pixbuf;

    data = md_view_get_data (view);

    if (!data->label || !data->doc)
        return;

    status = md_document_get_status (data->doc);

    deleted = (status & (MD_DOCUMENT_DELETED | MD_DOCUMENT_MODIFIED_ON_DISK)) != 0;
    modified = (status & MD_DOCUMENT_MODIFIED) != 0;

    label_text = g_strdup_printf ("%s%s%s",
                                  deleted ? "!" : "",
                                  modified ? "*" : "",
                                  md_document_get_display_basename (data->doc));
    gtk_label_set_text (GTK_LABEL (data->label), label_text);

    pixbuf = _md_document_get_icon (data->doc, GTK_WIDGET (view), GTK_ICON_SIZE_MENU);
    set_tab_icon (data->icon, data->icon_evbox, pixbuf);

    if (pixbuf)
        g_object_unref (pixbuf);
    g_free (label_text);
}
