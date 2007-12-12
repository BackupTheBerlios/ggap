#include "mddialogs.h"
#include "ggap-i18n.h"
#include "mooedit/mooeditsavemult-glade.h"
#include "mooutils/moodialogs.h"
#include "mooutils/mooglade.h"
#include "mooutils/moostock.h"
#include "mooutils/mooprefs.h"
#include <gtk/gtk.h>


MdSaveChangesDialogResponse
md_save_changes_dialog (MdDocument *doc)
{
    GtkWidget *parent;
    GtkWidget *dialog;
    int response;
    const char *display_name;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc),
                          MD_SAVE_CHANGES_RESPONSE_CANCEL);

    parent = gtk_widget_get_toplevel (GTK_WIDGET (doc));
    display_name = md_document_get_display_basename (doc);
    dialog = gtk_message_dialog_new (parent ? GTK_WINDOW (parent) : NULL,
                                     GTK_DIALOG_MODAL,
                                     GTK_MESSAGE_WARNING,
                                     GTK_BUTTONS_NONE,
                                     display_name ?
                                        _("Save changes to document \"%s\" before closing?") :
                                        _("Save changes to the document before closing?"),
                                     display_name);

    gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
                                              _("If you don't save, changes will be discarded"));

    gtk_dialog_add_buttons (GTK_DIALOG (dialog),
                            GTK_STOCK_DISCARD, GTK_RESPONSE_NO,
                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                            GTK_STOCK_SAVE, GTK_RESPONSE_YES,
                            NULL);

    gtk_dialog_set_alternative_button_order (GTK_DIALOG (dialog),
                                             GTK_RESPONSE_YES,
                                             GTK_RESPONSE_NO,
                                             GTK_RESPONSE_CANCEL,
                                             -1);

    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);

    if (parent)
        moo_window_set_parent (dialog, parent);

    response = gtk_dialog_run (GTK_DIALOG (dialog));
    if (response == GTK_RESPONSE_DELETE_EVENT)
        response = GTK_RESPONSE_CANCEL;
    gtk_widget_destroy (dialog);

    switch (response)
    {
        case GTK_RESPONSE_NO:
            return MD_SAVE_CHANGES_RESPONSE_DONT_SAVE;
        case GTK_RESPONSE_CANCEL:
            return MD_SAVE_CHANGES_RESPONSE_CANCEL;
        case GTK_RESPONSE_YES:
            return MD_SAVE_CHANGES_RESPONSE_SAVE;
    }

    g_return_val_if_reached (MD_SAVE_CHANGES_RESPONSE_CANCEL);
}


/****************************************************************************/
/* Save multiple
 */

enum {
    COLUMN_SAVE = 0,
    COLUMN_DOC,
    NUM_COLUMNS
};

static void
name_data_func (G_GNUC_UNUSED GtkTreeViewColumn *column,
                GtkCellRenderer   *cell,
                GtkTreeModel      *model,
                GtkTreeIter       *iter)
{
    MdDocument *doc = NULL;

    gtk_tree_model_get (model, iter, COLUMN_DOC, &doc, -1);
    g_return_if_fail (MD_IS_DOCUMENT (doc));

    g_object_set (cell, "text", md_document_get_display_basename (doc), NULL);
    g_object_unref (doc);
}


static void
save_toggled (GtkCellRendererToggle *cell,
              gchar                 *path,
              GtkTreeModel          *model)
{
    GtkTreePath *tree_path;
    GtkTreeIter iter;
    gboolean save = TRUE;
    gboolean active;
    gboolean sensitive;
    GtkDialog *dialog;

    g_return_if_fail (GTK_IS_LIST_STORE (model));

    tree_path = gtk_tree_path_new_from_string (path);
    g_return_if_fail (tree_path != NULL);

    gtk_tree_model_get_iter (model, &iter, tree_path);
    gtk_tree_model_get (model, &iter, COLUMN_SAVE, &save, -1);

    active = gtk_cell_renderer_toggle_get_active (cell);

    if (active == save)
        gtk_list_store_set (GTK_LIST_STORE (model), &iter, COLUMN_SAVE, !save, -1);

    gtk_tree_path_free (tree_path);

    dialog = g_object_get_data (G_OBJECT (model), "moo-dialog");
    g_return_if_fail (dialog != NULL);

    if (!save)
    {
        sensitive = TRUE;
    }
    else
    {
        sensitive = FALSE;
        gtk_tree_model_get_iter_first (model, &iter);

        do
        {
            gtk_tree_model_get (model, &iter, COLUMN_SAVE, &save, -1);
            if (save)
            {
                sensitive = TRUE;
                break;
            }
        }
        while (gtk_tree_model_iter_next (model, &iter));
    }

    gtk_dialog_set_response_sensitive (dialog, GTK_RESPONSE_YES, sensitive);
}

static void
files_treeview_init (GtkTreeView *treeview, GtkWidget *dialog, GSList  *docs)
{
    GtkListStore *store;
    GtkTreeViewColumn *column;
    GtkCellRenderer *cell;
    GSList *l;

    store = gtk_list_store_new (NUM_COLUMNS, G_TYPE_BOOLEAN, MD_TYPE_DOCUMENT);

    for (l = docs; l != NULL; l = l->next)
    {
        GtkTreeIter iter;
        gtk_list_store_append (store, &iter);
        gtk_list_store_set (store, &iter,
                            COLUMN_SAVE, TRUE,
                            COLUMN_DOC, l->data,
                            -1);
    }

    gtk_tree_view_set_model (treeview, GTK_TREE_MODEL (store));

    column = gtk_tree_view_column_new ();
    gtk_tree_view_append_column (treeview, column);
    cell = gtk_cell_renderer_toggle_new ();
    gtk_tree_view_column_pack_start (column, cell, FALSE);
    g_object_set (cell, "activatable", TRUE, NULL);
    gtk_tree_view_column_add_attribute (column, cell, "active", COLUMN_SAVE);
    g_signal_connect (cell, "toggled", G_CALLBACK (save_toggled), store);

    column = gtk_tree_view_column_new ();
    gtk_tree_view_append_column (treeview, column);
    cell = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (column, cell, TRUE);
    gtk_tree_view_column_set_cell_data_func (column, cell,
                                             (GtkTreeCellDataFunc) name_data_func,
                                             NULL, NULL);

    g_object_set_data (G_OBJECT (store), "moo-dialog", dialog);

    g_object_unref (store);
}


static GSList *
files_treeview_get_to_save (GtkWidget *treeview)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GSList *list = NULL;

    g_return_val_if_fail (GTK_IS_TREE_VIEW (treeview), NULL);

    model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
    g_return_val_if_fail (model != NULL, NULL);

    gtk_tree_model_get_iter_first (model, &iter);

    do
    {
        MdDocument *doc = NULL;
        gboolean save = TRUE;

        gtk_tree_model_get (model, &iter,
                            COLUMN_SAVE, &save,
                            COLUMN_DOC, &doc, -1);
        g_return_val_if_fail (MD_IS_DOCUMENT (doc), list);

        if (save)
            list = g_slist_prepend (list, doc);

        g_object_unref (doc);
    }
    while (gtk_tree_model_iter_next (model, &iter));

    return g_slist_reverse (list);
}

MdSaveChangesDialogResponse
md_save_multiple_changes_dialog (GSList  *docs,
                                 GSList **to_save)
{
    GSList *l;
    GtkWidget *dialog, *label, *treeview;
    char *msg, *question;
    int response;
    MdSaveChangesDialogResponse retval;
    MooGladeXML *xml;

    g_return_val_if_fail (docs != NULL, MD_SAVE_CHANGES_RESPONSE_CANCEL);
    g_return_val_if_fail (to_save != NULL, MD_SAVE_CHANGES_RESPONSE_CANCEL);

    for (l = docs; l != NULL; l = l->next)
        g_return_val_if_fail (MD_IS_DOCUMENT (l->data), MD_SAVE_CHANGES_RESPONSE_CANCEL);

    xml = moo_glade_xml_new_from_buf (mooeditsavemult_glade_xml, -1,
                                      "dialog", GETTEXT_PACKAGE, NULL);
    dialog = moo_glade_xml_get_widget (xml, "dialog");

    moo_window_set_parent (dialog, docs->data);

    gtk_dialog_add_buttons (GTK_DIALOG (dialog),
                            MOO_STOCK_SAVE_NONE, GTK_RESPONSE_NO,
                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                            MOO_STOCK_SAVE_SELECTED, GTK_RESPONSE_YES,
                            NULL);

    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);

    gtk_dialog_set_alternative_button_order (GTK_DIALOG (dialog),
                                             GTK_RESPONSE_YES,
                                             GTK_RESPONSE_NO,
                                             GTK_RESPONSE_CANCEL, -1);

    label = moo_glade_xml_get_widget (xml, "label");
    question = g_strdup_printf (dngettext (GETTEXT_PACKAGE,
                                           /* Translators: number of documents here is always greater than one, so
                                              ignore singular form (which is simply copy of the plural here) */
                                           "There are %u documents with unsaved changes. "
                                            "Save changes before closing?",
                                           "There are %u documents with unsaved changes. "
                                            "Save changes before closing?",
                                           g_slist_length (docs)),
                                g_slist_length (docs));
    msg = g_markup_printf_escaped ("<span weight=\"bold\" size=\"larger\">%s</span>",
                                   question);
    gtk_label_set_markup (GTK_LABEL (label), msg);

    treeview = moo_glade_xml_get_widget (xml, "treeview");
    files_treeview_init (GTK_TREE_VIEW (treeview), dialog, docs);

    response = gtk_dialog_run (GTK_DIALOG (dialog));

    switch (response)
    {
        case GTK_RESPONSE_NO:
            retval = MD_SAVE_CHANGES_RESPONSE_DONT_SAVE;
            break;
        case GTK_RESPONSE_YES:
            *to_save = files_treeview_get_to_save (treeview);
            retval = MD_SAVE_CHANGES_RESPONSE_SAVE;
            break;
        default:
            retval = MD_SAVE_CHANGES_RESPONSE_CANCEL;
    }

    g_free (question);
    g_free (msg);
    gtk_widget_destroy (dialog);
    g_object_unref (xml);
    return retval;
}


MdFileInfo *
md_save_as_dialog (MdDocument *doc)
{
    const char *start = NULL;
    const char *uri = NULL;
    MooFileDialog *dialog;
    MdFileInfo *file_info;

//     moo_prefs_create_key (moo_edit_setting (MD_EDIT_PREFS_LAST_DIR),
//                           MD_PREFS_STATE, G_TYPE_STRING, NULL);
//     start = moo_prefs_get_filename (moo_edit_setting (MD_EDIT_PREFS_LAST_DIR));

    dialog = moo_file_dialog_new (MOO_FILE_DIALOG_SAVE, GTK_WIDGET (doc),
                                  FALSE, GTK_STOCK_SAVE_AS, start,
                                  md_document_get_display_basename (doc));
    g_object_set (dialog, "enable-encodings", TRUE, NULL);
    moo_file_dialog_set_help_id (dialog, "dialog-save");

    if (!moo_file_dialog_run (dialog))
    {
        g_object_unref (dialog);
        return NULL;
    }

    uri = moo_file_dialog_get_uri (dialog);
    g_return_val_if_fail (uri != NULL, NULL);
    file_info = md_file_info_new (uri);

    g_object_unref (dialog);
    return file_info;
}


MdFileInfo **
md_open_dialog (MdWindow *window)
{
    MooFileDialog *dialog;
    const char *start_dir = NULL;
    char *freeme = NULL;
    GSList *uris = NULL;
    MdFileInfo **infos;
    GSList *l;
    guint i, n_elms;

    dialog = moo_file_dialog_new (MOO_FILE_DIALOG_OPEN, GTK_WIDGET (window),
                                  TRUE, GTK_STOCK_OPEN, start_dir,
                                  NULL);
    g_object_set (dialog, "enable-encodings", TRUE, NULL);
    moo_file_dialog_set_help_id (dialog, "dialog-open");

    if (!moo_file_dialog_run (dialog))
        goto out;

    uris = moo_file_dialog_get_uris (dialog);
    g_return_val_if_fail (uris != NULL, NULL);

    n_elms = g_slist_length (uris);
    infos = g_new (MdFileInfo*, n_elms + 1);
    infos[n_elms] = NULL;

    for (l = uris, i = 0; l != NULL; l = l->next, i++)
        infos[i] = md_file_info_new (l->data);

out:
    g_free (freeme);
    g_object_unref (dialog);
    g_slist_foreach (uris, (GFunc) g_free, NULL);
    g_slist_free (uris);
    return infos;
}


void
md_open_error_dialog (GtkWidget   *widget,
                      MdFileInfo *file,
                      GError      *error)
{
    char *filename_utf8, *msg = NULL;
    char *secondary;

    filename_utf8 = md_file_info_get_display_name (file);

    if (!filename_utf8)
        g_critical ("%s: could not convert filename to utf8", G_STRLOC);

    if (filename_utf8)
        /* Could not open file foo.txt */
        msg = g_strdup_printf (_("Could not open file\n%s"), filename_utf8);
    else
        msg = g_strdup (_("Could not open file"));

    secondary = error ? g_strdup (error->message) : NULL;

    moo_error_dialog (widget, msg, secondary);

    g_free (msg);
    g_free (secondary);
    g_free (filename_utf8);
}


void
md_save_error_dialog (GtkWidget   *widget,
                      MdFileInfo *file,
                      GError      *error)
{
    char *filename_utf8, *msg = NULL;

    filename_utf8 = md_file_info_get_display_name (file);

    if (!filename_utf8)
        g_critical ("%s: could not convert filename to utf8", G_STRLOC);

    if (filename_utf8)
        /* Could not save file foo.txt */
        msg = g_strdup_printf (_("Could not save file\n%s"), filename_utf8);
    else
        msg = g_strdup (_("Could not save file"));

    moo_error_dialog (widget, msg, error ? error->message : NULL);

    g_free (msg);
    g_free (filename_utf8);
}
