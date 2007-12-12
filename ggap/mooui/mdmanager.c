#include "mdmanager-private.h"
#include "mdwindow-private.h"
#include "mddocument-private.h"
#include "mddialogs.h"
#include "marshals.h"
#include "ggap-i18n.h"


struct MdManagerPrivate {
    char *app_name;
    GPtrArray *untitled;
    GType doc_type;
    GType window_type;
    char *default_ext;

    GSList *windows;
    GSList *windowless;
};


static GObject *md_manager_constructor          (GType           type,
                                                 guint           n_props,
                                                 GObjectConstructParam *props);
static void     md_manager_dispose              (GObject        *object);
// static void     md_manager_set_property   (GObject        *object,
//                                                      guint           property_id,
//                                                      const GValue   *value,
//                                                      GParamSpec     *pspec);
// static void     md_manager_get_property   (GObject        *object,
//                                                      guint           property_id,
//                                                      GValue         *value,
//                                                      GParamSpec     *pspec);

static void     handler_close_doc               (MdManager      *mgr,
                                                 MdDocument     *doc);
static void     handler_close_window            (MdManager      *mgr,
                                                 MdWindow       *window);
static void     handler_action_close_docs       (MdManager      *mgr,
                                                 GSList         *docs);
static void     handler_action_close_windows    (MdManager      *mgr,
                                                 GSList         *windows);
static void     emit_close_doc                  (MdManager      *mgr,
                                                 MdDocument     *doc);
static void     emit_close_window               (MdManager      *mgr,
                                                 MdWindow       *window);
static void     handler_action_open_files       (MdManager      *mgr,
                                                 MdWindow       *window,
                                                 MdFileInfo    **files);
static MdFileInfo  *handler_ask_save_as         (MdManager      *mgr,
                                                 MdDocument     *doc);
static MdFileInfo  **handler_ask_open           (MdManager      *mgr,
                                                 MdWindow       *window);

static MdWindow    *create_window               (MdManager      *mgr);
static MdDocument  *create_doc                  (MdManager      *mgr,
                                                 MdWindow       *window,
                                                 MdFileInfo     *file);
static gboolean     save_and_close_doc          (MdManager      *mgr,
                                                 MdDocument     *doc);
static void         schedule_close_doc          (MdDocument     *doc);
static void         cancel_close_doc            (MdDocument     *doc);
static void         schedule_close_window       (MdWindow       *window);
static void         cancel_close_window         (MdWindow       *window);
static MdFileInfo  *ask_save_as                 (MdManager      *mgr,
                                                 MdDocument     *doc);
static MdFileInfo **ask_open                    (MdManager      *mgr,
                                                 MdWindow       *window);
static void         save_file                   (MdManager      *mgr,
                                                 MdDocument     *doc,
                                                 MdFileInfo     *file_info);
static MdDocument  *open_file                   (MdManager      *mgr,
                                                 MdWindow       *window,
                                                 MdFileInfo     *file);
static void         reload_doc                  (MdManager      *mgr,
                                                 MdDocument     *doc);


G_DEFINE_TYPE (MdManager, md_manager, G_TYPE_OBJECT)

enum {
    NEW_DOC,
    NEW_WINDOW,
    CLOSE_DOC,
    CLOSE_WINDOW,
    ASK_SAVE_AS,
    ASK_OPEN,
    ACTION_CLOSE_DOCS,
    ACTION_CLOSE_WINDOWS,
    ACTION_OPEN_FILES,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

static void
md_manager_class_init (MdManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (MdManagerPrivate));

//     object_class->set_property = md_manager_set_property;
//     object_class->get_property = md_manager_get_property;
    object_class->constructor = md_manager_constructor;
    object_class->dispose = md_manager_dispose;

    klass->close_doc = handler_close_doc;
    klass->close_window = handler_close_window;
    klass->ask_save_as = handler_ask_save_as;
    klass->ask_open = handler_ask_open;

    klass->action_close_docs = handler_action_close_docs;
    klass->action_close_windows = handler_action_close_windows;
    klass->action_open_files = handler_action_open_files;

    signals[NEW_DOC] =
        g_signal_new ("new-doc",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, new_doc),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1,
                      MD_TYPE_DOCUMENT);

    signals[NEW_WINDOW] =
        g_signal_new ("new-window",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, new_window),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1,
                      MD_TYPE_WINDOW);

    signals[CLOSE_DOC] =
        g_signal_new ("close-doc",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, close_doc),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1,
                      MD_TYPE_DOCUMENT);

    signals[CLOSE_WINDOW] =
        g_signal_new ("close-window",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, close_window),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1,
                      MD_TYPE_WINDOW);

    signals[ACTION_CLOSE_DOCS] =
        g_signal_new ("action-close-docs",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, action_close_docs),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__POINTER,
                      G_TYPE_NONE, 1,
                      G_TYPE_POINTER);

    signals[ACTION_CLOSE_WINDOWS] =
        g_signal_new ("action-close-windows",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, action_close_windows),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__POINTER,
                      G_TYPE_NONE, 1,
                      G_TYPE_POINTER);

    signals[ACTION_OPEN_FILES] =
        g_signal_new ("action-open-files",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, action_open_files),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__OBJECT_POINTER,
                      G_TYPE_NONE, 2,
                      MD_TYPE_WINDOW,
                      G_TYPE_POINTER);

    signals[ASK_OPEN] =
        g_signal_new ("ask-open",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, ask_open),
                      NULL, NULL,
                      _moo_ui_marshal_BOXED__OBJECT,
                      MD_TYPE_FILE_INFO_ARRAY, 1,
                      MD_TYPE_WINDOW);

    signals[ASK_SAVE_AS] =
        g_signal_new ("ask-save-as",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, ask_save_as),
                      NULL, NULL,
                      _moo_ui_marshal_BOXED__OBJECT,
                      MD_TYPE_FILE_INFO, 1,
                      MD_TYPE_DOCUMENT);
}

static void
md_manager_init (MdManager *mgr)
{
    mgr->priv = G_TYPE_INSTANCE_GET_PRIVATE (mgr, MD_TYPE_MANAGER, MdManagerPrivate);
    mgr->priv->untitled = g_ptr_array_new ();
    mgr->priv->default_ext = g_strdup ("");
    mgr->priv->window_type = MD_TYPE_WINDOW;
    mgr->priv->doc_type = 0;
}

static GObject *
md_manager_constructor (GType                  type,
                        guint                  n_props,
                        GObjectConstructParam *props)
{
    GObject *object;
    MdManager *manager;

    object = G_OBJECT_CLASS (md_manager_parent_class)->constructor (type, n_props, props);
    manager = MD_MANAGER (object);

    return object;
}

static void
md_manager_dispose (GObject *object)
{
    MdManager *mgr = MD_MANAGER (object);

    if (mgr->priv)
    {
        g_free (mgr->priv->app_name);
        g_ptr_array_free (mgr->priv->untitled, TRUE);
        g_free (mgr->priv->default_ext);
        mgr->priv = NULL;
    }

    G_OBJECT_CLASS (md_manager_parent_class)->dispose (object);
}

// static void
// md_manager_set_property (GObject      *object,
//                                    guint         prop_id,
//                                    const GValue *value,
//                                    GParamSpec   *pspec)
// {
//     MdManager *manager = MD_MANAGER (manager);
//
//     switch (prop_id)
//     {
// //         case PROP_ALLOW_MULTILINE:
// //             ws->priv->allow_multiline = g_value_get_boolean (value);
// //             g_object_notify (object, "allow-multiline");
// //             break;
//
//         default:
//             G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
//             break;
//     }
// }

// static void
// md_manager_get_property (GObject    *object,
//                                    guint       prop_id,
//                                    GValue     *value,
//                                    GParamSpec *pspec)
// {
//     MdManager *manager = MD_MANAGER (manager);
//
//     switch (prop_id)
//     {
// //         case PROP_GAP_STATE:
// //             g_value_set_enum (value, ws->priv->gap_state);
// //             break;
//
//         default:
//             G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
//             break;
//     }
// }


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

    if (i)
        return g_strdup_printf (_("Untitled%s"), mgr->priv->default_ext);
    else
        return g_strdup_printf (_("Untitled %u%s"), i, mgr->priv->default_ext);
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

    g_return_if_fail (i < mgr->priv->untitled->len);
    mgr->priv->untitled->pdata[i] = NULL;
}


void
md_manager_set_doc_type (MdManager *mgr,
                         GType      type)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (g_type_is_a (type, MD_TYPE_DOCUMENT));
    g_return_if_fail (g_type_is_a (type, GTK_TYPE_WIDGET));

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

    doc = create_doc (mgr, window, NULL);
    md_window_set_active_doc (window, doc);
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
        _md_manager_action_open_files (mgr, window, files);
        md_file_info_array_free (files);
    }
}

void
_md_manager_action_reload (MdManager *mgr,
                           MdDocument        *doc)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (md_document_get_manager (doc) == mgr);

    reload_doc (mgr, doc);
}

void
_md_manager_action_save (MdManager *mgr,
                         MdDocument        *doc)
{
    MdFileInfo *file_info;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (md_document_get_manager (doc) == mgr);

    file_info = md_document_get_file_info (doc);

    if (file_info)
    {
        MdFileInfo *copy = md_file_info_copy (file_info);
        save_file (mgr, doc, copy);
        md_file_info_free (copy);
    }
    else
    {
        _md_manager_action_save_as (mgr, doc);
    }
}

void
_md_manager_action_save_as (MdManager *mgr,
                            MdDocument        *doc)
{
    MdFileInfo *file_info;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_DOCUMENT (doc));
    g_return_if_fail (md_document_get_manager (doc) == mgr);

    file_info = ask_save_as (mgr, doc);

    if (!file_info)
        return;

    save_file (mgr, doc, file_info);
    md_file_info_free (file_info);
}

void
_md_manager_action_open_files (MdManager   *mgr,
                               MdWindow    *window,
                               MdFileInfo **files)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (!window || MD_IS_WINDOW (window));
    g_return_if_fail (!window || md_window_get_manager (window) == mgr);
    g_return_if_fail (files && *files);

    g_signal_emit (mgr, signals[ACTION_OPEN_FILES], 0, window, files);
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

    window = g_object_new (mgr->priv->window_type, "document-manager", mgr, NULL);
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
            MdWindow   *window,
            MdFileInfo *file)
{
    MdDocument *doc;

    doc = g_object_new (mgr->priv->doc_type,
                        "doc-uri", file ? md_file_info_get_uri (file) : NULL,
                        NULL);
    g_return_val_if_fail (doc != NULL, NULL);

    gtk_widget_show (GTK_WIDGET (doc));
    MOO_OBJECT_REF_SINK (doc);

    _md_document_set_manager (doc, mgr);

    if (window)
        _md_window_insert_doc (window, doc);
    else
        mgr->priv->windowless = g_slist_prepend (mgr->priv->windowless, doc);

    g_signal_connect (doc, "destroy", G_CALLBACK (doc_destroyed), mgr);

    g_signal_emit (mgr, signals[NEW_DOC], 0, doc);

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

    g_object_unref (doc);
}


static void
schedule_close_doc (MdDocument *doc)
{
    g_object_set_data (G_OBJECT (doc),
                       "md-document-need-close",
                       GINT_TO_POINTER (TRUE));
}

static void
cancel_close_doc (MdDocument *doc)
{
    MdWindow *window;

    g_object_set_data (G_OBJECT (doc), "md-document-need-close", NULL);

    if ((window = md_document_get_window (doc)))
        cancel_close_window (window);
}

static void
schedule_close_window (MdWindow *window)
{
    g_object_set_data (G_OBJECT (window),
                       "md-window-need-close",
                       GINT_TO_POINTER (TRUE));
}

static void
cancel_close_window (MdWindow *window)
{
    g_object_set_data (G_OBJECT (window), "md-window-need-close", NULL);
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
        if (_md_document_need_save (docs->data))
            *modified = g_slist_prepend (*modified, docs->data);
        else
            *saved = g_slist_prepend (*saved, docs->data);
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

static gboolean
action_close_docs (MdManager *mgr,
                   GSList    *docs)
{
    GSList *modified = NULL;
    GSList *saved = NULL;
    GSList *to_save = NULL;
    GSList *tmp;
    gboolean cancelled = FALSE;

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
            saved = g_slist_prepend (saved, modified->data);
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


static void
handler_action_open_files (MdManager   *mgr,
                           MdWindow    *window,
                           MdFileInfo **files)
{
    g_return_if_fail (files && *files);

    for ( ; *files; files++)
    {
        MdFileInfo *fi = *files;
        MdDocument *doc;

        if (!(doc = open_file (mgr, window, fi)))
            continue;

        if (!window)
            window = create_window (mgr);

        _md_window_insert_doc (window, doc);
    }
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
handler_ask_save_as (G_GNUC_UNUSED MdManager *mgr,
                     MdDocument *doc)
{
    return md_save_as_dialog (doc);
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
handler_ask_open (G_GNUC_UNUSED MdManager *mgr,
                  MdWindow *window)
{
    return md_open_dialog (window);
}


static void
setup_async_loading (G_GNUC_UNUSED MdManager  *mgr,
                     G_GNUC_UNUSED MdDocument *doc)
{
    g_critical ("%s: implement me", G_STRFUNC);
}

static void
reload_doc (MdManager  *mgr,
            MdDocument *doc)
{
    MdFileOpStatus status;
    GError *error = NULL;
    MdFileInfo *file_info, *copy;

    file_info = md_document_get_file_info (doc);
    g_return_if_fail (file_info != NULL);

    copy = md_file_info_copy (file_info);
    status = _md_document_load_file (doc, copy, &error);

    switch (status)
    {
        case MD_FILE_OP_STATUS_IN_PROGRESS:
            setup_async_loading (mgr, doc);
            break;

        case MD_FILE_OP_STATUS_ERROR:
            md_open_error_dialog (GTK_WIDGET (doc), copy, error);
            g_error_free (error);
            break;

        case MD_FILE_OP_STATUS_SUCCESS:
            break;

        case MD_FILE_OP_STATUS_CANCELLED:
            g_return_if_reached ();
    }
}

static MdDocument *
open_file (MdManager  *mgr,
           MdWindow   *window,
           MdFileInfo *file)
{
    MdFileOpStatus status;
    GError *error = NULL;
    MdDocument *doc;

    doc = create_doc (mgr, window, file);
    g_return_val_if_fail (doc != NULL, NULL);

    status = _md_document_load_file (doc, file, &error);

    switch (status)
    {
        case MD_FILE_OP_STATUS_IN_PROGRESS:
            setup_async_loading (mgr, doc);
            return doc;

        case MD_FILE_OP_STATUS_ERROR:
            md_open_error_dialog (window ? GTK_WIDGET (window) : NULL, file, error);
            g_error_free (error);
            g_object_unref (doc);
            return NULL;

        case MD_FILE_OP_STATUS_SUCCESS:
            return doc;

        case MD_FILE_OP_STATUS_CANCELLED:
            break;
    }

    g_return_val_if_reached (NULL);
}


static void
save_finished (G_GNUC_UNUSED MdDocument *doc)
{
    g_critical ("%s: implement me", G_STRFUNC);
}

static void
setup_async_saving (G_GNUC_UNUSED MdManager  *mgr,
                    G_GNUC_UNUSED MdDocument *doc)
{
    g_critical ("%s: implement me", G_STRFUNC);
}

static void
save_file (MdManager  *mgr,
           MdDocument *doc,
           MdFileInfo *file)
{
    MdFileOpStatus status;
    GError *error = NULL;

    status = _md_document_save_file (doc, file, &error);

    switch (status)
    {
        case MD_FILE_OP_STATUS_IN_PROGRESS:
            setup_async_saving (mgr, doc);
            break;

        case MD_FILE_OP_STATUS_ERROR:
            md_save_error_dialog (GTK_WIDGET (doc), file, error);
            g_error_free (error);
            cancel_close_doc (doc);
            break;

        case MD_FILE_OP_STATUS_SUCCESS:
            save_finished (doc);
            break;

        case MD_FILE_OP_STATUS_CANCELLED:
            g_return_if_reached ();
            break;
    }
}
