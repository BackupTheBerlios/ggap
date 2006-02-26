/*
 *   usermenu.c
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

#include "usermenu.h"
#include "gapeditwindow.h"
#include "gaptermwindow.h"
#include "gapapp.h"
#include "usermenu-glade.h"
#include "mooutils/mooprefsdialog.h"
#include "mooutils/moostock.h"
#include "mooutils/mooscript/mooscript-parser.h"
#include <string.h>


#define MENU_XML        "UserMenu"
#define MENU_XML_ROOT   "UserMenu/%s"
#define MENU_ITEM       "item"
#define PROP_LABEL      "label"
#define PROP_ICON       "icon"
#define PROP_HIDDEN     "hidden"


typedef struct {
    char *label;
    char *icon;
    char *script;
    char *action_id;
    guint visible : 1;
} MenuItem;

typedef struct {
    MenuItem **items;
    guint n_items;
    guint n_items_allocd;
    guint merge_id;
} Menu;

typedef struct {
    MooAction base;
    MooWindow *window;
    MenuItem *item;
} UserAction;

typedef struct {
    MooActionClass base;
} UserActionClass;

enum {
    EDITOR = 0,
    TERM = 1
};

static Menu* menus[2];


typedef void (*LoadItemsFunc) (const char *label,
                               const char *icon,
                               const char *script,
                               gboolean    visible,
                               gpointer    data1,
                               gpointer    data2);


static MenuItem *menu_item_new          (const char *label,
                                         const char *icon,
                                         const char *script,
                                         gboolean    visible);
static void      menu_item_free         (MenuItem   *item);

static Menu     *menu_new               (void);

static GType     user_action_get_type   (void) G_GNUC_CONST;
static void      user_action_activate   (MooAction  *action);


G_DEFINE_TYPE (UserAction, user_action, MOO_TYPE_ACTION)


static void
user_action_init (G_GNUC_UNUSED UserAction *action)
{
}

static void
user_action_class_init (UserActionClass *klass)
{
    MooActionClass *action_class = MOO_ACTION_CLASS (klass);
    action_class->activate = user_action_activate;
}


static void
user_action_activate (MooAction *moo_action)
{
    UserAction *action = (UserAction*) moo_action;
    MSContext *ctx;
    MSNode *script;
    MSValue *result;

    if (!action->item->script || !action->item->script[0])
        return;

    script = ms_script_parse (action->item->script);
    g_return_if_fail (script != NULL);

    if (GAP_IS_EDIT_WINDOW (action->window))
        ctx = gap_app_get_editor_context (action->window);
    else
        ctx = gap_app_get_terminal_context (action->window);

    g_return_if_fail (MS_IS_CONTEXT (ctx));
    result = ms_node_eval (script, ctx);

    if (result)
    {
        ms_value_unref (result);
    }
    else
    {
        g_warning ("%s", action->item->script);
        g_warning ("error: %s", ms_context_get_error_msg (ctx));
        ms_context_clear_error (ctx);
    }

    g_object_unref (ctx);
}


static MenuItem *
menu_item_new (const char *label,
               const char *icon,
               const char *script,
               gboolean    visible)
{
    MenuItem *item;

    g_return_val_if_fail (label != NULL, NULL);

    item = g_new0 (MenuItem, 1);
    item->label = g_strdup (label);
    item->icon = g_strdup (icon);
    item->script = g_strdup (script);
    item->visible = visible != 0;

    return item;
}


static void
menu_item_free (MenuItem *item)
{
    if (item)
    {
        g_free (item->label);
        g_free (item->icon);
        g_free (item->script);
        g_free (item->action_id);
        g_free (item);
    }
}


static Menu*
menu_new (void)
{
    Menu *menu = g_new0 (Menu, 1);
    return menu;
}


static void
menu_add_item (Menu     *menu,
               MenuItem *item)
{
    if (menu->n_items == menu->n_items_allocd)
    {
        MenuItem **tmp;

        menu->n_items_allocd = MAX (menu->n_items_allocd * 1.2,
                                    menu->n_items_allocd + 2);
        tmp = g_new (MenuItem*, menu->n_items_allocd);

        if (menu->n_items)
            memcpy (tmp, menu->items, menu->n_items * sizeof (gpointer));

        g_free (menu->items);
        menu->items = tmp;
    }

    menu->items[menu->n_items++] = item;
}


static void
load_items (const char   *name,
            LoadItemsFunc func,
            gpointer      data1,
            gpointer      data2)
{
    MooMarkupDoc *doc;
    MooMarkupNode *root, *node;
    char *path;

    doc = moo_prefs_get_markup ();
    g_return_if_fail (doc != NULL);

    path = g_strdup_printf (MENU_XML_ROOT, name);
    root = moo_markup_get_element (MOO_MARKUP_NODE (doc), path);
    g_free (path);

    if (!root)
        return;

    for (node = root->children; node != NULL; node = node->next)
    {
        const char *label, *icon, *script;
        gboolean hidden;

        if (!MOO_MARKUP_IS_ELEMENT (node))
            continue;

        if (strcmp (node->name, MENU_ITEM))
        {
            g_warning ("%s: invalid element '%s'", G_STRLOC, node->name);
            continue;
        }

        label = moo_markup_get_prop (node, PROP_LABEL);
        icon = moo_markup_get_prop (node, PROP_ICON);
        hidden = moo_markup_get_bool_prop (node, PROP_HIDDEN, FALSE);
        script = moo_markup_get_content (node);

        if (!label)
        {
            g_warning ("%s: label missing", G_STRLOC);
            continue;
        }

        if (!script || !script[0])
            script = NULL;

        func (label, icon, script, !hidden, data1, data2);
    }
}


static void
menu_purge (Menu           *menu,
            MooWindowClass *klass)
{
    MooUIXML *xml;
    guint i;

    xml = moo_app_get_ui_xml (moo_app_get_instance ());

    if (menu->merge_id)
        moo_ui_xml_remove_ui (xml, menu->merge_id);

    menu->merge_id = 0;

    for (i = 0; i < menu->n_items; ++i)
    {
        MenuItem *item = menu->items[i];
        if (item->action_id)
            moo_window_class_remove_action (klass, item->action_id);
        menu_item_free (menu->items[i]);
    }

    menu->n_items = 0;
}


static void
create_menu_item (const char *label,
                  const char *icon,
                  const char *script,
                  gboolean    visible,
                  Menu       *menu)
{
    MenuItem *item = menu_item_new (label, icon, script, visible);
    menu_add_item (menu, item);
}


static MooAction *
create_action (MooWindow *window,
               MenuItem  *item)
{
    UserAction *action;

    action = g_object_new (user_action_get_type(),
                           "label", item->label,
                           "name", item->label,
                           "visible", (gboolean) item->visible,
                           "icon-stock-id", item->icon,
                           NULL);
    action->item = item;
    action->window = window;

    return MOO_ACTION (action);
}


static void
menu_update (Menu           *menu,
             const char     *name,
             const char     *path,
             MooWindowClass *klass)
{
    MooUIXML *xml;
    MooUINode *parent;
    GString *markup;
    guint i;

    menu_purge (menu, klass);
    load_items (name, (LoadItemsFunc) create_menu_item, menu, NULL);

    if (!menu->n_items)
        return;

    markup = g_string_new (NULL);

    for (i = 0; i < menu->n_items; ++i)
    {
        MenuItem *item;

        item = menu->items[i];

        if (item->action_id)
            g_free (item->action_id);
        item->action_id = g_strdup_printf ("UserMenu-%d", i);

        moo_window_class_new_action_custom (klass, item->action_id,
                                            (MooWindowActionFunc) create_action,
                                            item, NULL);
        g_string_append_printf (markup, "<item action=\"%s\"/>", item->action_id);
    }

    xml = moo_app_get_ui_xml (moo_app_get_instance ());
    menu->merge_id = moo_ui_xml_new_merge_id (xml);
    parent = moo_ui_xml_find_node (xml, path);
    moo_ui_xml_insert (xml, menu->merge_id, parent, -1, markup->str);

    g_string_free (markup, TRUE);
}


void
user_menu_init (void)
{
    guint i;
    MooWindowClass *klass;

    g_assert (!menus[0]);

    for (i = 0; i < 2; ++i)
        menus[i] = menu_new ();

    klass = g_type_class_ref (GAP_TYPE_EDIT_WINDOW);
    menu_update (menus[EDITOR], "Editor", "EditorUserMenu", klass);
    g_type_class_unref (klass);

    klass = g_type_class_ref (GAP_TYPE_TERM_WINDOW);
    menu_update (menus[TERM], "Terminal", "TerminalUserMenu", klass);
    g_type_class_unref (klass);
}


void
user_menu_update (void)
{
    g_assert (menus[0] != NULL);
    menu_update (menus[EDITOR], "Editor", "EditorUserMenu",
                 g_type_class_peek (GAP_TYPE_EDIT_WINDOW));
    menu_update (menus[TERM], "Terminal", "TerminalUserMenu",
                 g_type_class_peek (GAP_TYPE_TERM_WINDOW));
}


/***************************************************************************/
/* Preferences page
 */

enum {
    COLUMN_LABEL,
    COLUMN_SCRIPT,
    COLUMN_ICON,
    COLUMN_VISIBLE,
    N_COLUMNS
};


static void prefs_page_apply    (MooGladeXML        *xml);
static void prefs_page_init     (MooGladeXML        *xml);
static void prefs_page_destroy  (MooGladeXML        *xml);
static void selection_changed   (GtkTreeSelection   *selection,
                                 MooGladeXML        *xml);
static void set_from_widgets    (MooGladeXML        *xml,
                                 GtkTreeModel       *model,
                                 GtkTreePath        *path);
static void set_from_model      (MooGladeXML        *xml,
                                 GtkTreeModel       *model,
                                 GtkTreePath        *path);

static void label_data_func     (GtkTreeViewColumn  *column,
                                 GtkCellRenderer    *cell,
                                 GtkTreeModel       *model,
                                 GtkTreeIter        *iter);
static void button_new          (MooGladeXML        *xml);
static void button_delete       (MooGladeXML        *xml);
static void button_up           (MooGladeXML        *xml);
static void button_down         (MooGladeXML        *xml);
static void label_changed       (MooGladeXML        *xml);
static void visible_changed     (MooGladeXML        *xml);


static void
setup_script_view (MooTextView *script)
{
    MooLangMgr *mgr;
    MooLang *lang;
    MooIndenter *indent;

    g_object_set (script, "highlight-current-line", FALSE, NULL);

    mgr = moo_editor_get_lang_mgr (moo_app_get_editor (moo_app_get_instance ()));
    lang = moo_lang_mgr_get_lang (mgr, "MooScript");
    if (lang)
        moo_text_view_set_lang (script, lang);

    moo_text_view_set_font_from_string (script, "Monospace");

    indent = moo_indenter_new (NULL, NULL);
    moo_text_view_set_indenter (script, indent);
    g_object_set (indent, "use-tabs", FALSE, "indent", 2, NULL);
    g_object_unref (indent);
}


GtkWidget *
user_menu_prefs_page_new (void)
{
    GtkWidget *page;
    MooGladeXML *xml;
    GtkTreeSelection *selection;
    GtkTreeView *treeview;
    GtkTreeViewColumn *column;
    GtkTreeStore *store;
    GtkCellRenderer *cell;

    xml = moo_glade_xml_new_empty ();
    moo_glade_xml_map_id (xml, "script", MOO_TYPE_TEXT_VIEW);
    moo_glade_xml_map_id (xml, "page", MOO_TYPE_PREFS_DIALOG_PAGE);
    moo_glade_xml_parse_memory (xml, USER_MENU_GLADE_UI, -1, "page");

    page = moo_glade_xml_get_widget (xml, "page");
    g_return_val_if_fail (page != NULL, NULL);
    g_object_set (page, "label", "User Menus",
                  "icon-stock-id", MOO_STOCK_MENU, NULL);
    g_object_set_data_full (G_OBJECT (page), "moo-glade-xml", xml,
                            (GDestroyNotify) g_object_unref);

    g_signal_connect_swapped (page, "apply", G_CALLBACK (prefs_page_apply), xml);
    g_signal_connect_swapped (page, "init", G_CALLBACK (prefs_page_init), xml);
    g_signal_connect_swapped (page, "destroy", G_CALLBACK (prefs_page_destroy), xml);

    setup_script_view (moo_glade_xml_get_widget (xml, "script"));

    store = gtk_tree_store_new (N_COLUMNS,
                                G_TYPE_STRING, G_TYPE_STRING,
                                G_TYPE_STRING, G_TYPE_BOOLEAN);
    treeview = moo_glade_xml_get_widget (xml, "treeview");
    selection = gtk_tree_view_get_selection (treeview);
    g_signal_connect (selection, "changed",
                      G_CALLBACK (selection_changed), xml);

    gtk_tree_view_set_model (treeview, GTK_TREE_MODEL (store));
    g_object_unref (store);

    column = gtk_tree_view_column_new ();
    gtk_tree_view_append_column (treeview, column);

    cell = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (column, cell, TRUE);
    gtk_tree_view_column_set_cell_data_func (column, cell,
                                             (GtkTreeCellDataFunc) label_data_func,
                                             NULL, NULL);

    g_signal_connect_swapped (moo_glade_xml_get_widget (xml, "new"),
                              "clicked", G_CALLBACK (button_new), xml);
    g_signal_connect_swapped (moo_glade_xml_get_widget (xml, "delete"),
                              "clicked", G_CALLBACK (button_delete), xml);
    g_signal_connect_swapped (moo_glade_xml_get_widget (xml, "up"),
                              "clicked", G_CALLBACK (button_up), xml);
    g_signal_connect_swapped (moo_glade_xml_get_widget (xml, "down"),
                              "clicked", G_CALLBACK (button_down), xml);
    g_signal_connect_swapped (moo_glade_xml_get_widget (xml, "label"),
                              "changed", G_CALLBACK (label_changed), xml);
    g_signal_connect_swapped (moo_glade_xml_get_widget (xml, "visible"),
                              "toggled", G_CALLBACK (visible_changed), xml);

    return page;
}


static void
label_data_func (G_GNUC_UNUSED GtkTreeViewColumn *column,
                 GtkCellRenderer    *cell,
                 GtkTreeModel       *model,
                 GtkTreeIter        *iter)
{
    GtkTreeIter dummy;
    gboolean visible;
    char *label;

    gtk_tree_model_get (model, iter,
                        COLUMN_VISIBLE, &visible,
                        COLUMN_LABEL, &label, -1);

    if (gtk_tree_model_iter_parent (model, &dummy, iter))
        g_object_set (cell, "text", label,
                      "foreground", visible ? NULL : "grey",
                      "style", visible ? PANGO_STYLE_NORMAL : PANGO_STYLE_ITALIC,
                      NULL);
    else
        g_object_set (cell, "text", label,
                      "foreground", NULL,
                      "style", PANGO_STYLE_NORMAL,
                      NULL);

    g_free (label);
}


static gboolean
is_empty_string (const char *string)
{
    if (!string)
        return TRUE;

#define IS_SPACE(c) (c == ' ' || c == '\t' || c == '\r' || c == '\n')
    while (*string)
    {
        if (*string && !IS_SPACE (*string))
            return FALSE;
        string++;
    }
#undef IS_SPACE

    return TRUE;
}


static void
save_items (GtkTreeModel *model,
            GtkTreeIter  *parent,
            const char   *window_name,
            MooMarkupDoc *doc)
{
    GtkTreeIter iter;
    MooMarkupNode *root;
    char *path;

    if (!gtk_tree_model_iter_children (model, &iter, parent))
        return;

    path = g_strdup_printf (MENU_XML_ROOT, window_name);
    root = moo_markup_create_element (MOO_MARKUP_NODE (doc), path);
    g_free (path);

    do
    {
        MooMarkupNode *node;
        char *label, *icon, *script;
        gboolean visible;

        gtk_tree_model_get (model, &iter,
                            COLUMN_LABEL, &label,
                            COLUMN_ICON, &icon,
                            COLUMN_SCRIPT, &script,
                            COLUMN_VISIBLE, &visible,
                            -1);

        if (is_empty_string (script))
        {
            g_free (script);
            script = NULL;
        }

        if (script)
            node = moo_markup_create_text_element (root, MENU_ITEM, script);
        else
            node = moo_markup_create_element (root, MENU_ITEM);

        moo_markup_set_prop (node, PROP_LABEL, label);
        moo_markup_set_prop (node, PROP_ICON, icon);

        if (!visible)
            moo_markup_set_bool_prop (node, PROP_HIDDEN, TRUE);

        g_free (label);
        g_free (icon);
        g_free (script);
    }
    while (gtk_tree_model_iter_next (model, &iter));
}


static void
prefs_page_apply (MooGladeXML *xml)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    MooMarkupDoc *doc;
    MooMarkupNode *root;

    doc = moo_prefs_get_markup ();
    g_return_if_fail (doc != NULL);

    selection = gtk_tree_view_get_selection (moo_glade_xml_get_widget (xml, "treeview"));

    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        GtkTreePath *path = gtk_tree_model_get_path (model, &iter);
        set_from_widgets (xml, model, path);
        gtk_tree_path_free (path);
    }

    root = moo_markup_get_element (MOO_MARKUP_NODE (doc), MENU_XML);

    if (root)
        moo_markup_delete_node (root);

    gtk_tree_model_get_iter_first (model, &iter);
    save_items (model, &iter, "Terminal", doc);
    gtk_tree_model_iter_next (model, &iter);
    save_items (model, &iter, "Editor", doc);

    user_menu_update ();
}


static void
insert_item (const char   *label,
             const char   *icon,
             const char   *script,
             gboolean      visible,
             GtkTreeStore *store,
             GtkTreeIter  *parent)
{
    GtkTreeIter iter;
    gtk_tree_store_append (store, &iter, parent);
    gtk_tree_store_set (store, &iter,
                        COLUMN_LABEL, label,
                        COLUMN_ICON, icon,
                        COLUMN_SCRIPT, script,
                        COLUMN_VISIBLE, visible,
                        -1);
}


static void
prefs_page_init (MooGladeXML *xml)
{
    GtkTreeIter iter;
    GtkTreeStore *store;
    GtkTreeModel *model;

    model = gtk_tree_view_get_model (moo_glade_xml_get_widget (xml, "treeview"));
    store = GTK_TREE_STORE (model);

    if (gtk_tree_model_get_iter_first (model, &iter))
        while (gtk_tree_store_remove (store, &iter))
            ;

    gtk_tree_store_append (store, &iter, NULL);
    gtk_tree_store_set (store, &iter,
                        COLUMN_LABEL, "Terminal",
                        -1);
    load_items ("Terminal", (LoadItemsFunc) insert_item,
                model, &iter);

    gtk_tree_store_append (store, &iter, NULL);
    gtk_tree_store_set (store, &iter,
                        COLUMN_LABEL, "Editor",
                        -1);
    load_items ("Editor", (LoadItemsFunc) insert_item,
                model, &iter);

    set_from_model (xml, model, NULL);
}


static gboolean
get_selected (MooGladeXML   *xml,
              GtkTreeModel **model,
              GtkTreeIter   *iter)
{
    GtkTreeSelection *selection;
    selection = gtk_tree_view_get_selection (moo_glade_xml_get_widget (xml, "treeview"));
    return gtk_tree_selection_get_selected (selection, model, iter);
}


static void
set_from_widgets (MooGladeXML  *xml,
                  GtkTreeModel *model,
                  GtkTreePath  *path)
{
    GtkTreeIter iter, dummy;
    GtkToggleButton *button;
    GtkEntry *entry;
    GtkTextBuffer *buffer;
    GtkTextIter start, end;
    const char *label;
    char *script;

    gtk_tree_model_get_iter (model, &iter, path);

    if (!gtk_tree_model_iter_parent (model, &dummy, &iter))
        return;

    entry = moo_glade_xml_get_widget (xml, "label");
    label = gtk_entry_get_text (entry);
    button = moo_glade_xml_get_widget (xml, "visible");
    buffer = gtk_text_view_get_buffer (moo_glade_xml_get_widget (xml, "script"));
    gtk_text_buffer_get_bounds (buffer, &start, &end);
    script = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

    gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
                        COLUMN_LABEL, label,
                        COLUMN_SCRIPT, script,
                        COLUMN_VISIBLE, gtk_toggle_button_get_active (button),
                        -1);

    g_free (script);
}


static void
set_from_model (MooGladeXML  *xml,
                GtkTreeModel *model,
                GtkTreePath  *path)
{
    GtkTreeIter iter, parent;
    GtkWidget *button_new, *button_delete, *button_up, *button_down;
    GtkWidget *table;
    GtkToggleButton *button_visible;
    GtkEntry *entry_label;
    GtkTextView *script_view;
    GtkTextBuffer *buffer;

    g_signal_handlers_block_by_func (moo_glade_xml_get_widget (xml, "label"),
                                     (gpointer) label_changed, xml);
    g_signal_handlers_block_by_func (moo_glade_xml_get_widget (xml, "visible"),
                                     (gpointer) visible_changed, xml);

    button_new = moo_glade_xml_get_widget (xml, "new");
    button_delete = moo_glade_xml_get_widget (xml, "delete");
    button_up = moo_glade_xml_get_widget (xml, "up");
    button_down = moo_glade_xml_get_widget (xml, "down");
    table = moo_glade_xml_get_widget (xml, "table");
    entry_label = moo_glade_xml_get_widget (xml, "label");
    button_visible = moo_glade_xml_get_widget (xml, "visible");
    script_view = moo_glade_xml_get_widget (xml, "script");
    buffer = gtk_text_view_get_buffer (script_view);

    if (path)
        gtk_tree_model_get_iter (model, &iter, path);

    if (!path || !gtk_tree_model_iter_parent (model, &parent, &iter))
    {
        gtk_widget_set_sensitive (button_delete, FALSE);
        gtk_widget_set_sensitive (button_up, FALSE);
        gtk_widget_set_sensitive (button_down, FALSE);
        gtk_widget_set_sensitive (GTK_WIDGET (button_visible), FALSE);
        gtk_widget_set_sensitive (table, FALSE);
        gtk_entry_set_text (entry_label, "");
        gtk_text_buffer_set_text (buffer, "", -1);
        gtk_toggle_button_set_active (button_visible, TRUE);
    }
    else
    {
        char *label, *script;
        gboolean visible;
        int *indices;

        gtk_tree_model_get (model, &iter,
                            COLUMN_LABEL, &label,
                            COLUMN_SCRIPT, &script,
                            COLUMN_VISIBLE, &visible,
                            -1);

        gtk_widget_set_sensitive (button_delete, TRUE);
        gtk_widget_set_sensitive (GTK_WIDGET (button_visible), TRUE);
        gtk_widget_set_sensitive (table, TRUE);
        gtk_entry_set_text (entry_label, label ? label : "");
        gtk_text_buffer_set_text (buffer, script ? script : "", -1);
        gtk_toggle_button_set_active (button_visible, visible);

        indices = gtk_tree_path_get_indices (path);
        gtk_widget_set_sensitive (button_up, indices[1] != 0);
        gtk_widget_set_sensitive (button_down, indices[1] !=
                                        gtk_tree_model_iter_n_children (model, &parent) - 1);

    }

    g_signal_handlers_unblock_by_func (moo_glade_xml_get_widget (xml, "label"),
                                       (gpointer) label_changed, xml);
    g_signal_handlers_unblock_by_func (moo_glade_xml_get_widget (xml, "visible"),
                                       (gpointer) visible_changed, xml);
}


static void
selection_changed (GtkTreeSelection *selection,
                   MooGladeXML      *xml)
{
    GtkTreeRowReference *old_row;
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkTreePath *path, *old_path;

    old_row = g_object_get_data (G_OBJECT (selection), "current-row");
    old_path = old_row ? gtk_tree_row_reference_get_path (old_row) : NULL;

    if (old_row && !old_path)
    {
        g_object_set_data (G_OBJECT (selection), "current-row", NULL);
        old_row = NULL;
    }

    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        path = gtk_tree_model_get_path (model, &iter);

        if (old_path && !gtk_tree_path_compare (old_path, path))
        {
            gtk_tree_path_free (old_path);
            gtk_tree_path_free (path);
            return;
        }
    }
    else
    {
        if (!old_path)
            return;

        path = NULL;
    }

    if (old_path)
        set_from_widgets (xml, model, old_path);

    set_from_model (xml, model, path);

    if (path)
    {
        GtkTreeRowReference *row;
        row = gtk_tree_row_reference_new (model, path);
        g_object_set_data_full (G_OBJECT (selection), "current-row", row,
                                (GDestroyNotify) gtk_tree_row_reference_free);
    }
    else
    {
        g_object_set_data (G_OBJECT (selection), "current-row", NULL);
    }

    gtk_tree_path_free (path);
    gtk_tree_path_free (old_path);
}


static void
button_new (MooGladeXML *xml)
{
    GtkTreeModel *model;
    GtkTreeIter iter, parent, after;
    GtkTreeSelection *selection;
    GtkTreeView *treeview;
    GtkTreePath *path;

    treeview = moo_glade_xml_get_widget (xml, "treeview");

    if (!get_selected (xml, &model, &after))
    {
        model = gtk_tree_view_get_model (treeview);
        gtk_tree_model_get_iter_first (model, &parent);
        gtk_tree_store_append (GTK_TREE_STORE (model), &iter, &parent);
    }
    else if (gtk_tree_model_iter_parent (model, &parent, &after))
    {
        gtk_tree_store_insert_after (GTK_TREE_STORE (model), &iter,
                                     &parent, &after);
    }
    else
    {
        gtk_tree_store_prepend (GTK_TREE_STORE (model), &iter, &after);
    }

    gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
                        COLUMN_LABEL, "New Item",
                        COLUMN_VISIBLE, TRUE, -1);

    path = gtk_tree_model_get_path (model, &iter);
    gtk_tree_view_expand_to_path (treeview, path);
    selection = gtk_tree_view_get_selection (treeview);
    gtk_tree_selection_select_iter (selection, &iter);
    gtk_tree_path_free (path);
}


static void
button_delete (MooGladeXML *xml)
{
    GtkTreeModel *model;
    GtkTreeIter iter, parent;
    GtkTreeSelection *selection;
    GtkTreeView *treeview;

    if (!get_selected (xml, &model, &iter))
        g_return_if_reached ();
    if (!gtk_tree_model_iter_parent (model, &parent, &iter))
        g_return_if_reached ();

    if (!gtk_tree_store_remove (GTK_TREE_STORE (model), &iter))
    {
        int n_children;

        n_children = gtk_tree_model_iter_n_children (model, &parent);

        if (n_children)
        {
            GtkTreePath *path, *parent_path;
            parent_path = gtk_tree_model_get_path (model, &parent);
            path = gtk_tree_path_new_from_indices (gtk_tree_path_get_indices(parent_path)[0],
                                                   n_children - 1, -1);
            gtk_tree_model_get_iter (model, &iter, path);
            gtk_tree_path_free (path);
            gtk_tree_path_free (parent_path);
        }
        else
        {
            iter = parent;
        }
    }

    treeview = moo_glade_xml_get_widget (xml, "treeview");
    selection = gtk_tree_view_get_selection (treeview);
    gtk_tree_selection_select_iter (selection, &iter);
}


static void
button_up (MooGladeXML *xml)
{
    GtkTreeModel *model;
    GtkTreeIter iter, parent;
    GtkTreeIter swap_with;
    GtkTreePath *path, *new_path;
    int *indices;

    if (!get_selected (xml, &model, &iter))
        g_return_if_reached ();
    if (!gtk_tree_model_iter_parent (model, &parent, &iter))
        g_return_if_reached ();

    path = gtk_tree_model_get_path (model, &iter);
    indices = gtk_tree_path_get_indices (path);

    if (!indices[1])
        g_return_if_reached ();

    new_path = gtk_tree_path_new_from_indices (indices[0], indices[1] - 1, -1);
    gtk_tree_model_get_iter (model, &swap_with, new_path);
    gtk_tree_store_swap (GTK_TREE_STORE (model), &iter, &swap_with);
    set_from_model (xml, model, new_path);

    gtk_tree_path_free (new_path);
    gtk_tree_path_free (path);
}


static void
button_down (MooGladeXML *xml)
{
    GtkTreeModel *model;
    GtkTreeIter iter, parent;
    GtkTreeIter swap_with;
    GtkTreePath *path, *new_path;
    int *indices;
    int n_children;

    if (!get_selected (xml, &model, &iter))
        g_return_if_reached ();
    if (!gtk_tree_model_iter_parent (model, &parent, &iter))
        g_return_if_reached ();

    path = gtk_tree_model_get_path (model, &iter);
    indices = gtk_tree_path_get_indices (path);
    n_children = gtk_tree_model_iter_n_children (model, &parent);

    if (indices[1] == n_children - 1)
        g_return_if_reached ();

    new_path = gtk_tree_path_new_from_indices (indices[0], indices[1] + 1, -1);
    gtk_tree_model_get_iter (model, &swap_with, new_path);
    gtk_tree_store_swap (GTK_TREE_STORE (model), &iter, &swap_with);
    set_from_model (xml, model, new_path);

    gtk_tree_path_free (new_path);
    gtk_tree_path_free (path);
}


static void
label_changed (MooGladeXML *xml)
{
    GtkEntry *entry;
    GtkTreeModel *model;
    GtkTreeIter iter;
    const char *label;

    if (!get_selected (xml, &model, &iter))
        return;

    entry = moo_glade_xml_get_widget (xml, "label");
    label = gtk_entry_get_text (entry);

    gtk_tree_store_set (GTK_TREE_STORE (model), &iter, COLUMN_LABEL, label, -1);
}


static void
visible_changed (MooGladeXML *xml)
{
    GtkToggleButton *button;
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (!get_selected (xml, &model, &iter))
        return;

    button = moo_glade_xml_get_widget (xml, "visible");
    gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
                        COLUMN_VISIBLE, gtk_toggle_button_get_active (button), -1);
}


static void
prefs_page_destroy (MooGladeXML *xml)
{
    GtkTreeSelection *selection;

    selection = gtk_tree_view_get_selection (moo_glade_xml_get_widget (xml, "treeview"));
    g_signal_handlers_disconnect_by_func (selection, (gpointer) selection_changed, xml);

    g_signal_handlers_disconnect_by_func (moo_glade_xml_get_widget (xml, "new"),
                                          (gpointer) button_new, xml);
    g_signal_handlers_disconnect_by_func (moo_glade_xml_get_widget (xml, "delete"),
                                          (gpointer) button_delete, xml);
    g_signal_handlers_disconnect_by_func (moo_glade_xml_get_widget (xml, "up"),
                                          (gpointer) button_up, xml);
    g_signal_handlers_disconnect_by_func (moo_glade_xml_get_widget (xml, "down"),
                                          (gpointer) button_down, xml);
    g_signal_handlers_disconnect_by_func (moo_glade_xml_get_widget (xml, "label"),
                                          (gpointer) label_changed, xml);
    g_signal_handlers_disconnect_by_func (moo_glade_xml_get_widget (xml, "visible"),
                                          (gpointer) visible_changed, xml);
}
