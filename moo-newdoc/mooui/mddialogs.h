/*
 *   mddialogs.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef MOO_DOC_DIALOGS_H
#define MOO_DOC_DIALOGS_H

#include <mooui/mdtypes.h>


#define MD_TYPE_PASSWORD_DIALOG     (md_password_dialog_get_type ())
#define MD_PASSWORD_DIALOG(obj)     (G_TYPE_CHECK_INSTANCE_CAST ((obj), MD_TYPE_PASSWORD_DIALOG, MdPasswordDialog))
#define MD_IS_PASSWORD_DIALOG(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MD_TYPE_PASSWORD_DIALOG))

typedef struct MdPasswordDialog MdPasswordDialog;
typedef struct MdPasswordDialogClass MdPasswordDialogClass;

GType       md_password_dialog_get_type             (void) G_GNUC_CONST;
GtkWidget  *md_password_dialog_new                  (GtkWidget          *parent,
                                                     const char         *message,
                                                     const char         *username,
                                                     gboolean            readonly_username);
void        md_password_dialog_set_show_username    (MdPasswordDialog   *dialog,
                                                     gboolean            show);
void        md_password_dialog_set_show_password    (MdPasswordDialog   *dialog,
                                                     gboolean            show);
void        md_password_dialog_set_show_domain      (MdPasswordDialog   *dialog,
                                                     gboolean            show);
void        md_password_dialog_set_show_anonymous   (MdPasswordDialog   *dialog,
                                                     gboolean            show);
void        md_password_dialog_set_domain           (MdPasswordDialog   *dialog,
                                                     const char         *domain);
const char *md_password_dialog_get_username         (MdPasswordDialog   *dialog);
const char *md_password_dialog_get_domain           (MdPasswordDialog   *dialog);
const char *md_password_dialog_get_password         (MdPasswordDialog   *dialog);
gboolean    md_password_dialog_anon_selected        (MdPasswordDialog   *dialog);

typedef enum {
    MD_SAVE_CHANGES_RESPONSE_CANCEL,
    MD_SAVE_CHANGES_RESPONSE_SAVE,
    MD_SAVE_CHANGES_RESPONSE_DONT_SAVE
} MdSaveChangesDialogResponse;

MdSaveChangesDialogResponse
                md_save_changes_dialog          (MdDocument *doc);
MdSaveChangesDialogResponse
                md_save_multiple_changes_dialog (GSList     *docs,
                                                 GSList    **to_save);
MdFileInfo     *md_save_as_dialog               (MdDocument *doc);
MdFileInfo    **md_open_dialog                  (MdWindow   *window);
MdFileInfo    **md_open_uri_dialog              (MdWindow   *window);

void            md_open_error_dialog            (GtkWidget  *widget,
                                                 MdFileInfo *file,
                                                 GError     *error);
void            md_save_error_dialog            (GtkWidget  *widget,
                                                 MdFileInfo *file,
                                                 GError     *error);


#endif /* MOO_DOC_DIALOGS_H */
