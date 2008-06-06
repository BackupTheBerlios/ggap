/*
 *   gapeditwindow.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "ggap-i18n.h"
#include "gapeditwindow.h"
#include "gapapp.h"
#include "gap.h"
#include "mooedit/mooeditor.h"
#include "mooutils/mooutils-misc.h"
#include <gtk/gtkstock.h>


static GHashTable *tmp_to_real;    /* char* -> MooEdit* */
static GHashTable *real_to_tmp;    /* MooEdit* -> GSList* (char*) */


static void     gap_edit_window_close_doc   (MooEditWindow      *window,
                                             MooEdit            *doc);
static void     gap_edit_window_send_file   (GapEditWindow      *window);
static void     gap_edit_window_send_selection (GapEditWindow   *window);


/* GAP_TYPE_EDIT_WINDOW */
G_DEFINE_TYPE (GapEditWindow, gap_edit_window, MOO_TYPE_EDIT_WINDOW)


static void
gap_edit_window_class_init (GapEditWindowClass *klass)
{
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);
    MooEditWindowClass *edit_window_class = MOO_EDIT_WINDOW_CLASS (klass);

    tmp_to_real = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    real_to_tmp = g_hash_table_new (g_direct_hash, g_direct_equal);

    edit_window_class->close_doc = gap_edit_window_close_doc;

    moo_window_class_new_action (window_class, "SendFile", NULL,
                                 "display-name", _("Send File"),
                                 "label", _("Send File"),
                                 "tooltip", _("Send file"),
                                 "stock-id", GTK_STOCK_EXECUTE,
                                 "closure-callback", gap_edit_window_send_file,
                                 "condition::sensitive", "has-open-document",
                                 NULL);
    moo_window_class_new_action (window_class, "SendSelection", NULL,
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
    moo_window_set_global_accels (MOO_WINDOW (window), TRUE);
}


static void
send_filename (const char *filename)
{
    char *cmd = gap_read_file_string (filename);
    gap_app_send_and_bring_to_front (cmd);
    g_free (cmd);
}


static void
send_copy (GapEditWindow *window,
           MooEdit       *doc)
{
    MooEditor *editor;
    char *filename;
    GError *error = NULL;

    g_return_if_fail (doc != NULL);

    editor = moo_edit_window_get_editor (MOO_EDIT_WINDOW (window));

    filename = moo_tempnam ();
    g_return_if_fail (filename != NULL);

    if (moo_editor_save_copy (editor, doc, filename, NULL, &error))
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
    const char *filename;

    doc = moo_edit_window_get_active_doc (MOO_EDIT_WINDOW (window));
    g_return_if_fail (doc != NULL);

    filename = moo_edit_get_filename (doc);

    if (!filename)
        return send_copy (window, doc);

    if (!MOO_EDIT_IS_MODIFIED (doc) || moo_edit_save (doc, NULL))
        send_filename (filename);
}


static void
gap_edit_window_send_selection (GapEditWindow *window)
{
    MooEdit *doc;
    char *text;

    doc = moo_edit_window_get_active_doc (MOO_EDIT_WINDOW (window));
    g_return_if_fail (doc != NULL);

    text = moo_text_view_get_selection (MOO_TEXT_VIEW (doc));

    if (!text)
        text = moo_text_view_get_text (MOO_TEXT_VIEW (doc));

    if (text)
    {
        gap_app_send_and_bring_to_front (text);
        g_free (text);
    }
}


static void
gap_edit_window_close_doc (MooEditWindow      *window,
                           MooEdit            *doc)
{
    GSList *list, *l;

    list = g_hash_table_lookup (real_to_tmp, doc);

    if (list)
    {
        for (l = list; l != NULL; l = l->next)
            g_hash_table_remove (tmp_to_real, l->data);
        g_slist_foreach (list, (GFunc) g_free, NULL);
        g_slist_free (list);
        g_hash_table_remove (real_to_tmp, doc);
    }

    if (MOO_EDIT_WINDOW_CLASS(gap_edit_window_parent_class)->close_doc)
        MOO_EDIT_WINDOW_CLASS(gap_edit_window_parent_class)->close_doc (window, doc);
}


void
gap_edit_window_open_file (const char *filename,
                           int         line,
                           GtkWidget  *widget)
{
    MooEdit *doc;
    MooEditor *editor;

    g_return_if_fail (filename != NULL);

    editor = moo_editor_instance ();
    doc = g_hash_table_lookup (tmp_to_real, filename);

    if (!doc)
    {
        if (moo_editor_open_file (editor, NULL, widget, filename, NULL))
            doc = moo_editor_get_doc (editor, filename);
    }

    if (doc)
    {
        moo_editor_set_active_doc (editor, doc);
        moo_text_view_move_cursor (MOO_TEXT_VIEW (doc), line, 0, FALSE, TRUE);
    }
}
