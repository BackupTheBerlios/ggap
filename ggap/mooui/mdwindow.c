/*
 *   mdwindow.c
 *
 *   Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "mdwindow-private.h"
#include "mdview-private.h"
#include "mddocument-private.h"
#include "mdmanager-private.h"
#include "marshals.h"
#include "ggap-i18n.h"
#include <mooutils/mooaction.h>
#include <mooutils/mooaction.h>
#include <mooutils/moomenuaction.h>
#include <mooutils/mooutils-misc.h>
#include <mooutils/moonotebook.h>
#include <mooutils/moostock.h>
#include <gtk/gtk.h>
#include <string.h>
#include <math.h>

#if GTK_CHECK_VERSION(2,10,0)
#undef ENABLE_PRINTING
#endif

#define DEFAULT_TITLE_FORMAT        "%a - %f%s"
#define DEFAULT_TITLE_FORMAT_NO_DOC "%a"

#define ACTIVE_DOC md_window_get_active_doc
#define ACTIVE_VIEW md_window_get_active_view

struct MdWindowPrivate {
    MdManager *mgr;

    MdView *active;

    char *title_format;
    char *title_format_no_doc;

    MooUIXML *xml;
    guint doc_list_merge_id;
    guint doc_list_update_idle;

    GList *history;
    guint history_blocked : 1;
};

enum {
    TARGET_DOCUMENT_TAB = 1,
    TARGET_URI_LIST = 2
};

enum {
    ACTIVE_VIEW_CHANGED,
    INSERT_VIEW,
    REMOVE_VIEW,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

static GdkAtom document_tab_atom;
static GdkAtom text_uri_atom;

static GtkTargetEntry dest_targets[] = {
    {(char*) "MD_DOCUMENT_TAB", GTK_TARGET_SAME_APP, TARGET_DOCUMENT_TAB},
    {(char*) "text/uri-list", 0, TARGET_URI_LIST}
};

static GObject     *md_window_constructor           (GType               type,
                                                     guint               n_props,
                                                     GObjectConstructParam *props);
static void         md_window_dispose               (GObject            *object);
static void         md_window_set_property          (GObject            *object,
                                                     guint               property_id,
                                                     const GValue       *value,
                                                     GParamSpec         *pspec);
static void         md_window_get_property          (GObject            *object,
                                                     guint               property_id,
                                                     GValue             *value,
                                                     GParamSpec         *pspec);

static gboolean     md_window_close                 (MooWindow  *window);

static void         md_window_active_view_changed   (MdWindow   *window);
static void         md_window_insert_view_real      (MdWindow   *window,
                                                     MdView     *view);
static void         md_window_remove_view_real      (MdWindow   *window,
                                                     MdView     *view);

static GtkWidget   *create_notebook                 (MdWindow   *window);
static void         update_window_title             (MdWindow   *window);

static int          md_window_n_views               (MdWindow   *window);

static void         proxy_doc_boolean_property      (MdWindow   *window,
                                                     GParamSpec *prop,
                                                     MdDocument *doc);
static GtkWidget   *create_tab_label                (MdWindow   *window,
                                                     MdView     *view);
static void         set_title_format                (MdWindow   *window,
                                                     const char *format,
                                                     const char *format_no_doc);

static MdView      *get_nth_tab                     (MdWindow   *window,
                                                     guint       n);
static int          get_page_num                    (MdWindow   *window,
                                                     MdView     *view);

static void         update_doc_list                 (MdWindow   *window);

static void         notebook_drag_data_recv         (GtkWidget          *widget,
                                                     GdkDragContext     *context,
                                                     int                 x,
                                                     int                 y,
                                                     GtkSelectionData   *data,
                                                     guint               info,
                                                     guint               time,
                                                     MdWindow           *window);
static gboolean     notebook_drag_drop              (GtkWidget          *widget,
                                                     GdkDragContext     *context,
                                                     int                 x,
                                                     int                 y,
                                                     guint               time,
                                                     MdWindow           *window);
static gboolean     notebook_drag_motion            (GtkWidget          *widget,
                                                     GdkDragContext     *context,
                                                     int                 x,
                                                     int                 y,
                                                     guint               time,
                                                     MdWindow           *window);

/* actions */
static void action_new_doc          (MdWindow   *window);
static void action_new_window       (MdWindow   *window);
static void action_open             (MdWindow   *window);
static void action_reload           (MdWindow   *window);
static void action_save             (MdWindow   *window);
static void action_save_as          (MdWindow   *window);
static void action_close_tab        (MdWindow   *window);
static void action_close_all        (MdWindow   *window);
static void action_previous_tab     (MdWindow   *window);
static void action_next_tab         (MdWindow   *window);
static void action_switch_to_tab    (MdWindow   *window,
                                     guint       n);

#ifdef ENABLE_PRINTING
static void action_page_setup       (MdWindow   *window);
static void action_print            (MdWindow   *window);
static void action_print_preview    (MdWindow   *window);
static void action_print_pdf        (MdWindow   *window);
#endif


/* MD_TYPE_WINDOW */
G_DEFINE_TYPE (MdWindow, md_window, MOO_TYPE_WINDOW)

enum {
    PROP_0,
    PROP_DOCUMENT_MANAGER,
    PROP_ACTIVE_VIEW,

    /* aux properties */
    PROP_MD_CAN_RELOAD,
    PROP_MD_HAS_OPEN_DOCUMENT,
    PROP_MD_HAS_UNDO,
    PROP_MD_CAN_UNDO,
    PROP_MD_CAN_REDO
};


#define INSTALL_BOOL_PROP(prop_id,name)                                     \
    g_object_class_install_property (gobject_class, prop_id,                \
        g_param_spec_boolean (name, name, name, FALSE, G_PARAM_READABLE))


static void
md_window_class_init (MdWindowClass *klass)
{
    guint i;
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);

    gobject_class->constructor = md_window_constructor;
    gobject_class->dispose = md_window_dispose;
    gobject_class->set_property = md_window_set_property;
    gobject_class->get_property = md_window_get_property;

    window_class->close = md_window_close;

    klass->active_view_changed = md_window_active_view_changed;
    klass->insert_view = md_window_insert_view_real;
    klass->remove_view = md_window_remove_view_real;

    document_tab_atom = gdk_atom_intern ("MD_DOCUMENT_TAB", FALSE);
    text_uri_atom = gdk_atom_intern ("text/uri-list", FALSE);

    g_type_class_add_private (klass, sizeof (MdWindowPrivate));

    g_object_class_install_property (gobject_class, PROP_DOCUMENT_MANAGER,
        g_param_spec_object ("document-manager",
                             "document-manager",
                             "document-manager",
                             MD_TYPE_MANAGER,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (gobject_class, PROP_ACTIVE_VIEW,
        g_param_spec_object ("active-view",
                             "active-view",
                             "active-view",
                             MD_TYPE_VIEW,
                             G_PARAM_READWRITE));

    signals[ACTIVE_VIEW_CHANGED] =
            g_signal_new ("active-view-changed",
                          G_OBJECT_CLASS_TYPE (klass),
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (MdWindowClass, active_view_changed),
                          NULL, NULL,
                          _moo_ui_marshal_VOID__VOID,
                          G_TYPE_NONE, 0);

    signals[INSERT_VIEW] =
            g_signal_new ("insert-view",
                          G_OBJECT_CLASS_TYPE (klass),
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (MdWindowClass, insert_view),
                          NULL, NULL,
                          _moo_ui_marshal_VOID__OBJECT,
                          G_TYPE_NONE, 1,
                          MD_TYPE_VIEW);

    signals[REMOVE_VIEW] =
            g_signal_new ("remove-view",
                          G_OBJECT_CLASS_TYPE (klass),
                          G_SIGNAL_RUN_LAST,
                          G_STRUCT_OFFSET (MdWindowClass, remove_view),
                          NULL, NULL,
                          _moo_ui_marshal_VOID__OBJECT,
                          G_TYPE_NONE, 1,
                          MD_TYPE_VIEW);

    INSTALL_BOOL_PROP (PROP_MD_CAN_RELOAD, "md-can-reload");
    INSTALL_BOOL_PROP (PROP_MD_HAS_OPEN_DOCUMENT, "md-has-open-document");
    INSTALL_BOOL_PROP (PROP_MD_HAS_UNDO, "md-has-undo");
    INSTALL_BOOL_PROP (PROP_MD_CAN_UNDO, "md-can-undo");
    INSTALL_BOOL_PROP (PROP_MD_CAN_REDO, "md-can-redo");

    moo_window_class_new_action (window_class, "NewDoc", NULL,
                                 "display-name", GTK_STOCK_NEW,
                                 "label", GTK_STOCK_NEW,
                                 "tooltip", _("Create new document"),
                                 "stock-id", GTK_STOCK_NEW,
                                 "accel", "<ctrl>N",
                                 "closure-callback", action_new_doc,
                                 NULL);

    moo_window_class_new_action (window_class, "NewWindow", NULL,
                                 "display-name", MOO_STOCK_NEW_WINDOW,
                                 "label", MOO_STOCK_NEW_WINDOW,
                                 "tooltip", _("Open new window"),
                                 "stock-id", MOO_STOCK_NEW_WINDOW,
                                 "accel", "<shift><ctrl>N",
                                 "closure-callback", action_new_window,
                                 NULL);

    moo_window_class_new_action (window_class, "Open", NULL,
                                 "display-name", GTK_STOCK_OPEN,
                                 "label", _("_Open..."),
                                 "tooltip", _("Open..."),
                                 "stock-id", GTK_STOCK_OPEN,
                                 "accel", "<ctrl>O",
                                 "closure-callback", action_open,
                                 NULL);

    moo_window_class_new_action (window_class, "Reload", NULL,
                                 "display-name", _("Reload"),
                                 "label", _("_Reload"),
                                 "tooltip", _("Reload document"),
                                 "stock-id", GTK_STOCK_REFRESH,
                                 "accel", "F5",
                                 "closure-callback", action_reload,
                                 "condition::sensitive", "md-can-reload",
                                 NULL);

    moo_window_class_new_action (window_class, "Save", NULL,
                                 "display-name", GTK_STOCK_SAVE,
                                 "label", GTK_STOCK_SAVE,
                                 "tooltip", GTK_STOCK_SAVE,
                                 "stock-id", GTK_STOCK_SAVE,
                                 "accel", "<ctrl>S",
                                 "closure-callback", action_save,
                                 "condition::sensitive", "md-has-open-document",
                                 NULL);

    moo_window_class_new_action (window_class, "SaveAs", NULL,
                                 "display-name", GTK_STOCK_SAVE_AS,
                                 "label", _("Save _As..."),
                                 "tooltip", _("Save as..."),
                                 "stock-id", GTK_STOCK_SAVE_AS,
                                 "accel", "<ctrl><shift>S",
                                 "closure-callback", action_save_as,
                                 "condition::sensitive", "md-has-open-document",
                                 NULL);

    moo_window_class_new_action (window_class, "Close", NULL,
                                 "display-name", GTK_STOCK_CLOSE,
                                 "label", GTK_STOCK_CLOSE,
                                 "tooltip", _("Close document"),
                                 "stock-id", GTK_STOCK_CLOSE,
                                 "accel", "<ctrl>W",
                                 "closure-callback", action_close_tab,
                                 "condition::sensitive", "md-has-open-document",
                                 NULL);

    moo_window_class_new_action (window_class, "CloseAll", NULL,
                                 "display-name", _("Close All"),
                                 "label", _("Close A_ll"),
                                 "tooltip", _("Close all documents"),
                                 "accel", "<shift><ctrl>W",
                                 "closure-callback", action_close_all,
                                 "condition::sensitive", "md-has-open-document",
                                 NULL);

    moo_window_class_new_action (window_class, "Undo", NULL,
                                 "display-name", GTK_STOCK_UNDO,
                                 "label", GTK_STOCK_UNDO,
                                 "tooltip", GTK_STOCK_UNDO,
                                 "stock-id", GTK_STOCK_UNDO,
                                 "accel", "<ctrl>Z",
                                 "closure-signal", "undo",
                                 "closure-proxy-func", md_window_get_active_doc,
                                 "condition::visible", "md-has-undo",
                                 "condition::sensitive", "md-can-undo",
                                 NULL);

    moo_window_class_new_action (window_class, "Redo", NULL,
                                 "display-name", GTK_STOCK_REDO,
                                 "label", GTK_STOCK_REDO,
                                 "tooltip", GTK_STOCK_REDO,
                                 "stock-id", GTK_STOCK_REDO,
                                 "accel", "<shift><ctrl>Z",
                                 "closure-signal", "redo",
                                 "closure-proxy-func", md_window_get_active_doc,
                                 "condition::visible", "md-has-undo",
                                 "condition::sensitive", "md-can-redo",
                                 NULL);

    moo_window_class_new_action (window_class, "PreviousTab", NULL,
                                 "display-name", _("Previous Tab"),
                                 "label", _("_Previous Tab"),
                                 "tooltip", _("Previous tab"),
                                 "stock-id", GTK_STOCK_GO_BACK,
                                 "accel", "<alt>Left",
                                 "closure-callback", action_previous_tab,
                                 "condition::sensitive", "md-has-open-document",
                                 NULL);

    moo_window_class_new_action (window_class, "NextTab", NULL,
                                 "display-name", _("Next Tab"),
                                 "label", _("_Next Tab"),
                                 "tooltip", _("Next tab"),
                                 "stock-id", GTK_STOCK_GO_FORWARD,
                                 "accel", "<alt>Right",
                                 "closure-callback", action_next_tab,
                                 "condition::sensitive", "md-has-open-document",
                                 NULL);

    moo_window_class_new_action (window_class, "FocusDoc", NULL,
                                 "display-name", _("Focus Document"),
                                 "label", _("_Focus Document"),
                                 "accel", "<alt>C",
                                 "closure-callback", gtk_widget_grab_focus,
                                 "closure-proxy-func", md_window_get_active_doc,
                                 "condition::sensitive", "md-has-open-document",
                                 NULL);

    for (i = 1; i < 10; ++i)
    {
        char *action_id = g_strdup_printf ("SwitchToTab%u", i);
        char *accel = g_strdup_printf ("<Alt>%u", i);
        _moo_window_class_new_action_callback (window_class, action_id, NULL,
                                               G_CALLBACK (action_switch_to_tab),
                                               _moo_ui_marshal_VOID__UINT,
                                               G_TYPE_NONE, 1,
                                               G_TYPE_UINT, i - 1,
                                               "accel", accel,
                                               "connect-accel", TRUE,
                                               "accel-editable", FALSE,
                                               NULL);
        g_free (accel);
        g_free (action_id);
    }

    moo_window_class_new_action (window_class, "NoDocuments", NULL,
                                 /* Insensitive menu item which appears in Window menu with no documents open */
                                 "label", _("No Documents"),
                                 "no-accel", TRUE,
                                 "sensitive", FALSE,
                                 "condition::visible", "!md-has-open-document",
                                 NULL);

#ifdef ENABLE_PRINTING
    moo_window_class_new_action (window_class, "PageSetup", NULL,
                                 "display-name", _("Page Setup"),
                                 "label", _("Page S_etup..."),
                                 "tooltip", _("Page Setup..."),
                                 "accel", "<ctrl><shift>P",
                                 "closure-callback", action_page_setup,
                                 NULL);

    moo_window_class_new_action (window_class, "PrintPreview", NULL,
                                 "display-name", GTK_STOCK_PRINT_PREVIEW,
                                 "label", GTK_STOCK_PRINT_PREVIEW,
                                 "tooltip", GTK_STOCK_PRINT_PREVIEW,
                                 "stock-id", GTK_STOCK_PRINT_PREVIEW,
                                 "closure-callback", action_print_preview,
                                 "condition::sensitive", "md-has-open-document",
                                 NULL);

    moo_window_class_new_action (window_class, "Print", NULL,
                                 "display-name", GTK_STOCK_PRINT,
                                 "label", _("Print..."),
                                 "tooltip", _("Print..."),
                                 "accel", "<ctrl>P",
                                 "stock-id", GTK_STOCK_PRINT,
                                 "closure-callback", action_print,
                                 "condition::sensitive", "md-has-open-document",
                                 NULL);

    moo_window_class_new_action (window_class, "PrintPdf", NULL,
                                 "display-name", _("Export as PDF"),
                                 "label", _("E_xport as PDF..."),
                                 "tooltip", _("Export as PDF..."),
                                 "stock-id", GTK_STOCK_PRINT,
                                 "closure-callback", action_print_pdf,
                                 "condition::sensitive", "md-has-open-document",
                                 NULL);
#endif
}


static void
md_window_init (MdWindow *window)
{
    window->priv = G_TYPE_INSTANCE_GET_PRIVATE (window, MD_TYPE_WINDOW, MdWindowPrivate);

    window->notebook = create_notebook (window);
    MOO_OBJECT_REF_SINK (window->notebook);

    window->priv->history = NULL;
    window->priv->history_blocked = FALSE;

    set_title_format (window, DEFAULT_TITLE_FORMAT, DEFAULT_TITLE_FORMAT_NO_DOC);
}

static void
md_window_dispose (GObject *object)
{
    MdWindow *window = MD_WINDOW (object);

    if (window->notebook)
    {
        g_object_unref (window->notebook);
        window->notebook = NULL;
    }

    if (window->priv)
    {
        if (window->priv->doc_list_merge_id)
        {
            moo_ui_xml_remove_ui (window->priv->xml,
                                  window->priv->doc_list_merge_id);
            window->priv->doc_list_merge_id = 0;
        }

        if (window->priv->doc_list_update_idle)
        {
            g_source_remove (window->priv->doc_list_update_idle);
            window->priv->doc_list_update_idle = 0;
        }

        g_free (window->priv->title_format);
        g_free (window->priv->title_format_no_doc);

        window->priv = NULL;
    }

    G_OBJECT_CLASS (md_window_parent_class)->dispose (object);
}

gboolean
_md_window_destroyed (MdWindow *window)
{
    g_return_val_if_fail (MD_IS_WINDOW (window), TRUE);
    return window->priv == NULL;
}


static void
md_window_set_property (GObject      *object,
                        guint         prop_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
    MdWindow *window = MD_WINDOW (object);

    switch (prop_id)
    {
        case PROP_DOCUMENT_MANAGER:
            window->priv->mgr = g_value_get_object (value);
            break;

        case PROP_ACTIVE_VIEW:
            md_window_set_active_view (window, g_value_get_object (value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
md_window_get_property (GObject    *object,
                        guint       prop_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
    MdWindow *window = MD_WINDOW (object);
    MdDocument *doc;

    switch (prop_id)
    {
        case PROP_DOCUMENT_MANAGER:
            g_value_set_object (value, window->priv->mgr);
            break;

        case PROP_ACTIVE_VIEW:
            g_value_set_object (value, ACTIVE_VIEW (window));
            break;

        case PROP_MD_CAN_RELOAD:
            doc = ACTIVE_DOC (window);
            g_value_set_boolean (value, doc && !_md_document_is_untitled (doc));
            break;
        case PROP_MD_HAS_OPEN_DOCUMENT:
            g_value_set_boolean (value, ACTIVE_DOC (window) != NULL);
            break;
        case PROP_MD_HAS_UNDO:
            doc = ACTIVE_DOC (window);
            g_value_set_boolean (value, doc && MD_IS_HAS_UNDO (doc));
            break;
        case PROP_MD_CAN_UNDO:
            doc = ACTIVE_DOC (window);
            g_value_set_boolean (value, doc && MD_IS_HAS_UNDO (doc) && _md_document_can_undo (doc));
            break;
        case PROP_MD_CAN_REDO:
            doc = ACTIVE_DOC (window);
            g_value_set_boolean (value, doc && MD_IS_HAS_UNDO (doc) && _md_document_can_redo (doc));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static GObject *
md_window_constructor (GType                  type,
                       guint                  n_props,
                       GObjectConstructParam *props)
{
    MdWindow *window;
    GtkWindowGroup *group;

    GObject *object = G_OBJECT_CLASS(md_window_parent_class)->constructor (type, n_props, props);

    window = MD_WINDOW (object);
    g_return_val_if_fail (window->priv->mgr != NULL, object);

    gtk_box_pack_start (GTK_BOX (MOO_WINDOW (window)->vbox), window->notebook, TRUE, TRUE, 0);

    group = gtk_window_group_new ();
    gtk_window_group_add_window (group, GTK_WINDOW (window));
    g_object_unref (group);

    g_signal_connect (window, "notify::ui-xml",
                      G_CALLBACK (update_doc_list), NULL);
    update_doc_list (window);

    return object;
}


MdManager *
md_window_get_manager (MdWindow *window)
{
    g_return_val_if_fail (MD_IS_WINDOW (window), NULL);
    return window->priv->mgr;
}


static const char *
get_doc_status_string (MdDocument *doc)
{
    MdDocumentStatus status;
    gboolean modified;
    const char *doc_title_format;

    status = md_document_get_status (doc);
    modified = md_document_get_modified (doc);

    if (!modified)
    {
        if (status & MD_DOCUMENT_NEW)
            /* Translators: this goes into window title, %s stands for filename, e.g. "/foo/bar.txt [new file]" */
            doc_title_format = _(" [new file]");
        else if (status & MD_DOCUMENT_MODIFIED_ON_DISK)
            /* Translators: this goes into window title, %s stands for filename, e.g. "/foo/bar.txt [modified on disk]" */
            doc_title_format = _(" [modified on disk]");
        else if (status & MD_DOCUMENT_DELETED)
            /* Translators: this goes into window title, %s stands for filename, e.g. "/foo/bar.txt [deleted]" */
            doc_title_format = _(" [deleted]");
        else
            doc_title_format = "";
    }
    else
    {
        if (status & MD_DOCUMENT_NEW)
            /* Translators: this goes into window title, %s stands for filename, e.g. "/foo/bar.txt [new file] [modified]" */
            doc_title_format = _(" [new file] [modified]");
        else if (status & MD_DOCUMENT_MODIFIED_ON_DISK)
            /* Translators: this goes into window title, %s stands for filename, e.g. "/foo/bar.txt [modified on disk] [modified]" */
            doc_title_format = _(" [modified on disk] [modified]");
        else if (status & MD_DOCUMENT_DELETED)
            /* Translators: this goes into window title, %s stands for filename, e.g. "/foo/bar.txt [deleted] [modified]" */
            doc_title_format = _(" [deleted] [modified]");
        else
            /* Translators: this goes into window title, %s stands for filename, e.g. "/foo/bar.txt [modified]" */
            doc_title_format = _(" [modified]");
    }

    return doc_title_format;
}

static char *
parse_title_format (const char *format,
                    MdWindow   *window,
                    MdDocument *doc)
{
    GString *str;

    str = g_string_new (NULL);

    while (*format)
    {
        if (*format == '%')
        {
            format++;

            if (!*format)
            {
                g_critical ("%s: trailing percent sign", G_STRFUNC);
                break;
            }

            switch (*format)
            {
                case 'a':
                    g_string_append (str, _md_manager_get_app_name (window->priv->mgr));
                    break;
                case 'b':
                    if (!doc)
                        g_critical ("%s: %%b used without document", G_STRFUNC);
                    else
                        g_string_append (str, md_document_get_display_basename (doc));
                    break;
                case 'f':
                    if (!doc)
                        g_critical ("%s: %%f used without document", G_STRFUNC);
                    else
                        g_string_append (str, md_document_get_display_name (doc));
                    break;
                case 'u':
                    if (!doc)
                        g_critical ("%s: %%u used without document", G_STRFUNC);
                    else
                    {
                        const char *uri = md_document_get_uri (doc);
                        if (uri)
                            g_string_append (str, uri);
                        else
                            g_string_append (str, md_document_get_display_name (doc));
                    }
                    break;
                case 's':
                    if (!doc)
                        g_critical ("%s: %%s used without document", G_STRFUNC);
                    else
                    {
                        const char *status = get_doc_status_string (doc);
                        if (status)
                            g_string_append (str, status);
                    }
                    break;
                case '%':
                    g_string_append_c (str, '%');
                    break;
                default:
                    g_critical ("%s: unknown format '%%%c'", G_STRFUNC, *format);
                    break;
            }
        }
        else
        {
            g_string_append_c (str, *format);
        }

        format++;
    }

    return g_string_free (str, FALSE);
}

static void
update_window_title (MdWindow *window)
{
    MdDocument *doc;
    char *title;

    doc = ACTIVE_DOC (window);

    if (doc)
        title = parse_title_format (window->priv->title_format, window, doc);
    else
        title = parse_title_format (window->priv->title_format_no_doc, window, NULL);

    gtk_window_set_title (GTK_WINDOW (window), title);

    g_free (title);
}

static const char *
check_format (const char *format)
{
    if (!format || !format[0])
        return DEFAULT_TITLE_FORMAT;

    if (!g_utf8_validate (format, -1, NULL))
    {
        g_critical ("%s: window title format is not valid UTF8", G_STRLOC);
        return DEFAULT_TITLE_FORMAT;
    }

    return format;
}

static void
set_title_format (MdWindow   *window,
                  const char *format,
                  const char *format_no_doc)
{
    format = check_format (format);
    format_no_doc = check_format (format_no_doc);

    g_free (window->priv->title_format);
    g_free (window->priv->title_format_no_doc);

    window->priv->title_format = g_strdup (format);
    window->priv->title_format_no_doc = g_strdup (format_no_doc);

    if (GTK_WIDGET_REALIZED (window))
        update_window_title (window);
}


static gboolean
md_window_close (MooWindow *window)
{
    MdWindow *doc_window = MD_WINDOW (window);
    _md_manager_action_close_window (doc_window->priv->mgr, doc_window);
    return TRUE;
}


/****************************************************************************/
/* Actions
 */

static void
action_new_doc (MdWindow *window)
{
    _md_manager_action_new_doc (window->priv->mgr, window);
}

static void
action_new_window (MdWindow *window)
{
    _md_manager_action_new_window (window->priv->mgr);
}

static void
action_open (MdWindow *window)
{
    _md_manager_action_open (window->priv->mgr, window);
}

static void
action_reload (MdWindow *window)
{
    MdView *view = ACTIVE_VIEW (window);
    g_return_if_fail (view != NULL);
    _md_manager_action_reload (window->priv->mgr, view);
}

static void
action_save (MdWindow *window)
{
    MdView *view = ACTIVE_VIEW (window);
    g_return_if_fail (view != NULL);
    _md_manager_action_save (window->priv->mgr, view);
}

static void
action_save_as (MdWindow *window)
{
    MdView *view = ACTIVE_VIEW (window);
    g_return_if_fail (view != NULL);
    _md_manager_action_save_as (window->priv->mgr, view);
}

static void
action_close_one (MdWindow *window,
                  MdView   *view)
{
    GSList *list;

    g_return_if_fail (view != NULL);

    list = g_slist_prepend (NULL, view);
    _md_manager_action_close_views (window->priv->mgr, list);
    g_slist_free (list);
}

static void
action_close_tab (MdWindow *window)
{
    action_close_one (window, ACTIVE_VIEW (window));
}

static void
action_close_all (MdWindow *window)
{
    GSList *list = md_window_list_views (window);
    _md_manager_action_close_views (window->priv->mgr, list);
    g_slist_free (list);
}


static void
switch_to_tab (MdWindow *window,
               int       n)
{
    MdDocument *doc;

    if (n < 0)
        n = md_window_n_views (window) - 1;

    if (n < 0 || n >= md_window_n_views (window))
        return;

    moo_notebook_set_current_page (MOO_NOTEBOOK (window->notebook), n);

    if ((doc = ACTIVE_DOC (window)))
        gtk_widget_grab_focus (GTK_WIDGET (doc));
}

static void
action_previous_tab (MdWindow *window)
{
    int n;

    n = moo_notebook_get_current_page (MOO_NOTEBOOK (window->notebook));

    if (n > 0)
        switch_to_tab (window, n - 1);
    else
        switch_to_tab (window, -1);
}


static void
action_next_tab (MdWindow *window)
{
    int n;

    n = moo_notebook_get_current_page (MOO_NOTEBOOK (window->notebook));

    if (n < moo_notebook_get_n_pages (MOO_NOTEBOOK (window->notebook)) - 1)
        switch_to_tab (window, n + 1);
    else
        switch_to_tab (window, 0);
}

static void
action_switch_to_tab (MdWindow *window,
                      guint              n)
{
    switch_to_tab (window, n);
}


#ifdef ENABLE_PRINTING
static void
action_page_setup (MdWindow *window)
{
    _moo_edit_page_setup (GTK_WIDGET (window));
}


static void
action_print (MdWindow *window)
{
    gpointer doc = ACTIVE_DOC (window);
    g_return_if_fail (doc != NULL);
    _moo_edit_print (doc, GTK_WIDGET (window));
}


static void
action_print_preview (MdWindow *window)
{
    gpointer doc = ACTIVE_DOC (window);
    g_return_if_fail (doc != NULL);
    _moo_edit_print_preview (doc, GTK_WIDGET (window));
}


static void
action_print_pdf (MdWindow *window)
{
    char *start_name;
    const char *doc_name, *dot;
    const char *filename;
    gpointer doc = ACTIVE_DOC (window);

    doc_name = doc ? moo_edit_get_display_basename (doc) : "output";
    dot = strrchr (doc_name, '.');

    if (dot && dot != doc_name)
    {
        start_name = g_new (char, (dot - doc_name) + 5);
        memcpy (start_name, doc_name, dot - doc_name);
        memcpy (start_name + (dot - doc_name), ".pdf", 5);
    }
    else
    {
        start_name = g_strdup_printf ("%s.pdf", doc_name);
    }

    doc = ACTIVE_DOC (window);
    g_return_if_fail (doc != NULL);

    filename = moo_file_dialogp (GTK_WIDGET (window),
                                 MOO_FILE_DIALOG_SAVE,
                                 start_name,
                                 _("Export as PDF"),
                                 moo_edit_setting (MOO_EDIT_PREFS_PDF_LAST_DIR),
                                 NULL);

    if (filename)
        _moo_edit_export_pdf (doc, filename);

    g_free (start_name);
}
#endif


/****************************************************************************/
/* Notebook popup menu
 */

static void
close_activated (GtkWidget *item,
                 MdWindow  *window)
{
    MdView *view = g_object_get_data (G_OBJECT (item), "md-document-view");

    g_return_if_fail (MD_IS_WINDOW (window));
    g_return_if_fail (MD_IS_VIEW (view));

    action_close_one (window, view);
}

static void
close_others_activated (GtkWidget *item,
                        MdWindow  *window)
{
    GSList *list;
    MdView *view = g_object_get_data (G_OBJECT (item), "md-document-view");

    g_return_if_fail (MD_IS_WINDOW (window));
    g_return_if_fail (MD_IS_VIEW (view));

    list = md_window_list_views (window);
    list = g_slist_remove (list, view);

    if (list)
        _md_manager_action_close_views (window->priv->mgr, list);

    g_slist_free (list);
}

static void
detach_activated (GtkWidget *item,
                  MdWindow  *window)
{
    MdView *view = g_object_get_data (G_OBJECT (item), "md-document-view");

    g_return_if_fail (MD_IS_WINDOW (window));
    g_return_if_fail (MD_IS_VIEW (view));

    _md_manager_move_view (window->priv->mgr, view, NULL);
}


/****************************************************************************/
/* Documents
 */

MdDocument *
md_window_get_active_doc (MdWindow *window)
{
    g_return_val_if_fail (MD_IS_WINDOW (window), NULL);
    return (window->priv && window->priv->active) ?
                md_view_get_doc (window->priv->active) : NULL;
}

MdView *
md_window_get_active_view (MdWindow *window)
{
    g_return_val_if_fail (MD_IS_WINDOW (window), NULL);
    return window->priv ? window->priv->active : NULL;
}

void
md_window_set_active_view (MdWindow  *window,
                           MdView    *view)
{
    GtkWidget *swin;
    int page;

    g_return_if_fail (MD_IS_WINDOW (window));
    g_return_if_fail (MD_IS_VIEW (view));

    swin = GTK_WIDGET (view)->parent;
    page = moo_notebook_page_num (MOO_NOTEBOOK (window->notebook), swin);
    g_return_if_fail (page >= 0);

    moo_notebook_set_current_page (MOO_NOTEBOOK (window->notebook), page);
}

static void
update_active_doc (MdWindow *window)
{
    MdView *view;
    int page;

    if (!window->notebook)
        return;

    page = moo_notebook_get_current_page (MOO_NOTEBOOK (window->notebook));

    if (page < 0)
    {
        view = NULL;
    }
    else
    {
        GtkWidget *swin = moo_notebook_get_nth_page (MOO_NOTEBOOK (window->notebook), page);
        view = MD_VIEW (gtk_bin_get_child (GTK_BIN (swin)));
    }

    if (view != window->priv->active)
    {
        window->priv->active = view;
        g_object_notify (G_OBJECT (window), "active-view");
        g_signal_emit (window, signals[ACTIVE_VIEW_CHANGED], 0);
    }
}

static void
notebook_switch_page (G_GNUC_UNUSED MooNotebook *notebook,
                      G_GNUC_UNUSED guint page_num,
                      MdWindow *window)
{
    update_active_doc (window);
}


static gboolean
notebook_populate_popup (MooNotebook *notebook,
                         GtkWidget   *child,
                         GtkMenu     *menu,
                         MdWindow    *window)
{
    MdDocument *doc;
    GtkWidget *item;

    g_return_val_if_fail (MD_IS_WINDOW (window), TRUE);
    g_return_val_if_fail (window->notebook == GTK_WIDGET (notebook), TRUE);
    g_return_val_if_fail (GTK_IS_SCROLLED_WINDOW (child), TRUE);

    doc = MD_DOCUMENT (gtk_bin_get_child (GTK_BIN (child)));
    g_return_val_if_fail (MD_IS_DOCUMENT (doc), TRUE);

    item = gtk_menu_item_new_with_label ("Close");
    gtk_widget_show (item);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    g_object_set_data (G_OBJECT (item), "md-document-view", doc);
    g_signal_connect (item, "activate",
                      G_CALLBACK (close_activated),
                      window);

    if (md_window_n_views (window) > 1)
    {
        item = gtk_menu_item_new_with_label ("Close All Others");
        gtk_widget_show (item);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
        g_object_set_data (G_OBJECT (item), "md-document-view", doc);
        g_signal_connect (item, "activate",
                          G_CALLBACK (close_others_activated),
                          window);
    }

    if (md_window_n_views (window) > 1)
    {
        gtk_menu_shell_append (GTK_MENU_SHELL (menu),
                               g_object_new (GTK_TYPE_SEPARATOR_MENU_ITEM,
                                             "visible", TRUE, NULL));

        item = gtk_menu_item_new_with_label ("Detach");
        gtk_widget_show (item);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
        g_object_set_data (G_OBJECT (item), "md-document-view", doc);
        g_signal_connect (item, "activate",
                          G_CALLBACK (detach_activated),
                          window);
    }

    return FALSE;
}


static gboolean
notebook_button_press (MooNotebook    *notebook,
                       GdkEventButton *event,
                       MdWindow       *window)
{
    int n;

    if (event->button != 2 || event->type != GDK_BUTTON_PRESS)
        return FALSE;

    n = moo_notebook_get_event_tab (notebook, (GdkEvent*) event);

    if (n < 0)
        return FALSE;

    action_close_one (window, get_nth_tab (window, n));

    return TRUE;
}


// static void
// set_use_tabs (MdWindow *window)
// {
//     g_return_if_fail (MD_IS_WINDOW (window));
//     g_object_set (window->notebook, "show-tabs",
//                   moo_prefs_get_bool (moo_edit_setting (MOO_EDIT_PREFS_USE_TABS)), NULL);
// }
//
// void
// _md_window_set_use_tabs (void)
// {
//     GSList *l;
//
//     for (l = windows; l != NULL; l = l->next)
//         set_use_tabs (l->data);
// }


static GtkWidget *
create_notebook (MdWindow *window)
{
    GtkWidget *notebook, *button, *icon, *frame;

    notebook = g_object_new (MOO_TYPE_NOTEBOOK,
                             "show-tabs", TRUE,
                             "enable-popup", TRUE,
                             "enable-reordering", TRUE,
                             NULL);
    gtk_widget_show (notebook);

    g_signal_connect_after (notebook, "moo-switch-page",
                            G_CALLBACK (notebook_switch_page), window);
    g_signal_connect (notebook, "populate-popup",
                      G_CALLBACK (notebook_populate_popup), window);
    g_signal_connect (notebook, "button-press-event",
                      G_CALLBACK (notebook_button_press), window);

    frame = gtk_aspect_frame_new (NULL, 0.5, 0.5, 1.0, FALSE);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

    button = gtk_button_new ();
    gtk_button_set_focus_on_click (GTK_BUTTON (button), FALSE);
    g_signal_connect_swapped (button, "clicked",
                              G_CALLBACK (action_close_tab), window);
    moo_bind_bool_property (button, "sensitive", window, "md-has-open-document", FALSE);

    icon = gtk_image_new_from_stock (MOO_STOCK_CLOSE, MOO_ICON_SIZE_REAL_SMALL);

    gtk_container_add (GTK_CONTAINER (button), icon);
    gtk_container_add (GTK_CONTAINER (frame), button);
    gtk_widget_show_all (frame);
    moo_notebook_set_action_widget (MOO_NOTEBOOK (notebook), frame, TRUE);

    gtk_drag_dest_set (GTK_WIDGET (notebook), 0,
                       dest_targets, G_N_ELEMENTS (dest_targets),
                       GDK_ACTION_COPY | GDK_ACTION_MOVE);
    gtk_drag_dest_add_text_targets (GTK_WIDGET (notebook));
    g_signal_connect (notebook, "drag-motion",
                      G_CALLBACK (notebook_drag_motion), window);
    g_signal_connect (notebook, "drag-drop",
                      G_CALLBACK (notebook_drag_drop), window);
    g_signal_connect (notebook, "drag-data-received",
                      G_CALLBACK (notebook_drag_data_recv), window);

    return notebook;
}


static void
proxy_doc_boolean_property (MdWindow   *window,
                            GParamSpec *prop,
                            MdDocument *doc)
{
    if (doc == ACTIVE_DOC (window))
        g_object_notify (G_OBJECT (window), prop->name);
}


GSList *
md_window_list_views (MdWindow *window)
{
    GSList *list = NULL;
    int num, i;

    g_return_val_if_fail (MD_IS_WINDOW (window), NULL);

    num = moo_notebook_get_n_pages (MOO_NOTEBOOK (window->notebook));

    for (i = 0; i < num; i++)
        list = g_slist_prepend (list, get_nth_tab (window, i));

    return g_slist_reverse (list);
}


static int
md_window_n_views (MdWindow *window)
{
    g_return_val_if_fail (MD_IS_WINDOW (window), 0);
    return moo_notebook_get_n_pages (MOO_NOTEBOOK (window->notebook));
}


// MdDocument *
// md_window_get_nth_doc (MdWindow *window,
//                                  guint              n)
// {
//     g_return_val_if_fail (MD_IS_WINDOW (window), NULL);
//
//     if (!window->notebook || n >= (guint) moo_notebook_get_n_pages (MOO_NOTEBOOK (window->notebook)))
//         return NULL;
//
//     return get_nth_tab (window, n);
// }


static MdView *
get_nth_tab (MdWindow *window,
             guint     n)
{
    GtkWidget *swin;

    swin = moo_notebook_get_nth_page (MOO_NOTEBOOK (window->notebook), n);

    if (swin)
        return MD_VIEW (gtk_bin_get_child (GTK_BIN (swin)));
    else
        return NULL;
}


static int
get_page_num (MdWindow *window,
              MdView   *view)
{
    GtkWidget *swin;

    g_return_val_if_fail (MD_IS_WINDOW (window), -1);
    g_return_val_if_fail (MD_IS_VIEW (view), -1);

    swin = GTK_WIDGET (view)->parent;
    return moo_notebook_page_num (MOO_NOTEBOOK (window->notebook), swin);
}


void
_md_window_insert_view (MdWindow *window,
                        MdView   *view)
{
    g_return_if_fail (MD_IS_WINDOW (window));
    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (md_view_get_window (view) == NULL);

    g_signal_emit (window, signals[INSERT_VIEW], 0, view);
}

void
_md_window_remove_view (MdWindow *window,
                        MdView   *view)
{
    g_return_if_fail (MD_IS_WINDOW (window));
    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (md_view_get_window (view) == window);

    g_signal_emit (window, signals[REMOVE_VIEW], 0, view);
}


static void
md_window_active_view_changed (MdWindow *window)
{
    g_object_freeze_notify (G_OBJECT (window));
    g_object_notify (G_OBJECT (window), "md-can-reload");
    g_object_notify (G_OBJECT (window), "md-has-undo");
    g_object_notify (G_OBJECT (window), "md-can-undo");
    g_object_notify (G_OBJECT (window), "md-can-redo");
    g_object_thaw_notify (G_OBJECT (window));

    update_window_title (window);
    update_doc_list (window);
}


static void
doc_status_notify (MdWindow   *window,
                   G_GNUC_UNUSED GParamSpec *pspec,
                   MdDocument *doc)
{
    if (doc == ACTIVE_DOC (window))
        update_window_title (window);
}

static void
doc_file_info_notify (MdWindow   *window,
                      G_GNUC_UNUSED GParamSpec *pspec,
                      MdDocument *doc)
{
    update_doc_list (window);

    if (doc == ACTIVE_DOC (window))
        update_window_title (window);
}

static void
connect_doc (MdWindow   *window,
             MdDocument *doc)
{
    /* XXX multiple views */

    g_signal_connect_swapped (doc, "notify::md-doc-status",
                              G_CALLBACK (doc_status_notify), window);
    g_signal_connect_swapped (doc, "notify::md-doc-file-info",
                              G_CALLBACK (doc_file_info_notify), window);

    if (MD_IS_HAS_UNDO (doc))
    {
        g_signal_connect_swapped (doc, "notify::md-can-undo",
                                  G_CALLBACK (proxy_doc_boolean_property), window);
        g_signal_connect_swapped (doc, "notify::md-can-redo",
                                  G_CALLBACK (proxy_doc_boolean_property), window);
    }
}

static void
disconnect_doc (MdWindow   *window,
                MdDocument *doc)
{
    /* XXX multiple views */

    g_signal_handlers_disconnect_by_func (doc, (gpointer) doc_status_notify, window);
    g_signal_handlers_disconnect_by_func (doc, (gpointer) doc_file_info_notify, window);

    if (MD_IS_HAS_UNDO (doc))
        g_signal_handlers_disconnect_by_func (doc, (gpointer) proxy_doc_boolean_property, window);
}

static void
view_set_document (MdView     *view,
                   MdDocument *doc,
                   MdWindow   *window)
{
    MdDocument *old_doc;

    /* XXX multiple views */

    if ((old_doc = md_view_get_doc (view)))
        disconnect_doc (window, old_doc);

    if (doc)
        connect_doc (window, doc);
}

static void
md_window_insert_view_real (MdWindow *window,
                            MdView   *view)
{
    GtkWidget *label;
    GtkWidget *scrolledwindow;
    MdDocument *doc;
    int position;

    _md_view_set_window (view, window);

    label = create_tab_label (window, view);
    gtk_widget_show (label);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow),
                                         GTK_SHADOW_ETCHED_IN);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), GTK_WIDGET (view));
    gtk_widget_show_all (scrolledwindow);

    position = moo_notebook_get_current_page (MOO_NOTEBOOK (window->notebook)) + 1;
    moo_notebook_insert_page (MOO_NOTEBOOK (window->notebook), scrolledwindow, label, position);

    if (moo_notebook_get_n_pages (MOO_NOTEBOOK (window->notebook)) == 1)
    {
        gtk_widget_grab_focus (GTK_WIDGET (view));
        g_object_notify (G_OBJECT (window), "md-has-open-document");
    }

    g_signal_connect (view, "set-document", G_CALLBACK (view_set_document), window);
    if ((doc = md_view_get_doc (view)))
        connect_doc (window, doc);

    update_active_doc (window);
    update_doc_list (window);
}


static void
md_window_remove_view_real (MdWindow *window,
                            MdView   *view)
{
    int page;
    GtkAction *action;
    MdView *new_view;
//     gboolean had_focus;
    gboolean was_active;
    MdDocument *doc;

    page = get_page_num (window, view);
    g_return_if_fail (page >= 0);

//     had_focus = GTK_WIDGET_HAS_FOCUS (view);

    if ((doc = md_view_get_doc (view)))
        disconnect_doc (window, doc);
    g_signal_handlers_disconnect_by_func (view, (gpointer) view_set_document, window);

    action = g_object_get_data (G_OBJECT (view), "md-view-list-action");

    if (action)
    {
        moo_action_collection_remove_action (moo_window_get_actions (MOO_WINDOW (window)), action);
        g_object_set_data (G_OBJECT (view), "md-view-list-action", NULL);
    }

    window->priv->history = g_list_remove (window->priv->history, view);
    window->priv->history_blocked = TRUE;

    was_active = window->priv->active == view;
    if (was_active)
        window->priv->active = NULL;

    update_doc_list (window);

    /* removing scrolled window from the notebook will destroy the scrolled window,
     * and that in turn will destroy the doc if it's not removed before */
    gtk_container_remove (GTK_CONTAINER (GTK_WIDGET (view)->parent), GTK_WIDGET (view));
    moo_notebook_remove_page (MOO_NOTEBOOK (window->notebook), page);

    window->priv->history_blocked = FALSE;

    if (was_active && window->priv->history)
        md_window_set_active_view (window, window->priv->history->data);

    update_active_doc (window);
    new_view = ACTIVE_VIEW (window);

//     if (was_active && new_view && had_focus)
//         gtk_widget_grab_focus (GTK_WIDGET (new_view));

    if (!new_view)
        g_object_notify (G_OBJECT (window), "md-has-open-document");
}


typedef struct {
    int x;
    int y;
    gboolean drag_started;
} DragInfo;

static gboolean tab_icon_button_press       (GtkWidget      *evbox,
                                             GdkEventButton *event,
                                             MdWindow       *window);
static gboolean tab_icon_button_release     (GtkWidget      *evbox,
                                             GdkEventButton *event,
                                             MdWindow       *window);
static gboolean tab_icon_motion_notify      (GtkWidget      *evbox,
                                             GdkEventMotion *event,
                                             MdWindow       *window);

static void     tab_icon_drag_begin         (GtkWidget      *evbox,
                                             GdkDragContext *context,
                                             MdWindow       *window);
static void     tab_icon_drag_data_delete   (GtkWidget      *evbox,
                                             GdkDragContext *context,
                                             MdWindow       *window);
static void     tab_icon_drag_data_get      (GtkWidget      *evbox,
                                             GdkDragContext *context,
                                             GtkSelectionData *data,
                                             guint           info,
                                             guint           time,
                                             MdWindow       *window);
static void     tab_icon_drag_end           (GtkWidget      *evbox,
                                             GdkDragContext *context,
                                             MdWindow       *window);

static gboolean
tab_icon_button_release (GtkWidget *evbox,
                         G_GNUC_UNUSED GdkEventButton *event,
                         MdWindow  *window)
{
    g_object_set_data (G_OBJECT (evbox), "md-drag-info", NULL);
    g_signal_handlers_disconnect_by_func (evbox, (gpointer) tab_icon_button_release, window);
    g_signal_handlers_disconnect_by_func (evbox, (gpointer) tab_icon_motion_notify, window);
    return FALSE;
}

static void
tab_icon_start_drag (GtkWidget *evbox,
                     GdkEvent  *event,
                     MdWindow  *window)
{
    GtkTargetList *targets;
    MdDocument *doc;

    doc = g_object_get_data (G_OBJECT (evbox), "md-document-view");
    g_return_if_fail (MD_IS_DOCUMENT (doc));

    g_signal_connect (evbox, "drag-begin", G_CALLBACK (tab_icon_drag_begin), window);
    g_signal_connect (evbox, "drag-data-delete", G_CALLBACK (tab_icon_drag_data_delete), window);
    g_signal_connect (evbox, "drag-data-get", G_CALLBACK (tab_icon_drag_data_get), window);
    g_signal_connect (evbox, "drag-end", G_CALLBACK (tab_icon_drag_end), window);

    targets = gtk_target_list_new (NULL, 0);

    gtk_target_list_add (targets,
                         gdk_atom_intern ("text/uri-list", FALSE),
                         0, TARGET_URI_LIST);
    gtk_target_list_add (targets,
                         gdk_atom_intern ("MD_DOCUMENT_TAB", FALSE),
                         GTK_TARGET_SAME_APP,
                         TARGET_DOCUMENT_TAB);

    gtk_drag_begin (evbox, targets,
                    GDK_ACTION_COPY | GDK_ACTION_MOVE | GDK_ACTION_LINK,
                    1, (GdkEvent*) event);

    gtk_target_list_unref (targets);
}

static gboolean
tab_icon_motion_notify (GtkWidget      *evbox,
                        GdkEventMotion *event,
                        MdWindow       *window)
{
    DragInfo *info;

    info = g_object_get_data (G_OBJECT (evbox), "md-drag-info");
    g_return_val_if_fail (info != NULL, FALSE);

    if (info->drag_started)
        return TRUE;

    if (gtk_drag_check_threshold (evbox, info->x, info->y, event->x, event->y))
    {
        info->drag_started = TRUE;
        tab_icon_start_drag (evbox, (GdkEvent*) event, window);
    }

    return TRUE;
}

static gboolean
tab_icon_button_press (GtkWidget      *evbox,
                       GdkEventButton *event,
                       MdWindow       *window)
{
    DragInfo *info;

    if (event->window != evbox->window || event->button != 1 || event->type != GDK_BUTTON_PRESS)
        return FALSE;

    info = g_new0 (DragInfo, 1);
    info->x = event->x;
    info->y = event->y;
    g_object_set_data_full (G_OBJECT (evbox), "md-drag-info", info, g_free);

    g_signal_connect (evbox, "motion-notify-event", G_CALLBACK (tab_icon_motion_notify), window);
    g_signal_connect (evbox, "button-release-event", G_CALLBACK (tab_icon_button_release), window);

    return FALSE;
}

static void
tab_icon_drag_begin (GtkWidget      *evbox,
                     GdkDragContext *context,
                     G_GNUC_UNUSED MdWindow *window)
{
    GdkPixbuf *pixbuf;
    GtkImage *icon;
    icon = g_object_get_data (G_OBJECT (evbox), "md-document-icon");
    pixbuf = gtk_image_get_pixbuf (icon);
    gtk_drag_set_icon_pixbuf (context, pixbuf, 0, 0);
}

static void
tab_icon_drag_data_delete (G_GNUC_UNUSED GtkWidget      *evbox,
                           G_GNUC_UNUSED GdkDragContext *context,
                           G_GNUC_UNUSED MdWindow       *window)
{
    g_print ("delete!\n");
}

static void
tab_icon_drag_data_get (GtkWidget    *evbox,
                        G_GNUC_UNUSED GdkDragContext *context,
                        GtkSelectionData *data,
                        guint         info,
                        G_GNUC_UNUSED guint time,
                        G_GNUC_UNUSED MdWindow *window)
{
    MdView *view;

    view = g_object_get_data (G_OBJECT (evbox), "md-document-view");
    g_return_if_fail (MD_IS_VIEW (view));

    if (info == TARGET_DOCUMENT_TAB)
    {
        moo_selection_data_set_pointer (data,
                                        gdk_atom_intern ("MD_DOCUMENT_TAB", FALSE),
                                        view);
    }
    else if (info == TARGET_URI_LIST)
    {
        /* XXX cancel dnd if no uri */
        const char *uris[] = {NULL, NULL};
        MdDocument *doc;

        doc = md_view_get_doc (view);
        uris[0] = md_document_get_uri (doc);

        if (!uris[0])
            g_critical ("%s: oops", G_STRLOC);

        gtk_selection_data_set_uris (data, (char**) uris);
    }
    else
    {
        g_print ("drag-data-get WTF?\n");
        gtk_selection_data_set_text (data, "", -1);
    }
}

static void
tab_icon_drag_end (GtkWidget *evbox,
                   G_GNUC_UNUSED GdkDragContext *context,
                   MdWindow *window)
{
    g_object_set_data (G_OBJECT (evbox), "md-drag-info", NULL);
    g_signal_handlers_disconnect_by_func (evbox, (gpointer) tab_icon_drag_begin, window);
    g_signal_handlers_disconnect_by_func (evbox, (gpointer) tab_icon_drag_data_delete, window);
    g_signal_handlers_disconnect_by_func (evbox, (gpointer) tab_icon_drag_data_get, window);
    g_signal_handlers_disconnect_by_func (evbox, (gpointer) tab_icon_drag_end, window);
}


static GtkWidget *
create_tab_label (MdWindow *window,
                  MdView   *view)
{
    GtkWidget *hbox, *icon, *evbox = NULL;

    hbox = _md_view_create_tab_label (view, &evbox);
    g_return_val_if_fail (hbox != NULL, NULL);
    g_return_val_if_fail (evbox != NULL, hbox);

    icon = gtk_bin_get_child (GTK_BIN (evbox));
    g_return_val_if_fail (GTK_IS_IMAGE (icon), hbox);

    g_object_set_data (G_OBJECT (evbox), "md-document-icon", icon);
    g_object_set_data (G_OBJECT (evbox), "md-document-view", view);

    g_signal_connect (evbox, "button-press-event",
                      G_CALLBACK (tab_icon_button_press),
                      window);

    return hbox;
}


/************************************************************************/
/* Doc list
 */

static void
doc_list_action_toggled (gpointer  action,
                         MdWindow *window)
{
    MdView *view;

    if (window->priv->doc_list_update_idle ||
        !gtk_toggle_action_get_active (action))
            return;

    view = g_object_get_data (action, "md-document-view");
    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (MD_IS_WINDOW (window));

    if (view != ACTIVE_VIEW (window))
        md_window_set_active_view (window, view);
}


static int
compare_doc_list_actions (gconstpointer a1,
                          gconstpointer a2,
                          gpointer      window)
{
    MdView *v1, *v2;
    MdDocument *d1, *d2;
    int result;

    v1 = g_object_get_data ((gpointer) a1, "md-document-view");
    v2 = g_object_get_data ((gpointer) a2, "md-document-view");
    g_return_val_if_fail (v1 && v2, 0);

    d1 = md_view_get_doc (v1);
    d2 = md_view_get_doc (v2);
    g_return_val_if_fail (d1 && d2, 0);

    result = strcmp (md_document_get_display_basename (d1),
                     md_document_get_display_basename (d2));

    if (!result)
        result = get_page_num (window, v1) - get_page_num (window, v2);

    return result;
}


static gboolean
do_update_doc_list (MdWindow *window)
{
    MooUIXML *xml;
    GSList *actions = NULL, *views;
    GSList *group = NULL;
    MooUINode *ph;
    MdView *active_view;

    active_view = ACTIVE_VIEW (window);

    xml = moo_window_get_ui_xml (MOO_WINDOW (window));
    g_return_val_if_fail (xml != NULL, FALSE);

    if (xml != window->priv->xml)
    {
        if (window->priv->xml)
        {
            if (window->priv->doc_list_merge_id)
                moo_ui_xml_remove_ui (window->priv->xml,
                                      window->priv->doc_list_merge_id);
            g_object_unref (window->priv->xml);
        }

        window->priv->xml = g_object_ref (xml);
    }
    else if (window->priv->doc_list_merge_id)
    {
        moo_ui_xml_remove_ui (xml, window->priv->doc_list_merge_id);
    }

    window->priv->doc_list_merge_id = 0;

    ph = moo_ui_xml_find_placeholder (xml, "DocList");

    if (!ph)
        goto out;

    views = md_window_list_views (window);

    if (!views)
        goto out;

    while (views)
    {
        GtkRadioAction *action;
        MdView *view = views->data;
        MdDocument *doc = md_view_get_doc (view);

        action = g_object_get_data (G_OBJECT (view), "md-view-list-action");

        if (action)
        {
            g_object_set (action,
                          "label", md_document_get_display_basename (doc),
                          "tooltip", md_document_get_display_name (doc),
                          NULL);
        }
        else
        {
            GtkActionGroup *action_group;
            char *name = g_strdup_printf ("MdView-%p", view);
            action = g_object_new (MOO_TYPE_RADIO_ACTION ,
                                   "name", name,
                                   "label", md_document_get_display_basename (doc),
                                   "tooltip", md_document_get_display_name (doc),
                                   "use-underline", FALSE,
                                   NULL);
            g_object_set_data_full (G_OBJECT (view), "md-view-list-action", action, g_object_unref);
            g_object_set_data (G_OBJECT (action), "md-document-view", view);
            moo_action_set_no_accel (GTK_ACTION (action), TRUE);
            g_signal_connect (action, "toggled", G_CALLBACK (doc_list_action_toggled), window);
            action_group = moo_action_collection_get_group (moo_window_get_actions (MOO_WINDOW (window)), NULL);
            gtk_action_group_add_action (action_group, GTK_ACTION (action));
            g_free (name);
        }

        gtk_radio_action_set_group (action, group);
        group = gtk_radio_action_get_group (action);
        actions = g_slist_prepend (actions, action);

        views = g_slist_delete_link (views, views);
    }

    window->priv->doc_list_merge_id = moo_ui_xml_new_merge_id (xml);
    actions = g_slist_sort_with_data (actions, compare_doc_list_actions, window);

    while (actions)
    {
        gpointer action = actions->data;
        MdView *view = g_object_get_data (G_OBJECT (action), "md-document-view");
        char *markup = g_markup_printf_escaped ("<item action=\"%s\"/>",
                                                gtk_action_get_name (action));

        moo_ui_xml_insert (xml, window->priv->doc_list_merge_id, ph, -1, markup);
        gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
                                      view == active_view);

        g_free (markup);
        actions = g_slist_delete_link (actions, actions);
    }

out:
    /* toggled callback checks this, so it must be unset at the end */
    window->priv->doc_list_update_idle = 0;
    return FALSE;
}


static void
update_doc_list (MdWindow *window)
{
    MdView *view;

    if (!window->priv->doc_list_update_idle)
        window->priv->doc_list_update_idle =
            moo_idle_add_full (G_PRIORITY_HIGH,
                               (GSourceFunc) do_update_doc_list,
                               window, NULL);

    if (!window->priv->history_blocked &&
        (view = ACTIVE_VIEW (window)))
    {
        GList *link = g_list_find (window->priv->history, view);

        if (link && link != window->priv->history)
        {
            window->priv->history = g_list_delete_link (window->priv->history, link);
            window->priv->history = g_list_prepend (window->priv->history, view);
        }
        else if (!link)
        {
            window->priv->history = g_list_prepend (window->priv->history, view);
            if (g_list_length (window->priv->history) > 2)
                window->priv->history = g_list_delete_link (window->priv->history,
                                                            g_list_last (window->priv->history));
        }
    }
}


/************************************************************************/
/* Drag into the window
 */

static gboolean
notebook_drag_motion (GtkWidget          *widget,
                      GdkDragContext     *context,
                      G_GNUC_UNUSED int   x,
                      G_GNUC_UNUSED int   y,
                      guint               time,
                      G_GNUC_UNUSED MdWindow *window)
{
    GdkAtom target;

    target = gtk_drag_dest_find_target (widget, context, NULL);

    if (target == GDK_NONE)
        return FALSE;

    if (target == document_tab_atom)
        gtk_drag_get_data (widget, context, document_tab_atom, time);
    else
        gdk_drag_status (context, context->suggested_action, time);

    return TRUE;
}


static gboolean
notebook_drag_drop (GtkWidget          *widget,
                    GdkDragContext     *context,
                    G_GNUC_UNUSED int   x,
                    G_GNUC_UNUSED int   y,
                    guint               time,
                    G_GNUC_UNUSED MdWindow *window)
{
    GdkAtom target;

    target = gtk_drag_dest_find_target (widget, context, NULL);

    if (target == GDK_NONE)
    {
        gtk_drag_finish (context, FALSE, FALSE, time);
    }
    else
    {
        g_object_set_data (G_OBJECT (widget), "md-window-drop",
                           GINT_TO_POINTER (TRUE));
        gtk_drag_get_data (widget, context, target, time);
    }

    return TRUE;
}


static void
notebook_drag_data_recv (GtkWidget          *widget,
                         GdkDragContext     *context,
                         G_GNUC_UNUSED int   x,
                         G_GNUC_UNUSED int   y,
                         GtkSelectionData   *data,
                         guint               info,
                         guint               time,
                         MdWindow           *window)
{
    gboolean finished = FALSE;

    if (g_object_get_data (G_OBJECT (widget), "md-window-drop"))
    {
        g_object_set_data (G_OBJECT (widget), "md-window-drop", NULL);

        if (data->target == document_tab_atom)
        {
            GtkWidget *toplevel;
            MdView *view = moo_selection_data_get_pointer (data, document_tab_atom);

            if (!view)
                goto out;

            toplevel = gtk_widget_get_toplevel (GTK_WIDGET (view));

            if (toplevel != GTK_WIDGET (window))
                _md_manager_move_view (window->priv->mgr, view, window);

            goto out;
        }
        else if (data->target == text_uri_atom)
        {
            char **uris;
            MdFileInfo **files;

            /* XXX this is wrong but works. gtk_selection_data_get_uris()
             * does not work on windows */
            uris = g_uri_list_extract_uris ((char*) data->data);

            if (!uris)
                goto out;

            files = md_file_info_array_new_uri_list (uris);
            _md_manager_action_open_files (window->priv->mgr, window, files);

            md_file_info_array_free (files);
            g_strfreev (uris);
            gtk_drag_finish (context, TRUE, FALSE, time);
            finished = TRUE;
        }
        else
        {
            goto out;
        }
    }
    else
    {
        if (info == TARGET_DOCUMENT_TAB)
        {
            GtkWidget *toplevel;
            MdDocument *doc = moo_selection_data_get_pointer (data, document_tab_atom);

            if (!doc)
            {
                g_critical ("%s: oops", G_STRLOC);
                gdk_drag_status (context, 0, time);
                return;
            }

            toplevel = gtk_widget_get_toplevel (GTK_WIDGET (doc));

            if (toplevel == GTK_WIDGET (window))
            {
                gdk_drag_status (context, 0, time);
                return;
            }

            gdk_drag_status (context, GDK_ACTION_MOVE, time);
        }
        else
        {
            gdk_drag_status (context, 0, time);
        }

        return;
    }

out:
    if (!finished)
        gtk_drag_finish (context, FALSE, FALSE, time);
}
