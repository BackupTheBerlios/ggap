/*
 *   mddocument.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MD_DOCUMENT_H
#define MD_DOCUMENT_H

#include <mooui/mdtypes.h>


#define MD_DOCUMENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_DOCUMENT, MdDocument))
#define MD_IS_DOCUMENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_DOCUMENT))
#define MD_DOCUMENT_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), MD_TYPE_DOCUMENT, MdDocumentIface))

typedef struct MdDocumentIface MdDocumentIface;

typedef enum
{
    MD_DOCUMENT_SUPPORTS_SAVE       = 1 << 0,
    MD_DOCUMENT_SUPPORTS_PRINTING   = 1 << 1,

    MD_DOCUMENT_SUPPORTS_EVERYTHING = (1<<2)-1
} MdDocumentCapabilities;

typedef enum
{
    MD_DOCUMENT_MODIFIED_ON_DISK    = 1 << 0,
    MD_DOCUMENT_DELETED             = 1 << 1,
    MD_DOCUMENT_MODIFIED            = 1 << 2,
    MD_DOCUMENT_NEW                 = 1 << 3,
    MD_DOCUMENT_CHANGED_ON_DISK     = MD_DOCUMENT_MODIFIED_ON_DISK | MD_DOCUMENT_DELETED
} MdDocumentStatus;

typedef enum
{
    MD_DOCUMENT_STATE_NORMAL,
    MD_DOCUMENT_STATE_LOADING,
    MD_DOCUMENT_STATE_SAVING,
    MD_DOCUMENT_STATE_PRINTING
} MdDocumentState;

typedef enum
{
    MD_FILE_OP_OPEN,
    MD_FILE_OP_RELOAD,
    MD_FILE_OP_SAVE,
    MD_FILE_OP_SAVE_COPY
} MdFileOpType;

struct MdFileOpInfo {
    MdFileOpType type;
    MdFileOpStatus status;
    GError *error;
};

struct MdDocumentIface {
    GTypeInterface g_iface;

    GdkPixbuf*      (*get_icon)         (MdDocument     *doc,
                                         GtkIconSize     size);
    gboolean        (*get_empty)        (MdDocument     *doc);
    void            (*set_status)       (MdDocument     *doc,
                                         MdDocumentStatus status);
    void            (*set_state)        (MdDocument     *doc,
                                         MdDocumentState state);

    void            (*load_file)        (MdDocument     *doc,
                                         MdFileInfo     *file_info,
                                         MdFileOpInfo   *op_info);
    void            (*save_file)        (MdDocument     *doc,
                                         MdFileInfo     *file_info,
                                         MdFileOpInfo   *op_info);

    void            (*load_before)      (MdDocument     *doc,
                                         MdFileInfo     *file_info,
                                         MdFileOpInfo   *op_info);
    void            (*load_after)       (MdDocument     *doc,
                                         MdFileInfo     *file_info,
                                         MdFileOpInfo   *op_info);
    void            (*save_before)      (MdDocument     *doc,
                                         MdFileInfo     *file_info,
                                         MdFileOpInfo   *op_info);
    void            (*save_after)       (MdDocument     *doc,
                                         MdFileInfo     *file_info,
                                         MdFileOpInfo   *op_info);

    void            (*close)            (MdDocument     *doc);

    void            (*apply_prefs)      (MdDocument     *doc);
};

#define MD_DOCUMENT_IS_BUSY(doc)    (md_document_get_state (doc) != MD_DOCUMENT_STATE_NORMAL)
#define MD_DOCUMENT_IS_LOADING(doc) (md_document_get_state (doc) == MD_DOCUMENT_STATE_LOADING)
#define MD_DOCUMENT_IS_SAVING(doc)  (md_document_get_state (doc) == MD_DOCUMENT_STATE_SAVING)

MdDocumentIface    *md_document_interface_peek_parent   (MdDocumentIface *iface);

void                md_document_set_capabilities        (MdDocument     *doc,
                                                         MdDocumentCapabilities caps);

MdManager          *md_document_get_manager             (MdDocument     *doc);
MdWindow           *md_document_get_window              (MdDocument     *doc);

void                md_document_set_modified            (MdDocument     *doc,
                                                         gboolean        modified);
gboolean            md_document_get_modified            (MdDocument     *doc);
void                md_document_set_status              (MdDocument     *doc,
                                                         MdDocumentStatus status);
MdDocumentStatus    md_document_get_status              (MdDocument     *doc);
void                md_document_set_readonly            (MdDocument     *doc,
                                                         gboolean        readonly);
gboolean            md_document_get_readonly            (MdDocument     *doc);

void                md_document_set_state               (MdDocument     *doc,
                                                         MdDocumentState state);
MdDocumentState     md_document_get_state               (MdDocument     *doc);

void                md_document_set_file_info           (MdDocument     *doc,
                                                         MdFileInfo     *file_info);
MdFileInfo         *md_document_get_file_info           (MdDocument     *doc);
gboolean            md_document_is_untitled             (MdDocument     *doc);
gboolean            md_document_is_empty                (MdDocument     *doc);
char               *md_document_get_filename            (MdDocument     *doc);
char               *md_document_get_uri                 (MdDocument     *doc);
char               *md_document_get_display_name        (MdDocument     *doc);
char               *md_document_get_display_basename    (MdDocument     *doc);

void                md_document_finished_loading        (MdDocument     *doc,
                                                         MdFileInfo     *file,
                                                         MdFileOpInfo   *op_info);
void                md_document_finished_saving         (MdDocument     *doc,
                                                         MdFileInfo     *file,
                                                         MdFileOpInfo   *op_info);

MdFileInfo         *md_file_info_new                    (const char     *uri);
MdFileInfo         *md_file_info_new_filename           (const char     *filename);
MdFileInfo         *md_file_info_new_from_history_item  (MdHistoryItem  *item);
MdFileInfo         *md_file_info_copy                   (MdFileInfo     *file_info);
void                md_file_info_free                   (MdFileInfo     *file_info);
void                md_file_info_set                    (MdFileInfo     *file_info,
                                                         const char     *key,
                                                         const char     *value);
void                md_file_info_set_q                  (MdFileInfo     *file_info,
                                                         GQuark          key,
                                                         const char     *value);
const char         *md_file_info_get                    (MdFileInfo     *file_info,
                                                         const char     *key);
const char         *md_file_info_get_q                  (MdFileInfo     *file_info,
                                                         GQuark          key);
MdFileInfo        **md_file_info_array_new_uri          (const char     *uri);
MdFileInfo        **md_file_info_array_new_uri_list     (char          **uris);
MdFileInfo        **md_file_info_array_copy             (MdFileInfo    **files);
void                md_file_info_array_free             (MdFileInfo    **files);

const char         *md_file_info_get_uri                (MdFileInfo     *file_info);
gboolean            md_file_info_is_local               (MdFileInfo     *file_info);
char               *md_file_info_get_filename           (MdFileInfo     *file_info);
char               *md_file_info_get_display_name       (MdFileInfo     *file_info);
char               *md_file_info_get_basename           (MdFileInfo     *file_info);

MdFileOpInfo       *md_file_op_info_new                 (MdFileOpType    type);
MdFileOpInfo       *md_file_op_info_copy                (MdFileOpInfo   *info);
void                md_file_op_info_free                (MdFileOpInfo   *info);
void                md_file_op_info_set_gerror          (MdFileOpInfo   *info,
                                                         GError         *error);
void                md_file_op_info_take_error          (MdFileOpInfo   *info,
                                                         GError         *error);
void                md_file_op_info_set_error           (MdFileOpInfo   *info,
                                                         GQuark          domain,
                                                         int             code,
                                                         const char     *fmt,
                                                         ...) G_GNUC_PRINTF(4,5);


#endif /* MD_DOCUMENT_H */
