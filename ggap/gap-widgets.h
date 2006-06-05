/*
 *   gap-widgets.h
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

#ifndef __GAP_WIDGETS_H__
#define __GAP_WIDGETS_H__

#include <mooutils/mooglade.h>
#include <mooscript/mooscript-value.h>
#include <gtk/gtktreeview.h>

G_BEGIN_DECLS


/**********************************************************************************/
/* GType stuff
 */

#define GAP_TYPE_DICT (gap_dict_get_type ())
#define GAP_VALUE_HOLDS_DICT(value) G_TYPE_CHECK_VALUE_TYPE ((value), GAP_TYPE_DICT)
#define GAP_TYPE_LIST (gap_list_get_type ())
#define GAP_VALUE_HOLDS_LIST(value) G_TYPE_CHECK_VALUE_TYPE ((value), GAP_TYPE_LIST)

typedef struct _GapList GapList;
typedef struct _GapDict GapDict;

struct _GapList {
    GSList *head; /* GValue* */
    guint ref_count;
};

struct _GapDict {
    GHashTable *hash; /* char* -> GValue* */
    guint ref_count;
};

GType       gap_list_get_type   (void) G_GNUC_CONST;
GType       gap_dict_get_type   (void) G_GNUC_CONST;

GapList    *gap_value_get_list  (GValue     *value);
void        gap_value_set_list  (GValue     *value,
                                 GapList    *list);
GapDict    *gap_value_get_dict  (GValue     *value);
void        gap_value_set_dict  (GValue     *value,
                                 GapDict    *dict);

gboolean    gap_set_property    (gpointer    object,
                                 const char *prop_name,
                                 MSValue    *value,
                                 char      **error_msg);
/* value must be uninitialized */
gboolean    gap_get_property    (gpointer    object,
                                 const char *prop_name,
                                 GValue     *value,
                                 char      **error_msg);


/**********************************************************************************/
/* Glade xml
 */

typedef struct _GapCallback GapCallback;

struct _GapCallback {
    gpointer thing;
};


void        gap_callback_free   (GapCallback    *cb);
gboolean    gap_glade_xml_new   (const char     *file,
                                 const char     *root,
                                 MooGladeXML   **xml,
                                 GSList        **callbacks);


/**********************************************************************************/
/* GapTreeView
 */

#define GAP_TYPE_TREE_VIEW                (gap_tree_view_get_type ())
#define GAP_TREE_VIEW(object)             (G_TYPE_CHECK_INSTANCE_CAST ((object), GAP_TYPE_TREE_VIEW, GapTreeView))
#define GAP_TREE_VIEW_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), GAP_TYPE_TREE_VIEW, GapTreeViewClass))
#define GAP_IS_TREE_VIEW(object)          (G_TYPE_CHECK_INSTANCE_TYPE ((object), GAP_TYPE_TREE_VIEW))
#define GAP_IS_TREE_VIEW_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), GAP_TYPE_TREE_VIEW))
#define GAP_TREE_VIEW_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), GAP_TYPE_TREE_VIEW, GapTreeViewClass))

typedef struct _GapTreeView      GapTreeView;
typedef struct _GapTreeViewClass GapTreeViewClass;

struct _GapTreeView
{
    GtkTreeView parent;

    GtkTreeSelection *selection;
    GtkTreeViewColumn *column;

    guint list_mode : 1;
};

struct _GapTreeViewClass
{
    GtkTreeViewClass parent_class;

    void (*selection_changed) (GapTreeView *gtv);
};


GType        gap_tree_view_get_type         (void) G_GNUC_CONST;

void         gap_tree_view_set_list_mode    (GapTreeView    *gtv,
                                             gboolean        list);
void         gap_tree_view_set_title        (GapTreeView    *gtv,
                                             const char     *title);
void         gap_tree_view_set_sortable     (GapTreeView    *gtv,
                                             gboolean        sortable);

void         gap_tree_view_set_val_list     (GapTreeView    *gtv,
                                             MSValue       **elms,
                                             guint           n_elms);
gboolean     gap_tree_view_get_items        (GapTreeView    *gtv,
                                             GValue         *value);

GtkTreePath *gap_tree_view_get_selected_row (GapTreeView    *gtv);
void         gap_tree_view_select_row       (GapTreeView    *gtv,
                                             GtkTreePath    *path);
void         gap_tree_view_unselect_row     (GapTreeView    *gtv,
                                             GtkTreePath    *path);
void         gap_tree_view_select_all       (GapTreeView    *gtv);
void         gap_tree_view_unselect_all     (GapTreeView    *gtv);


G_END_DECLS

#endif /* __GAP_WIDGETS_H__ */
