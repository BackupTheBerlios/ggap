#include "mddocument-private.h"
#include "mdmanager-private.h"
#include "marshals.h"
#include <mooutils/mooutils-misc.h>
#include <gtk/gtk.h>


typedef struct {
    MdManager *mgr;
    MdWindow *window;
    MdDocumentCapabilities caps;
    MdDocumentStatus status;

    MdFileInfo *file_info;
    char *filename;
    char *display_name;
    char *display_basename;
} MdDocumentData;


static GQuark md_document_data_quark;

static MdDocumentData   *md_document_get_data           (MdDocument     *doc);

static GdkPixbuf        *md_document_get_icon_default   (MdDocument     *doc,
                                                         GtkIconSize     size);
static MdFileOpStatus    md_document_load_file_default  (MdDocument     *doc,
                                                         MdFileInfo     *file,
                                                         GError        **error);
static MdFileOpStatus    md_document_save_file_default  (MdDocument     *doc,
                                                         MdFileInfo     *file,
                                                         GError        **error);


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
    iface->get_icon = md_document_get_icon_default;
    iface->load_file = md_document_load_file_default;
    iface->save_file = md_document_save_file_default;
}

static void
md_document_class_init (MdDocumentIface *iface)
{
    md_document_data_quark = g_quark_from_static_string ("md-document-data");

    g_object_interface_install_property (iface,
        g_param_spec_flags ("doc-status", "doc-status", "doc-status",
                            MD_TYPE_DOCUMENT_STATUS, 0, G_PARAM_READABLE));

    g_object_interface_install_property (iface,
        g_param_spec_boxed ("doc-file-info", "doc-file-info", "doc-file-info",
                            MD_TYPE_FILE_INFO, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

//     g_signal_new ("can-undo-changed",
//                   MD_TYPE_DOCUMENT,
//                   G_SIGNAL_RUN_LAST,
//                   0,
//                   NULL, NULL,
//                   _moo_marshal_VOID__VOID,
//                   G_TYPE_NONE, 0);

//     g_object_interface_install_property (iface,
//         g_param_spec_boolean ("doc-can-undo", "doc-can-undo", "doc-can-undo",
//                               FALSE, G_PARAM_READABLE));
//
//     g_object_interface_install_property (iface,
//         g_param_spec_boolean ("doc-can-redo", "doc-can-redo", "doc-can-redo",
//                               FALSE, G_PARAM_READABLE));
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

        g_type_interface_add_prerequisite (type, GTK_TYPE_WIDGET);
    }

    return type;
}


static MdDocumentData *
md_document_get_data (MdDocument *doc)
{
    MdDocumentData *data;
    data = g_object_get_qdata (G_OBJECT (doc), md_document_data_quark);
    g_return_val_if_fail (data != NULL, NULL);
    return data;
}

static void
md_document_data_free (MdDocumentData *data)
{
    if (data)
    {
        md_file_info_free (data->file_info);
        g_free (data->filename);
        g_free (data->display_name);
        g_free (data->display_basename);
        g_free (data);
    }
}

void
md_document_init (MdDocument            *doc,
                  MdDocumentCapabilities caps)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = g_object_get_qdata (G_OBJECT (doc), md_document_data_quark);
    g_return_if_fail (data == NULL);

    data = g_new0 (MdDocumentData, 1);
    g_object_set_qdata_full (G_OBJECT (doc), md_document_data_quark, data,
                             (GDestroyNotify) md_document_data_free);

    data->caps = caps;
    data->mgr = NULL;
    data->window = NULL;
    data->status = 0;
    data->file_info = NULL;
    data->filename = NULL;
    data->display_name = NULL;
    data->display_basename = NULL;
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

        g_object_notify (G_OBJECT (doc), "doc-status");
    }
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
        g_object_notify (G_OBJECT (doc), "doc-status");
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


void
_md_document_set_window (MdDocument *doc,
                         MdWindow   *window)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (!window || MD_IS_WINDOW (window));

    data = md_document_get_data (doc);
    data->window = window;
}

MdWindow *
md_document_get_window (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);
    return data->window;
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
md_document_set_file_info (MdDocument *doc,
                           MdFileInfo *file_info)
{
    MdDocumentData *data;
    MdFileInfo *tmp;

    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (file_info != NULL);

    data = md_document_get_data (doc);
    g_return_if_fail (data != NULL);

    if (file_info == data->file_info)
        return;

    if (!data->file_info)
        _md_manager_remove_untitled (data->mgr, doc);

    tmp = data->file_info;
    data->file_info = md_file_info_copy (file_info);
    md_file_info_free (tmp);

    g_free (data->filename);
    data->filename = md_file_info_get_filename (data->file_info);

    g_free (data->display_name);
    g_free (data->display_basename);
    data->display_name = NULL;
    data->display_basename = NULL;

    g_object_notify (G_OBJECT (doc), "doc-file-info");
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


GType
md_document_status_get_type (void)
{
    static GType type;

    if (G_UNLIKELY (!type))
    {
        static const GFlagsValue values[] = {
            {MD_DOCUMENT_MODIFIED_ON_DISK, "MD_DOCUMENT_MODIFIED_ON_DISK", "modified-on-disk"},
            {MD_DOCUMENT_DELETED, "MD_DOCUMENT_DELETED", "deleted"},
            {MD_DOCUMENT_MODIFIED, "MD_DOCUMENT_MODIFIED", "modified"},
            {MD_DOCUMENT_NEW, "MD_DOCUMENT_NEW", "new"},
            {0, NULL, NULL}
        };

        type = g_flags_register_static ("MdDocumentStatus", values);
    }

    return type;
}


GdkPixbuf *
_md_document_get_icon (MdDocument  *doc,
                       GtkIconSize  size)
{
    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);
    return MD_DOCUMENT_GET_IFACE (doc)->get_icon (doc, size);
}

static GdkPixbuf *
md_document_get_icon_default (G_GNUC_UNUSED MdDocument *doc,
                              G_GNUC_UNUSED GtkIconSize size)
{
    g_warning ("%s: implement me", G_STRFUNC);
    return NULL;
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
md_file_info_array_new_uri (const char *uri)
{
    MdFileInfo **array;

    g_return_val_if_fail (uri != NULL, NULL);

    array = g_new (MdFileInfo*, 2);
    array[0] = md_file_info_new (uri);
    array[1] = NULL;

    return array;
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

static void
setup_async_loading (G_GNUC_UNUSED MdDocument *doc,
                     G_GNUC_UNUSED MdFileInfo *file)
{
    g_critical ("%s: implement me", G_STRFUNC);
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
        setup_async_loading (doc, file);

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
        g_warning ("%s: implement me", G_STRFUNC);
        return MD_FILE_OP_STATUS_ERROR;
    }
}


static void
setup_async_saving (G_GNUC_UNUSED MdDocument *doc,
                    G_GNUC_UNUSED MdFileInfo *file)
{
    g_critical ("%s: implement me", G_STRFUNC);
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
        setup_async_saving (doc, file);

    return MD_FILE_OP_STATUS_IN_PROGRESS;
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
        g_warning ("%s: implement me", G_STRFUNC);
        return MD_FILE_OP_STATUS_ERROR;
    }
}
