#include "mdmanager-private.h"
#include "mdwindow-private.h"
#include "mddocument-private.h"
#include "mdview-private.h"
#include "mdasyncop.h"
#include "mdapp.h"
#include "mddialogs.h"
#include "marshals.h"
#include "ggap-i18n.h"
#include "mooutils/mooutils-misc.h"


struct MdManagerPrivate {
    char *app_name;
    GPtrArray *untitled;
    GType doc_type;
    GType view_type;
    GType window_type;
    char *default_ext;

    GSList *windows;
    GSList *windowless;
    GSList *docs;

    gboolean handling_quit;

    MooUIXML *xml;
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
static void     handler_close_view              (MdManager      *mgr,
                                                 MdView         *view);
static void     handler_close_window            (MdManager      *mgr,
                                                 MdWindow       *window);
static MdCloseAllResult handler_close_all       (MdManager      *mgr);
static void     handler_action_close_views      (MdManager      *mgr,
                                                 GSList         *docs);
static void     handler_action_close_windows    (MdManager      *mgr,
                                                 GSList         *windows);
static void     emit_close_doc                  (MdManager      *mgr,
                                                 MdDocument     *doc);
static void     emit_close_view                 (MdManager      *mgr,
                                                 MdView         *view);
static void     emit_close_window               (MdManager      *mgr,
                                                 MdWindow       *window);
static void     handler_action_open_files       (MdManager      *mgr,
                                                 MdWindow       *window,
                                                 MdFileInfo    **files);
static MdFileInfo  *handler_ask_save_as         (MdManager      *mgr,
                                                 MdView         *view);
static MdFileInfo **handler_ask_open            (MdManager      *mgr,
                                                 MdWindow       *window);

static MdWindow    *create_window               (MdManager      *mgr);
static MdDocument  *create_doc                  (MdManager      *mgr,
                                                 MdFileInfo     *file);
static MdView      *create_view                 (MdManager      *mgr,
                                                 MdDocument     *doc);

static gboolean     save_and_close_view         (MdManager      *mgr,
                                                 MdView         *view);
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
                                                 MdView         *view);
static MdFileInfo **ask_open                    (MdManager      *mgr,
                                                 MdWindow       *window);
static void         save_file                   (MdManager      *mgr,
                                                 MdView         *view,
                                                 MdFileInfo     *file_info);
static MdView      *open_file                   (MdManager      *mgr,
                                                 MdFileInfo     *file,
                                                 GError        **error);
static void         reload_doc                  (MdManager      *mgr,
                                                 MdView         *view);

static void         app_quit_requested          (MdManager      *mgr,
                                                 MdApp          *app);
static void         app_quit_cancelled          (MdManager      *mgr);
static void         app_quit                    (MdManager      *mgr);


G_DEFINE_TYPE (MdManager, md_manager, G_TYPE_OBJECT)

enum {
    NEW_DOC,
    NEW_VIEW,
    NEW_WINDOW,
    CLOSE_DOC,
    CLOSE_VIEW,
    CLOSE_WINDOW,
    ASK_SAVE_AS,
    ASK_OPEN,
    ACTION_CLOSE_VIEWS,
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
    klass->close_view = handler_close_view;
    klass->close_window = handler_close_window;
    klass->close_all = handler_close_all;
    klass->ask_save_as = handler_ask_save_as;
    klass->ask_open = handler_ask_open;

    klass->action_close_views = handler_action_close_views;
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

    signals[NEW_VIEW] =
        g_signal_new ("new-view",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, new_view),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1,
                      MD_TYPE_VIEW);

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

    signals[CLOSE_VIEW] =
        g_signal_new ("close-view",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, close_view),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1,
                      MD_TYPE_VIEW);

    signals[CLOSE_WINDOW] =
        g_signal_new ("close-window",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, close_window),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__OBJECT,
                      G_TYPE_NONE, 1,
                      MD_TYPE_WINDOW);

    signals[ACTION_CLOSE_VIEWS] =
        g_signal_new ("action-close-views",
                      MD_TYPE_MANAGER,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdManagerClass, action_close_views),
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
                      MD_TYPE_VIEW);
}

static void
md_manager_init (MdManager *mgr)
{
    mgr->priv = G_TYPE_INSTANCE_GET_PRIVATE (mgr, MD_TYPE_MANAGER, MdManagerPrivate);
    mgr->priv->untitled = g_ptr_array_new ();
    mgr->priv->default_ext = g_strdup ("");
    mgr->priv->window_type = MD_TYPE_WINDOW;
    mgr->priv->doc_type = 0;
    mgr->priv->view_type = 0;
}

static GObject *
md_manager_constructor (GType                  type,
                        guint                  n_props,
                        GObjectConstructParam *props)
{
    GObject *object;
    MdManager *mgr;
    MdApp *app;

    object = G_OBJECT_CLASS (md_manager_parent_class)->constructor (type, n_props, props);
    mgr = MD_MANAGER (object);

    if ((app = md_app_instance ()))
    {
        g_signal_connect_swapped (app, "quit-requested", G_CALLBACK (app_quit_requested), mgr);
        g_signal_connect_swapped (app, "quit-cancelled", G_CALLBACK (app_quit_cancelled), mgr);
        g_signal_connect_swapped (app, "quit", G_CALLBACK (app_quit), mgr);
    }

    return object;
}

static void
md_manager_dispose (GObject *object)
{
    MdManager *mgr = MD_MANAGER (object);

    if (mgr->priv)
    {
        MdApp *app;

        if ((app = md_app_instance ()))
        {
            g_signal_handlers_disconnect_by_func (app, (gpointer) app_quit_requested, mgr);
            g_signal_handlers_disconnect_by_func (app, (gpointer) app_quit_cancelled, mgr);
            g_signal_handlers_disconnect_by_func (app, (gpointer) app_quit, mgr);
        }

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

    g_return_if_fail (i < mgr->priv->untitled->len);
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
md_manager_set_view_type (MdManager *mgr,
                          GType      type)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (g_type_is_a (type, MD_TYPE_VIEW));
    mgr->priv->view_type = type;
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
        if (mgr->priv->xml)
            g_object_unref (mgr->priv->xml);
        mgr->priv->xml = xml;
        if (mgr->priv->xml)
            g_object_ref (mgr->priv->xml);
    }
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
_md_manager_action_close_views (MdManager *mgr,
                                GSList    *list)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (list != NULL);

    g_signal_emit (mgr, signals[ACTION_CLOSE_VIEWS], 0, list);
}

static void
emit_close_doc (MdManager  *mgr,
                MdDocument *doc)
{
    g_signal_emit (mgr, signals[CLOSE_DOC], 0, doc);
}

static void
emit_close_view (MdManager *mgr,
                 MdView    *view)
{
    g_signal_emit (mgr, signals[CLOSE_VIEW], 0, view);
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
    MdView *view;
    MdDocument *doc;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (!window || MD_IS_WINDOW (window));
    g_return_if_fail (!window || md_window_get_manager (window) == mgr);

    if (!window)
        window = create_window (mgr);

    doc = create_doc (mgr, NULL);
    view = create_view (mgr, doc);

    _md_manager_move_view (mgr, view, window);
    md_window_set_active_view (window, view);
    gtk_widget_grab_focus (GTK_WIDGET (view));
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
                           MdView    *view)
{
    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (md_view_get_manager (view) == mgr);

    reload_doc (mgr, view);
}

void
_md_manager_action_save (MdManager *mgr,
                         MdView    *view)
{
    MdFileInfo *file_info;
    MdDocument *doc;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (md_view_get_manager (view) == mgr);

    doc = md_view_get_doc (view);
    file_info = md_document_get_file_info (doc);

    if (file_info)
    {
        MdFileInfo *copy = md_file_info_copy (file_info);
        save_file (mgr, view, copy);
        md_file_info_free (copy);
    }
    else
    {
        _md_manager_action_save_as (mgr, view);
    }
}

void
_md_manager_action_save_as (MdManager *mgr,
                            MdView    *view)
{
    MdFileInfo *file_info;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (md_view_get_manager (view) == mgr);

    file_info = ask_save_as (mgr, view);

    if (!file_info)
        return;

    save_file (mgr, view, file_info);
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
_md_manager_move_view (MdManager  *mgr,
                       MdView     *view,
                       MdWindow   *dest)
{
    MdWindow *old_window;

    g_return_if_fail (MD_IS_MANAGER (mgr));
    g_return_if_fail (MD_IS_VIEW (view));
    g_return_if_fail (md_view_get_manager (view) == mgr);
    g_return_if_fail (!dest || MD_IS_WINDOW (dest));
    g_return_if_fail (!dest || md_window_get_manager (dest) == mgr);

    old_window = md_view_get_window (view);
    g_return_if_fail (!old_window || old_window != dest);

    if (!dest)
    {
        dest = create_window (mgr);
        g_return_if_fail (dest != NULL);
    }

    g_object_ref (view);

    if (old_window)
        _md_window_remove_view (old_window, view);
    else
        mgr->priv->windowless = g_slist_remove (mgr->priv->windowless, view);

    _md_window_insert_view (dest, view);

    g_object_unref (view);
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
    GSList *views;

    g_return_if_fail (md_window_get_manager (window) == mgr);

    views = md_window_list_views (window);
    g_slist_foreach (views, (GFunc) g_object_ref, NULL);

    while (views)
    {
        emit_close_view (mgr, views->data);
        g_object_unref (views->data);
        views = g_slist_delete_link (views, views);
    }

    g_signal_handlers_disconnect_by_func (window, (gpointer) window_destroyed, mgr);
    mgr->priv->windows = g_slist_remove (mgr->priv->windows, window);
    gtk_widget_destroy (GTK_WIDGET (window));
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

    mgr->priv->docs = g_slist_prepend (mgr->priv->docs, doc);
    g_signal_emit (mgr, signals[NEW_DOC], 0, doc);

    return doc;
}

static void
view_destroyed (MdView    *view,
                MdManager *mgr)
{
    emit_close_view (mgr, view);
}

static MdView *
create_view (MdManager  *mgr,
             MdDocument *doc)
{
    MdView *view;

    g_return_val_if_fail (MD_IS_DOCUMENT (doc), NULL);

    view = g_object_new (mgr->priv->view_type, "md-document", doc, NULL);
    g_return_val_if_fail (view != NULL, NULL);

    _md_view_set_manager (view, mgr);
    _md_document_add_view (doc, view);

    gtk_widget_show (GTK_WIDGET (view));
    MOO_OBJECT_REF_SINK (view);

    mgr->priv->windowless = g_slist_prepend (mgr->priv->windowless, view);
    g_signal_connect (view, "destroy", G_CALLBACK (view_destroyed), mgr);
    g_signal_emit (mgr, signals[NEW_VIEW], 0, view, doc);

    return view;
}


static void
handler_close_view (MdManager *mgr,
                    MdView    *view)
{
    MdWindow *window;
    MdDocument *doc;

    g_return_if_fail (md_view_get_manager (view) == mgr);

    g_object_ref (view);

    g_signal_handlers_disconnect_by_func (view, (gpointer) view_destroyed, mgr);

    if ((window = md_view_get_window (view)))
    {
        _md_window_remove_view (window, view);
    }
    else
    {
        mgr->priv->windowless = g_slist_remove (mgr->priv->windowless, view);
        g_object_unref (view);
    }

    doc = md_view_get_doc (view);
    _md_document_remove_view (doc, view);

    if (!md_document_get_view (doc))
    {
        /* XXX */
        emit_close_doc (mgr, doc);
    }

    if (window && !close_window_if_scheduled (mgr, window))
    {
        /* new empty doc */
    }

    if (!mgr->priv->windows && !mgr->priv->windowless && mgr->priv->handling_quit)
    {
        mgr->priv->handling_quit = FALSE;
        md_app_resume_quit (md_app_instance ());
    }

    g_object_unref (view);
}

static void
handler_close_doc (MdManager  *mgr,
                   MdDocument *doc)
{
    g_return_if_fail (md_document_get_manager (doc) == mgr);
    mgr->priv->docs = g_slist_remove (mgr->priv->docs, doc);
    _md_document_close (doc);
    g_object_unref (doc);
}


static void
cancel_quit (MdManager *mgr)
{
    if (mgr->priv->handling_quit)
    {
        mgr->priv->handling_quit = FALSE;
        md_app_cancel_quit (md_app_instance ());
    }
}

static gboolean
close_doc_if_scheduled (MdManager  *mgr,
                        MdDocument *doc)
{
    GSList *views;

    if (!g_object_get_data (G_OBJECT (doc), "md-document-need-close") &&
        !mgr->priv->handling_quit)
            return FALSE;

    views = md_document_get_views (doc);

    while (views)
    {
        emit_close_view (mgr, views->data);
        views = g_slist_delete_link (views, views);
    }

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
    GSList *views;

    cancel_quit (mgr);

    g_object_set_data (G_OBJECT (doc), "md-document-need-close", NULL);

    views = md_document_get_views (doc);

    while (views)
    {
        MdWindow *window;

        if ((window = md_view_get_window (views->data)))
            cancel_close_window (mgr, window);

        views = g_slist_delete_link (views, views);
    }
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
                    MdApp     *app)
{
    MdCloseAllResult result;

    if (mgr->priv->handling_quit)
        return;

    mgr->priv->handling_quit = TRUE;

    result = MD_MANAGER_GET_CLASS (mgr)->close_all (mgr);

    switch (result)
    {
        case MD_CLOSE_ALL_DONE:
            mgr->priv->handling_quit = FALSE;
            break;
        case MD_CLOSE_ALL_CANCELLED:
            mgr->priv->handling_quit = FALSE;
            md_app_cancel_quit (app);
            break;
        case MD_CLOSE_ALL_IN_PROGRESS:
            md_app_delay_quit (app);
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
        emit_close_view (mgr, mgr->priv->windowless->data);
    while (mgr->priv->docs)
        emit_close_doc (mgr, mgr->priv->docs->data);
}


static gboolean
save_and_close_view (MdManager *mgr,
                     MdView    *view)
{
    MdFileInfo *file_info;
    MdFileInfo *copy;
    MdDocument *doc;

    doc = md_view_get_doc (view);
    file_info = md_document_get_file_info (doc);

    if (file_info)
        copy = md_file_info_copy (file_info);
    else
        copy = ask_save_as (mgr, view);

    if (!copy)
        return FALSE;

    schedule_close_doc (doc);
    save_file (mgr, view, copy);

    md_file_info_free (copy);
    return TRUE;
}

static void
find_modified (GSList  *views,
               GSList **modified,
               GSList **saved)
{
    for ( ; views != NULL; views = views->next)
    {
        /* XXX multiple views */

        MdDocument *doc = md_view_get_doc (views->data);

        if (_md_document_need_save (doc))
            *modified = g_slist_prepend (*modified, views->data);
        else
            *saved = g_slist_prepend (*saved, views->data);
    }
}

static gboolean
ask_save (GSList  *modified,
          GSList **to_save)
{
    /* XXX multiple views */

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
action_close_views (MdManager *mgr,
                    GSList    *views)
{
    GSList *modified = NULL;
    GSList *saved = NULL;
    GSList *to_save = NULL;
    GSList *tmp;
    gboolean cancelled = FALSE;

    find_modified (views, &modified, &saved);

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
        emit_close_view (mgr, saved->data);
        saved = g_slist_delete_link (saved, saved);
    }

    while (modified)
    {
        if (!save_and_close_view (mgr, modified->data))
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
handler_action_close_views (MdManager *mgr,
                            GSList    *views)
{
    action_close_views (mgr, views);
}

static void
handler_action_close_windows (MdManager *mgr,
                              GSList    *windows)
{
    GSList *views = NULL;
    GSList *l;

    for (l = windows, windows = NULL; l != NULL; l = l->next)
    {
        MdWindow *window = l->data;
        GSList *views_here = md_window_list_views (window);

        if (views_here)
        {
            views = g_slist_concat (views, views_here);
            windows = g_slist_prepend (windows, window);
        }
        else
        {
            emit_close_window (mgr, window);
        }
    }

    if (!action_close_views (mgr, views))
        return;

    while (windows)
    {
        MdWindow *window = windows->data;

        if (md_window_get_active_view (window))
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
    GSList *views = NULL;
    GSList *l;

    for (l = mgr->priv->windows; l != NULL; l = l->next)
    {
        GSList *tmp = md_window_list_views (l->data);
        views = g_slist_concat (tmp, views);
    }

    views = g_slist_concat (mgr->priv->windowless, views);

    if (views)
    {
        if (!action_close_views (mgr, views))
        {
            result = MD_CLOSE_ALL_CANCELLED;
        }
        else
        {
            GSList *tmp = g_slist_copy (mgr->priv->windows);

            while (tmp)
            {
                MdWindow *window = tmp->data;

                if (md_window_get_active_view (window))
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

    g_slist_free (views);
    return result;
}


static MdFileInfo *
ask_save_as (MdManager *mgr,
             MdView    *view)
{
    MdFileInfo *file_info = NULL;
    g_signal_emit (mgr, signals[ASK_SAVE_AS], 0, view, &file_info);
    return file_info;
}

static MdFileInfo *
handler_ask_save_as (G_GNUC_UNUSED MdManager *mgr,
                     MdView *view)
{
    return md_save_as_dialog (view);
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
setup_async_loading (G_GNUC_UNUSED MdManager *mgr,
                     G_GNUC_UNUSED MdView    *view,
                     G_GNUC_UNUSED gboolean   reloading)
{
    g_critical ("%s: implement me", G_STRFUNC);
}

static void
reload_doc (MdManager *mgr,
            MdView    *view)
{
    MdFileOpStatus status;
    GError *error = NULL;
    MdFileInfo *file_info, *copy;
    MdDocument *doc;

    doc = md_view_get_doc (view);
    file_info = md_document_get_file_info (doc);
    g_return_if_fail (file_info != NULL);

    copy = md_file_info_copy (file_info);
    status = _md_document_load_file (doc, copy, &error);

    switch (status)
    {
        case MD_FILE_OP_STATUS_IN_PROGRESS:
            setup_async_loading (mgr, view, TRUE);
            break;

        case MD_FILE_OP_STATUS_ERROR:
            md_open_error_dialog (GTK_WIDGET (view), copy, error);
            g_error_free (error);
            break;

        case MD_FILE_OP_STATUS_SUCCESS:
            md_document_set_status (doc, 0);
            break;

        case MD_FILE_OP_STATUS_CANCELLED:
            g_return_if_reached ();
    }

    md_file_info_free (copy);
    if (error)
        g_error_free (error);
}

static MdView *
open_file (MdManager   *mgr,
           MdFileInfo  *file,
           GError     **error)
{
    MdFileOpStatus status;
    MdDocument *doc;
    MdView *view;

    doc = create_doc (mgr, file);
    g_return_val_if_fail (doc != NULL, NULL);
    view = create_view (mgr, doc);

    md_document_set_file_info (doc, file);
    status = _md_document_load_file (doc, file, error);

    switch (status)
    {
        case MD_FILE_OP_STATUS_IN_PROGRESS:
            setup_async_loading (mgr, view, FALSE);
            return view;

        case MD_FILE_OP_STATUS_SUCCESS:
            md_document_set_status (doc, 0);
            return view;

        case MD_FILE_OP_STATUS_ERROR:
            g_object_unref (doc);
            return NULL;

        case MD_FILE_OP_STATUS_CANCELLED:
            break;
    }

    g_object_unref (view);
    g_object_unref (doc);
    g_return_val_if_reached (NULL);
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
        MdView *view;
        GError *error = NULL;

        if (!(view = open_file (mgr, fi, &error)))
        {
            md_open_error_dialog (window ? GTK_WIDGET (window) : NULL,
                                  fi, error);
            g_error_free (error);
            continue;
        }

        if (!window)
            window = create_window (mgr);

        _md_manager_move_view (mgr, view, window);
    }
}


static void
save_finished (MdManager      *mgr,
               MdDocument     *doc,
               MdFileInfo     *file,
               MdFileOpStatus  status,
               const char     *error)
{
    switch (status)
    {
        case MD_FILE_OP_STATUS_ERROR:
            md_save_error_dialog (GTK_WIDGET (doc), file, error);
            cancel_close_doc (mgr, doc);
            break;

        case MD_FILE_OP_STATUS_SUCCESS:
            md_document_set_file_info (doc, file);
            md_document_set_status (doc, 0);
            close_doc_if_scheduled (mgr, doc);
            break;

        case MD_FILE_OP_STATUS_CANCELLED:
            cancel_close_doc (mgr, doc);
            break;

        default:
            g_return_if_reached ();
    }
}

static void
async_save_finished (MdAsyncOp       *op,
                     MdAsyncOpResult  result,
                     const char      *error,
                     MdManager       *mgr)
{
    MdDocument *doc;
    MdFileInfo *file;
    MdFileOpStatus status = MD_FILE_OP_STATUS_ERROR;

    doc = md_async_op_get_doc (op);
    g_return_if_fail (MD_IS_DOCUMENT (doc));

    file = g_object_get_data (G_OBJECT (op), "md-manager-save-file-info");
    g_return_if_fail (file != NULL);

    switch (result)
    {
        case MD_ASYNC_OP_ERROR:
            status = MD_FILE_OP_STATUS_ERROR;
            break;
        case MD_ASYNC_OP_SUCCESS:
            status = MD_FILE_OP_STATUS_SUCCESS;
            break;
        case MD_ASYNC_OP_CANCELLED:
            status = MD_FILE_OP_STATUS_CANCELLED;
            break;
    }

    save_finished (mgr, doc, file, status, error);
}

static void
save_file (MdManager  *mgr,
           MdView     *view,
           MdFileInfo *file)
{
    MdFileOpStatus status;
    GError *error = NULL;
    MdDocument *doc;
    MdAsyncOp *op;

    doc = md_view_get_doc (view);
    status = _md_document_save_file (doc, file, &error);

    switch (status)
    {
        case MD_FILE_OP_STATUS_IN_PROGRESS:
            op = md_document_get_async_op (doc);
            g_object_set_data_full (G_OBJECT (op), "md-manager-save-file-info",
                                    md_file_info_copy (file),
                                    (GDestroyNotify) md_file_info_free);
            g_signal_connect (op, "finished", G_CALLBACK (async_save_finished), mgr);
            break;

        case MD_FILE_OP_STATUS_ERROR:
        case MD_FILE_OP_STATUS_SUCCESS:
            save_finished (mgr, doc, file, status, error ? error->message : "Error");
            break;

        case MD_FILE_OP_STATUS_CANCELLED:
            g_return_if_reached ();
            break;
    }

    if (error)
        g_error_free (error);
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
