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


#define MENU_XML_ROOT   "UserMenu/%s"
#define MENU_ITEM       "item"
#define PROP_LABEL      "label"
#define PROP_ICON       "icon"
#define PROP_NAME       "name"
#define PROP_HIDDEN     "hidden"


typedef struct {
    char *label;
    char *name;
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
                               const char *name,
                               const char *icon,
                               const char *script,
                               gboolean    visible,
                               gpointer    data1,
                               gpointer    data2);


static MenuItem *menu_item_new          (const char *label,
                                         const char *name,
                                         const char *icon,
                                         const char *script,
                                         gboolean    visible);
static void      menu_item_free         (MenuItem   *item);

static Menu     *menu_new               (void);
// static void      menu_free              (Menu       *menu);

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
               const char *name,
               const char *icon,
               const char *script,
               gboolean    visible)
{
    MenuItem *item;

    g_return_val_if_fail (label != NULL, NULL);

    item = g_new0 (MenuItem, 1);
    item->label = g_strdup (label);
    item->name = g_strdup (name);
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
        g_free (item->name);
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
        const char *label, *icon, *script, *name;
        gboolean hidden;

        if (!MOO_MARKUP_IS_ELEMENT (node))
            continue;

        if (strcmp (node->name, MENU_ITEM))
        {
            g_warning ("%s: invalid element '%s'", G_STRLOC, node->name);
            continue;
        }

        label = moo_markup_get_prop (node, PROP_LABEL);
        name = moo_markup_get_prop (node, PROP_NAME);
        icon = moo_markup_get_prop (node, PROP_ICON);
        hidden = moo_markup_get_bool_prop (node, PROP_HIDDEN, FALSE);
        script = moo_markup_get_content (node);

        if (!label && !name)
        {
            g_warning ("%s: label and name missing", G_STRLOC);
            continue;
        }

        if (!script || !script[0])
            script = NULL;

        func (label, name, icon, script, !hidden, data1, data2);
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
                  const char *name,
                  const char *icon,
                  const char *script,
                  gboolean    visible,
                  Menu       *menu)
{
    MenuItem *item = menu_item_new (label, name, icon, script, visible);
    menu_add_item (menu, item);
}


static MooAction *
create_action (MooWindow *window,
               MenuItem  *item)
{
    UserAction *action;

    action = g_object_new (user_action_get_type(),
                           "label", item->label,
                           "name", item->name ? item->name : item->label,
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
        item->action_id = g_strdup_printf ("UserMenu-%d", g_random_int ());

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


/***************************************************************************/
/* Preferences page
 */

enum {
    COLUMN_NAME,
    COLUMN_LABEL,
    COLUMN_SCRIPT,
    COLUMN_ICON,
    COLUMN_VISIBLE,
    N_COLUMNS
};


static void prefs_page_apply    (GtkTreeModel       *model);
static void prefs_page_init     (GtkTreeModel       *model);
static void selection_changed   (GtkTreeSelection   *selection,
                                 MooGladeXML        *xml);
static void icon_data_func      (GtkTreeViewColumn  *column,
                                 GtkCellRenderer    *cell,
                                 GtkTreeModel       *model,
                                 GtkTreeIter        *iter);
static void label_data_func     (GtkTreeViewColumn  *column,
                                 GtkCellRenderer    *cell,
                                 GtkTreeModel       *model,
                                 GtkTreeIter        *iter);
static void new_item            (MooGladeXML        *xml);
static void delete_item         (MooGladeXML        *xml);


GtkWidget *
user_menu_prefs_page_new (void)
{
    GtkWidget *page, *button;
    MooGladeXML *xml;
    GtkTreeSelection *selection;
    GtkTreeView *treeview;
    GtkTreeViewColumn *column;
    GtkTreeStore *store;
    GtkCellRenderer *cell;

    store = gtk_tree_store_new (N_COLUMNS,
                                G_TYPE_STRING, G_TYPE_STRING,
                                G_TYPE_STRING, G_TYPE_STRING,
                                G_TYPE_BOOLEAN);

    page = moo_prefs_dialog_page_new_from_xml ("User Menus",
                                               MOO_STOCK_MENU,
                                               USER_MENU_GLADE_UI, -1,
                                               "page", NULL);
    g_return_val_if_fail (page != NULL, NULL);

    g_signal_connect_swapped (page, "apply", G_CALLBACK (prefs_page_apply), store);
    g_signal_connect_swapped (page, "init", G_CALLBACK (prefs_page_init), store);

    xml = MOO_PREFS_DIALOG_PAGE(page)->xml;
    treeview = moo_glade_xml_get_widget (xml, "treeview");
    selection = gtk_tree_view_get_selection (treeview);
    g_signal_connect (selection, "changed",
                      G_CALLBACK (selection_changed), xml);

    gtk_tree_view_set_model (treeview, GTK_TREE_MODEL (store));
    g_object_unref (store);

    column = gtk_tree_view_column_new ();
    gtk_tree_view_append_column (treeview, column);
    cell = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_column_pack_start (column, cell, FALSE);
    gtk_tree_view_column_set_cell_data_func (column, cell,
                                             (GtkTreeCellDataFunc) icon_data_func,
                                             NULL, NULL);

    column = gtk_tree_view_column_new ();
    gtk_tree_view_append_column (treeview, column);
    cell = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (column, cell, TRUE);
    gtk_tree_view_column_set_cell_data_func (column, cell,
                                             (GtkTreeCellDataFunc) label_data_func,
                                             NULL, NULL);

    button = moo_glade_xml_get_widget (xml, "new");
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (new_item), xml);
    button = moo_glade_xml_get_widget (xml, "delete");
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (delete_item), xml);

    return page;
}


static void
icon_data_func (G_GNUC_UNUSED GtkTreeViewColumn *column,
                GtkCellRenderer    *cell,
                GtkTreeModel       *model,
                GtkTreeIter        *iter)
{
    char *icon;
    gtk_tree_model_get (model, iter, COLUMN_ICON, &icon, -1);
    g_object_set (cell, "stock-id", icon, NULL);
    g_free (icon);
}


static void
label_data_func (G_GNUC_UNUSED GtkTreeViewColumn *column,
                 GtkCellRenderer    *cell,
                 GtkTreeModel       *model,
                 GtkTreeIter        *iter)
{
    gboolean visible;
    char *label;

    gtk_tree_model_get (model, iter,
                        COLUMN_VISIBLE, &visible,
                        COLUMN_LABEL, &label, -1);

    if (gtk_tree_model_iter_parent (model, NULL, iter))
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


static void
prefs_page_apply (GtkTreeModel *model)
{
}


static void
insert_item (const char   *label,
             const char   *name,
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
                        COLUMN_NAME, name,
                        COLUMN_ICON, icon,
                        COLUMN_SCRIPT, script,
                        COLUMN_VISIBLE, visible,
                        -1);
}


static void
prefs_page_init (GtkTreeModel *model)
{
    GtkTreeIter iter;
    GtkTreeStore *store;

    store = GTK_TREE_STORE (model);

    if (gtk_tree_model_get_iter_first (model, &iter))
        while (gtk_tree_store_remove (store, &iter))
            ;

    gtk_tree_store_append (store, &iter, NULL);
    gtk_tree_store_set (store, &iter,
                        COLUMN_LABEL, "Terminal",
                        COLUMN_ICON, MOO_STOCK_GAP,
                        -1);
    load_items ("Terminal", (LoadItemsFunc) insert_item,
                model, &iter);

    gtk_tree_store_append (store, &iter, NULL);
    gtk_tree_store_set (store, &iter,
                        COLUMN_LABEL, "Editor",
                        COLUMN_ICON, GTK_STOCK_EDIT,
                        -1);
    load_items ("Editor", (LoadItemsFunc) insert_item,
                model, &iter);
}


static void
selection_changed (GtkTreeSelection   *selection,
                   MooGladeXML        *xml)
{
}


static void
new_item (MooGladeXML *xml)
{
}


static void
delete_item (MooGladeXML *xml)
{
}
