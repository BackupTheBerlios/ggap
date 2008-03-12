/*
 *   gapeditwindow.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "ggap-i18n.h"
#include "gapeditwindow.h"
#include "gapapp.h"
#include "gap.h"
#include "mooedit/mooeditor.h"
#include "mooedit/mooedit.h"
#include "mooedit/mootextview.h"
#include "mooutils/mooutils-misc.h"
#include <gtk/gtkstock.h>


static GHashTable *tmp_to_real;    /* char* -> MooEditView* */
static GHashTable *real_to_tmp;    /* MooEditView* -> GSList* (char*) */


static void     gap_edit_window_remove_view (MdWindow           *window,
                                             MdView             *view);
static void     gap_edit_window_send_file   (GapEditWindow      *window);
static void     gap_edit_window_send_selection (GapEditWindow   *window);


/* GAP_TYPE_EDIT_WINDOW */
G_DEFINE_TYPE (GapEditWindow, gap_edit_window, MOO_TYPE_EDIT_WINDOW)


static void
gap_edit_window_class_init (GapEditWindowClass *klass)
{
    MdWindowClass *md_window_class = MD_WINDOW_CLASS (klass);
    MdAppWindowClass *window_class = MD_APP_WINDOW_CLASS (klass);

    tmp_to_real = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    real_to_tmp = g_hash_table_new (g_direct_hash, g_direct_equal);

    md_app_window_class_set_id (window_class, "Editor", "Editor");

    md_window_class->remove_view = gap_edit_window_remove_view;

    md_app_window_class_new_action (window_class, "SendFile", NULL,
                                    "display-name", _("Send File"),
                                    "label", _("Send File"),
                                    "tooltip", _("Send file"),
                                    "stock-id", GTK_STOCK_EXECUTE,
                                    "closure-callback", gap_edit_window_send_file,
                                    "condition::sensitive", "has-open-document",
                                    NULL);
    md_app_window_class_new_action (window_class, "SendSelection", NULL,
                                    "display-name", _("Send Selection"),
                                    "label", _("Send Selection"),
                                    "tooltip", _("Send selection"),
                                    "stock-id", GTK_STOCK_JUMP_TO,
                                    "closure-callback", gap_edit_window_send_selection,
                                    "condition::sensitive", "has-open-document",
                                    "accel", "<ctrl>Return",
                                    NULL);
}


static void
gap_edit_window_init (GapEditWindow *window)
{
    md_app_window_set_global_accels (MD_APP_WINDOW (window), TRUE);
}


static void
send_filename (const char *filename)
{
    char *cmd = gap_read_file_string (filename);
    gap_app_send_and_bring_to_front (cmd);
    g_free (cmd);
}


static void
send_copy (MooEdit *doc)
{
    MooEditor *editor;
    MdApp *app;
    char *filename;
    GError *error = NULL;

    g_return_if_fail (doc != NULL);

    app = md_app_instance ();
    editor = moo_editor_instance ();

    filename = moo_tempnam ();
    g_return_if_fail (filename != NULL);

    if (moo_edit_save_copy (doc, filename, NULL, &error))
    {
        GSList *list;

        g_hash_table_insert (tmp_to_real, g_strdup (filename), doc);

        list = g_hash_table_lookup (real_to_tmp, doc);
        list = g_slist_prepend (list, g_strdup (filename));
        g_hash_table_insert (real_to_tmp, doc, list);

        send_filename (filename);
    }
    else
    {
        g_warning ("%s: could not save file '%s'", G_STRLOC, filename);

        if (error)
        {
            g_warning ("%s: %s", G_STRLOC, error->message);
            g_error_free (error);
        }
    }

    g_free (filename);
}


static void
gap_edit_window_send_file (GapEditWindow *window)
{
    MooEdit *doc;
    char *filename;

    doc = moo_edit_window_get_active_doc (MOO_EDIT_WINDOW (window));
    g_return_if_fail (doc != NULL);

    filename = md_document_get_filename (MD_DOCUMENT (doc));

    if (!filename)
        send_copy (doc);
    else if (!md_document_get_modified (MD_DOCUMENT (doc)) || moo_editor_ui_save (doc))
        send_filename (filename);

    g_free (filename);
}


static void
gap_edit_window_send_selection (GapEditWindow *window)
{
    MooEditView *view;
    char *text;

    view = moo_edit_window_get_active_view (MOO_EDIT_WINDOW (window));
    g_return_if_fail (view != NULL);

    text = moo_text_view_get_selection (MOO_TEXT_VIEW (view));

    if (!text)
        text = moo_text_view_get_text (MOO_TEXT_VIEW (view));

    if (text)
    {
        gap_app_send_and_bring_to_front (text);
        g_free (text);
    }
}


static void
gap_edit_window_remove_view (MdWindow *window,
                             MdView   *view)
{
    GSList *list, *l;

    list = g_hash_table_lookup (real_to_tmp, view);

    if (list)
    {
        for (l = list; l != NULL; l = l->next)
            g_hash_table_remove (tmp_to_real, l->data);
        g_slist_foreach (list, (GFunc) g_free, NULL);
        g_slist_free (list);
        g_hash_table_remove (real_to_tmp, view);
    }

    MD_WINDOW_CLASS (gap_edit_window_parent_class)->remove_view (window, view);
}


void
gap_edit_window_open_file (const char *filename,
                           int         line,
                           GtkWidget  *widget)
{
    MooEditView *view;
    MooEditor *editor;

    g_return_if_fail (filename != NULL);

    editor = moo_editor_instance ();
    view = g_hash_table_lookup (tmp_to_real, filename);

    if (!view)
        view = moo_editor_ui_open_file (filename, NULL, line, NULL, widget);
}
