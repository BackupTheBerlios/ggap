/*
 *   gap-widgets.c
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

#include "config.h"
#include "gapsession.h"
#include "gap-widgets.h"
#include "mooutils/moomarshals.h"
#include "mooedit/mooeditor.h"
#include "mooapp/moohtml.h"
#include <gtk/gtk.h>


/**********************************************************************************/
/* list and dict
 */

static GValue *
value_new (GType type)
{
    GValue *val = g_new0 (GValue, 1);
    g_value_init (val, type);
    return val;
}

static void
value_free (gpointer value)
{
    if (value)
    {
        g_value_unset (value);
        g_free (value);
    }
}


static GapList *
gap_list_new (void)
{
    GapList *list = g_new0 (GapList, 1);
    list->ref_count = 1;
    return list;
}


static GapList *
gap_list_ref (GapList *list)
{
    if (list)
        list->ref_count++;
    return list;
}

static void
gap_list_unref (GapList *list)
{
    if (list && !--list->ref_count)
    {
        g_slist_foreach (list->head, (GFunc) value_free, NULL);
        g_slist_free (list->head);
        g_free (list);
    }
}

GType
gap_list_get_type (void)
{
    static GType type;

    if (!type)
        g_boxed_type_register_static ("GapList",
                                      (GBoxedCopyFunc) gap_list_ref,
                                      (GBoxedFreeFunc) gap_list_unref);

    return type;
}


static GapDict *
gap_dict_ref (GapDict *dict)
{
    if (dict)
        dict->ref_count++;
    return dict;
}

static void
gap_dict_unref (GapDict *dict)
{
    if (dict && !--dict->ref_count)
    {
        g_hash_table_destroy (dict->hash);
        g_free (dict);
    }
}

GType
gap_dict_get_type (void)
{
    static GType type;

    if (!type)
        g_boxed_type_register_static ("GapDict",
                                      (GBoxedCopyFunc) gap_dict_ref,
                                      (GBoxedFreeFunc) gap_dict_unref);

    return type;
}


GapList *
gap_value_get_list (GValue *value)
{
    g_return_val_if_fail (GAP_VALUE_HOLDS_LIST (value), NULL);
    return value->data[0].v_pointer;
}


void
gap_value_set_list (GValue     *value,
                    GapList    *list)
{
    g_return_if_fail (GAP_VALUE_HOLDS_LIST (value));

    if (list)
        gap_list_ref (list);

    if (value->data[0].v_pointer)
        gap_list_unref (value->data[0].v_pointer);

    value->data[0].v_pointer = list;
}


GapDict *
gap_value_get_dict (GValue *value)
{
    g_return_val_if_fail (GAP_VALUE_HOLDS_DICT (value), NULL);
    return value->data[0].v_pointer;
}


void
gap_value_set_dict (GValue     *value,
                    GapDict    *dict)
{
    g_return_if_fail (GAP_VALUE_HOLDS_DICT (value));

    if (dict)
        gap_dict_ref (dict);

    if (value->data[0].v_pointer)
        gap_dict_unref (value->data[0].v_pointer);

    value->data[0].v_pointer = dict;
}


/**********************************************************************************/
/* properties
 */

typedef struct _PropSpec PropSpec;
typedef struct _PropAlias PropAlias;

typedef gboolean (*PropSetter) (gpointer  object,
                                MSValue  *value,
                                char    **msg,
                                gpointer  data);
typedef gboolean (*PropGetter) (gpointer  object,
                                GValue   *value,
                                char    **msg,
                                gpointer  data);

static GHashTable *registered_props;
static GQuark props_quark;

static void init_props (void);


struct _PropSpec {
    PropSetter setter;
    PropGetter getter;
    gpointer data;
};


static PropSpec *
prop_spec_new (PropSetter  setter,
               PropGetter  getter,
               gpointer    data)
{
    PropSpec *spec;

    g_return_val_if_fail (setter || getter, NULL);

    spec = g_new0 (PropSpec, 1);
    spec->getter = getter;
    spec->setter = setter;
    spec->data = data;

    return spec;
}

static void
prop_spec_free (PropSpec *spec)
{
    g_free (spec);
}


static void
install_fake_prop (GType       type,
                   const char *prop_name,
                   PropSetter  setter,
                   PropGetter  getter,
                   gpointer    data)
{
    char *norm_name;
    PropSpec *spec;
    GHashTable *hash;

    norm_name = g_strdelimit (g_strdup (prop_name), "_", '-');
    spec = prop_spec_new (setter, getter, data);

    hash = g_type_get_qdata (type, props_quark);

    if (!hash)
    {
        hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
                                      (GDestroyNotify) prop_spec_free);
        g_type_set_qdata (type, props_quark, hash);
    }

    g_hash_table_insert (registered_props, norm_name, norm_name);
    g_hash_table_insert (hash, norm_name, spec);
}


static PropSpec *
find_fake_prop (GType       type,
                const char *prop_name)
{
    char *norm_name;
    PropSpec *spec = NULL;

    norm_name = g_strdelimit (g_strdup (prop_name), "_", '-');

    if (!g_hash_table_lookup (registered_props, norm_name))
        return NULL;

    do
    {
        GHashTable *hash = g_type_get_qdata (type, props_quark);

        if (hash)
            spec = g_hash_table_lookup (hash, norm_name);
    }
    while (!spec && (type = g_type_parent (type)));

    g_free (norm_name);
    return spec;
}


static gboolean
ms_value_to_gvalue (MSValue    *mval,
                    GValue     *gval,
                    GParamSpec *pspec)
{
    g_return_val_if_fail (mval != NULL, FALSE);
    g_return_val_if_fail (G_IS_VALUE (gval), FALSE);
    g_return_val_if_fail (pspec != NULL, FALSE);

    if (G_VALUE_TYPE (gval) == G_TYPE_STRING)
    {
        char *string = NULL;

        if (!ms_value_is_none (mval))
            string = ms_value_print (mval);

        g_value_take_string (gval, string);
        return TRUE;
    }

    return FALSE;
}


gboolean
gap_set_property (gpointer    object,
                  const char *prop_name,
                  MSValue    *value,
                  char      **error_msg)
{
    PropSpec *fake_prop;
    GParamSpec *pspec;
    GValue gval;

    g_return_val_if_fail (G_IS_OBJECT (object), FALSE);
    g_return_val_if_fail (prop_name != NULL, FALSE);
    g_return_val_if_fail (value != NULL, FALSE);

    init_props ();

    if (GAP_IS_OBJECT (object))
        object = GAP_OBJECT(object)->obj;

    fake_prop = find_fake_prop (G_OBJECT_TYPE (object), prop_name);

    if (fake_prop)
    {
        if (!fake_prop->setter)
        {
            *error_msg = g_strdup_printf ("can't set property '%s' of class '%s'",
                                          prop_name, g_type_name (G_OBJECT_TYPE (object)));
            g_warning ("%s: %s", G_STRLOC, *error_msg);
            return FALSE;
        }

        return fake_prop->setter (object, value, error_msg, fake_prop->data);
    }

    pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (object), prop_name);

    if (!pspec)
    {
        *error_msg = g_strdup_printf ("no property '%s' in class '%s'",
                                      prop_name, g_type_name (G_OBJECT_TYPE (object)));
        g_warning ("%s: %s", G_STRLOC, *error_msg);
        return FALSE;
    }

    if ((pspec->flags & G_PARAM_CONSTRUCT_ONLY) || !(pspec->flags & G_PARAM_WRITABLE))
    {
        *error_msg = g_strdup_printf ("property '%s' of class '%s' is not writable",
                                      prop_name, g_type_name (G_OBJECT_TYPE (object)));
        g_warning ("%s: %s", G_STRLOC, *error_msg);
        return FALSE;
    }

    gval.g_type = 0;
    g_value_init (&gval, pspec->value_type);

    if (!ms_value_to_gvalue (value, &gval, pspec))
    {
        *error_msg = g_strdup_printf ("could not convert property '%s' of class '%s'",
                                      prop_name, g_type_name (G_OBJECT_TYPE (object)));
        g_warning ("%s: %s", G_STRLOC, *error_msg);
        g_value_unset (&gval);
        return FALSE;
    }

    g_object_set_property (object, prop_name, &gval);
    g_value_unset (&gval);
    return TRUE;
}


gboolean
gap_get_property (gpointer    object,
                  const char *prop_name,
                  GValue     *value,
                  char      **error_msg)
{
    PropSpec *fake_prop;
    GParamSpec *pspec;

    g_return_val_if_fail (G_IS_OBJECT (object), FALSE);
    g_return_val_if_fail (prop_name != NULL, FALSE);
    g_return_val_if_fail (value != NULL, FALSE);

    init_props ();

    if (GAP_IS_OBJECT (object))
        object = GAP_OBJECT(object)->obj;

    fake_prop = find_fake_prop (G_OBJECT_TYPE (object), prop_name);

    if (fake_prop)
    {
        if (!fake_prop->getter)
        {
            *error_msg = g_strdup_printf ("can't get property '%s' of class '%s'",
                                          prop_name, g_type_name (G_OBJECT_TYPE (object)));
            g_warning ("%s: %s", G_STRLOC, *error_msg);
            return FALSE;
        }

        return fake_prop->getter (object, value, error_msg, fake_prop->data);
    }

    pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (object), prop_name);

    if (!pspec)
    {
        *error_msg = g_strdup_printf ("no property '%s' in class '%s'",
                                      prop_name, g_type_name (G_OBJECT_TYPE (object)));
        g_warning ("%s: %s", G_STRLOC, *error_msg);
        return FALSE;
    }

    if (!(pspec->flags & G_PARAM_READABLE))
    {
        *error_msg = g_strdup_printf ("property '%s' of class '%s' is not readable",
                                      prop_name, g_type_name (G_OBJECT_TYPE (object)));
        g_warning ("%s: %s", G_STRLOC, *error_msg);
        return FALSE;
    }

    g_value_init (value, pspec->value_type);
    g_object_get_property (object, prop_name, value);
    return TRUE;
}


/*************************************************************************/
/* Fake properties
 */

static gboolean
widget_set_font (gpointer  object,
                 MSValue  *value,
                 char    **error_message)
{
    char *font;
    PangoFontDescription *pfont;

    font = ms_value_print (value);
    pfont = pango_font_description_from_string (font);

    if (!pfont)
    {
        *error_message = g_strdup_printf ("invalid font '%s'", font);
        g_free (font);
        return FALSE;
    }

    gtk_widget_modify_font (object, pfont);

    pango_font_description_free (pfont);
    g_free (font);
    return TRUE;
}


static gboolean
text_view_set_highlight (gpointer   view,
                         MSValue   *arg)
{
    if (ms_value_is_none (arg) || MS_VALUE_TYPE (arg) == MS_VALUE_INT)
    {
        g_object_set (view, "enable-highlight",
                      ms_value_get_bool (arg), NULL);
    }
    else
    {
        char *name = ms_value_print (arg);
        MooEditor *editor = moo_editor_instance ();
        MooLangMgr *lang_mgr = moo_editor_get_lang_mgr (editor);
        MooLang *lang = moo_lang_mgr_get_lang (lang_mgr, name);
        moo_text_view_set_lang (view, lang);
        g_free (name);
    }

    return TRUE;
}


static gboolean
tree_view_set_items (GapTreeView *gtv,
                     MSValue     *arg,
                     char       **msg)
{
    g_return_val_if_fail (GAP_IS_TREE_VIEW (gtv), FALSE);

    if (MS_VALUE_TYPE (arg) != MS_VALUE_LIST)
    {
        char *val = ms_value_print (arg);
        *msg = g_strdup_printf ("invalid item list '%s'", val);
        g_free (val);
        return FALSE;
    }

    gap_tree_view_set_val_list (gtv, arg->list.elms, arg->list.n_elms);
    return TRUE;
}


static gboolean
tree_view_get_items (GapTreeView *gtv,
                     GValue      *value)
{
    g_return_val_if_fail (GAP_IS_TREE_VIEW (gtv), FALSE);
    return gap_tree_view_get_items (gtv, value);
}


static gboolean
set_text (gpointer   object,
          MSValue   *arg,
          char     **msg)
{
    char *text;
    gboolean result = TRUE;

    text = ms_value_print (arg);

    if (GTK_IS_ENTRY (object))
    {
        gtk_entry_set_text (object, text);
    }
    else if (GTK_IS_TEXT_VIEW (object))
    {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer (object);
        gtk_text_buffer_set_text (buffer, text, -1);
    }
    else if (GTK_IS_TEXT_BUFFER (object))
    {
        gtk_text_buffer_set_text (object, text, -1);
    }
    else
    {
        *msg = g_strdup_printf ("can't set property 'text' for object of type '%s'",
                                g_type_name (G_OBJECT_TYPE (object)));
        result = FALSE;
        goto out;
    }

out:
    g_free (text);
    return result;
}


static gboolean
get_text (gpointer   object,
          GValue    *value,
          char     **msg)
{
    char *freeme = NULL;
    const char *text;
    gboolean result = TRUE;

    if (GTK_IS_EDITABLE (object))
    {
        freeme = gtk_editable_get_chars (object, 0, -1);
        text = freeme;
    }
    else if (GTK_IS_TEXT_BUFFER (object))
    {
        GtkTextIter start, end;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer (object);
        gtk_text_buffer_get_bounds (buffer, &start, &end);
        freeme = gtk_text_buffer_get_text (buffer, &start, &end, TRUE);
        text = freeme;
    }
    else
    {
        *msg = g_strdup_printf ("can't get property 'text' for object of type '%s'",
                                g_type_name (G_OBJECT_TYPE (object)));
        result = FALSE;
        goto out;
    }

    g_value_init (value, G_TYPE_STRING);
    g_value_set_string (value, text);

out:
    g_free (freeme);
    return result;
}


static void
init_props (void)
{
    if (registered_props)
        return;

    registered_props = g_hash_table_new (g_str_hash, g_str_equal);
    props_quark = g_quark_from_static_string ("gap-fake-properties");

    install_fake_prop (GTK_TYPE_WIDGET, "gap-font",
                       (PropSetter) widget_set_font,
                       NULL, NULL);
    install_fake_prop (MOO_TYPE_TEXT_VIEW, "gap-highlight",
                       (PropSetter) text_view_set_highlight,
                       NULL, NULL);
    install_fake_prop (GAP_TYPE_TREE_VIEW, "gap-items",
                       (PropSetter) tree_view_set_items,
                       (PropGetter) tree_view_get_items, NULL);
    install_fake_prop (GTK_TYPE_ENTRY, "gap-text",
                       (PropSetter) set_text,
                       (PropGetter) get_text, NULL);
    install_fake_prop (GTK_TYPE_TEXT_VIEW, "gap-text",
                       (PropSetter) set_text,
                       (PropGetter) get_text, NULL);
}


/*************************************************************************/
/* GladeXML
 */

static GapCallback *
gap_callback_new (void)
{
    return g_new0 (GapCallback, 1);
}


void
gap_callback_free (GapCallback *cb)
{
    if (cb)
    {
        g_free (cb);
    }
}


static gboolean
gap_glade_xml_signal_func (MooGladeXML *xml,
                           const char  *widget_id,
                           GtkWidget   *widget,
                           const char  *signal,
                           const char  *handler,
                           const char  *object,
                           gpointer     data)
{
    GSList **list = data;
    return FALSE;
}


static void
map_id (const char  *id,
        MSValue     *val,
        MooGladeXML *xml)
{
    char *typename;
    GType type;

    typename = ms_value_print (val);
    type = g_type_from_name (typename);

    if (!type)
        g_warning ("no type '%s' for id '%s'", typename, id);
    else
        moo_glade_xml_map_id (xml, id, type);

    g_free (typename);
}


static void
ref_types (void)
{
    static gboolean been_here;
    volatile GType type;

    if (!been_here)
    {
        been_here = TRUE;

        type = MOO_TYPE_EDIT;
#ifdef MOO_USE_XML
        type = MOO_TYPE_HTML;
#endif
    }
}


gboolean
gap_glade_xml_new (const char     *file,
                   const char     *root,
                   MSValue        *type_dict,
                   MooGladeXML   **xml_p,
                   GSList        **callbacks_p)
{
    MooGladeXML *xml;
    GSList *callbacks = NULL;

    g_return_val_if_fail (file != NULL, FALSE);
    g_return_val_if_fail (xml_p != NULL, FALSE);
    g_return_val_if_fail (callbacks_p != NULL, FALSE);
    g_return_val_if_fail (!type_dict || MS_VALUE_TYPE (type_dict) == MS_VALUE_DICT, FALSE);

    ref_types ();

    root = root && root[0] ? root : NULL;
    xml = moo_glade_xml_new_empty ();

    moo_glade_xml_map_class (xml, "GtkTextView", MOO_TYPE_TEXT_VIEW);
    moo_glade_xml_map_class (xml, "GtkTreeView", GAP_TYPE_TREE_VIEW);
    moo_glade_xml_set_signal_func (xml, gap_glade_xml_signal_func, &callbacks);

    if (type_dict)
        g_hash_table_foreach (type_dict->hash, (GHFunc) map_id, xml);

    if (!moo_glade_xml_parse_file (xml, file, root))
    {
        g_object_unref (xml);
        g_slist_foreach (callbacks, (GFunc) gap_callback_free, NULL);
        g_slist_free (callbacks);
        return FALSE;
    }

    *xml_p = xml;
    *callbacks_p = callbacks;
    return TRUE;
}


/*************************************************************************/
/* GapTreeView
 */

G_DEFINE_TYPE (GapTreeView, gap_tree_view, GTK_TYPE_TREE_VIEW)


enum {
    TREE_SELECTION_CHANGED,
    TREE_NUM_SIGNALS
};

static guint tree_signals[TREE_NUM_SIGNALS];


static void
selection_changed (GapTreeView *gtv)
{
    g_signal_emit (gtv, tree_signals[TREE_SELECTION_CHANGED], 0);
}


static void
gap_tree_view_destroy (GtkObject *object)
{
    GapTreeView *gtv = GAP_TREE_VIEW (object);

    if (gtv->selection)
    {
        g_signal_handlers_disconnect_by_func (gtv->selection,
                                              (gpointer) selection_changed,
                                              gtv);
        gtv->selection = NULL;
        gtv->column = NULL;
    }

    GTK_OBJECT_CLASS (gap_tree_view_parent_class)->destroy (object);
}


static void
gap_tree_view_class_init (GapTreeViewClass *klass)
{
    GTK_OBJECT_CLASS(klass)->destroy = gap_tree_view_destroy;

    tree_signals[TREE_SELECTION_CHANGED] =
            g_signal_new ("selection-changed",
                          G_TYPE_FROM_CLASS (klass),
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (GapTreeViewClass, selection_changed),
                          NULL, NULL,
                          _moo_marshal_VOID__VOID,
                          G_TYPE_NONE, 0);
}


static void
gap_tree_view_init (GapTreeView *gtv)
{
    GtkTreeView *treeview;
    GtkTreeSelection *selection;
    GtkCellRenderer *cell;

    treeview = GTK_TREE_VIEW (gtv);
    selection = gtk_tree_view_get_selection (treeview);
    gtv->selection = selection;
    g_signal_connect_swapped (selection, "changed",
                              G_CALLBACK (selection_changed), gtv);

    cell = gtk_cell_renderer_text_new ();
    gtv->column = gtk_tree_view_column_new_with_attributes (NULL, cell, "text", 0, NULL);
    gtk_tree_view_append_column (treeview, gtv->column);

    gtk_tree_view_set_headers_visible (treeview, FALSE);
    gap_tree_view_set_list_mode (gtv, TRUE);
}


void
gap_tree_view_set_list_mode (GapTreeView *gtv,
                             gboolean     list)
{
    GtkTreeModel *model;

    g_return_if_fail (GAP_IS_TREE_VIEW (gtv));

    if (!gtv->list_mode == !list)
        return;

    gtv->list_mode = list != 0;

    if (list)
    {
        GtkListStore *store = gtk_list_store_new (1, G_TYPE_STRING);
        model = GTK_TREE_MODEL (store);
    }
    else
    {
        GtkTreeStore *store = gtk_tree_store_new (1, G_TYPE_STRING);
        model = GTK_TREE_MODEL (store);
    }

    gtk_tree_view_set_model (GTK_TREE_VIEW (gtv), model);
    g_object_unref (model);
}


void
gap_tree_view_set_title (GapTreeView *gtv,
                         const char  *title)
{
    g_return_if_fail (GAP_IS_TREE_VIEW (gtv));

    if (title)
        gtk_tree_view_column_set_title (gtv->column, title);

    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (gtv), title != NULL);
}


void
gap_tree_view_set_sortable (GapTreeView *gtv,
                            gboolean     sortable)
{
    g_return_if_fail (GAP_IS_TREE_VIEW (gtv));
    gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW (gtv), sortable != 0);
}


void
gap_tree_view_set_val_list (GapTreeView   *gtv,
                            MSValue      **elms,
                            guint          n_elms)
{
    GtkListStore *store;
    guint i;

    g_return_if_fail (GAP_IS_TREE_VIEW (gtv));
    g_return_if_fail (!n_elms || elms);

    gap_tree_view_set_list_mode (gtv, TRUE);
    store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (gtv)));
    gtk_list_store_clear (store);

    for (i = 0; i < n_elms; ++i)
    {
        GtkTreeIter iter;
        char *string = ms_value_print (elms[i]);
        gtk_list_store_append (store, &iter);
        gtk_list_store_set (store, &iter, 0, string, -1);
    }
}


gboolean
gap_tree_view_get_items (GapTreeView *gtv,
                         GValue      *value)
{
    guint n_elms, i;
    GtkTreeModel *model;
    GapList *list;

    g_return_val_if_fail (GAP_IS_TREE_VIEW (gtv), FALSE);
    g_return_val_if_fail (gtv->list_mode, FALSE);

    list = gap_list_new ();
    model = gtk_tree_view_get_model (GTK_TREE_VIEW (gtv));
    n_elms = gtk_tree_model_iter_n_children (model, NULL);

    for (i = 0; i < n_elms; ++i)
    {
        GtkTreeIter iter;
        char *string = NULL;
        GValue *elm;

        gtk_tree_model_iter_nth_child (model, &iter, NULL, i);
        gtk_tree_model_get (model, &iter, 0, &string, -1);

        elm = value_new (G_TYPE_STRING);
        g_value_take_string (elm, string);
        list->head = g_slist_prepend (list->head, elm);
    }

    list->head = g_slist_reverse (list->head);
    g_value_init (value, GAP_TYPE_LIST);
    g_value_take_boxed (value, list);

    return TRUE;
}


GtkTreePath *
gap_tree_view_get_selected_row (GapTreeView *gtv)
{
    GList *list;
    GtkTreePath *ret;

    g_return_val_if_fail (GAP_IS_TREE_VIEW (gtv), NULL);

    list = gtk_tree_selection_get_selected_rows (gtv->selection, NULL);

    if (!list)
        return NULL;

    ret = list->data;
    list = g_list_delete_link (list, list);
    g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
    g_list_free (list);

    return ret;
}


void
gap_tree_view_select_row (GapTreeView *gtv,
                          GtkTreePath *path)
{
    g_return_if_fail (GAP_IS_TREE_VIEW (gtv));
    g_return_if_fail (path != NULL);
    gtk_tree_selection_select_path (gtv->selection, path);
}


void
gap_tree_view_unselect_row (GapTreeView *gtv,
                            GtkTreePath *path)
{
    g_return_if_fail (GAP_IS_TREE_VIEW (gtv));
    g_return_if_fail (path != NULL);
    gtk_tree_selection_unselect_path (gtv->selection, path);
}


void
gap_tree_view_unselect_all (GapTreeView *gtv)
{
    g_return_if_fail (GAP_IS_TREE_VIEW (gtv));
    gtk_tree_selection_unselect_all (gtv->selection);
}


void
gap_tree_view_select_all (GapTreeView *gtv)
{
    g_return_if_fail (GAP_IS_TREE_VIEW (gtv));
    gtk_tree_selection_select_all (gtv->selection);
}
