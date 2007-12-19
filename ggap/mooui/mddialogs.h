#ifndef MOO_DOC_DIALOGS_H
#define MOO_DOC_DIALOGS_H

#include <mooui/mdtypes.h>


typedef enum {
    MD_SAVE_CHANGES_RESPONSE_CANCEL,
    MD_SAVE_CHANGES_RESPONSE_SAVE,
    MD_SAVE_CHANGES_RESPONSE_DONT_SAVE
} MdSaveChangesDialogResponse;

MdSaveChangesDialogResponse
                md_save_changes_dialog          (MdView     *view);
MdSaveChangesDialogResponse
                md_save_multiple_changes_dialog (GSList     *views,
                                                 GSList    **to_save);
MdFileInfo     *md_save_as_dialog               (MdView     *view);
MdFileInfo    **md_open_dialog                  (MdWindow   *window);

void            md_open_error_dialog            (GtkWidget  *widget,
                                                 MdFileInfo *file,
                                                 GError     *error);
void            md_save_error_dialog            (GtkWidget  *widget,
                                                 MdFileInfo *file,
                                                 const char *error);


#endif /* MOO_DOC_DIALOGS_H */
