#include "mddocument-private.h"
#include "mdmanager-private.h"
#include "mdview-private.h"
#include "mdfileop.h"
#include "marshals.h"
#include "mdenums.h"
#include <mooutils/mooutils-misc.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <glib/gstdio.h>


typedef struct {
    MdManager *mgr;
    MdDocumentCapabilities caps;
    MdDocumentStatus status;

    MdFileInfo *file_info;
    char *filename;
    char *display_name;
    char *display_basename;

    GSList *views;
    MdAsyncOp *async_op;
} MdDocumentData;

static GQuark md_document_data_quark;

static MdDocumentData   *md_document_get_data           (MdDocument     *doc);

static MdFileOpStatus    md_document_load_file_default  (MdDocument     *doc,
                                                         MdFileInfo     *file,
                                                         GError        **error);
static MdFileOpStatus    md_document_save_file_default  (MdDocument     *doc,
                                                         MdFileInfo     *file,
                                                         GError        **error);
static void              md_document_close_default      (MdDocument     *doc);


static void
md_has_undo_class_init (MdHasUndoIface *iface)
{
    g_object_interface_install_property (iface,
        g_param_spec_boolean ("md-can-undo", "md-can-undo", "md-can-undo",
                              FALSE, G_PARAM_READABLE));

    g_object_interface_install_property (iface,
        g_param_spec_boolean ("md-can-redo", "md-can-redo", "md-can-redo",
                              FALSE, G_PARAM_READABLE));
}

GType
md_has_undo_get_type (void)
{
    static GType type;

    if (G_UNLIKELY (!type))
    {
        GTypeInfo type_info = {
            sizeof (MdHasUndoIface), /* class_size */
            NULL, /* base_init */
            NULL, /* base_finalize */
            (GClassInitFunc) md_has_undo_class_init, /* class_init */
            NULL
        };

        type = g_type_register_static (G_TYPE_INTERFACE, "MdHasUndo",
                                       &type_info, 0);

        g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    }

    return type;
}

void
_md_document_undo (MdDocument *doc)
{
    g_return_if_fail (MD_IS_HAS_UNDO (doc));
    g_return_if_fail (MD_HAS_UNDO_GET_IFACE (doc)->undo != NULL);
    MD_HAS_UNDO_GET_IFACE (doc)->undo (MD_HAS_UNDO (doc));
}

void
_md_document_redo (MdDocument *doc)
{
    g_return_if_fail (MD_IS_HAS_UNDO (doc));
    g_return_if_fail (MD_HAS_UNDO_GET_IFACE (doc)->redo != NULL);
    MD_HAS_UNDO_GET_IFACE (doc)->redo (MD_HAS_UNDO (doc));
}

gboolean
_md_document_can_undo (MdDocument *doc)
{
    gboolean value = FALSE;
    g_object_get (doc, "md-can-undo", value, NULL);
    return value;
}

gboolean
_md_document_can_redo (MdDocument *doc)
{
    gboolean value = FALSE;
    g_object_get (doc, "md-can-redo", value, NULL);
    return value;
}


static void
md_document_base_init (MdDocumentIface *iface)
{
    iface->load_file = md_document_load_file_default;
    iface->save_file = md_document_save_file_default;
    /* so implementors can safely chain up */
    iface->close = md_document_close_default;
}

static void
md_document_class_init (MdDocumentIface *iface)
{
    md_document_data_quark = g_quark_from_static_string ("md-document-data");

    g_object_interface_install_property (iface,
        g_param_spec_flags ("md-doc-status", "md-doc-status", "md-doc-status",
                            MD_TYPE_DOCUMENT_STATUS, 0, G_PARAM_READABLE));

    g_object_interface_install_property (iface,
        g_param_spec_boxed ("md-doc-file-info", "md-doc-file-info", "md-doc-file-info",
                            MD_TYPE_FILE_INFO, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_signal_new ("async-op-start",
                  MD_TYPE_DOCUMENT,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdDocumentIface, async_op_start),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  MD_TYPE_ASYNC_OP);

    g_signal_new ("async-op-end",
                  MD_TYPE_DOCUMENT,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdDocumentIface, async_op_end),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  MD_TYPE_ASYNC_OP);

    g_signal_new ("load-before",
                  MD_TYPE_DOCUMENT,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdDocumentIface, load_before),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__BOXED,
                  G_TYPE_NONE, 1,
                  MD_TYPE_FILE_INFO | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("load-after",
                  MD_TYPE_DOCUMENT,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdDocumentIface, load_after),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__BOXED,
                  G_TYPE_NONE, 1,
                  MD_TYPE_FILE_INFO | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("save-before",
                  MD_TYPE_DOCUMENT,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdDocumentIface, save_before),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__BOXED,
                  G_TYPE_NONE, 1,
                  MD_TYPE_FILE_INFO | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("save-after",
                  MD_TYPE_DOCUMENT,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdDocumentIface, save_after),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__BOXED,
                  G_TYPE_NONE, 1,
                  MD_TYPE_FILE_INFO | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("close",
                  MD_TYPE_DOCUMENT,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdDocumentIface, close),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}

GType
md_document_get_type (void)
{
    static GType type;

    if (G_UNLIKELY (!type))
    {
        GTypeInfo type_info = {
            sizeof (MdDocumentIface), /* class_size */
            (GBaseInitFunc) md_document_base_init, /* base_init */
            NULL, /* base_finalize */
            (GClassInitFunc) md_document_class_init, /* class_init */
            NULL
        };

        type = g_type_register_static (G_TYPE_INTERFACE, "MdDocument",
                                       &type_info, 0);

        g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    }

    return type;
}

static void
md_document_data_free (MdDocumentData *data)
{
    if (data)
    {
        if (data->views)
        {
            g_critical ("%s: not all views detached", G_STRFUNC);
            g_slist_free (data->views);
        }

        if (data->async_op)
        {
            g_critical ("%s: async operation still running", G_STRFUNC);
            g_object_unref (data->async_op);
        }

        md_file_info_free (data->file_info);
        g_free (data->filename);
        g_free (data->display_name);
        g_free (data->display_basename);
        g_free (data);
    }
}

MdDocumentIface *
md_document_interface_peek_parent (MdDocumentIface *iface)
{
    MdDocumentIface *piface;

    g_return_val_if_fail (iface != NULL, NULL);

    piface = g_type_interface_peek_parent (iface);
    if (!piface)
        piface = g_type_default_interface_peek (MD_TYPE_DOCUMENT);

    return piface;
}

static void
md_document_check_iface (MdDocument *doc)
{
    MdDocumentIface *iface = MD_DOCUMENT_GET_IFACE (doc);

    if (!iface->get_icon)
        g_critical ("type '%s' does not provide MdDocumentIface:get_icon() method",
                    g_type_name (G_OBJECT_TYPE (doc)));

    if (iface->load_file == md_document_load_file_default &&
        !iface->load_local && !iface->load_content)
            g_critical ("type '%s' does not provide methods for loading",
                        g_type_name (G_OBJECT_TYPE (doc)));

    if (MD_IS_HAS_UNDO (doc))
    {
        MdHasUndoIface *undo_iface = MD_HAS_UNDO_GET_IFACE (doc);
        if (!undo_iface->undo)
            g_critical ("type '%s' does not provide undo() method",
                        g_type_name (G_OBJECT_TYPE (doc)));
        if (!undo_iface->redo)
            g_critical ("type '%s' does not provide redo() method",
                        g_type_name (G_OBJECT_TYPE (doc)));
    }
}


static MdDocumentData *
md_document_get_data (MdDocument *doc)
{
    MdDocumentData *data;

    data = g_object_get_qdata (G_OBJECT (doc), md_document_data_quark);

    if (!data)
    {
        md_document_check_iface (doc);

        data = g_new0 (MdDocumentData, 1);
        g_object_set_qdata_full (G_OBJECT (doc), md_document_data_quark, data,
                                 (GDestroyNotify) md_document_data_free);

        data->caps = 0;
        data->mgr = NULL;
        data->status = 0;
        data->file_info = NULL;
        data->filename = NULL;
        data->display_name = NULL;
        data->display_basename = NULL;
        data->views = NULL;
    }

    return data;
}

static void
md_document_close_default (MdDocument *doc)
{
    g_object_set_qdata (G_OBJECT (doc), md_document_data_quark, NULL);
}


void
md_document_set_capabilities (MdDocument             *doc,
                              MdDocumentCapabilities  caps)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);
    data->caps = caps;

    if (caps & MD_DOCUMENT_SUPPORTS_SAVE)
    {
        MdDocumentIface *iface = MD_DOCUMENT_GET_IFACE (doc);

        if (iface->save_file == md_document_save_file_default &&
            !iface->save_local && !iface->get_content)
                g_critical ("type '%s' does not provide methods for saving",
                            g_type_name (G_OBJECT_TYPE (doc)));
    }
}


void
md_document_set_modified (MdDocument *doc,
                          gboolean     modified)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);

    if (!(data->status & MD_DOCUMENT_MODIFIED) != !modified)
    {
        if (modified)
            data->status |= MD_DOCUMENT_MODIFIED;
        else
            data->status &= ~MD_DOCUMENT_MODIFIED;

        g_object_notify (G_OBJECT (doc), "md-doc-status");
    }
}

gboolean
md_document_get_modified (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), FALSE);

    data = md_document_get_data (doc);
    return (data->status & MD_DOCUMENT_MODIFIED) != 0;
}

void
md_document_set_status (MdDocument       *doc,
                        MdDocumentStatus  status)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);

    if (data->status != status)
    {
        data->status = status;
        g_object_notify (G_OBJECT (doc), "md-doc-status");
    }
}

MdDocumentStatus
md_document_get_status (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), 0);

    data = md_document_get_data (doc);
    return data->status;
}

gboolean
_md_document_need_save (MdDocument *doc)
{
    g_return_val_if_fail (MD_IS_DOCUMENT (doc), FALSE);
    return (md_document_get_status (doc) & MD_DOCUMENT_MODIFIED) != 0;
}

gboolean
_md_document_is_empty (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), FALSE);

    data = md_document_get_data (doc);

    if ((data->status & MD_DOCUMENT_MODIFIED) || data->file_info)
        return FALSE;

    if (MD_DOCUMENT_GET_IFACE (doc)->get_empty != NULL)
        return MD_DOCUMENT_GET_IFACE (doc)->get_empty (doc);
    else
        return FALSE;
}


void
_md_document_set_manager (MdDocument *doc,
                          MdManager  *mgr)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (MD_IS_MANAGER (mgr));

    data = md_document_get_data (doc);
    g_return_if_fail (data != NULL && data->mgr == NULL);

    data->mgr = mgr;
}

MdManager *
md_document_get_manager (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);
    g_return_val_if_fail (data != NULL, NULL);
    g_return_val_if_fail (data->mgr != NULL, NULL);

    return data->mgr;
}


void
_md_document_close (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);

    if (data->views)
        g_critical ("%s: closing doc with views still attached", G_STRFUNC);

    if (data->async_op)
    {
        g_critical ("%s: closing doc with async operation running", G_STRFUNC);
        md_async_op_abort (data->async_op, FALSE);
    }

    g_signal_emit_by_name (doc, "close");
}


void
_md_document_add_view (MdDocument *doc,
                       MdView     *view)
{
    MdDocumentData *data;
    MdDocument *old_doc;

    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (MD_IS_VIEW (view));

    old_doc = md_view_get_doc (view);
    g_return_if_fail (!old_doc || old_doc == doc);

    data = md_document_get_data (doc);
    g_return_if_fail (g_slist_find (data->views, view) == NULL);

    data->views = g_slist_append (data->views, view);
    md_view_set_doc (view, doc);
}

void
_md_document_remove_view (MdDocument *doc,
                          MdView     *view)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (md_view_get_doc (view) == doc);

    data = md_document_get_data (doc);
    g_return_if_fail (g_slist_find (data->views, view) != NULL);

    data->views = g_slist_remove (data->views, view);
    md_view_set_doc (view, NULL);
}

GSList *
md_document_get_views (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);
    return g_slist_copy (data->views);
}

MdView *
md_document_get_view (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);
    return data->views ? data->views->data : NULL;
}


void
md_document_set_file_info (MdDocument *doc,
                           MdFileInfo *file_info)
{
    MdDocumentData *data;
    MdFileInfo *tmp;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);
    g_return_if_fail (data != NULL);

    if (file_info == data->file_info)
        return;

    if (!data->file_info && data->display_name)
        _md_manager_remove_untitled (data->mgr, doc);

    tmp = data->file_info;
    data->file_info = file_info ? md_file_info_copy (file_info) : NULL;
    md_file_info_free (tmp);

    g_free (data->filename);
    data->filename = file_info ? md_file_info_get_filename (data->file_info) : NULL;

    g_free (data->display_name);
    g_free (data->display_basename);
    data->display_name = NULL;
    data->display_basename = NULL;

    g_object_notify (G_OBJECT (doc), "md-doc-file-info");
}

MdFileInfo *
md_document_get_file_info (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);
    return data->file_info;
}

gboolean
_md_document_is_untitled (MdDocument *doc)
{
    return md_document_get_file_info (doc) != NULL;
}

const char *
md_document_get_uri (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);
    return data->file_info ? md_file_info_get_uri (data->file_info) : NULL;
}

const char *
md_document_get_filename (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);
    return data->filename;
}

static char *
get_uri_basename (const char *uri)
{
    const char *slash;

    slash = strrchr (uri, '/');
    if (slash && slash[1])
        return g_strdup (slash + 1);
    else
        return g_strdup (uri);
}

static void
calc_display_name (MdDocument     *doc,
                   MdDocumentData *data)
{
    if (data->filename)
    {
        data->display_name = g_filename_display_name (data->filename);
        data->display_basename = g_filename_display_basename (data->filename);
    }
    else if (data->file_info)
    {
        const char *uri = md_file_info_get_uri (data->file_info);
        data->display_name = g_strdup (uri);
        data->display_basename = get_uri_basename (uri);
    }
    else
    {
        data->display_name = _md_manager_add_untitled (data->mgr, doc);

        if (!data->display_name)
            data->display_name = g_strdup ("Document");

        data->display_basename = g_strdup (data->display_name);
    }
}

const char *
md_document_get_display_name (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);

    if (!data->display_name)
        calc_display_name (doc, data);

    return data->display_name;
}

const char *
md_document_get_display_basename (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);

    if (!data->display_basename)
        calc_display_name (doc, data);

    return data->display_basename;
}


GdkPixbuf *
_md_document_get_icon (MdDocument  *doc,
                       GtkWidget   *widget,
                       GtkIconSize  size)
{
    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);
    g_return_val_if_fail (MD_DOCUMENT_GET_IFACE (doc)->get_icon != NULL, NULL);
    return MD_DOCUMENT_GET_IFACE (doc)->get_icon (doc, widget, size);
}


/*********************************************************************/
/* MdFileInfo
 */

struct MdFileInfo {
    char *uri;
    GData *data;
};

MdFileInfo *
md_file_info_new (const char *uri)
{
    MdFileInfo *file_info;

    g_return_val_if_fail (uri != NULL, NULL);

    file_info = moo_new (MdFileInfo);
    file_info->uri = g_strdup (uri);
    file_info->data = NULL;

    return file_info;
}

void
md_file_info_free (MdFileInfo *file_info)
{
    if (file_info)
    {
        g_free (file_info->uri);
        g_datalist_clear (&file_info->data);
        moo_free (MdFileInfo, file_info);
    }
}

static void
copy_data (GQuark       key,
           const char  *value,
           MdFileInfo  *dest)
{
    g_dataset_id_set_data_full (&dest->data, key, g_strdup (value), g_free);
}

MdFileInfo *
md_file_info_copy (MdFileInfo *file_info)
{
    MdFileInfo *copy;

    if (!file_info)
        return NULL;

    copy = md_file_info_new (file_info->uri);
    g_datalist_foreach (&file_info->data, (GDataForeachFunc) copy_data, copy);

    return copy;
}

void
md_file_info_set (MdFileInfo *file_info,
                  const char *key,
                  const char *value)
{
    g_return_if_fail (file_info != NULL);
    g_return_if_fail (key != NULL);

    if (value)
        g_dataset_set_data_full (&file_info->data, key, g_strdup (value), g_free);
    else
        g_dataset_remove_data (&file_info->data, key);
}

const char *
md_file_info_get (MdFileInfo *file_info,
                  const char *key)
{
    g_return_val_if_fail (file_info != NULL, NULL);
    g_return_val_if_fail (key != NULL, NULL);
    return g_dataset_get_data (&file_info->data, key);
}


const char *
md_file_info_get_uri (MdFileInfo *file_info)
{
    g_return_val_if_fail (file_info != NULL, NULL);
    return file_info->uri;
}

gboolean
md_file_info_is_local (MdFileInfo *file_info)
{
    g_return_val_if_fail (file_info != NULL, FALSE);

    if (strncmp (file_info->uri, "file://", 7) != 0)
        return FALSE;

    /* XXX */
    return TRUE;
}

char *
md_file_info_get_filename (MdFileInfo *file_info)
{
    g_return_val_if_fail (file_info != NULL, NULL);
    /* XXX */
    return g_filename_from_uri (file_info->uri, NULL, NULL);
}

char *
md_file_info_get_display_name (MdFileInfo *file_info)
{
    g_return_val_if_fail (file_info != NULL, NULL);
    /* XXX */
    return g_strdup (file_info->uri);
}


MdFileInfo **
md_file_info_array_new_uri_list (char **uris)
{
    guint n_elms, i;
    MdFileInfo **array;

    if (!uris || !uris[0])
        return NULL;

    n_elms = g_strv_length (uris);
    array = g_new (MdFileInfo*, n_elms + 1);
    for (i = 0; i < n_elms; ++i)
        array[i] = md_file_info_new (uris[i]);
    array[n_elms] = NULL;

    return array;
}

MdFileInfo **
md_file_info_array_copy (MdFileInfo **files)
{
    guint n_elms, i;
    MdFileInfo **copy;

    if (!files || !files[0])
        return NULL;

    for (n_elms = 0; files[n_elms] != NULL; n_elms++) ;

    copy = g_new (MdFileInfo*, n_elms + 1);
    for (i = 0; i < n_elms; ++i)
        copy[i] = md_file_info_copy (files[i]);
    copy[n_elms] = NULL;

    return copy;
}

void
md_file_info_array_free (MdFileInfo **files)
{
    MdFileInfo **p;

    for (p = files; p && *p; ++p)
        md_file_info_free (*p);

    g_free (files);
}

GType
md_file_info_get_type (void)
{
    static GType type;

    if (G_UNLIKELY (!type))
        type = g_boxed_type_register_static ("MdFileInfo",
                                             (GBoxedCopyFunc) md_file_info_copy,
                                             (GBoxedFreeFunc) md_file_info_free);

    return type;
}

GType
md_file_info_array_get_type (void)
{
    static GType type;

    if (G_UNLIKELY (!type))
        type = g_boxed_type_register_static ("MdFileInfoArray",
                                             (GBoxedCopyFunc) md_file_info_array_copy,
                                             (GBoxedFreeFunc) md_file_info_array_free);

    return type;
}


/*********************************************************************/
/* Loading/saving
 */

MdAsyncOp *
md_document_get_async_op (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);
    return data->async_op;
}

static void
async_op_finished (MdDocument      *doc,
                   MdAsyncOpResult  result,
                   const char      *error,
                   MdAsyncOp       *op)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (MD_IS_ASYNC_OP (op));
    g_return_if_fail (!error || result == MD_ASYNC_OP_ERROR);
    g_return_if_fail (result != MD_ASYNC_OP_ERROR || error != NULL);

    data = md_document_get_data (doc);
    g_return_if_fail (op == data->async_op);

    g_signal_emit_by_name (doc, "async-op-end", op);

    g_object_unref (data->async_op);
    data->async_op = NULL;
}

void
md_document_start_async_op (MdDocument *doc,
                            MdAsyncOp  *op)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (MD_IS_ASYNC_OP (op));
    g_return_if_fail (!md_async_op_finished (op));

    data = md_document_get_data (doc);
    g_return_if_fail (data->async_op == NULL);

    data->async_op = g_object_ref (op);

    if (!md_async_op_started (op))
        md_async_op_start (op);

    g_signal_connect_swapped (op, "finished",
                              G_CALLBACK (async_op_finished), doc);
    g_signal_emit_by_name (doc, "async-op-start");
}


MdFileOpStatus
_md_document_load_file (MdDocument  *doc,
                        MdFileInfo  *file,
                        GError     **error)
{
    MdFileOpStatus status;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), MD_FILE_OP_STATUS_ERROR);
    g_return_val_if_fail (file != NULL, MD_FILE_OP_STATUS_ERROR);
    g_return_val_if_fail (!error || !*error, MD_FILE_OP_STATUS_ERROR);

    status = MD_DOCUMENT_GET_IFACE (doc)->load_file (doc, file, error);

    if (status == MD_FILE_OP_STATUS_IN_PROGRESS)
    {
        MdAsyncOp *op = md_document_get_async_op (doc);

        if (!op || !MD_IS_ASYNC_LOADING_OP (op))
        {
            g_critical ("%s: load_file() returned MD_FILE_OP_STATUS_IN_PROGRESS, "
                        "but loading operation is not running", G_STRFUNC);
            g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
                         "Internal error");
            status = MD_FILE_OP_STATUS_ERROR;
        }
    }

    return status;
}

static gboolean
load_local_file (MdDocument  *doc,
                 MdFileInfo  *file,
                 const char  *filename,
                 GError     **error)
{
    GMappedFile *mfile;
    gboolean retval;

    if (MD_DOCUMENT_GET_IFACE (doc)->load_local)
        return MD_DOCUMENT_GET_IFACE (doc)->load_local (doc, file, filename, error);

    if (!(mfile = g_mapped_file_new (filename, FALSE, error)))
        return FALSE;

    retval = MD_DOCUMENT_GET_IFACE (doc)->load_content (doc, file,
                                                        g_mapped_file_get_contents (mfile),
                                                        g_mapped_file_get_length (mfile),
                                                        error);

    g_mapped_file_free (mfile);
    return retval;
}

static void
async_loading_finished (MdAsyncLoadingOp *op,
                        MdAsyncOpResult   result,
                        const char       *error_text)
{
    MdDocument *doc;
    MdFileInfo *file_info;
    const char *filename;
    GError *error = NULL;

    doc = md_async_op_get_doc (MD_ASYNC_OP (op));
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (md_document_get_async_op (doc) == MD_ASYNC_OP (op));

    if (result != MD_ASYNC_OP_SUCCESS)
    {
        md_async_op_finish (MD_ASYNC_OP (op), result, error_text);
        return;
    }

    filename = _md_async_loading_op_get_tmp_file (op);
    file_info = _md_async_loading_op_get_file_info (op);
    g_return_if_fail (filename && file_info);

    if (load_local_file (doc, file_info, filename, &error))
        md_async_op_finish (MD_ASYNC_OP (op), MD_ASYNC_OP_SUCCESS, NULL);
    else
        md_async_op_finish (MD_ASYNC_OP (op), MD_ASYNC_OP_ERROR,
                            error ? error->message : "Error");
}

static MdFileOpStatus
md_document_load_file_default (MdDocument  *doc,
                               MdFileInfo  *file,
                               GError     **error)
{
    g_return_val_if_fail (MD_DOCUMENT_GET_IFACE (doc)->load_content != NULL ||
                            MD_DOCUMENT_GET_IFACE (doc)->load_local != NULL,
                          MD_FILE_OP_STATUS_ERROR);

    if (md_file_info_is_local (file))
    {
        gboolean result;
        char *filename;

        filename = md_file_info_get_filename (file);
        g_return_val_if_fail (filename != NULL, MD_FILE_OP_STATUS_ERROR);

        result = load_local_file (doc, file, filename, error);

        g_free (filename);
        return result ? MD_FILE_OP_STATUS_SUCCESS : MD_FILE_OP_STATUS_ERROR;
    }
    else
    {
        MdAsyncOp *op;

        op = _md_async_loading_op_new (doc, file, error);

        if (!op)
            return MD_FILE_OP_STATUS_ERROR;

        g_signal_connect (op, "md-async-loading-finished",
                          G_CALLBACK (async_loading_finished),
                          NULL);
        md_document_start_async_op (doc, op);

        g_object_unref (op);
        return MD_FILE_OP_STATUS_IN_PROGRESS;
    }
}


MdFileOpStatus
_md_document_save_file (MdDocument  *doc,
                        MdFileInfo  *file,
                        GError     **error)
{
    MdFileOpStatus status;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), MD_FILE_OP_STATUS_ERROR);
    g_return_val_if_fail (file != NULL, MD_FILE_OP_STATUS_ERROR);
    g_return_val_if_fail (!error || !*error, MD_FILE_OP_STATUS_ERROR);

    status = MD_DOCUMENT_GET_IFACE (doc)->save_file (doc, file, error);

    if (status == MD_FILE_OP_STATUS_IN_PROGRESS)
    {
        MdAsyncOp *op = md_document_get_async_op (doc);

        if (!op || !MD_IS_ASYNC_SAVING_OP (op))
        {
            g_critical ("%s: save_file() returned MD_FILE_OP_STATUS_IN_PROGRESS, "
                        "but saving operation is not running", G_STRFUNC);
            g_set_error (error, MD_FILE_ERROR, MD_FILE_ERROR_FAILED,
                         "Internal error");
            status = MD_FILE_OP_STATUS_ERROR;
        }
    }

    return status;
}

static gboolean
save_content (const char  *filename,
              const char  *data,
              gsize        data_len,
              GError     **error)
{
    GIOChannel *file;
    GIOStatus status;
    gsize bytes_written;

    file = g_io_channel_new_file (filename, "w", error);

    if (!file)
        return FALSE;

    g_io_channel_set_encoding (file, NULL, NULL);

    status = g_io_channel_write_chars (file, data, data_len,
                                       &bytes_written, error);

    if (status != G_IO_STATUS_NORMAL || bytes_written != data_len)
    {
        /* glib #320668 */
        g_io_channel_flush (file, NULL);
        g_io_channel_shutdown (file, FALSE, NULL);
        g_io_channel_unref (file);
        return FALSE;
    }

    /* glib #320668 */
    g_io_channel_flush (file, NULL);
    g_io_channel_shutdown (file, FALSE, NULL);
    g_io_channel_unref (file);
    return TRUE;
}

static gboolean
save_local_file (MdDocument  *doc,
                 MdFileInfo  *file,
                 const char  *filename,
                 GError     **error)
{
    gboolean retval;
    char *content;
    gsize content_len;

    if (MD_DOCUMENT_GET_IFACE (doc)->save_local)
        return MD_DOCUMENT_GET_IFACE (doc)->save_local (doc, file, filename, error);

    if (!MD_DOCUMENT_GET_IFACE (doc)->get_content (doc, file, &content, &content_len, error))
        return FALSE;

    retval = save_content (filename, content, content_len, error);

    g_free (content);
    return retval;
}

static MdFileOpStatus
md_document_save_file_default (MdDocument  *doc,
                               MdFileInfo  *file,
                               GError     **error)
{
    g_return_val_if_fail (MD_DOCUMENT_GET_IFACE (doc)->get_content != NULL ||
                            MD_DOCUMENT_GET_IFACE (doc)->save_local != NULL,
                          MD_FILE_OP_STATUS_ERROR);

    if (md_file_info_is_local (file))
    {
        gboolean result;
        char *filename;

        filename = md_file_info_get_filename (file);
        g_return_val_if_fail (filename != NULL, MD_FILE_OP_STATUS_ERROR);

        result = save_local_file (doc, file, filename, error);

        g_free (filename);
        return result ? MD_FILE_OP_STATUS_SUCCESS : MD_FILE_OP_STATUS_ERROR;
    }
    else
    {
        MdAsyncOp *op;
        char *filename = NULL;
        char *template;
        int fd;

        template = g_strdup_printf ("%s-%s-XXXXXX.tmp", g_get_prgname (), g_get_user_name ());
        fd = g_file_open_tmp (template, &filename, error);
        g_free (template);

        if (fd == -1)
            return MD_FILE_OP_STATUS_ERROR;

        close (fd);

        if (!save_local_file (doc, file, filename, error))
        {
            g_unlink (filename);
            g_free (filename);
            return MD_FILE_OP_STATUS_ERROR;
        }

        op = _md_async_saving_op_new (doc, file, filename, error);

        if (!op)
        {
            g_unlink (filename);
            g_free (filename);
            return MD_FILE_OP_STATUS_ERROR;
        }

        if (op)
            md_document_start_async_op (doc, op);

        g_object_unref (op);
        g_free (filename);
        return MD_FILE_OP_STATUS_IN_PROGRESS;
    }
}
