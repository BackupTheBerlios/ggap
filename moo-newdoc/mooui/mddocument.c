/*
 *   mddocument.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "mooui/mddocument-private.h"
#include "mooui/mdmanager-private.h"
#include "mooui/marshals.h"
#include "mooui/mdenums.h"
#include "mooui/mdutils.h"
#include "mooui/mdfileops.h"
#include "moofileview/moofile.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/mooutils-fs.h"
#include <gtk/gtk.h>
#include <unistd.h>
#include <errno.h>
#include <glib/gstdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>


typedef struct {
    MdDocument *doc;
    MdManager *mgr;
    MdWindow *window;
    MdDocumentCapabilities caps;
    MdDocumentStatus status;
    MdDocumentState state;

    MdFileInfo *file_info;
    char *filename;
    char *display_name;
    char *display_basename;

//     MdAsyncOp *async_op;

    GtkWidget *label;
    GtkWidget *icon;
    GtkWidget *icon_evbox;

    guint file_monitor_id;
    guint modified_on_disk : 1;
    guint deleted_from_disk : 1;

    guint readonly : 1;
} MdDocumentData;

static GQuark md_document_data_quark;

static MdDocumentData *md_document_get_data     (MdDocument     *doc);

static void         md_document_close_default       (MdDocument     *doc);
static void         md_document_set_status_default  (MdDocument     *doc,
                                                     MdDocumentStatus status);
static void         md_document_set_state_default   (MdDocument     *doc,
                                                     MdDocumentState state);
static GdkPixbuf   *md_document_get_icon_default    (MdDocument     *doc,
                                                     GtkIconSize     size);

static void         md_document_start_file_watch    (MdDocument     *doc);
static void         md_document_stop_file_watch     (MdDocument     *doc);

static void         md_document_detach_tab_label    (MdDocument     *doc);
static void         md_document_update_tab_label    (MdDocument     *doc);


static void
md_document_base_init (MdDocumentIface *iface)
{
    iface->set_status = md_document_set_status_default;
    iface->set_state = md_document_set_state_default;
    iface->close = md_document_close_default;
    iface->get_icon = md_document_get_icon_default;
}

static void
md_document_class_init (MdDocumentIface *iface)
{
    md_document_data_quark = g_quark_from_static_string ("md-document-data");

    g_object_interface_install_property (iface,
        g_param_spec_enum ("md-doc-state", "md-doc-state", "md-doc-state",
                           MD_TYPE_DOCUMENT_STATE, 0, G_PARAM_READABLE));

    g_object_interface_install_property (iface,
        g_param_spec_flags ("md-doc-status", "md-doc-status", "md-doc-status",
                            MD_TYPE_DOCUMENT_STATUS, 0, G_PARAM_READABLE));

    g_object_interface_install_property (iface,
        g_param_spec_boolean ("md-doc-readonly", "md-doc-readonly", "md-doc-readonly",
                              FALSE, G_PARAM_READABLE));

    g_object_interface_install_property (iface,
        g_param_spec_boxed ("md-doc-file-info", "md-doc-file-info", "md-doc-file-info",
                            MD_TYPE_FILE_INFO, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_interface_install_property (iface,
        g_param_spec_boxed ("md-doc-uri", "md-doc-uri", "md-doc-uri",
                            MD_TYPE_FILE_INFO, G_PARAM_READABLE));

    g_signal_new ("load-before",
                  MD_TYPE_DOCUMENT,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdDocumentIface, load_before),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__BOXED_BOXED,
                  G_TYPE_NONE, 2,
                  MD_TYPE_FILE_INFO | G_SIGNAL_TYPE_STATIC_SCOPE,
                  MD_TYPE_FILE_OP_INFO | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("load-after",
                  MD_TYPE_DOCUMENT,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdDocumentIface, load_after),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__BOXED_BOXED,
                  G_TYPE_NONE, 2,
                  MD_TYPE_FILE_INFO | G_SIGNAL_TYPE_STATIC_SCOPE,
                  MD_TYPE_FILE_OP_INFO | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("save-before",
                  MD_TYPE_DOCUMENT,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdDocumentIface, save_before),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__BOXED_BOXED,
                  G_TYPE_NONE, 2,
                  MD_TYPE_FILE_INFO | G_SIGNAL_TYPE_STATIC_SCOPE,
                  MD_TYPE_FILE_OP_INFO | G_SIGNAL_TYPE_STATIC_SCOPE);

    g_signal_new ("save-after",
                  MD_TYPE_DOCUMENT,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (MdDocumentIface, save_after),
                  NULL, NULL,
                  _moo_ui_marshal_VOID__BOXED_BOXED,
                  G_TYPE_NONE, 2,
                  MD_TYPE_FILE_INFO | G_SIGNAL_TYPE_STATIC_SCOPE,
                  MD_TYPE_FILE_OP_INFO | G_SIGNAL_TYPE_STATIC_SCOPE);

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
//         if (data->async_op)
//         {
//             g_critical ("%s: async operation still running", G_STRFUNC);
//             g_object_unref (data->async_op);
//         }

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

    if (!iface->load_file)
        g_critical ("type '%s' can't load files", g_type_name (G_OBJECT_TYPE (doc)));

    if (iface->set_status == md_document_set_status_default)
        g_critical ("type '%s' does not provide set_status() method",
                    g_type_name (G_OBJECT_TYPE (doc)));
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

        data->doc = doc;
        data->caps = 0;
        data->mgr = NULL;
        data->status = 0;
        data->file_info = NULL;
        data->filename = NULL;
        data->display_name = NULL;
        data->display_basename = NULL;
    }

    return data;
}

static void
md_document_close_default (MdDocument *doc)
{
    MdDocumentData *data = md_document_get_data (doc);

    if (!data->file_info)
        _md_manager_remove_untitled (data->mgr, doc);

    if (data->state != MD_DOCUMENT_STATE_NORMAL)
        g_critical ("%s: closing busy document", G_STRFUNC);

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

        if (!iface->save_file)
            g_critical ("type '%s' can't save files",
                        g_type_name (G_OBJECT_TYPE (doc)));
    }
}


void
md_document_set_readonly (MdDocument *doc,
                          gboolean    readonly)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);

    if (!data->readonly != !readonly)
    {
        data->readonly = readonly != 0;
        g_object_notify (G_OBJECT (doc), "md-doc-readonly");

        md_document_update_tab_label (doc);
    }
}

gboolean
md_document_get_readonly (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), 0);

    data = md_document_get_data (doc);
    return data->readonly;
}

void
md_document_set_status (MdDocument       *doc,
                        MdDocumentStatus  status)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);

    if (data->status != status)
        MD_DOCUMENT_GET_IFACE (doc)->set_status (doc, status);
}

void
md_document_set_status_default (MdDocument       *doc,
                                MdDocumentStatus  status)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);

    data->status = status;
    g_object_notify (G_OBJECT (doc), "md-doc-status");

    md_document_update_tab_label (doc);
}

MdDocumentStatus
md_document_get_status (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), 0);

    data = md_document_get_data (doc);
    return data->status;
}

void
md_document_set_modified (MdDocument *doc,
                          gboolean    modified)
{
    MdDocumentData *data;
    MdDocumentStatus new_status;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);

    if (modified)
        new_status = data->status | MD_DOCUMENT_MODIFIED;
    else
        new_status = data->status & ~MD_DOCUMENT_MODIFIED;

    if (new_status != data->status)
        MD_DOCUMENT_GET_IFACE (doc)->set_status (doc, new_status);
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
md_document_set_state (MdDocument      *doc,
                       MdDocumentState  state)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);

    if (data->state != state)
        MD_DOCUMENT_GET_IFACE (doc)->set_state (doc, state);
}

void
md_document_set_state_default (MdDocument      *doc,
                               MdDocumentState  state)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);

    data->state = state;
    g_object_notify (G_OBJECT (doc), "md-doc-state");

    md_document_update_tab_label (doc);
}

MdDocumentState
md_document_get_state (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), 0);

    data = md_document_get_data (doc);
    return data->state;
}


gboolean
_md_document_need_save (MdDocument *doc)
{
    g_return_val_if_fail (MD_IS_DOCUMENT (doc), FALSE);
    return (md_document_get_status (doc) & MD_DOCUMENT_MODIFIED) != 0;
}

gboolean
md_document_is_empty (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), FALSE);

    data = md_document_get_data (doc);

    if ((data->status & MD_DOCUMENT_MODIFIED) || data->file_info)
        return FALSE;

    if (MD_DOCUMENT_GET_IFACE (doc)->get_empty != NULL)
        return MD_DOCUMENT_GET_IFACE (doc)->get_empty (doc);
    else
        return TRUE;
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
_md_document_set_window (MdDocument *doc,
                         MdWindow   *window)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (!window || MD_IS_WINDOW (window));

    data = md_document_get_data (doc);
    g_return_if_fail (data != NULL);

    if (data->window != window)
    {
        g_return_if_fail (!window || !data->window);
        md_document_detach_tab_label (doc);
        data->window = window;
    }
}

MdWindow *
md_document_get_window (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);
    g_return_val_if_fail (data != NULL, NULL);

    return data->window;
}


void
_md_document_close (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);

//     if (data->async_op)
//     {
//         g_critical ("%s: closing doc with async operation running", G_STRFUNC);
//         md_async_op_abort (data->async_op, FALSE);
//     }

    g_signal_emit_by_name (doc, "close");
}


void
md_document_set_file_info (MdDocument *doc,
                           MdFileInfo *file_info)
{
    MdDocumentData *data;
    MdFileInfo *old_info;
    gboolean uri_changed;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);
    g_return_if_fail (data != NULL);

    if (file_info == data->file_info)
        return;

    if (!data->file_info && data->display_name)
        _md_manager_remove_untitled (data->mgr, doc);

    old_info = data->file_info;
    data->file_info = file_info ? md_file_info_copy (file_info) : NULL;

    g_free (data->filename);
    data->filename = file_info ? md_file_info_get_filename (data->file_info) : NULL;

    g_free (data->display_name);
    g_free (data->display_basename);
    data->display_name = NULL;
    data->display_basename = NULL;

    uri_changed = (!old_info && file_info) || (old_info && !file_info) ||
        (old_info && file_info &&
         strcmp (md_file_info_get_uri (old_info), md_file_info_get_uri (file_info)) != 0);

    g_object_freeze_notify (G_OBJECT (doc));
    g_object_notify (G_OBJECT (doc), "md-doc-file-info");
    if (uri_changed)
        g_object_notify (G_OBJECT (doc), "md-doc-uri");
    g_object_thaw_notify (G_OBJECT (doc));

    md_document_update_tab_label (doc);

    md_file_info_free (old_info);
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
md_document_is_untitled (MdDocument *doc)
{
    return md_document_get_file_info (doc) == NULL;
}

char *
md_document_get_uri (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);
    return data->file_info ?
            g_strdup (md_file_info_get_uri (data->file_info)) :
            NULL;
}

char *
md_document_get_filename (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);
    return g_strdup (data->filename);
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

char *
md_document_get_display_name (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);

    if (!data->display_name)
        calc_display_name (doc, data);

    return g_strdup (data->display_name);
}

char *
md_document_get_display_basename (MdDocument *doc)
{
    MdDocumentData *data;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    data = md_document_get_data (doc);

    if (!data->display_basename)
        calc_display_name (doc, data);

    return g_strdup (data->display_basename);
}


GdkPixbuf *
_md_document_get_icon (MdDocument  *doc,
                       GtkIconSize  size)
{
    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);
    g_return_val_if_fail (MD_DOCUMENT_GET_IFACE (doc)->get_icon != NULL, NULL);
    return MD_DOCUMENT_GET_IFACE (doc)->get_icon (doc, size);
}

void
_md_document_apply_prefs (MdDocument *doc)
{
    g_return_if_fail (MD_IS_DOCUMENT (doc));

    if (MD_DOCUMENT_GET_IFACE (doc)->apply_prefs)
        MD_DOCUMENT_GET_IFACE (doc)->apply_prefs (doc);
}


GType
md_file_op_info_get_type (void)
{
    static GType type;

    if (G_UNLIKELY (!type))
        type = g_boxed_type_register_static ("MdFileOpInfo",
                                             (GBoxedCopyFunc) md_file_op_info_copy,
                                             (GBoxedFreeFunc) md_file_op_info_free);

    return type;
}

MdFileOpInfo *
md_file_op_info_new (MdFileOpType type)
{
    MdFileOpInfo *info;

    info = moo_new0 (MdFileOpInfo);
    info->type = type;

    return info;
}

MdFileOpInfo *
md_file_op_info_copy (MdFileOpInfo *info)
{
    return info ? md_file_op_info_new (info->type) : NULL;
}

void
md_file_op_info_free (MdFileOpInfo *info)
{
    if (info)
    {
        if (info->error)
            g_error_free (info->error);
        moo_free (MdFileOpInfo, info);
    }
}


void
md_file_op_info_set_gerror (MdFileOpInfo *info,
                            GError       *error)
{
    g_return_if_fail (info != NULL);
    g_return_if_fail (error != NULL);
    md_file_op_info_take_error (info, g_error_copy (error));
}

void
md_file_op_info_take_error (MdFileOpInfo *info,
                            GError       *error)
{
    GError *tmp;

    g_return_if_fail (info != NULL);

    if (!error)
    {
        g_critical ("%s: oops", G_STRLOC);
        error = g_error_new_literal (MD_FILE_ERROR,
                                     MD_FILE_ERROR_FAILED,
                                     "Failed");
    }

    tmp = info->error;
    info->error = error;
    info->status = MD_FILE_OP_STATUS_ERROR;

    if (tmp)
        g_error_free (tmp);
}

void
md_file_op_info_set_error (MdFileOpInfo *info,
                           GQuark        domain,
                           int           code,
                           const char   *fmt,
                           ...)
{
    GError *error = NULL;
    char *message;
    va_list args;

    g_return_if_fail (info != NULL);
    g_return_if_fail (fmt != NULL);

    va_start (args, fmt);
    message = g_strdup_vprintf (fmt, args);
    va_end (args);

    g_return_if_fail (message != NULL);

    error = g_error_new_literal (domain, code, message);
    md_file_op_info_take_error (info, error);
    g_free (message);
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

MdFileInfo *
md_file_info_new_filename (const char *filename)
{
    char *uri;
    MdFileInfo *file_info;
    GError *error = NULL;

    g_return_val_if_fail (filename != NULL, NULL);

    if (!(uri = _moo_filename_to_uri (filename, &error)))
    {
        g_critical ("%s: could not convert filename to uri: %s",
                    G_STRFUNC, error->message);
        g_error_free (error);
        return NULL;
    }

    file_info = md_file_info_new (uri);

    g_free (uri);
    return file_info;
}

static void
copy_data (GQuark      key,
           const char *value,
           MdFileInfo *dest)
{
    g_datalist_id_set_data_full (&dest->data, key, g_strdup (value), g_free);
}

MdFileInfo *
md_file_info_new_from_history_item (MdHistoryItem *item)
{
    MdFileInfo *file_info;

    g_return_val_if_fail (item != NULL, NULL);

    file_info = md_file_info_new (md_history_item_get_uri (item));
    md_history_item_foreach (item, (GDataForeachFunc) copy_data, file_info);

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
    md_file_info_set_q (file_info, g_quark_from_string (key), value);
}

void
md_file_info_set_q (MdFileInfo *file_info,
                    GQuark      key,
                    const char *value)
{
    g_return_if_fail (file_info != NULL);

    if (value)
        g_datalist_id_set_data_full (&file_info->data, key, g_strdup (value), g_free);
    else
        g_datalist_id_remove_data (&file_info->data, key);
}

const char *
md_file_info_get (MdFileInfo *file_info,
                  const char *key)
{
    g_return_val_if_fail (file_info != NULL, NULL);
    g_return_val_if_fail (key != NULL, NULL);
    return md_file_info_get_q (file_info, g_quark_from_string (key));
}

const char *
md_file_info_get_q (MdFileInfo *file_info,
                    GQuark      key)
{
    g_return_val_if_fail (file_info != NULL, NULL);
    return g_datalist_id_get_data (&file_info->data, key);
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

    if (md_file_info_is_local (file_info))
    {
        char *filename = md_file_info_get_filename (file_info);

        if (filename)
        {
            char *display_name = g_filename_display_name (filename);

            if (display_name)
            {
                g_free (filename);
                return display_name;
            }

            g_free (filename);
        }
    }

    return g_strdup (file_info->uri);
}

char *
md_file_info_get_basename (MdFileInfo *file_info)
{
    const char *last_slash;

    g_return_val_if_fail (file_info != NULL, NULL);

    /* XXX percent encoding */
    last_slash = strrchr (file_info->uri, '/');
    if (last_slash)
        return g_strdup (last_slash + 1);
    else
        return g_strdup (file_info->uri);
}


MdFileInfo **
md_file_info_array_new_uri (const char *uri)
{
    char *array[2] = {NULL};

    if (!uri || !uri[0])
        return NULL;

    array[0] = (char*) uri;
    return md_file_info_array_new_uri_list (array);
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

// static MdAsyncOp *
// md_document_get_async_op (MdDocument *doc)
// {
//     MdDocumentData *data;
//
//     g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);
//
//     data = md_document_get_data (doc);
//     return data->async_op;
// }
//
// static void
// async_op_finished (MdDocument *doc,
//                    MdAsyncOp  *op)
// {
//     MdDocumentData *data;
//
//     g_return_if_fail (MD_IS_DOCUMENT (doc));
//     g_return_if_fail (MD_IS_ASYNC_OP (op));
//
//     data = md_document_get_data (doc);
//     g_return_if_fail (op == data->async_op);
//
//     data->async_op = NULL;
//
//     /* the doc had reference to it */
//     g_object_unref (op);
// }
//
// static void
// md_document_start_async_op (MdDocument *doc,
//                             MdAsyncOp  *op)
// {
//     MdDocumentData *data;
//
//     g_return_if_fail (MD_IS_DOCUMENT (doc));
//     g_return_if_fail (MD_IS_ASYNC_OP (op));
//     g_return_if_fail (!md_async_op_finished (op));
//
//     data = md_document_get_data (doc);
//     g_return_if_fail (data->async_op == NULL);
//
//     data->async_op = g_object_ref (op);
//
//     if (!md_async_op_started (op))
//         md_async_op_start (op);
//
//     g_signal_connect_swapped (op, "finished",
//                               G_CALLBACK (async_op_finished), doc);
// }


static void
check_writable_uri_scheme (MdDocument *doc,
                           MdFileInfo *file)
{
#if 1
    const char *uri;

    uri = md_file_info_get_uri (file);
    g_return_if_fail (uri != NULL);

    md_document_set_readonly (doc, g_str_has_prefix (uri, "http"));
#else
    md_document_set_readonly (doc, !md_file_info_is_local (file));
#endif
}


static void
emit_load_before (MdDocument   *doc,
                  MdFileInfo   *file,
                  MdFileOpInfo *op_info)
{
    g_signal_emit_by_name (doc, "load-before", file, op_info);
}

static void
emit_load_after (MdDocument     *doc,
                 MdFileInfo     *file,
                 MdFileOpInfo   *op_info)
{
    g_signal_emit_by_name (doc, "load-after", file, op_info);
}

static void
call_load_file (MdDocument   *doc,
                MdFileInfo   *file,
                MdFileOpInfo *op_info)
{
    MD_DOCUMENT_GET_IFACE (doc)->load_file (doc, file, op_info);
}

static void
update_after_successful_file_op (MdDocument *doc,
                                 MdFileInfo *file)
{
    /* XXX file watch and permissions */
    MdDocumentData *data = md_document_get_data (doc);

    md_document_set_file_info (doc, file);
    check_writable_uri_scheme (doc, file);
    md_document_set_status (doc, 0);

    _md_manager_add_recent (data->mgr, doc);

    if (md_file_info_is_local (file))
        md_document_start_file_watch (doc);
}

// static void
// async_loading_finished (MdAsyncFileOp *op)
// {
//     MdDocument *doc;
//     MdFileInfo *file_info;
//     MdFileOpInfo *op_info;
//
//     doc = md_async_op_get_doc (MD_ASYNC_OP (op));
//     g_return_if_fail (MD_IS_DOCUMENT (doc));
//     g_return_if_fail (md_document_get_async_op (doc) == MD_ASYNC_OP (op));
//
//     file_info = _md_async_file_op_get_file_info (op);
//     op_info = _md_async_file_op_get_info (op);
//     g_return_if_fail (file_info && op_info);
//     g_return_if_fail (op_info->status != MD_FILE_OP_STATUS_IN_PROGRESS);
//
//     if (op_info->status == MD_FILE_OP_STATUS_SUCCESS)
//     {
//         int fd;
//         const char *filename;
//         GError *error = NULL;
//
//         filename = _md_async_loading_op_get_tmp_file (MD_ASYNC_LOADING_OP (op));
//         fd = _md_open_file_for_reading (filename, NULL, &error);
//
//         if (fd == -1 || !load_file (doc, file_info, &fd, op_info, &error))
//         {
//             op_info->status = MD_FILE_OP_STATUS_ERROR;
//             g_propagate_error (&op_info->error, error);
//         }
//
//         if (fd != -1)
//             close (fd);
//
//         if (error)
//             g_error_free (error);
//     }
//
//     if (op_info->status == MD_FILE_OP_STATUS_SUCCESS)
//         update_after_successful_file_op (doc, file_info);
//
//     md_async_op_finish (MD_ASYNC_OP (op));
//
//     md_document_set_state (doc, MD_DOCUMENT_STATE_NORMAL);
//     emit_load_after (doc, file_info, op_info);
// }

// static MdFileOpStatus
// start_async_loading (MdDocument    *doc,
//                      MdFileInfo    *file,
//                      MdFileOpInfo  *op_info,
//                      GError       **error)
// {
//     MdAsyncOp *op;
//
//     op = _md_async_loading_op_new (doc, file, op_info, error);
//
//     if (!op)
//         return MD_FILE_OP_STATUS_ERROR;
//
//     g_signal_connect (op, "md-async-file-op-finished",
//                       G_CALLBACK (async_loading_finished),
//                       NULL);
//     md_document_start_async_op (doc, op);
//
//     g_object_unref (op);
//     return MD_FILE_OP_STATUS_IN_PROGRESS;
// }

// static MdFileOpStatus
// load_local_file (MdDocument    *doc,
//                  MdFileInfo    *file,
//                  MdFileOpInfo  *op_info,
//                  GError       **error)
// {
//     gboolean result;
//     char *filename;
//     int fd;
//     struct stat statbuf;
//
//     filename = md_file_info_get_filename (file);
//     g_return_val_if_fail (filename != NULL, MD_FILE_OP_STATUS_ERROR);
//
//     fd = _md_open_file_for_reading (filename, &statbuf, error);
//
//     if (fd == -1)
//         return MD_FILE_OP_STATUS_ERROR;
//
//     /* XXX permissions */
//
//     result = load_file (doc, file, &fd, op_info, error);
//
//     if (fd != -1)
//         close (fd);
//
//     if (result)
//         update_after_successful_file_op (doc, file);
//
//     g_free (filename);
//     return result ? MD_FILE_OP_STATUS_SUCCESS : MD_FILE_OP_STATUS_ERROR;
// }

static void
loading_finished (MdDocument   *doc,
                  MdFileInfo   *file,
                  MdFileOpInfo *op_info)
{
    if (op_info->status == MD_FILE_OP_STATUS_SUCCESS)
        update_after_successful_file_op (doc, file);
    md_document_set_state (doc, MD_DOCUMENT_STATE_NORMAL);
    emit_load_after (doc, file, op_info);

    if (op_info->status == MD_FILE_OP_STATUS_SUCCESS)
        g_message ("successfully loaded %s", md_file_info_get_uri (file));
    else if (op_info->status == MD_FILE_OP_STATUS_CANCELLED)
        g_message ("loading %s was cancelled", md_file_info_get_uri (file));
    else if (op_info->status == MD_FILE_OP_STATUS_ERROR)
        g_message ("could not load %s: %s", md_file_info_get_uri (file),
                   op_info->error->message);
}

void
md_document_finished_loading (MdDocument   *doc,
                              MdFileInfo   *file,
                              MdFileOpInfo *op_info)
{
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (file != NULL);
    g_return_if_fail (op_info != NULL);
    g_return_if_fail (op_info->status != MD_FILE_OP_STATUS_IN_PROGRESS);
    g_return_if_fail (MD_DOCUMENT_IS_LOADING (doc));
    loading_finished (doc, file, op_info);
}

void
_md_document_load_file (MdDocument   *doc,
                        MdFileInfo   *file,
                        MdFileOpInfo *op_info)
{
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (file != NULL);
    g_return_if_fail (op_info != NULL);
    g_return_if_fail (op_info->error == NULL);
    g_return_if_fail (!MD_DOCUMENT_IS_BUSY (doc));

    md_document_stop_file_watch (doc);

    op_info->status = MD_FILE_OP_STATUS_IN_PROGRESS;
    emit_load_before (doc, file, op_info);
    md_document_set_state (doc, MD_DOCUMENT_STATE_LOADING);

    call_load_file (doc, file, op_info);

    if (MD_DOCUMENT_IS_LOADING (doc))
    {
        if (op_info->status != MD_FILE_OP_STATUS_IN_PROGRESS)
            loading_finished (doc, file, op_info);
        else
            md_document_set_file_info (doc, file);
    }
}


static void
emit_save_before (MdDocument   *doc,
                  MdFileInfo   *file,
                  MdFileOpInfo *op_info)
{
    g_signal_emit_by_name (doc, "save-before", file, op_info);
}

static void
emit_save_after (MdDocument     *doc,
                 MdFileInfo     *file,
                 MdFileOpInfo   *op_info)
{
    g_signal_emit_by_name (doc, "save-after", file, op_info);
}

// static gboolean
// save_file (MdDocument    *doc,
//            MdFileInfo    *file,
//            int           *fd,
//            MdFileOpInfo  *op_info,
//            GError       **error)
// {
//     gboolean result;
//
//     result = MD_DOCUMENT_GET_IFACE (doc)->save_file (doc, file, fd, op_info, error);
//
//     if (*fd != -1)
//     {
//         errno = 0;
//
//         if (close (*fd) != 0 && result)
//         {
//             int err = errno;
//             g_set_error (error, G_FILE_ERROR,
//                          g_file_error_from_errno (err),
//                          "%s", g_strerror (err));
//             result = FALSE;
//         }
//
//         *fd = -1;
//     }
//
//     return result;
// }
//
// static void
// async_saving_finished (MdAsyncFileOp *op)
// {
//     MdDocument *doc;
//     MdFileInfo *file_info;
//     MdFileOpInfo *op_info;
//
//     doc = md_async_op_get_doc (MD_ASYNC_OP (op));
//     g_return_if_fail (MD_IS_DOCUMENT (doc));
//     g_return_if_fail (md_document_get_async_op (doc) == MD_ASYNC_OP (op));
//
//     file_info = _md_async_file_op_get_file_info (op);
//     op_info = _md_async_file_op_get_info (op);
//     g_return_if_fail (file_info && op_info);
//     g_return_if_fail (op_info->status != MD_FILE_OP_STATUS_IN_PROGRESS);
//
//     if (op_info->status == MD_FILE_OP_STATUS_SUCCESS && op_info->type == MD_FILE_OP_SAVE)
//         update_after_successful_file_op (doc, file_info);
//
//     md_async_op_finish (MD_ASYNC_OP (op));
//     md_document_set_state (doc, MD_DOCUMENT_STATE_NORMAL);
//     emit_save_after (doc, file_info, op_info);
// }
//
// static MdFileOpStatus
// start_async_saving (MdDocument *doc,
//                     MdFileInfo    *file,
//                     MdFileOpInfo  *op_info,
//                     GError       **error)
// {
//     MdAsyncOp *op;
//     char *filename = NULL;
//     int fd;
//
//     filename = moo_tempnam ();
//
//     if ((fd = _md_open_file_for_writing (filename, error)) == -1)
//         return MD_FILE_OP_STATUS_ERROR;
//
//     if (!save_file (doc, file, &fd, op_info, error))
//     {
//         g_unlink (filename);
//         g_free (filename);
//         return MD_FILE_OP_STATUS_ERROR;
//     }
//
//     op = _md_async_saving_op_new (doc, file, filename, op_info, error);
//
//     if (!op)
//     {
//         g_unlink (filename);
//         g_free (filename);
//         return MD_FILE_OP_STATUS_ERROR;
//     }
//
//     g_signal_connect (op, "md-async-file-op-finished",
//                       G_CALLBACK (async_saving_finished),
//                       NULL);
//     md_document_start_async_op (doc, op);
//
//     if (op)
//         g_object_unref (op);
//     g_free (filename);
//
//     return MD_FILE_OP_STATUS_IN_PROGRESS;
// }
//
// static MdFileOpStatus
// save_local_file (MdDocument    *doc,
//                  MdFileInfo    *file,
//                  MdFileOpInfo  *op_info,
//                  GError       **error)
// {
//     gboolean result;
//     char *filename;
//     int fd;
//
//     filename = md_file_info_get_filename (file);
//     g_return_val_if_fail (filename != NULL, MD_FILE_OP_STATUS_ERROR);
//
//     if ((fd = _md_open_file_for_writing (filename, error)) == -1)
//         return MD_FILE_OP_STATUS_ERROR;
//
//     result = save_file (doc, file, &fd, op_info, error);
//
//     if (result && op_info->type == MD_FILE_OP_SAVE)
//         update_after_successful_file_op (doc, file);
//
//     g_free (filename);
//     return result ? MD_FILE_OP_STATUS_SUCCESS : MD_FILE_OP_STATUS_ERROR;
// }


static void
call_save_file (MdDocument   *doc,
                MdFileInfo   *file,
                MdFileOpInfo *op_info)
{
    MD_DOCUMENT_GET_IFACE (doc)->save_file (doc, file, op_info);
}

static void
saving_finished (MdDocument   *doc,
                 MdFileInfo   *file,
                 MdFileOpInfo *op_info)
{
    if (op_info->status == MD_FILE_OP_STATUS_SUCCESS &&
        op_info->type == MD_FILE_OP_SAVE)
            update_after_successful_file_op (doc, file);
    md_document_set_state (doc, MD_DOCUMENT_STATE_NORMAL);
    emit_save_after (doc, file, op_info);
}

void
md_document_finished_saving (MdDocument   *doc,
                             MdFileInfo   *file,
                             MdFileOpInfo *op_info)
{
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (MD_DOCUMENT_IS_SAVING (doc));
    g_return_if_fail (file != NULL);
    g_return_if_fail (op_info != NULL);
    g_return_if_fail (op_info->status != MD_FILE_OP_STATUS_IN_PROGRESS);
    saving_finished (doc, file, op_info);
}

void
_md_document_save_file (MdDocument    *doc,
                        MdFileInfo    *file,
                        MdFileOpInfo  *op_info)
{
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (file != NULL);
    g_return_if_fail (op_info != NULL);
    g_return_if_fail (!op_info->error);
    g_return_if_fail (!MD_DOCUMENT_IS_BUSY (doc));

    if (op_info->type == MD_FILE_OP_SAVE)
        md_document_stop_file_watch (doc);

    op_info->status = MD_FILE_OP_STATUS_IN_PROGRESS;

    emit_save_before (doc, file, op_info);
    md_document_set_state (doc, MD_DOCUMENT_STATE_SAVING);

    call_save_file (doc, file, op_info);

    if (MD_DOCUMENT_IS_SAVING (doc))
    {
        if (op_info->type == MD_FILE_OP_SAVE &&
            (op_info->status == MD_FILE_OP_STATUS_IN_PROGRESS ||
             op_info->status == MD_FILE_OP_STATUS_SUCCESS))
                md_document_set_file_info (doc, file);

        if (op_info->status != MD_FILE_OP_STATUS_IN_PROGRESS)
            saving_finished (doc, file, op_info);
    }
}


static void
file_modified_on_disk (MdDocument     *doc,
                       MdDocumentData *data)
{
    data->modified_on_disk = FALSE;
    data->deleted_from_disk = FALSE;
    md_document_stop_file_watch (doc);
    md_document_set_status (doc, data->status | MD_DOCUMENT_MODIFIED_ON_DISK);
}

static void
file_deleted (MdDocument     *doc,
              MdDocumentData *data)
{
    data->modified_on_disk = FALSE;
    data->deleted_from_disk = FALSE;
    md_document_stop_file_watch (doc);
    md_document_set_status (doc, data->status | MD_DOCUMENT_DELETED);
}

static void
check_file_status (MdDocument     *doc,
                   MdDocumentData *data,
                   G_GNUC_UNUSED gboolean in_focus_only)
{
//     if (in_focus_only && !GTK_WIDGET_HAS_FOCUS (edit))
//         return;

    g_return_if_fail (!(data->status & MD_DOCUMENT_CHANGED_ON_DISK));

    if (data->deleted_from_disk)
        file_deleted (doc, data);
    else if (data->modified_on_disk)
        file_modified_on_disk (doc, data);
}

static void
file_watch_callback (G_GNUC_UNUSED MooFileWatch *watch,
                     MooFileEvent  *event,
                     gpointer       user_data)
{
    MdDocumentData *data;
    MdDocument *doc = user_data;

    g_return_if_fail (MD_IS_DOCUMENT (user_data));

    doc = user_data;
    data = md_document_get_data (doc);

    g_return_if_fail (event->monitor_id == data->file_monitor_id);
    g_return_if_fail (!(data->status & MD_DOCUMENT_CHANGED_ON_DISK));

    switch (event->code)
    {
        case MOO_FILE_EVENT_CHANGED:
            data->modified_on_disk = TRUE;
            break;

        case MOO_FILE_EVENT_DELETED:
            data->deleted_from_disk = TRUE;
            data->file_monitor_id = 0;
            break;

        case MOO_FILE_EVENT_ERROR:
            /* XXX and what to do now? */
            break;

        case MOO_FILE_EVENT_CREATED:
            g_critical ("%s: oops", G_STRLOC);
            break;
    }

    check_file_status (doc, data, FALSE);
}

static void
md_document_start_file_watch (MdDocument *doc)
{
    MdDocumentData *data;
    char *filename;
    MooFileWatch *watch;
    GError *error = NULL;

    data = md_document_get_data (doc);

    if (data->file_monitor_id)
    {
        g_critical ("%s: oops", G_STRLOC);
        md_document_stop_file_watch (doc);
        return;
    }

    if (!md_file_info_is_local (data->file_info))
        return;

    g_return_if_fail (!(data->status & MD_DOCUMENT_CHANGED_ON_DISK));

    filename = md_file_info_get_filename (data->file_info);
    g_return_if_fail (filename != NULL);

    watch = _md_manager_get_file_watch (data->mgr);
    g_return_if_fail (watch != NULL);

    data->file_monitor_id = moo_file_watch_create_monitor (watch, filename,
                                                           file_watch_callback,
                                                           doc, NULL, &error);

    if (!data->file_monitor_id)
    {
        g_warning ("%s: could not start watch for '%s': %s",
                   G_STRLOC, filename,
                   error ? error->message : "");
        g_error_free (error);
    }

    g_free (filename);

    /* XXX focus? */
//     if (!edit->priv->focus_in_handler_id)
//         edit->priv->focus_in_handler_id =
//                 g_signal_connect (edit, "focus-in-event",
//                                   G_CALLBACK (focus_in_cb),
//                                   NULL);
}

static void
md_document_stop_file_watch (MdDocument *doc)
{
    MdDocumentData *data;

    data = md_document_get_data (doc);

    if (data->file_monitor_id)
    {
        MooFileWatch *watch;

        watch = _md_manager_get_file_watch (data->mgr);
        g_return_if_fail (watch != NULL);

        if (data->file_monitor_id)
            moo_file_watch_cancel_monitor (watch, data->file_monitor_id);
        data->file_monitor_id = 0;
    }

    /* XXX focus? */
//     if (edit->priv->focus_in_handler_id)
//     {
//         g_signal_handler_disconnect (edit, edit->priv->focus_in_handler_id);
//         edit->priv->focus_in_handler_id = 0;
//     }
}


/***************************************************************************/
/* Tab label
 */

static GdkPixbuf *
md_document_get_icon_default (MdDocument  *doc,
                              GtkIconSize  size)
{
    /* XXX non-local */
    char *filename = md_document_get_filename (doc);
    GdkPixbuf *pixbuf = _moo_get_icon_for_path (filename, GTK_WIDGET (doc), size);
    g_free (filename);
    return pixbuf ? g_object_ref (pixbuf) : NULL;
}


GtkWidget *
_md_document_create_tab_label (MdDocument  *doc,
                               GtkWidget  **icon_evbox)
{
    MdDocumentData *data;
    GtkWidget *hbox, *icon, *label, *evbox;
    GtkSizeGroup *group;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);
    g_return_val_if_fail (icon_evbox != NULL, NULL);

    data = md_document_get_data (doc);
    g_return_val_if_fail (data != NULL, NULL);
    g_return_val_if_fail (data->label == NULL, NULL);

    group = gtk_size_group_new (GTK_SIZE_GROUP_VERTICAL);

    hbox = gtk_hbox_new (FALSE, 3);
    gtk_widget_show (hbox);

    *icon_evbox = evbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (evbox), FALSE);
    gtk_box_pack_start (GTK_BOX (hbox), evbox, FALSE, FALSE, 0);

    icon = gtk_image_new ();
    gtk_container_add (GTK_CONTAINER (evbox), icon);
    gtk_widget_show_all (evbox);

    label = gtk_label_new (NULL);
    gtk_label_set_single_line_mode (GTK_LABEL (label), TRUE);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);

    gtk_size_group_add_widget (group, evbox);
    gtk_size_group_add_widget (group, label);

    data->label = label;
    data->icon = icon;
    data->icon_evbox = evbox;

    md_document_update_tab_label (doc);

    g_object_unref (group);

    return hbox;
}

static void
md_document_detach_tab_label (MdDocument *doc)
{
    MdDocumentData *data;

    data = md_document_get_data (doc);

    data->label = NULL;
    data->icon = NULL;
    data->icon_evbox = NULL;
}

static void
md_document_update_tab_label (MdDocument *doc)
{
    MdDocumentData *data;
    MdDocumentStatus status;
    char *label_text;
    gboolean modified, deleted;
    char *display_basename;

    g_return_if_fail (MD_IS_DOCUMENT (doc));

    data = md_document_get_data (doc);

    if (!data->label)
        return;

    status = md_document_get_status (doc);

    deleted = (status & (MD_DOCUMENT_DELETED | MD_DOCUMENT_MODIFIED_ON_DISK)) != 0;
    modified = (status & MD_DOCUMENT_MODIFIED) != 0;

    display_basename = md_document_get_display_basename (doc);
    label_text = g_strdup_printf ("%s%s%s",
                                  deleted ? "!" : "",
                                  modified ? "*" : "",
                                  display_basename);
    gtk_label_set_text (GTK_LABEL (data->label), label_text);

    if (!MD_DOCUMENT_IS_BUSY (doc) ||
        !_md_image_attach_throbber (GTK_IMAGE (data->icon), GTK_ICON_SIZE_MENU))
    {
        GdkPixbuf *pixbuf;

        pixbuf = _md_document_get_icon (doc, GTK_ICON_SIZE_MENU);

        if (pixbuf)
        {
            gtk_image_set_from_pixbuf (GTK_IMAGE (data->icon), pixbuf);
            g_object_unref (pixbuf);
        }
    }

    g_free (label_text);
    g_free (display_basename);
}
