#ifndef MD_DOCUMENT_H
#define MD_DOCUMENT_H

#include <gtk/gtkwidget.h>


#define MD_TYPE_DOCUMENT_STATUS     (md_document_status_get_type ())
#define MD_TYPE_FILE_INFO           (md_file_info_get_type ())
#define MD_TYPE_FILE_INFO_ARRAY     (md_file_info_array_get_type ())

#define MD_TYPE_HAS_UNDO            (md_has_undo_get_type ())
#define MD_HAS_UNDO(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_HAS_UNDO, MdHasUndo))
#define MD_HAS_UNDO_CLASS(obj)      (G_TYPE_CHECK_CLASS_CAST ((obj), MD_TYPE_HAS_UNDO, MdHasUndoIface))
#define MD_IS_HAS_UNDO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_HAS_UNDO))
#define MD_HAS_UNDO_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), MD_TYPE_HAS_UNDO, MdHasUndoIface))

#define MD_TYPE_DOCUMENT            (md_document_get_type ())
#define MD_DOCUMENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_DOCUMENT, MdDocument))
#define MD_DOCUMENT_CLASS(obj)      (G_TYPE_CHECK_CLASS_CAST ((obj), MD_TYPE_DOCUMENT, MdDocumentIface))
#define MD_IS_DOCUMENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_DOCUMENT))
#define MD_DOCUMENT_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), MD_TYPE_DOCUMENT, MdDocumentIface))

typedef struct MdDocument MdDocument;
typedef struct MdDocumentIface MdDocumentIface;

typedef struct MdHasUndo MdHasUndo;
typedef struct MdHasUndoIface MdHasUndoIface;

typedef struct MdWindow MdWindow;
typedef struct MdManager MdManager;
typedef struct MdFileInfo MdFileInfo;

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
    MD_DOCUMENT_NEW                 = 1 << 3
} MdDocumentStatus;

#define MD_DOCUMENT_CHANGED_ON_DISK (MD_DOCUMENT_MODIFIED_ON_DISK | MD_DOCUMENT_DELETED)

typedef enum
{
    MD_FILE_OP_STATUS_IN_PROGRESS,
    MD_FILE_OP_STATUS_ERROR,
    MD_FILE_OP_STATUS_SUCCESS,
    MD_FILE_OP_STATUS_CANCELLED
} MdFileOpStatus;

struct MdHasUndoIface {
    GTypeInterface g_iface;
    void (*undo) (MdHasUndo *obj);
    void (*redo) (MdHasUndo *obj);
};

struct MdDocumentIface {
    GTypeInterface g_iface;

    GdkPixbuf*      (*get_icon)         (MdDocument  *doc,
                                         GtkIconSize  size);

    MdFileOpStatus  (*load_file)        (MdDocument  *doc,
                                         MdFileInfo  *file_info,
                                         GError     **error);
    gboolean        (*load_local)       (MdDocument  *doc,
                                         MdFileInfo  *file_info,
                                         const char  *filename,
                                         GError     **error);
    gboolean        (*load_content)     (MdDocument  *doc,
                                         MdFileInfo  *file_info,
                                         const char  *content,
                                         gsize        len,
                                         GError     **error);

    MdFileOpStatus  (*save_file)        (MdDocument  *doc,
                                         MdFileInfo  *file_info,
                                         GError     **error);
    gboolean        (*get_content)      (MdDocument  *doc,
                                         MdFileInfo  *file_info,
                                         char       **content,
                                         gsize       *content_len,
                                         GError     **error);
    gboolean        (*save_local)       (MdDocument  *doc,
                                         MdFileInfo  *file_info,
                                         const char  *filename,
                                         GError     **error);
};


GType               md_document_get_type                (void) G_GNUC_CONST;
GType               md_has_undo_get_type                (void) G_GNUC_CONST;
GType               md_document_status_get_type         (void) G_GNUC_CONST;
GType               md_file_info_get_type               (void) G_GNUC_CONST;
GType               md_file_info_array_get_type         (void) G_GNUC_CONST;

void                md_document_init                    (MdDocument     *doc,
                                                         MdDocumentCapabilities caps);

MdWindow           *md_document_get_window              (MdDocument     *doc);
MdManager          *md_document_get_manager             (MdDocument     *doc);

void                md_document_set_modified            (MdDocument     *doc,
                                                         gboolean        modified);
void                md_document_set_status              (MdDocument     *doc,
                                                         MdDocumentStatus status);
MdDocumentStatus    md_document_get_status              (MdDocument     *doc);

void                md_document_set_file_info           (MdDocument     *doc,
                                                         MdFileInfo     *file_info);
MdFileInfo         *md_document_get_file_info           (MdDocument     *doc);
const char         *md_document_get_filename            (MdDocument     *doc);
const char         *md_document_get_uri                 (MdDocument     *doc);
const char         *md_document_get_display_name        (MdDocument     *doc);
const char         *md_document_get_display_basename    (MdDocument     *doc);

MdFileInfo         *md_file_info_new                    (const char     *uri);
MdFileInfo         *md_file_info_copy                   (MdFileInfo     *file_info);
void                md_file_info_free                   (MdFileInfo     *file_info);
void                md_file_info_set                    (MdFileInfo     *file_info,
                                                         const char     *key,
                                                         const char     *value);
const char         *md_file_info_get                    (MdFileInfo     *file_info,
                                                         const char     *key);
MdFileInfo        **md_file_info_array_new_uri          (const char     *uri);
MdFileInfo        **md_file_info_array_new_uri_list     (char          **uris);
MdFileInfo        **md_file_info_array_copy             (MdFileInfo    **files);
void                md_file_info_array_free             (MdFileInfo    **files);

const char         *md_file_info_get_uri                (MdFileInfo     *file_info);
gboolean            md_file_info_is_local               (MdFileInfo     *file_info);
char               *md_file_info_get_filename           (MdFileInfo     *file_info);
char               *md_file_info_get_display_name       (MdFileInfo     *file_info);


#endif /* MD_DOCUMENT_H */
