/*
 *   mdmanager.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "mooui/mdmanager-private.h"
#include "mooui/mdwindow-private.h"
#include "mooui/mddocument-private.h"
#include "mooui/mooapp.h"
#include "mooui/mddialogs.h"
#include "mooui/marshals.h"
#include "mooui/mdutils.h"
#include "mooui/mdprefs.h"
#include "mooutils/mooi18n.h"
#include "mooutils/mdhistorymgr.h"
#include "mooutils/mooutils-misc.h"


struct MdManagerPrivate {
    char *app_name;
    GPtrArray *untitled;
    GType doc_type;
    GType window_type;
    char *default_ext;

    MooFilterMgr *filter_mgr;
    MooFileWatch *file_watch;

    GSList *windows;
    GSList *windowless;

    gboolean handling_quit;

    char *name;
    MooUIXML *xml;

    MdHistoryMgr *history_mgr;
};


static GObject *md_manager_constructor          (GType           type,
                                                 guint           n_props,
                                                 GObjectConstructParam *props);
static void     md_manager_finalize             (GObject        *object);
static void     md_manager_set_property         (GObject        *object,
                                                 guint           property_id,
                                                 const GValue   *value,
                                                 GParamSpec     *pspec);
static void     md_manager_get_property         (GObject        *object,
                                                 guint           property_id,
                                                 GValue         *value,
                                                 GParamSpec     *pspec);

static void     md_manager_set_name             (MdManager      *mgr,
                                                 const char     *name);

static void     md_manager_init_prefs           (MdManager      *mgr);

static void     handler_quit                    (MdManager      *mgr);
static void     handler_apply_prefs             (MdManager      *mgr);

static void     handler_add_recent              (MdManager      *mgr,
                                                 MdDocument     *doc);
static void     handler_close_doc               (MdManager      *mgr,
                                                 MdDocument     *doc);
static void     handler_close_window            (MdManager      *mgr,
                                                 MdWindow       *window);
static MdCloseAllResult handler_close_all       (MdManager      *mgr);
static void     handler_action_close_docs       (MdManager      *mgr,
                                                 GSList         *docs);
static void     handler_action_close_windows    (MdManager      *mgr,
                                                 GSList         *windows);
static void     emit_close_doc                  (MdManager      *mgr,
                                                 MdDocument     *doc);
static void     emit_close_window               (MdManager      *mgr,
                                                 MdWindow       *window);
static void     do_open_files                   (MdManager      *mgr,
                                                 MdFileInfo    **files,
                                                 MdWindow       *window,
                                                 GtkWidget      *widget,
                                                 gboolean        new_file);
static MdFileInfo  *handler_ask_save_as         (MdManager      *mgr,
                                                 MdDocument     *doc);
static MdFileInfo  *handler_run_save_dialog     (MdManager      *mgr,
                                                 MdDocument     *doc,
                                                 MooFileDialog  *dialog);
static MdFileInfo **handler_ask_open            (MdManager      *mgr,
                                                 MdWindow       *window);
static MdFileInfo **handler_run_open_dialog     (MdManager      *mgr,
                                                 MooFileDialog  *dialog);
static MdFileInfo **handler_ask_open_uri        (MdManager      *mgr,
                                                 MdWindow       *window);

static gboolean accumulator_invoke_once         (GSignalInvocationHint *ihint,
                                                 GValue         *return_accu,
                                                 const GValue   *handler_return,
                                                 gpointer        dummy);

static MdWindow    *create_window               (MdManager      *mgr);
static MdDocument  *create_doc                  (MdManager      *mgr,
                                                 MdFileInfo     *file);

static gboolean     save_and_close_doc          (MdManager      *mgr,
                                                 MdDocument     *doc);
static void         schedule_close_doc          (MdDocument     *doc);
static void         cancel_close_doc            (MdManager      *mgr,
                                                 MdDocument     *doc);
static gboolean     close_doc_if_scheduled      (MdManager      *mgr,
                                                 MdDocument     *doc);
static void         schedule_close_window       (MdWindow       *window);
static void         cancel_close_window         (MdManager      *mgr,
                                                 MdWindow       *window);
static gboolean     close_window_if_scheduled   (MdManager      *mgr,
                                                 MdWindow       *window);
static MdFileInfo  *ask_save_as                 (MdManager      *mgr,
                                                 MdDocument     *doc);
static MdFileInfo **ask_open                    (MdManager      *mgr,
                                                 MdWindow       *window);
static MdFileInfo **ask_open_uri                (MdManager      *mgr,
                                                 MdWindow       *window);
static MdFileOpStatus save_file                 (MdManager      *mgr,
                                                 MdDocument     *doc,
                                                 MdFileInfo     *file_info);
static MdDocument  *open_file                   (MdManager      *mgr,
                                                 MdFileInfo     *file,
                                                 gboolean        new_file,
                                                 GError        **error);
static void         reload_doc                  (MdManager      *mgr,
                                                 MdDocument     *doc);

static void         app_quit_requested          (MdManager      *mgr,
                                                 MooApp         *app);
static void         app_quit_cancelled          (MdManager      *mgr);
static void         app_quit                    (MdManager      *mgr);


G_DEFINE_TYPE (MdManager, md_manager, G_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_NAME
};

enum {
    NEW_DOC,
    NEW_WINDOW,
    CLOSE_DOC,
    CLOSE_WINDOW,
    ASK_SAVE_AS,
    RUN_SAVE_DIALOG,
    ASK_OPEN,
    RUN_OPEN_DIALOG,
    ASK_OPEN_URI,
    ACTION_CLOSE_DOCS,
    ACTION_CLOSE_WINDOWS,
    APPLY_PREFS,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

static void
md_manager_class_init (MdManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (MdManagerPrivate));

    object_class->set_property = md_manager_set_property;
    object_class->get_property = md_manager_get_property;
    object_class->constructor = md_manager_constructor;
    object_class->finalize = md_manager_finalize;

    klass->close_doc = handler_close_doc;
    klass->close_window = handler_close_window;
    klass->close_all = handler_close_all;
    klass->ask_save_as = handler_ask_save_as;
    klass->run_save_dialog = handler_run_save_dialog;
    klass->ask_open = handler_ask_open;
    klass->run_open_dialog = handler_run_open_dialog;
    klass->ask_open_uri = handler_ask_open_uri;

    klass->action_close_docs = handler_action_close_docs;
    klass->action_close_windows = handler_action_close_windows;

    klass->quit = handler_quit;

    klass->apply_prefs = handler_apply_prefs;
    klass->add_recent = handler_add_recent;

    g_object_class_install_property (object_class, PROP_NAME,
        g_param_spec_string ("name", "name", "name", NULL, G_PARAM_READWRITE));

    signals[NEW_DOC] =
        g_signal_new ("new-doc",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, new_doc),
                      NULL, NULL,
                      _moo_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1,
                      MD_TYPE_DOCUMENT);

    signals[NEW_WINDOW] =
        g_signal_new ("new-window",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, new_window),
                      NULL, NULL,
                      _moo_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1,
                      MD_TYPE_WINDOW);

    signals[CLOSE_DOC] =
        g_signal_new ("close-doc",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, close_doc),
                      NULL, NULL,
                      _moo_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1,
                      MD_TYPE_DOCUMENT);

    signals[CLOSE_WINDOW] =
        g_signal_new ("close-window",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, close_window),
                      NULL, NULL,
                      _moo_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1,
                      MD_TYPE_WINDOW);

    signals[ACTION_CLOSE_DOCS] =
        g_signal_new ("action-close-docs",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, action_close_docs),
                      NULL, NULL,
                      _moo_marshal_VOID__POINTER,
                      G_TYPE_NONE, 1,
                      G_TYPE_POINTER);

    signals[ACTION_CLOSE_WINDOWS] =
        g_signal_new ("action-close-windows",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, action_close_windows),
                      NULL, NULL,
                      _moo_marshal_VOID__POINTER,
                      G_TYPE_NONE, 1,
                      G_TYPE_POINTER);

    signals[ASK_OPEN] =
        g_signal_new ("ask-open",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, ask_open),
                      NULL, NULL,
                      _moo_marshal_BOXED__OBJECT,
                      MD_TYPE_FILE_INFO_ARRAY, 1,
                      MD_TYPE_WINDOW);

    signals[RUN_OPEN_DIALOG] =
        g_signal_new ("run-open-dialog",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, run_open_dialog),
                      accumulator_invoke_once, NULL,
                      _moo_marshal_BOXED__OBJECT,
                      MD_TYPE_FILE_INFO_ARRAY, 1,
                      MOO_TYPE_FILE_DIALOG);

    signals[ASK_OPEN_URI] =
        g_signal_new ("ask-open-uri",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, ask_open_uri),
                      NULL, NULL,
                      _moo_marshal_BOXED__OBJECT,
                      MD_TYPE_FILE_INFO_ARRAY, 1,
                      MD_TYPE_WINDOW);

    signals[ASK_SAVE_AS] =
        g_signal_new ("ask-save-as",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, ask_save_as),
                      NULL, NULL,
                      _moo_marshal_BOXED__OBJECT,
                      MD_TYPE_FILE_INFO, 1,
                      MD_TYPE_DOCUMENT);

    signals[RUN_SAVE_DIALOG] =
        g_signal_new ("run-save-dialog",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, run_save_dialog),
                      accumulator_invoke_once, NULL,
                      _moo_marshal_BOXED__OBJECT_OBJECT,
                      MD_TYPE_FILE_INFO, 2,
                      MD_TYPE_DOCUMENT,
                      MOO_TYPE_FILE_DIALOG);

    signals[APPLY_PREFS] =
        g_signal_new ("apply-prefs",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, apply_prefs),
                      NULL, NULL,
                      _moo_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);
}

static void
md_manager_init (MdManager *mgr)
{
    mgr->priv = G_TYPE_INSTANCE_GET_PRIVATE (mgr, MD_TYPE_MANAGER, MdManagerPrivate);
    mgr->priv->untitled = g_ptr_array_new ();
    mgr->priv->default_ext = g_strdup ("");
    mgr->priv->name = NULL;
    mgr->priv->window_type = MD_TYPE_WINDOW;
    mgr->priv->doc_type = 0;
    mgr->priv->filter_mgr = moo_filter_mgr_new ();
}

static GObject *
md_manager_constructor (GType                  type,
                        guint                  n_props,
                        GObjectConstructParam *props)
{
    GObject *object;
    MdManager *mgr;
    MooApp *app;

    object = G_OBJECT_CLASS (md_manager_parent_class)->constructor (type, n_props, props);
    mgr = MD_MANAGER (object);

    if ((app = moo_app_instance ()))
    {
        g_signal_connect_swapped (app, "quit-requested", G_CALLBACK (app_quit_requested), mgr);
        g_signal_connect_swapped (app, "quit-cancelled", G_CALLBACK (app_quit_cancelled), mgr);
        g_signal_connect_swapped (app, "quit", G_CALLBACK (app_quit), mgr);
    }

    return object;
}

static void
md_manager_finalize (GObject *object)
{
    MdManager *mgr = MD_MANAGER (object);

    g_free (mgr->priv->name);
    g_free (mgr->priv->app_name);
    g_ptr_array_free (mgr->priv->untitled, TRUE);
    g_free (mgr->priv->default_ext);
    g_object_unref (mgr->priv->filter_mgr);

    G_OBJECT_CLASS (md_manager_parent_class)->dispose (object);
}


static gboolean
accumulator_invoke_once (G_GNUC_UNUSED GSignalInvocationHint *ihint,
                         GValue         *return_accu,
                         const GValue   *handler_return,
                         G_GNUC_UNUSED gpointer dummy)
{
    g_value_copy (handler_return, return_accu);
    return FALSE;
}


static void
handler_quit (MdManager *mgr)
{
    MooApp *app;

    if ((app = moo_app_instance ()))
    {
        g_signal_handlers_disconnect_by_func (app, (gpointer) app_quit_requested, mgr);
        g_signal_handlers_disconnect_by_func (app, (gpointer) app_quit_cancelled, mgr);
        g_signal_handlers_disconnect_by_func (app, (gpointer) app_quit, mgr);
    }

    if (mgr->priv->file_watch)
    {
        GError *error = NULL;

        if (!moo_file_watch_close (mgr->priv->file_watch, &error))
        {
            g_warning ("%s: error in moo_file_watch_close: %s", G_STRLOC,
                       error ? error->message : "");
            g_error_free (error);
        }

        moo_file_watch_unref (mgr->priv->file_watch);
        mgr->priv->file_watch = NULL;
    }

    if (mgr->priv->history_mgr)
    {
        g_object_unref (mgr->priv->history_mgr);
        mgr->priv->history_mgr = NULL;
    }
}

void
_md_manager_quit (MdManager *mgr)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    MD_MANAGER_GET_CLASS (mgr)->quit (mgr);
}


static void
md_manager_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
    MdManager *mgr = MD_MANAGER (object);

    switch (prop_id)
    {
        case PROP_NAME:
            md_manager_set_name (mgr, g_value_get_string (value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
md_manager_get_property (GObject    *object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
    MdManager *mgr = MD_MANAGER (object);

    switch (prop_id)
    {
        case PROP_NAME:
            g_value_set_string (value, mgr->priv->name);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}


MooFileWatch *
_md_manager_get_file_watch (MdManager *mgr)
{
    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);

    if (!mgr->priv->file_watch)
        mgr->priv->file_watch = moo_file_watch_new (NULL);

    return mgr->priv->file_watch;
}

MdHistoryMgr *
md_manager_get_history_mgr (MdManager *mgr)
{
    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);

    if (!mgr->priv->history_mgr)
        mgr->priv->history_mgr = g_object_new (MD_TYPE_HISTORY_MGR,
                                               "name", mgr->priv->name,
                                               NULL);

    return mgr->priv->history_mgr;
}

MooFilterMgr *
md_manager_get_filter_mgr (MdManager *mgr)
{
    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);
    return mgr->priv->filter_mgr;
}


GSList *
md_manager_list_windows (MdManager *mgr)
{
    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);
    return g_slist_copy (mgr->priv->windows);
}

GSList *
md_manager_list_docs (MdManager *mgr)
{
    GSList *list = NULL;
    GSList *l;

    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);

    for (l = mgr->priv->windows; l != NULL; l = l->next)
        list = g_slist_concat (md_window_list_docs (l->data), list);
    list = g_slist_concat (g_slist_copy (mgr->priv->windowless), list);

    return list;
}


static MdDocument *
doc_list_find_uri (GSList     *docs,
                   const char *uri)
{
    while (docs)
    {
        MdDocument *doc = docs->data;
        MdFileInfo *file = md_document_get_file_info (doc);

        if (file && strcmp (md_file_info_get_uri (file), uri) == 0)
            return doc;

        docs = docs->next;
    }

    return NULL;
}

MdDocument *
md_manager_get_doc (MdManager  *mgr,
                    const char *uri)
{
    GSList *l;
    MdDocument *doc;

    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);
    g_return_val_if_fail (uri != NULL, NULL);

    for (l = mgr->priv->windows; l != NULL; l = l->next)
    {
        MdWindow *window = l->data;
        GSList *docs = md_window_list_docs (window);
        doc = doc_list_find_uri (docs, uri);
        g_slist_free (docs);
        if (doc)
            return doc;
    }

    return doc_list_find_uri (mgr->priv->windowless, uri);
}


static void
md_manager_set_name (MdManager  *mgr,
                     const char *name)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (name && name[0]);
    MOO_ASSIGN_STRING (mgr->priv->name, name);
    if (name)
        md_manager_init_prefs (mgr);
    g_object_notify (G_OBJECT (mgr), "name");
}


const char *
_md_manager_get_app_name (MdManager *mgr)
{
    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);
    return mgr->priv->app_name ? mgr->priv->app_name : g_get_prgname ();
}


char *
_md_manager_add_untitled (MdManager  *mgr,
                          MdDocument *doc)
{
    guint i;

    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);
    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);
    g_return_val_if_fail (md_document_get_manager (doc) == mgr, NULL);

    for (i = 0; i < mgr->priv->untitled->len; ++i)
        if (!mgr->priv->untitled->pdata[i])
            break;

    if (i == mgr->priv->untitled->len)
        g_ptr_array_add (mgr->priv->untitled, doc);
    else
        mgr->priv->untitled->pdata[i] = doc;

    if (i == 0)
        return g_strdup_printf (_("Untitled%s"), mgr->priv->default_ext);
    else
        return g_strdup_printf (_("Untitled %u%s"), i+1, mgr->priv->default_ext);
}

void
_md_manager_remove_untitled (MdManager  *mgr,
                             MdDocument *doc)
{
    guint i;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (md_document_get_manager (doc) == mgr);

    for (i = 0; i < mgr->priv->untitled->len; ++i)
        if (mgr->priv->untitled->pdata[i] == doc)
            break;

    if (i < mgr->priv->untitled->len)
        mgr->priv->untitled->pdata[i] = NULL;
}


void
md_manager_set_doc_type (MdManager *mgr,
                         GType      type)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (g_type_is_a (type, MD_TYPE_DOCUMENT));
    mgr->priv->doc_type = type;
}

void
md_manager_set_window_type (MdManager *mgr,
                            GType      type)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (g_type_is_a (type, MD_TYPE_WINDOW));
    mgr->priv->window_type = type;
}


void
md_manager_set_ui_xml (MdManager *mgr,
                       MooUIXML  *xml)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (!xml || MOO_IS_UI_XML (xml));

    if (mgr->priv->xml != xml)
    {
        GSList *l;

        if (mgr->priv->xml)
            g_object_unref (mgr->priv->xml);
        mgr->priv->xml = xml;
        if (mgr->priv->xml)
            g_object_ref (mgr->priv->xml);

        for (l = mgr->priv->windows; l != NULL; l = l->next)
            moo_window_set_ui_xml (l->data, xml);
    }
}

MooUIXML *
md_manager_get_ui_xml (MdManager *mgr)
{
    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);
    return mgr->priv->xml;
}


void
_md_manager_action_close_window (MdManager *mgr,
                                 MdWindow  *window)
{
    GSList *list;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_WINDOW (window));
    g_return_if_fail (md_window_get_manager (window) == mgr);

    list = g_slist_prepend (NULL, window);
    g_signal_emit (mgr, signals[ACTION_CLOSE_WINDOWS], 0, list);
    g_slist_free (list);
}

void
_md_manager_action_close_docs (MdManager *mgr,
                               GSList    *list)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (list != NULL);

    g_signal_emit (mgr, signals[ACTION_CLOSE_DOCS], 0, list);
}

static void
emit_close_doc (MdManager  *mgr,
                MdDocument *doc)
{
    g_signal_emit (mgr, signals[CLOSE_DOC], 0, doc);
}

static void
emit_close_window (MdManager *mgr,
                   MdWindow  *window)
{
    g_signal_emit (mgr, signals[CLOSE_WINDOW], 0, window);
}


void
_md_manager_action_new_doc (MdManager *mgr,
                            MdWindow  *window)
{
    MdDocument *doc;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (!window || MD_IS_WINDOW (window));
    g_return_if_fail (!window || md_window_get_manager (window) == mgr);

    if (!window)
        window = create_window (mgr);

    doc = create_doc (mgr, NULL);

    _md_manager_move_doc (mgr, doc, window);
    md_window_set_active_doc (window, doc);
    gtk_widget_grab_focus (GTK_WIDGET (doc));
}

void
_md_manager_action_new_window (MdManager *mgr)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    create_window (mgr);
}

void
_md_manager_action_open (MdManager *mgr,
                         MdWindow  *window)
{
    MdFileInfo **files;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (!window || MD_IS_WINDOW (window));
    g_return_if_fail (!window || md_window_get_manager (window) == mgr);

    files = ask_open (mgr, window);

    if (files)
    {
        _md_manager_action_open_files (mgr, files, window, NULL);
        md_file_info_array_free (files);
    }
}

void
_md_manager_action_open_uri (MdManager *mgr,
                             MdWindow  *window)
{
    MdFileInfo **files;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (!window || MD_IS_WINDOW (window));
    g_return_if_fail (!window || md_window_get_manager (window) == mgr);

    files = ask_open_uri (mgr, window);

    if (files)
    {
        _md_manager_action_open_files (mgr, files, window, NULL);
        md_file_info_array_free (files);
    }
}

void
_md_manager_action_reload (MdManager  *mgr,
                           MdDocument *doc)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (md_document_get_manager (doc) == mgr);

    reload_doc (mgr, doc);
}

void
_md_manager_action_save (MdManager  *mgr,
                         MdDocument *doc)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (md_document_get_manager (doc) == mgr);

    md_manager_ui_save (mgr, doc);
}

void
_md_manager_action_save_as (MdManager  *mgr,
                            MdDocument *doc)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (md_document_get_manager (doc) == mgr);

    md_manager_ui_save_as (mgr, doc, NULL);
}

void
_md_manager_action_open_files (MdManager   *mgr,
                               MdFileInfo **files,
                               MdWindow    *window,
                               GtkWidget   *widget)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (!window || MD_IS_WINDOW (window));
    g_return_if_fail (!window || md_window_get_manager (window) == mgr);
    g_return_if_fail (!widget || GTK_IS_WIDGET (widget));
    g_return_if_fail (files && *files);

    do_open_files (mgr, files, window, widget, FALSE);
}


MdDocument *
md_manager_ui_new_file (MdManager  *mgr,
                        MdFileInfo *file_info,
                        MdWindow   *window)
{
    MdFileInfo *files[] = {NULL, NULL};

    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);
    g_return_val_if_fail (!window || MD_IS_WINDOW (window), NULL);
    g_return_val_if_fail (!window || md_window_get_manager (window) == mgr, NULL);
    g_return_val_if_fail (file_info != NULL, NULL);

    files[0] = file_info;
    do_open_files (mgr, files, window, NULL, TRUE);

    return md_manager_get_doc (mgr, md_file_info_get_uri (file_info));
}

MdDocument *
md_manager_ui_open_file (MdManager  *mgr,
                         MdFileInfo *file_info,
                         MdWindow   *window,
                         GtkWidget  *widget)
{
    MdDocument *doc;
    MdFileInfo *files[] = {NULL, NULL};

    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);
    g_return_val_if_fail (!window || MD_IS_WINDOW (window), NULL);
    g_return_val_if_fail (!window || md_window_get_manager (window) == mgr, NULL);
    g_return_val_if_fail (!widget || GTK_IS_WIDGET (widget), NULL);
    g_return_val_if_fail (file_info != NULL, NULL);

    files[0] = file_info;
    _md_manager_action_open_files (mgr, files, window, widget);

    if ((doc = md_manager_get_doc (mgr, md_file_info_get_uri (file_info))))
        gtk_widget_grab_focus (GTK_WIDGET (doc));

    return doc;
}

MdFileOpStatus
md_manager_ui_save (MdManager  *mgr,
                    MdDocument *doc)
{
    MdFileOpStatus status;
    MdFileInfo *file_info;
    MdFileInfo *copy;

    g_return_val_if_fail (MD_IS_MANAGER (mgr), MD_FILE_OP_STATUS_ERROR);
    g_return_val_if_fail (MD_IS_DOCUMENT (doc), MD_FILE_OP_STATUS_ERROR);
    g_return_val_if_fail (md_document_get_manager (doc) == mgr, MD_FILE_OP_STATUS_ERROR);

    file_info = md_document_get_file_info (doc);

    if (!file_info)
        return md_manager_ui_save_as (mgr, doc, NULL);

    copy = md_file_info_copy (file_info);
    status = save_file (mgr, doc, copy);
    md_file_info_free (copy);

    return status;
}

MdFileOpStatus
md_manager_ui_save_as (MdManager  *mgr,
                       MdDocument *doc,
                       MdFileInfo *file_info)
{
    MdFileInfo *freeme = NULL;
    MdFileOpStatus status;

    g_return_val_if_fail (MD_IS_MANAGER (mgr), MD_FILE_OP_STATUS_ERROR);
    g_return_val_if_fail (MD_IS_DOCUMENT (doc), MD_FILE_OP_STATUS_ERROR);
    g_return_val_if_fail (md_document_get_manager (doc) == mgr, MD_FILE_OP_STATUS_ERROR);

    if (!file_info)
    {
        file_info = freeme = ask_save_as (mgr, doc);

        if (!file_info)
            return MD_FILE_OP_STATUS_CANCELLED;
    }

    status = save_file (mgr, doc, file_info);

    md_file_info_free (freeme);
    return status;
}

gboolean
md_manager_ui_close_doc (MdManager  *mgr,
                         MdDocument *doc)
{
    gboolean retval;
    GSList *list;

    g_return_val_if_fail (MD_IS_MANAGER (mgr), FALSE);
    g_return_val_if_fail (MD_IS_DOCUMENT (doc), FALSE);
    g_return_val_if_fail (md_document_get_manager (doc) == mgr, FALSE);

    g_object_ref (doc);

    list = g_slist_prepend (NULL, doc);
    _md_manager_action_close_docs (mgr, list);

    /* XXX */
    retval = md_document_get_manager (doc) == NULL;

    g_slist_free (list);
    g_object_unref (doc);
    return retval;
}

gboolean
md_manager_ui_close_window (MdManager *mgr,
                            MdWindow  *window)
{
    gboolean retval;

    g_return_val_if_fail (MD_IS_MANAGER (mgr), FALSE);
    g_return_val_if_fail (MD_IS_WINDOW (window), FALSE);
    g_return_val_if_fail (md_window_get_manager (window) == mgr, FALSE);

    g_object_ref (window);

    _md_manager_action_close_window (mgr, window);

    /* XXX */
    retval = md_window_get_manager (window) == NULL;

    g_object_unref (window);
    return retval;
}

gboolean
md_manager_ui_close_all (MdManager *mgr)
{
    MdCloseAllResult result;

    g_return_val_if_fail (MD_IS_MANAGER (mgr), FALSE);

    result = MD_MANAGER_GET_CLASS (mgr)->close_all (mgr);
    return result == MD_CLOSE_ALL_DONE;
}


void
_md_manager_move_doc (MdManager  *mgr,
                      MdDocument *doc,
                      MdWindow   *dest)
{
    MdWindow *old_window;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (md_document_get_manager (doc) == mgr);
    g_return_if_fail (!dest || MD_IS_WINDOW (dest));
    g_return_if_fail (!dest || md_window_get_manager (dest) == mgr);

    old_window = md_document_get_window (doc);
    g_return_if_fail (!old_window || old_window != dest);

    if (!dest)
    {
        dest = create_window (mgr);
        g_return_if_fail (dest != NULL);
    }

    g_object_ref (doc);

    if (old_window)
        _md_window_remove_doc (old_window, doc);
    else
        mgr->priv->windowless = g_slist_remove (mgr->priv->windowless, doc);

    _md_window_insert_doc (dest, doc);

    g_object_unref (doc);
}


static void
window_destroyed (MdWindow  *window,
                  MdManager *mgr)
{
    emit_close_window (mgr, window);
}

static MdWindow *
create_window (MdManager *mgr)
{
    MdWindow *window;

    window = g_object_new (mgr->priv->window_type,
                           "document-manager", mgr,
                           "ui-xml", mgr->priv->xml,
                           NULL);
    g_return_val_if_fail (window != NULL, NULL);

    gtk_widget_show (GTK_WIDGET (window));
    mgr->priv->windows = g_slist_prepend (mgr->priv->windows, window);
    g_signal_connect (window, "destroy", G_CALLBACK (window_destroyed), mgr);

    g_signal_emit (mgr, signals[NEW_WINDOW], 0, window);

    return window;
}

static void
handler_close_window (MdManager *mgr,
                      MdWindow  *window)
{
    GSList *docs;

    g_return_if_fail (md_window_get_manager (window) == mgr);

    docs = md_window_list_docs (window);
    g_slist_foreach (docs, (GFunc) g_object_ref, NULL);

    while (docs)
    {
        emit_close_doc (mgr, docs->data);
        g_object_unref (docs->data);
        docs = g_slist_delete_link (docs, docs);
    }

    g_signal_handlers_disconnect_by_func (window, (gpointer) window_destroyed, mgr);
    mgr->priv->windows = g_slist_remove (mgr->priv->windows, window);
    gtk_widget_destroy (GTK_WIDGET (window));
}


static void
doc_destroyed (MdDocument *doc,
               MdManager  *mgr)
{
    emit_close_doc (mgr, doc);
}

static MdDocument *
create_doc (MdManager  *mgr,
            MdFileInfo *file)
{
    MdDocument *doc;

    doc = g_object_new (mgr->priv->doc_type,
                        "md-doc-file-info", file,
                        NULL);
    g_return_val_if_fail (doc != NULL, NULL);

    _md_document_set_manager (doc, mgr);

    gtk_widget_show (GTK_WIDGET (doc));
    MOO_OBJECT_REF_SINK (doc);

    mgr->priv->windowless = g_slist_prepend (mgr->priv->windowless, doc);
    g_signal_connect (doc, "destroy", G_CALLBACK (doc_destroyed), mgr);
    g_signal_emit (mgr, signals[NEW_DOC], 0, doc);

    _md_document_apply_prefs (doc);

    return doc;
}


static void
handler_close_doc (MdManager  *mgr,
                   MdDocument *doc)
{
    MdWindow *window;

    g_return_if_fail (md_document_get_manager (doc) == mgr);

    g_object_ref (doc);

    g_signal_handlers_disconnect_by_func (doc, (gpointer) doc_destroyed, mgr);

    if ((window = md_document_get_window (doc)))
    {
        _md_window_remove_doc (window, doc);
    }
    else
    {
        mgr->priv->windowless = g_slist_remove (mgr->priv->windowless, doc);
        g_object_unref (doc);
    }

    _md_document_close (doc);

    if (window && !close_window_if_scheduled (mgr, window))
    {
        /* XXX new empty doc */
    }

    if (!mgr->priv->windows && !mgr->priv->windowless && mgr->priv->handling_quit)
    {
        mgr->priv->handling_quit = FALSE;
        moo_app_resume_quit (moo_app_instance ());
    }

    g_object_unref (doc);
}


static void
cancel_quit (MdManager *mgr)
{
    if (mgr->priv->handling_quit)
    {
        mgr->priv->handling_quit = FALSE;
        moo_app_cancel_quit (moo_app_instance ());
    }
}

static gboolean
close_doc_if_scheduled (MdManager  *mgr,
                        MdDocument *doc)
{
    if (!g_object_get_data (G_OBJECT (doc), "md-document-need-close") &&
        !mgr->priv->handling_quit)
            return FALSE;

    emit_close_doc (mgr, doc);
    return TRUE;
}

static void
schedule_close_doc (MdDocument *doc)
{
    g_object_set_data (G_OBJECT (doc),
                       "md-document-need-close",
                       GINT_TO_POINTER (TRUE));
}

static void
cancel_close_doc (MdManager  *mgr,
                  MdDocument *doc)
{
    MdWindow *window;

    cancel_quit (mgr);

    g_object_set_data (G_OBJECT (doc), "md-document-need-close", NULL);

    if ((window = md_document_get_window (doc)))
        cancel_close_window (mgr, window);
}

static gboolean
close_window_if_scheduled (MdManager *mgr,
                           MdWindow  *window)
{
    if (md_window_get_active_doc (window))
        return FALSE;

    if (!g_object_get_data (G_OBJECT (window), "md-window-need-close") &&
        !mgr->priv->handling_quit)
            return FALSE;

    emit_close_window (mgr, window);
    return TRUE;
}

static void
schedule_close_window (MdWindow *window)
{
    g_object_set_data (G_OBJECT (window),
                       "md-window-need-close",
                       GINT_TO_POINTER (TRUE));
}

static void
cancel_close_window (MdManager *mgr,
                     MdWindow  *window)
{
    cancel_quit (mgr);
    g_object_set_data (G_OBJECT (window), "md-window-need-close", NULL);
}


static void
app_quit_requested (MdManager *mgr,
                    MooApp    *app)
{
    MdCloseAllResult result;

    if (mgr->priv->handling_quit)
        return;

    mgr->priv->handling_quit = TRUE;
    moo_app_delay_quit (app);

    result = MD_MANAGER_GET_CLASS (mgr)->close_all (mgr);

    switch (result)
    {
        case MD_CLOSE_ALL_DONE:
            mgr->priv->handling_quit = FALSE;
            moo_app_resume_quit (app);
            break;
        case MD_CLOSE_ALL_CANCELLED:
            cancel_quit (mgr);
            break;
        case MD_CLOSE_ALL_IN_PROGRESS:
            break;
    }
}

static void
app_quit_cancelled (MdManager *mgr)
{
    mgr->priv->handling_quit = FALSE;
}

static void
app_quit (MdManager *mgr)
{
    while (mgr->priv->windows)
        emit_close_window (mgr, mgr->priv->windows->data);
    while (mgr->priv->windowless)
        emit_close_doc (mgr, mgr->priv->windowless->data);
}


static gboolean
save_and_close_doc (MdManager  *mgr,
                    MdDocument *doc)
{
    MdFileInfo *file_info;
    MdFileInfo *copy;

    file_info = md_document_get_file_info (doc);

    if (file_info)
        copy = md_file_info_copy (file_info);
    else
        copy = ask_save_as (mgr, doc);

    if (!copy)
        return FALSE;

    schedule_close_doc (doc);
    save_file (mgr, doc, copy);

    md_file_info_free (copy);
    return TRUE;
}

static void
find_modified (GSList  *docs,
               GSList **modified,
               GSList **saved)
{
    for ( ; docs != NULL; docs = docs->next)
    {
        MdDocument *doc = docs->data;

        if (_md_document_need_save (doc))
            *modified = g_slist_prepend (*modified, doc);
        else
            *saved = g_slist_prepend (*saved, doc);
    }
}

static gboolean
ask_save (GSList  *modified,
          GSList **to_save)
{
    *to_save = NULL;

    if (modified && modified->next)
    {
        return md_save_multiple_changes_dialog (modified, to_save) !=
                MD_SAVE_CHANGES_RESPONSE_CANCEL;
    }
    else if (modified)
    {
        MdSaveChangesDialogResponse response;

        response = md_save_changes_dialog (modified->data);

        switch (response)
        {
            case MD_SAVE_CHANGES_RESPONSE_SAVE:
                *to_save = g_slist_prepend (*to_save, modified->data);
                /* fall through */
            case MD_SAVE_CHANGES_RESPONSE_DONT_SAVE:
                return TRUE;

            case MD_SAVE_CHANGES_RESPONSE_CANCEL:
                return FALSE;
        }
    }

    return TRUE;
}

static MdDocument *
find_busy (GSList *docs)
{
    while (docs)
    {
        MdDocument *d = docs->data;
        if (MD_DOCUMENT_IS_BUSY (d))
            return d;
        docs = docs->next;
    }

    return NULL;
}

static gboolean
action_close_docs (MdManager *mgr,
                   GSList    *docs)
{
    GSList *modified = NULL;
    GSList *saved = NULL;
    GSList *to_save = NULL;
    GSList *tmp;
    gboolean cancelled = FALSE;
    MdDocument *busy;

    if ((busy = find_busy (docs)))
    {
        md_manager_present_doc (mgr, busy);
        _md_widget_beep (GTK_WIDGET (busy));
        return FALSE;
    }

    find_modified (docs, &modified, &saved);

    if (!ask_save (modified, &to_save))
    {
        cancelled = TRUE;
        goto out;
    }

    tmp = modified;
    modified = NULL;
    while (tmp)
    {
        if (g_slist_find (to_save, tmp->data))
            modified = g_slist_prepend (modified, tmp->data);
        else
            saved = g_slist_prepend (saved, tmp->data);
        tmp = g_slist_delete_link (tmp, tmp);
    }
    g_slist_free (to_save);

    while (saved)
    {
        emit_close_doc (mgr, saved->data);
        saved = g_slist_delete_link (saved, saved);
    }

    while (modified)
    {
        if (!save_and_close_doc (mgr, modified->data))
        {
            cancelled = TRUE;
            break;
        }

        modified = g_slist_delete_link (modified, modified);
    }

out:
    g_slist_free (modified);
    g_slist_free (saved);
    return !cancelled;
}

static void
handler_action_close_docs (MdManager *mgr,
                           GSList    *docs)
{
    action_close_docs (mgr, docs);
}

static void
handler_action_close_windows (MdManager *mgr,
                              GSList    *windows)
{
    GSList *docs = NULL;
    GSList *l;

    for (l = windows, windows = NULL; l != NULL; l = l->next)
    {
        MdWindow *window = l->data;
        GSList *docs_here = md_window_list_docs (window);

        if (docs_here)
        {
            docs = g_slist_concat (docs, docs_here);
            windows = g_slist_prepend (windows, window);
        }
        else
        {
            emit_close_window (mgr, window);
        }
    }

    if (!action_close_docs (mgr, docs))
        return;

    while (windows)
    {
        MdWindow *window = windows->data;

        if (md_window_get_active_doc (window))
            schedule_close_window (window);
        else
            emit_close_window (mgr, window);

        windows = g_slist_delete_link (windows, windows);
    }
}

static MdCloseAllResult
handler_close_all (MdManager *mgr)
{
    MdCloseAllResult result = MD_CLOSE_ALL_DONE;
    GSList *docs = NULL;
    GSList *l;

    for (l = mgr->priv->windows; l != NULL; l = l->next)
    {
        GSList *tmp = md_window_list_docs (l->data);
        docs = g_slist_concat (tmp, docs);
    }

    docs = g_slist_concat (mgr->priv->windowless, docs);

    if (docs)
    {
        if (!action_close_docs (mgr, docs))
        {
            result = MD_CLOSE_ALL_CANCELLED;
        }
        else
        {
            GSList *tmp = g_slist_copy (mgr->priv->windows);

            while (tmp)
            {
                MdWindow *window = tmp->data;

                if (md_window_get_active_doc (window))
                {
                    schedule_close_window (window);
                    result = MD_CLOSE_ALL_IN_PROGRESS;
                }
                else
                {
                    emit_close_window (mgr, window);
                }

                tmp = g_slist_delete_link (tmp, tmp);
            }
        }
    }

    if (result == MD_CLOSE_ALL_DONE && mgr->priv->windowless)
        result = MD_CLOSE_ALL_IN_PROGRESS;

    g_slist_free (docs);
    return result;
}


static MdFileInfo *
ask_save_as (MdManager  *mgr,
             MdDocument *doc)
{
    MdFileInfo *file_info = NULL;
    g_signal_emit (mgr, signals[ASK_SAVE_AS], 0, doc, &file_info);
    return file_info;
}

static MdFileInfo *
md_manager_run_save_dialog (MdManager     *mgr,
                            MdDocument    *doc,
                            MooFileDialog *dialog)
{
    MdFileInfo *file_info = NULL;
    g_signal_emit (mgr, signals[RUN_SAVE_DIALOG], 0,
                   doc, dialog, &file_info);
    return file_info;
}

static MdFileInfo *
handler_run_save_dialog (G_GNUC_UNUSED MdManager  *mgr,
                         G_GNUC_UNUSED MdDocument *doc,
                         MooFileDialog *dialog)
{
    const char *uri;
    MdFileInfo *ret;

    if (!moo_file_dialog_run (dialog))
        return NULL;

    uri = moo_file_dialog_get_uri (dialog);
    g_return_val_if_fail (uri != NULL, NULL);

    ret = md_file_info_new (uri);

    return ret;
}

static MdFileInfo *
handler_ask_save_as (MdManager  *mgr,
                     MdDocument *doc)
{
    const char *start_dir = NULL;
    MooFileDialog *dialog;
    MdFileInfo *ret;
    char *display_basename = NULL;

    start_dir = moo_prefs_get_string (md_manager_setting (mgr, MD_PREFS_LAST_DIR));

    if (doc)
        display_basename = md_document_get_display_basename (doc);

    dialog = moo_file_dialog_new (MOO_FILE_DIALOG_SAVE,
                                  doc ? GTK_WIDGET (doc) : NULL,
                                  FALSE, GTK_STOCK_SAVE_AS,
                                  NULL, display_basename);

//     g_object_set (dialog, "enable-encodings", TRUE, NULL);
//     moo_file_dialog_set_encoding (dialog, moo_edit_get_encoding (edit));
//     moo_file_dialog_set_help_id (dialog, "dialog-save");

    moo_file_dialog_set_remember_size (dialog, md_manager_setting (mgr, MD_PREFS_DIALOG_SAVE));
    if (start_dir)
        moo_file_dialog_set_current_folder_uri (dialog, start_dir);
    moo_file_dialog_set_filter_mgr (dialog, mgr->priv->filter_mgr, mgr->priv->name);

    if ((ret = md_manager_run_save_dialog (mgr, doc, dialog)))
    {
        const char *new_start = moo_file_dialog_get_current_folder_uri (dialog);
        moo_prefs_set_string (md_manager_setting (mgr, MD_PREFS_LAST_DIR), new_start);
    }

    g_free (display_basename);
    g_object_unref (dialog);
    return ret;

//     encoding = moo_file_dialog_get_encoding (dialog);
//     filename = moo_file_dialog_get_filename (dialog);
//     g_return_val_if_fail (filename != NULL, NULL);
//     file_info = moo_edit_file_info_new (filename, encoding);
//
//     new_start = g_path_get_dirname (filename);
//     moo_prefs_set_filename (moo_edit_setting (MOO_EDIT_PREFS_LAST_DIR), new_start);
//     g_free (new_start);
//
//     g_object_unref (dialog);
//     return file_info;
}


static MdFileInfo **
ask_open (MdManager *mgr,
          MdWindow  *window)
{
    MdFileInfo **files = NULL;
    g_signal_emit (mgr, signals[ASK_OPEN], 0, window, &files);
    return files;
}

static MdFileInfo **
ask_open_uri (MdManager *mgr,
              MdWindow  *window)
{
    MdFileInfo **files = NULL;
    g_signal_emit (mgr, signals[ASK_OPEN_URI], 0, window, &files);
    return files;
}

static MdFileInfo **
md_manager_run_open_dialog (MdManager     *mgr,
                            MooFileDialog *dialog)
{
    MdFileInfo **files = NULL;
    g_signal_emit (mgr, signals[RUN_OPEN_DIALOG], 0, dialog, &files);
    return files;
}

static MdFileInfo **
handler_run_open_dialog (G_GNUC_UNUSED MdManager *mgr,
                         MooFileDialog *dialog)
{
    char **uris;
    MdFileInfo **ret;

    if (!moo_file_dialog_run (dialog))
        return NULL;

    uris = moo_file_dialog_get_uris (dialog);
    g_return_val_if_fail (uris != NULL, NULL);

    ret = md_file_info_array_new_uri_list (uris);

    g_strfreev (uris);
    return ret;
}

static MdFileInfo **
handler_ask_open (MdManager *mgr,
                  MdWindow  *window)
{
    MooFileDialog *dialog;
    const char *start_dir = NULL;
    const char *start_dir_uri = NULL;
    char *freeme = NULL;
    MdDocument *active_doc = NULL;
    MdFileInfo **ret;

    if (window)
        active_doc = md_window_get_active_doc (window);

    if (active_doc && moo_prefs_get_bool (md_manager_setting (mgr, MD_PREFS_DIALOGS_OPEN_FOLLOWS_DOC)))
    {
        char *uri = md_document_get_uri (active_doc);

        if (uri)
        {
            freeme = _md_uri_get_dirname (uri);
            start_dir_uri = freeme;
            g_free (uri);
        }
    }

    if (!start_dir)
        start_dir = moo_prefs_get_string (md_manager_setting (mgr, MD_PREFS_LAST_DIR));

    dialog = moo_file_dialog_new (MOO_FILE_DIALOG_OPEN,
                                  window ? GTK_WIDGET (window) : NULL,
                                  TRUE, GTK_STOCK_OPEN, NULL,
                                  NULL);
    moo_file_dialog_set_remember_size (dialog, md_manager_setting (mgr, MD_PREFS_DIALOG_OPEN));
    if (start_dir)
        moo_file_dialog_set_current_folder_uri (dialog, start_dir);
    moo_file_dialog_set_filter_mgr (dialog, mgr->priv->filter_mgr, mgr->priv->name);

//     moo_file_dialog_set_help_id (dialog, "dialog-open");

//     encoding = moo_file_dialog_get_encoding (dialog);
//
//     if (encoding && !strcmp (encoding, MOO_ENCODING_AUTO))
//         encoding = NULL;

    if ((ret = md_manager_run_open_dialog (mgr, dialog)))
    {
        const char *new_start = moo_file_dialog_get_current_folder_uri (dialog);
        moo_prefs_set_string (md_manager_setting (mgr, MD_PREFS_LAST_DIR), new_start);
    }

    g_free (freeme);
    g_object_unref (dialog);
    return ret;
}

static MdFileInfo **
handler_ask_open_uri (G_GNUC_UNUSED MdManager *mgr,
                      MdWindow *window)
{
    return md_open_uri_dialog (window);
}


static void
handler_add_recent (MdManager  *mgr,
                    MdDocument *doc)
{
    MdFileInfo *file;

    if ((file = md_document_get_file_info (doc)))
    {
        const char *uri = md_file_info_get_uri (file);
        MdHistoryMgr *history_mgr = md_manager_get_history_mgr (mgr);
        md_history_mgr_add_uri (history_mgr, uri);
    }
}

void
_md_manager_add_recent (MdManager  *mgr,
                        MdDocument *doc)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    MD_MANAGER_GET_CLASS (mgr)->add_recent (mgr, doc);
}


static void
beep (MdDocument *doc)
{
#if GTK_CHECK_VERSION(2,12,0)
    if (GTK_WIDGET_REALIZED (doc))
        gdk_window_beep (GTK_WIDGET (doc)->window);
    else
#endif
        gdk_display_beep (gdk_display_get_default ());
}

static void
reload_doc (MdManager  *mgr,
            MdDocument *doc)
{
    MdFileInfo *file_info;
    MdFileOpInfo *op_info;

    if (MD_DOCUMENT_IS_BUSY (doc))
    {
        beep (doc);
        return;
    }

    file_info = md_document_get_file_info (doc);
    g_return_if_fail (file_info != NULL);

    op_info = md_file_op_info_new (MD_FILE_OP_RELOAD);
    _md_document_load_file (doc, file_info, op_info);

    if (op_info->status == MD_FILE_OP_STATUS_ERROR)
        md_open_error_dialog (GTK_WIDGET (doc), file_info, op_info->error);

    md_file_op_info_free (op_info);
}

static gboolean
open_file_in_doc (MdManager   *mgr,
                  MdDocument  *doc,
                  MdFileInfo  *file,
                  gboolean     new_file,
                  GError     **error)
{
    MdFileOpInfo *op_info;

    md_document_set_file_info (doc, file);

    if (new_file)
        return TRUE;

    op_info = md_file_op_info_new (MD_FILE_OP_OPEN);
    _md_document_load_file (doc, file, op_info);

    switch (op_info->status)
    {
        case MD_FILE_OP_STATUS_IN_PROGRESS:
        case MD_FILE_OP_STATUS_SUCCESS:
            md_file_op_info_free (op_info);
            return TRUE;

        case MD_FILE_OP_STATUS_ERROR:
            g_propagate_error (error, op_info->error);
            op_info->error = NULL;
            md_file_op_info_free (op_info);
            md_document_set_file_info (doc, NULL);
            return FALSE;

        case MD_FILE_OP_STATUS_CANCELLED:
            break;
    }

    md_document_set_file_info (doc, NULL);
    md_file_op_info_free (op_info);
    g_return_val_if_reached (FALSE);
}

static MdDocument *
open_file (MdManager   *mgr,
           MdFileInfo  *file,
           gboolean     new_file,
           GError     **error)
{
    MdDocument *doc;

    doc = create_doc (mgr, file);
    g_return_val_if_fail (doc != NULL, NULL);

    if (open_file_in_doc (mgr, doc, file, new_file, error))
        return doc;

    emit_close_doc (mgr, doc);
    return NULL;
}

static void
do_open_files (MdManager   *mgr,
               MdFileInfo **files,
               MdWindow    *window,
               GtkWidget   *widget,
               gboolean     new_file)
{
    MdDocument *last = NULL;

    g_return_if_fail (files && *files);

    for ( ; *files; files++)
    {
        MdFileInfo *fi = *files;
        MdDocument *doc = NULL;
        GError *error = NULL;

        if ((doc = md_manager_get_doc (mgr, md_file_info_get_uri (fi))))
        {
            last = doc;
            continue;
        }

        if (window || (window = md_manager_get_active_window (mgr)))
        {
            MdDocument *doc;

            doc = md_window_get_active_doc (window);

            if (!doc || !md_document_is_empty (doc))
                doc = NULL;
        }

        if (doc)
        {
            if (!open_file_in_doc (mgr, doc, fi, new_file, &error))
            {
                if (!widget)
                    widget = window ? GTK_WIDGET (window) : NULL;
                md_open_error_dialog (widget, fi, error);
                if (error)
                    g_error_free (error);
                continue;
            }

            last = doc;
            continue;
        }

        if (!(doc = open_file (mgr, fi, new_file, &error)))
        {
            if (!widget)
                widget = window ? GTK_WIDGET (window) : NULL;
            md_open_error_dialog (widget, fi, error);
            if (error)
                g_error_free (error);
            continue;
        }

        _md_manager_move_doc (mgr, doc, window);
        last = doc;
    }

    if (last)
        md_manager_present_doc (mgr, last);
}


static void
save_finished (MdManager    *mgr,
               MdDocument   *doc,
               MdFileInfo   *file,
               MdFileOpInfo *op_info,
               gboolean      async)
{
    if (op_info->status != MD_FILE_OP_STATUS_SUCCESS)
        cancel_close_doc (mgr, doc);

    switch (op_info->status)
    {
        case MD_FILE_OP_STATUS_ERROR:
            if (!async)
                md_save_error_dialog (GTK_WIDGET (doc), file, op_info->error);
            break;

        case MD_FILE_OP_STATUS_SUCCESS:
            close_doc_if_scheduled (mgr, doc);
            break;

        default:
            g_return_if_reached ();
    }
}

static void
async_save_finished (MdDocument     *doc,
                     MdFileInfo     *file_info,
                     MdFileOpInfo   *op_info,
                     MdManager      *mgr)
{
    g_signal_handlers_disconnect_by_func (doc, (gpointer) async_save_finished, mgr);
    save_finished (mgr, doc, file_info, op_info, TRUE);
}

static MdFileOpStatus
save_file (MdManager  *mgr,
           MdDocument *doc,
           MdFileInfo *file)
{
    MdFileOpStatus retval;
    MdFileOpInfo *op_info;

    if (MD_DOCUMENT_IS_BUSY (doc))
    {
        beep (doc);
        return MD_FILE_OP_STATUS_ERROR;
    }

    op_info = md_file_op_info_new (MD_FILE_OP_SAVE);
    _md_document_save_file (doc, file, op_info);

    retval = op_info->status;

    switch (op_info->status)
    {
        case MD_FILE_OP_STATUS_IN_PROGRESS:
            g_signal_connect (doc, "save-after", G_CALLBACK (async_save_finished), mgr);
            break;

        case MD_FILE_OP_STATUS_ERROR:
        case MD_FILE_OP_STATUS_SUCCESS:
            save_finished (mgr, doc, file, op_info, FALSE);
            break;

        case MD_FILE_OP_STATUS_CANCELLED:
            g_return_val_if_reached (retval);
            break;
    }

    md_file_op_info_free (op_info);
    return retval;
}


// void
// md_manager_set_active_window (MdManager *mgr,
//                               MdWindow  *window)
// {
//     g_return_if_fail (MD_IS_MANAGER (mgr));
//     g_return_if_fail (MD_IS_WINDOW (window));
//     g_return_if_fail (md_window_get_manager (window) == mgr);
// }

MdWindow *
md_manager_get_active_window (MdManager *mgr)
{
    GSList *list;
    GtkWindow *window;

    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);

    if (!mgr->priv->windows)
        return NULL;

    list = g_slist_copy (mgr->priv->windows);
    window = _moo_get_top_window (list);

    if (!window)
        window = mgr->priv->windows->data;

    g_slist_free (list);
    return MD_WINDOW (window);
}

void
md_manager_present_doc (MdManager  *mgr,
                        MdDocument *doc)
{
    MdWindow *window;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (md_document_get_manager (doc) == mgr);

    window = md_document_get_window (doc);

    if (window)
    {
        md_window_set_active_doc (window, doc);
        gtk_window_present (GTK_WINDOW (window));
    }
}


/****************************************************************************/
/* Preferences
 */

void
md_manager_apply_prefs (MdManager *mgr)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_signal_emit (mgr, signals[APPLY_PREFS], 0);
}

static void
handler_apply_prefs (MdManager *mgr)
{
    GSList *l;

    for (l = mgr->priv->windows; l != NULL; l = l->next)
        moo_window_apply_prefs (l->data);

    for (l = mgr->priv->windowless; l != NULL; l = l->next)
        _md_document_apply_prefs (l->data);
}

static void
md_manager_init_prefs (MdManager *mgr)
{
    moo_prefs_create_key (md_manager_setting (mgr, MD_PREFS_LAST_DIR),
                          MOO_PREFS_STATE, G_TYPE_STRING, NULL);
    moo_prefs_create_key (md_manager_setting (mgr, MD_PREFS_DIALOGS_OPEN_FOLLOWS_DOC),
                          MOO_PREFS_RC, G_TYPE_BOOLEAN, TRUE);
}

const char *
md_manager_setting (MdManager  *mgr,
                    const char *setting)
{
#define N_STRINGS 8
    static GString *stack[N_STRINGS];
    static guint p;

    g_return_val_if_fail (MD_IS_MANAGER (mgr), NULL);
    g_return_val_if_fail (mgr->priv->name != NULL, NULL);
    g_return_val_if_fail (setting != NULL, NULL);

    if (!stack[0])
    {
        for (p = 0; p < N_STRINGS; ++p)
            stack[p] = g_string_new ("");
        p = N_STRINGS - 1;
    }

    if (p == N_STRINGS - 1)
        p = 0;
    else
        p++;

    g_string_printf (stack[p], "%s/%s", mgr->priv->name, setting);
    return stack[p]->str;
#undef N_STRINGS
}
