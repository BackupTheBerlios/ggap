
/* Generated data (by glib-mkenums) */

#include "mdenums.h"

#include "mddialogs.h"

GType
md_save_changes_dialog_response_get_type (void)
{
    static GType etype;
    if (G_UNLIKELY (!etype))
    {
        static const GEnumValue values[] = {
            { MD_SAVE_CHANGES_RESPONSE_CANCEL, (char*) "MD_SAVE_CHANGES_RESPONSE_CANCEL", (char*) "cancel" },
            { MD_SAVE_CHANGES_RESPONSE_SAVE, (char*) "MD_SAVE_CHANGES_RESPONSE_SAVE", (char*) "save" },
            { MD_SAVE_CHANGES_RESPONSE_DONT_SAVE, (char*) "MD_SAVE_CHANGES_RESPONSE_DONT_SAVE", (char*) "dont-save" },
            { 0, NULL, NULL }
        };
        etype = g_enum_register_static ("MdSaveChangesDialogResponse", values);
    }
    return etype;
}

#include "mddocument.h"

GType
md_document_capabilities_get_type (void)
{
    static GType etype;
    if (G_UNLIKELY (!etype))
    {
        static const GFlagsValue values[] = {
            { MD_DOCUMENT_SUPPORTS_SAVE, (char*) "MD_DOCUMENT_SUPPORTS_SAVE", (char*) "save" },
            { MD_DOCUMENT_SUPPORTS_PRINTING, (char*) "MD_DOCUMENT_SUPPORTS_PRINTING", (char*) "printing" },
            { MD_DOCUMENT_SUPPORTS_EVERYTHING, (char*) "MD_DOCUMENT_SUPPORTS_EVERYTHING", (char*) "everything" },
            { 0, NULL, NULL }
        };
        etype = g_flags_register_static ("MdDocumentCapabilities", values);
    }
    return etype;
}

GType
md_document_status_get_type (void)
{
    static GType etype;
    if (G_UNLIKELY (!etype))
    {
        static const GFlagsValue values[] = {
            { MD_DOCUMENT_MODIFIED_ON_DISK, (char*) "MD_DOCUMENT_MODIFIED_ON_DISK", (char*) "modified-on-disk" },
            { MD_DOCUMENT_DELETED, (char*) "MD_DOCUMENT_DELETED", (char*) "deleted" },
            { MD_DOCUMENT_MODIFIED, (char*) "MD_DOCUMENT_MODIFIED", (char*) "modified" },
            { MD_DOCUMENT_NEW, (char*) "MD_DOCUMENT_NEW", (char*) "new" },
            { MD_DOCUMENT_CHANGED_ON_DISK, (char*) "MD_DOCUMENT_CHANGED_ON_DISK", (char*) "changed-on-disk" },
            { 0, NULL, NULL }
        };
        etype = g_flags_register_static ("MdDocumentStatus", values);
    }
    return etype;
}

GType
md_document_state_get_type (void)
{
    static GType etype;
    if (G_UNLIKELY (!etype))
    {
        static const GEnumValue values[] = {
            { MD_DOCUMENT_STATE_NORMAL, (char*) "MD_DOCUMENT_STATE_NORMAL", (char*) "normal" },
            { MD_DOCUMENT_STATE_LOADING, (char*) "MD_DOCUMENT_STATE_LOADING", (char*) "loading" },
            { MD_DOCUMENT_STATE_SAVING, (char*) "MD_DOCUMENT_STATE_SAVING", (char*) "saving" },
            { MD_DOCUMENT_STATE_PRINTING, (char*) "MD_DOCUMENT_STATE_PRINTING", (char*) "printing" },
            { 0, NULL, NULL }
        };
        etype = g_enum_register_static ("MdDocumentState", values);
    }
    return etype;
}

GType
md_file_op_type_get_type (void)
{
    static GType etype;
    if (G_UNLIKELY (!etype))
    {
        static const GEnumValue values[] = {
            { MD_FILE_OP_OPEN, (char*) "MD_FILE_OP_OPEN", (char*) "open" },
            { MD_FILE_OP_RELOAD, (char*) "MD_FILE_OP_RELOAD", (char*) "reload" },
            { MD_FILE_OP_SAVE, (char*) "MD_FILE_OP_SAVE", (char*) "save" },
            { MD_FILE_OP_SAVE_COPY, (char*) "MD_FILE_OP_SAVE_COPY", (char*) "save-copy" },
            { 0, NULL, NULL }
        };
        etype = g_enum_register_static ("MdFileOpType", values);
    }
    return etype;
}

#include "mdmanager.h"

GType
md_close_all_result_get_type (void)
{
    static GType etype;
    if (G_UNLIKELY (!etype))
    {
        static const GEnumValue values[] = {
            { MD_CLOSE_ALL_DONE, (char*) "MD_CLOSE_ALL_DONE", (char*) "done" },
            { MD_CLOSE_ALL_CANCELLED, (char*) "MD_CLOSE_ALL_CANCELLED", (char*) "cancelled" },
            { MD_CLOSE_ALL_IN_PROGRESS, (char*) "MD_CLOSE_ALL_IN_PROGRESS", (char*) "in-progress" },
            { 0, NULL, NULL }
        };
        etype = g_enum_register_static ("MdCloseAllResult", values);
    }
    return etype;
}

#include "mdtypes.h"

GType
md_file_op_status_get_type (void)
{
    static GType etype;
    if (G_UNLIKELY (!etype))
    {
        static const GEnumValue values[] = {
            { MD_FILE_OP_STATUS_IN_PROGRESS, (char*) "MD_FILE_OP_STATUS_IN_PROGRESS", (char*) "in-progress" },
            { MD_FILE_OP_STATUS_ERROR, (char*) "MD_FILE_OP_STATUS_ERROR", (char*) "error" },
            { MD_FILE_OP_STATUS_SUCCESS, (char*) "MD_FILE_OP_STATUS_SUCCESS", (char*) "success" },
            { MD_FILE_OP_STATUS_CANCELLED, (char*) "MD_FILE_OP_STATUS_CANCELLED", (char*) "cancelled" },
            { 0, NULL, NULL }
        };
        etype = g_enum_register_static ("MdFileOpStatus", values);
    }
    return etype;
}


/* Generated data ends here */

