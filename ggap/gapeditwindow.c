/*
 *   gapeditwindow.c
 *
 *   Copyright (C) 2004-2006 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "gapeditwindow.h"
#include "gapapp.h"
#include "gap.h"
#include "mooutils/mooutils-misc.h"
#include <gtk/gtkstock.h>


struct _GapEditWindowPrivate {
};


static GHashTable *tmp_to_real;    /* char* -> MooEdit* */
static GHashTable *real_to_tmp;    /* MooEdit* -> GSList* (char*) */


static void     gap_edit_window_class_init  (GapEditWindowClass *klass);
static void     gap_edit_window_init        (GapEditWindow      *window);
static void     gap_edit_window_destroy     (GtkObject          *object);

static void     gap_edit_window_close_doc   (MooEditWindow      *window,
                                             MooEdit            *doc);

static void     gap_edit_window_send_file   (GapEditWindow      *window);
static void     gap_edit_window_send_copy   (GapEditWindow      *window);
static void     gap_edit_window_send_selection (GapEditWindow   *window);


/* GAP_TYPE_EDIT_WINDOW */
G_DEFINE_TYPE (GapEditWindow, gap_edit_window, MOO_TYPE_EDIT_WINDOW)


static void gap_edit_window_class_init (GapEditWindowClass *klass)
{
    GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);
    MooEditWindowClass *edit_window_class = MOO_EDIT_WINDOW_CLASS (klass);

    tmp_to_real = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    real_to_tmp = g_hash_table_new (g_direct_hash, g_direct_equal);

    moo_window_class_set_id (window_class, "Editor", "Editor");

    gtkobject_class->destroy = gap_edit_window_destroy;
    edit_window_class->close_doc = gap_edit_window_close_doc;

    if (!GAP_APP_EDITOR_MODE)
    {
        moo_window_class_new_action (window_class, "SendFile",
                                     "display-name", "Send File",
                                     "label", "Send File",
                                     "tooltip", "Send File",
                                     "stock-id", GTK_STOCK_EXECUTE,
                                     "closure-callback", gap_edit_window_send_file,
                                     "condition::sensitive", "has-open-document",
                                     NULL);
        moo_window_class_new_action (window_class, "SendCopy",
                                     "display-name", "Send Copy",
                                     "label", "Send Copy",
                                     "tooltip", "Send Copy",
                                     "stock-id", GTK_STOCK_CONVERT,
                                     "closure-callback", gap_edit_window_send_copy,
                                     "condition::sensitive", "has-open-document",
                                     NULL);
        moo_window_class_new_action (window_class, "SendSelection",
                                     "display-name", "Send Selection",
                                     "label", "Send Selection",
                                     "tooltip", "Send Selection",
                                     "stock-id", GTK_STOCK_JUMP_TO,
                                     "closure-callback", gap_edit_window_send_selection,
                                     "condition::sensitive", "has-open-document",
                                     NULL);
    }
}


static void gap_edit_window_init (GapEditWindow *window)
{
    window->priv = g_new0 (GapEditWindowPrivate, 1);
}


static void
gap_edit_window_destroy (GtkObject          *object)
{
    GapEditWindow *window = GAP_EDIT_WINDOW (object);

    if (window->priv)
    {
        g_free (window->priv);
        window->priv = NULL;
    }

    GTK_OBJECT_CLASS(gap_edit_window_parent_class)->destroy (object);
}


static void
send_and_bring_to_front (const char *string)
{
    gap_app_feed_gap (GAP_APP_INSTANCE, string);
    moo_window_present (GTK_WINDOW (GAP_APP_INSTANCE->term_window));
}


static void
send_filename (const char *filename)
{
    char *cmd = gap_read_file_string (filename);
    send_and_bring_to_front (cmd);
    g_free (cmd);
}


static void
send_copy (GapEditWindow *window,
           MooEdit       *doc)
{
    MooEditor *editor;
    MooApp *app;
    char *filename;
    GError *error = NULL;

    g_return_if_fail (doc != NULL);

    app = moo_app_get_instance ();
    editor = moo_edit_window_get_editor (MOO_EDIT_WINDOW (window));

    filename = moo_app_tempnam (app);
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
gap_edit_window_send_copy (GapEditWindow *window)
{
    MooEdit *doc;

    doc = moo_edit_window_get_active_doc (MOO_EDIT_WINDOW (window));
    g_return_if_fail (doc != NULL);
    send_copy (window, doc);
}


static void
gap_edit_window_send_file (GapEditWindow *window)
{
    MooEdit *doc;
    const char *filename;

    doc = moo_edit_window_get_active_doc (MOO_EDIT_WINDOW (window));
    g_return_if_fail (doc != NULL);

    filename = moo_edit_get_filename (doc);

    if (filename)
        send_filename (filename);
    else
        send_copy (window, doc);
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

    send_and_bring_to_front (text);

    g_free (text);
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
