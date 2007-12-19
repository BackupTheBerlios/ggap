#include "config.h"
#include "mdapp.h"
#include "mdappinput.h"
#include "mdmanager-private.h"
#include "marshals.h"
#include "mooutils/moostock.h"
#include "mooutils/mooprefs.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/mooutils-fs.h"
#include <stdlib.h>

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#if GTK_CHECK_VERSION(2,8,0) && defined(GDK_WINDOWING_X11)
#include <gdk/gdkx.h>
#define TIMESTAMP() (gdk_x11_display_get_user_time (gdk_display_get_default ()))
#else
#define TIMESTAMP() (0)
#endif

#ifndef __WIN32__
#define TMPL_RC_FILE    "%src"
#else
#define TMPL_RC_FILE    "%s.ini"
#endif
#define TMPL_STATE_FILE "%s.state"

static struct {
    MdApp *instance;
//     MdAppInput *input;
    gboolean atexit_installed;
} md_app_data;

static volatile int signal_received;

struct MdAppPrivate {
    gboolean need_display;
    gboolean need_ui;
    gboolean need_run;

    MooWindow *main_window;
    MdManager *md_mgr;

    gboolean running;
    int exit_code;
    guint gtk_quit_handler_id;

    gboolean handling_quit;
    guint delay_quit_count;

    gboolean quit_on_close_main_window;
    gboolean quit_on_close_docs;

    char *rc_files[2];

    char *version;
    char *version_string;
    char *short_name;
    char *full_name;
    char *website;
    char *website_label;
    char *description;
    char *logo;
    char *icon_name;
    char *credits;
    char *author;
};

static void     md_app_finalize                     (GObject    *object);
static void     md_app_set_property                 (GObject    *object,
                                                     guint       prop_id,
                                                     const GValue *value,
                                                     GParamSpec *pspec);
static void     md_app_get_property                 (GObject    *object,
                                                     guint       prop_id,
                                                     GValue     *value,
                                                     GParamSpec *pspec);

static gboolean md_app_handler_initialize           (MdApp          *app);
static int      md_app_handler_run                  (MdApp          *app,
                                                     int             argc,
                                                     char          **argv);
static void     md_app_handler_quit                 (MdApp          *app);
static void     md_app_handler_setup_option_context (MdApp          *app,
                                                     GOptionContext *ctx);

static gboolean md_app_quit                         (MdApp          *app);
static void     md_app_do_quit                      (MdApp          *app);
// static gboolean md_app_will_quit                    (MdApp          *app);

static void     md_app_load_prefs                   (MdApp          *app);
static void     md_app_save_prefs                   (MdApp          *app);
static void     md_app_install_cleanup              (void);
static void     md_app_cleanup                      (void);


G_DEFINE_TYPE (MdApp, md_app, G_TYPE_OBJECT)

enum {
    PROP_0,
    PROP_SHORT_NAME,
    PROP_FULL_NAME,
    PROP_VERSION,
    PROP_VERSION_STRING,
    PROP_WEBSITE,
    PROP_WEBSITE_LABEL,
    PROP_DESCRIPTION,
    PROP_LOGO,
    PROP_ICON_NAME,
    PROP_CREDITS,
    PROP_AUTHOR,
};

enum {
    QUIT,
    QUIT_REQUESTED,
    QUIT_CANCELLED,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

static void
md_app_class_init (MdAppClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->finalize = md_app_finalize;
    gobject_class->set_property = md_app_set_property;
    gobject_class->get_property = md_app_get_property;

    klass->initialize = md_app_handler_initialize;
    klass->run = md_app_handler_run;
    klass->quit = md_app_handler_quit;
    klass->setup_option_context = md_app_handler_setup_option_context;

    g_type_class_add_private (klass, sizeof (MdAppPrivate));

    g_object_class_install_property (gobject_class, PROP_SHORT_NAME,
        g_param_spec_string ("short-name", "Short name", "Short name, as in g_get_prgname()",
                             NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (gobject_class, PROP_FULL_NAME,
        g_param_spec_string ("full-name", "Full name", "Name displayed to user",
                             NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (gobject_class, PROP_VERSION,
        g_param_spec_string ("version", "version", "version",
                             NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (gobject_class, PROP_VERSION_STRING,
        g_param_spec_string ("version-string", "Version string", "Version string displayed to user",
                             NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (gobject_class, PROP_WEBSITE,
        g_param_spec_string ("website", "Website", "Website in the About dialog",
                             NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (gobject_class, PROP_WEBSITE_LABEL,
        g_param_spec_string ("website-label", "website-label", "website-label",
                             NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (gobject_class, PROP_DESCRIPTION,
        g_param_spec_string ("description", "description", "description",
                             NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (gobject_class, PROP_LOGO,
        g_param_spec_string ("logo", "logo", "logo",
                             NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (gobject_class, PROP_ICON_NAME,
        g_param_spec_string ("icon-name", "icon-name", "icon-name",
                             NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (gobject_class, PROP_CREDITS,
        g_param_spec_string ("credits", "credits", "credits",
                             NULL, G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

    signals[QUIT] =
        g_signal_new ("quit", MD_TYPE_APP,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAppClass, quit),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    signals[QUIT_REQUESTED] =
        g_signal_new ("quit-requested", MD_TYPE_APP,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAppClass, quit_requested),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    signals[QUIT_CANCELLED] =
        g_signal_new ("quit-cancelled", MD_TYPE_APP,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (MdAppClass, quit_cancelled),
                      NULL, NULL,
                      _moo_ui_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);
}

#if defined(HAVE_SIGNAL)
static void
setup_signals (void(*handler)(int))
{
    signal (SIGINT, handler);
#ifdef SIGHUP
    /* TODO: maybe detach from terminal in this case? */
    signal (SIGHUP, handler);
#endif
}

static void
sigint_handler (int sig)
{
    signal_received = sig;
    setup_signals (SIG_DFL);
}
#endif

static void
md_app_init (MdApp *app)
{
    g_return_if_fail (md_app_data.instance == NULL);
    md_app_data.instance = app;

    app->priv = G_TYPE_INSTANCE_GET_PRIVATE (app, MD_TYPE_APP, MdAppPrivate);

    app->priv->need_display = TRUE;
    app->priv->need_ui = TRUE;
    app->priv->need_run = TRUE;

    app->priv->quit_on_close_main_window = TRUE;
    app->priv->quit_on_close_docs = TRUE;

    app->priv->running = FALSE;
    app->priv->exit_code = EXIT_SUCCESS;

#if defined(HAVE_SIGNAL) && defined(SIGINT)
    setup_signals (sigint_handler);
#endif
    md_app_install_cleanup ();
}

static void
md_app_finalize (GObject *object)
{
    MdApp *app = MD_APP (object);

    if (md_app_data.instance == app)
        md_app_data.instance = NULL;

    g_free (app->priv->version_string);
    g_free (app->priv->rc_files[0]);
    g_free (app->priv->rc_files[1]);

    G_OBJECT_CLASS (md_app_parent_class)->finalize (object);
}

static void
md_app_set_property (GObject    *object,
                     guint       prop_id,
                     const GValue *value,
                     GParamSpec *pspec)
{
    MdApp *app = MD_APP (object);

    switch (prop_id)
    {
        case PROP_SHORT_NAME:
            g_free (app->priv->short_name);
            app->priv->short_name = g_value_dup_string (value);
            if (app->priv->short_name)
                g_set_prgname (app->priv->short_name);
            break;

        case PROP_FULL_NAME:
            g_free (app->priv->full_name);
            app->priv->full_name = g_value_dup_string (value);
            break;

        case PROP_VERSION:
            g_free (app->priv->version);
            app->priv->version = g_value_dup_string (value);
            break;

        case PROP_VERSION_STRING:
            g_free (app->priv->version_string);
            app->priv->version_string = g_value_dup_string (value);
            break;

        case PROP_WEBSITE:
            g_free (app->priv->website);
            app->priv->website = g_value_dup_string (value);
            break;

        case PROP_WEBSITE_LABEL:
            g_free (app->priv->website_label);
            app->priv->website_label = g_value_dup_string (value);
            break;

        case PROP_DESCRIPTION:
            g_free (app->priv->description);
            app->priv->description = g_value_dup_string (value);
            break;

        case PROP_LOGO:
            g_free (app->priv->logo);
            app->priv->logo = g_value_dup_string (value);
            break;

        case PROP_ICON_NAME:
            g_free (app->priv->icon_name);
            app->priv->icon_name = g_value_dup_string (value);
            break;

        case PROP_CREDITS:
            g_free (app->priv->credits);
            app->priv->credits = g_value_dup_string (value);
            break;

        case PROP_AUTHOR:
            g_free (app->priv->author);
            app->priv->author = g_value_dup_string (value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static const char *
get_short_name (MdApp *app)
{
    if (!app->priv->short_name)
        app->priv->short_name = g_strdup (g_get_prgname ());
    return app->priv->short_name;
}

static const char *
get_full_name (MdApp *app)
{
    if (!app->priv->full_name)
        app->priv->full_name = g_strdup (get_short_name (app));
    return app->priv->full_name;
}

static const char *
get_version (MdApp *app)
{
    if (!app->priv->version)
        app->priv->version = g_strdup ("");
    return app->priv->version;
}

static const char *
get_version_string (MdApp *app)
{
    if (!app->priv->version_string)
        app->priv->version_string = g_strdup_printf ("%s %s",
                                                     get_full_name (app),
                                                     get_version (app));
    return app->priv->version_string;
}

static const char *
get_website_label (MdApp *app)
{
    if (!app->priv->website_label && app->priv->website)
        app->priv->website_label = g_strdup (app->priv->website);
    return app->priv->website_label;
}

static const char *
get_icon_name (MdApp *app)
{
    if (!app->priv->icon_name)
        app->priv->icon_name = g_strdup (get_short_name (app));
    return app->priv->icon_name;
}

static const char *
get_logo (MdApp *app)
{
    if (!app->priv->logo)
        app->priv->logo = g_strdup (get_icon_name (app));
    return app->priv->logo;
}

static void
md_app_get_property (GObject    *object,
                     guint       prop_id,
                     GValue     *value,
                     GParamSpec *pspec)
{
    MdApp *app = MD_APP (object);

    switch (prop_id)
    {
        case PROP_SHORT_NAME:
            g_value_set_string (value, get_short_name (app));
            break;

        case PROP_FULL_NAME:
            g_value_set_string (value, get_full_name (app));
            break;

        case PROP_VERSION:
            g_value_set_string (value, get_version (app));
            break;

        case PROP_VERSION_STRING:
            g_value_set_string (value, get_version_string (app));
            break;

        case PROP_WEBSITE:
            g_value_set_string (value, app->priv->website);
            break;

        case PROP_WEBSITE_LABEL:
            g_value_set_string (value, get_website_label (app));
            break;

        case PROP_DESCRIPTION:
            g_value_set_string (value, app->priv->description);
            break;

        case PROP_LOGO:
            g_value_set_string (value, get_logo (app));
            break;

        case PROP_ICON_NAME:
            g_value_set_string (value, app->priv->icon_name);
            break;

        case PROP_CREDITS:
            g_value_set_string (value, app->priv->credits);
            break;

        case PROP_AUTHOR:
            g_value_set_string (value, app->priv->author);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}


MdApp *
md_app_instance (void)
{
    return md_app_data.instance;
}


static void
main_window_destroy (MdApp *app)
{
    app->priv->main_window = NULL;
    if (app->priv->quit_on_close_main_window)
        md_app_quit (app);
}

static gboolean
main_window_close (MdApp *app)
{
    if (app->priv->quit_on_close_main_window)
        return !md_app_quit (app);
    else
        return FALSE;
}

void
md_app_set_main_window (MdApp     *app,
                        MooWindow *window)
{
    g_return_if_fail (MD_IS_APP (app));
    g_return_if_fail (!window || MOO_IS_WINDOW (window));

    if (app->priv->main_window == window)
        return;

    if (app->priv->main_window)
    {
        g_signal_handlers_disconnect_by_func (app->priv->main_window,
                                              (gpointer) main_window_close,
                                              app);
        g_signal_handlers_disconnect_by_func (app->priv->main_window,
                                              (gpointer) main_window_destroy,
                                              app);
    }

    app->priv->main_window = window;

    if (window)
    {
        g_signal_connect_swapped (window, "destroy",
                                  G_CALLBACK (main_window_destroy),
                                  app);
        g_signal_connect_swapped (window, "close",
                                  G_CALLBACK (main_window_close),
                                  app);
    }
}


static void
mgr_close_window (MdManager *mgr)
{
    MdApp *app = md_app_instance ();

    if (app->priv->quit_on_close_docs &&
        !app->priv->main_window &&
        !md_manager_get_active_window (mgr))
            md_app_quit (app);
}

void
md_app_set_document_manager (MdApp     *app,
                             MdManager *mgr)
{
    g_return_if_fail (MD_IS_APP (app));
    g_return_if_fail (!mgr || MD_IS_MANAGER (mgr));

    if (app->priv->md_mgr == mgr)
        return;

    if (app->priv->md_mgr)
    {
        g_signal_handlers_disconnect_by_func (app->priv->md_mgr,
                                              (gpointer) mgr_close_window,
                                              NULL);
        g_object_unref (app->priv->md_mgr);
    }

    app->priv->md_mgr = mgr;

    if (mgr)
    {
        g_object_ref (mgr);
        g_signal_connect_after (mgr, "close-window",
                                G_CALLBACK (mgr_close_window),
                                NULL);
    }
}


void
md_app_setup_option_context (MdApp          *app,
                             GOptionContext *ctx)
{
    g_return_if_fail (MD_IS_APP (app));
    g_return_if_fail (ctx != NULL);
    MD_APP_GET_CLASS (app)->setup_option_context (app, ctx);
}

G_GNUC_NORETURN static void
show_version (void)
{
    g_print ("%s\n", get_version_string (md_app_instance ()));
    exit (EXIT_SUCCESS);
}

static void
md_app_handler_setup_option_context (MdApp          *app,
                                     GOptionContext *ctx)
{
    GOptionGroup *group;
    GOptionEntry entries[] = {
        { "version", 0, G_OPTION_FLAG_NO_ARG | G_OPTION_FLAG_IN_MAIN,
            G_OPTION_ARG_CALLBACK, show_version, "Show version and exit", NULL },
        { NULL }
    };

    group = g_option_group_new ("md-app", "", "", app, NULL);
    g_option_group_add_entries (group, entries);

    g_option_context_add_group (ctx, group);
}


void
md_app_set_need_display (MdApp    *app,
                         gboolean  need_display)
{
    g_return_if_fail (MD_IS_APP (app));
    app->priv->need_display = need_display;
    if (!need_display)
        app->priv->need_ui = FALSE;
}

void
md_app_set_need_ui (MdApp    *app,
                    gboolean  need_ui)
{
    g_return_if_fail (MD_IS_APP (app));
    app->priv->need_ui = need_ui;
    if (need_ui)
        app->priv->need_display = TRUE;
}

void
md_app_set_need_run (MdApp    *app,
                     gboolean  need_run)
{
    g_return_if_fail (MD_IS_APP (app));
    app->priv->need_run = need_run;
}

int
md_app_run (MdApp  *app,
            int     argc,
            char  **argv)
{
    int retval;

    if (app->priv->need_run)
    {
        if (!MD_APP_GET_CLASS (app)->initialize (app))
        {
            g_object_unref (app);
            exit (EXIT_FAILURE);
        }

        retval = MD_APP_GET_CLASS (app)->run (app, argc, argv);
    }

    g_object_unref (app);
    exit (retval);
}


static gboolean
open_display (void)
{
    if (!gdk_display_get_default ())
    {
        GdkDisplay *display;
        const char *display_name;

        display_name = gdk_get_display_arg_name ();
        display = gdk_display_open (display_name);

        if (!display)
        {
            g_printerr ("Could not open display: %s",
                        display_name ? display_name : "");
            return FALSE;
        }

        gdk_display_manager_set_default_display (gdk_display_manager_get (),
                                                 display);
    }

    return TRUE;
}

static gboolean
md_app_handler_initialize (MdApp *app)
{
    if (app->priv->need_display)
    {
        if (!open_display ())
            return FALSE;

        _moo_stock_init ();
        gdk_set_program_class (get_full_name (app));
        gtk_window_set_default_icon_name (get_icon_name (app));
    }

    md_app_load_prefs (app);

//     md_app_data.input = _md_app_input_new (get_short_name (app),
//                                            NULL, TRUE);

    return TRUE;
}


// static MdFileInfo **
// parse_files_argv (int    argc,
//                   char **argv);


static gboolean
on_gtk_main_quit (MdApp *app)
{
    g_critical ("main loop ended unexpectedly");
    app->priv->gtk_quit_handler_id = 0;
    md_app_do_quit (app);
    return FALSE;
}

static gboolean
check_signal (void)
{
    if (signal_received)
    {
        g_printerr ("%s\n", g_strsignal (signal_received));

        if (md_app_data.instance)
            md_app_do_quit (md_app_data.instance);

        g_object_unref (md_app_data.instance);
        exit (EXIT_FAILURE);
    }

    return TRUE;
}

static int
md_app_handler_run (MdApp  *app,
                    int     argc,
                    char  **argv)
{
    g_return_val_if_fail (!app->priv->running, EXIT_FAILURE);

    app->priv->running = TRUE;

//     if (argc > 1)
//     {
//         MdFileInfo **files = parse_files_argv (argc - 1, argv + 1);
//         if (files && *files)
//             md_app_open_files (app, files, TIMESTAMP ());
//         md_file_info_array_free (files);
//     }

    if (app->priv->need_ui && !app->priv->main_window && app->priv->md_mgr)
        if (!md_manager_get_active_window (app->priv->md_mgr))
            _md_manager_action_new_window (app->priv->md_mgr);

    app->priv->gtk_quit_handler_id =
            gtk_quit_add (1, (GtkFunction) on_gtk_main_quit, app);

    _moo_timeout_add (100, (GSourceFunc) check_signal, NULL);

    gtk_main ();

    return app->priv->exit_code;
}

static void
md_app_handler_quit (MdApp *app)
{
    guint i;

    if (!app->priv->running)
        return;

    app->priv->running = FALSE;

    if (app->priv->md_mgr)
    {
        g_object_unref (app->priv->md_mgr);
        app->priv->md_mgr = NULL;
    }

    md_app_save_prefs (app);

    if (app->priv->gtk_quit_handler_id)
        gtk_quit_remove (app->priv->gtk_quit_handler_id);
    app->priv->gtk_quit_handler_id = 0;

    i = 0;
    while (gtk_main_level () && i < 1000)
    {
        gtk_main_quit ();
        i++;
    }

    md_app_cleanup ();
}

static void
md_app_do_quit (MdApp *app)
{
    g_signal_emit (app, signals[QUIT], 0);
}

static gboolean
md_app_quit (MdApp *app)
{
    if (!app->priv->running || app->priv->handling_quit)
        return TRUE;

    app->priv->handling_quit = TRUE;

    if (app->priv->main_window)
    {
        if (!moo_window_close (app->priv->main_window))
            return FALSE;
        g_assert (app->priv->main_window == NULL);
    }

    g_signal_emit (app, signals[QUIT_REQUESTED], 0);

    if (!app->priv->delay_quit_count && app->priv->running)
        md_app_do_quit (app);

    return !app->priv->running;
}

void
md_app_delay_quit (MdApp *app)
{
    g_return_if_fail (MD_IS_APP (app));
    g_return_if_fail (app->priv->handling_quit);
    app->priv->delay_quit_count++;
}

void
md_app_resume_quit (MdApp *app)
{
    g_return_if_fail (MD_IS_APP (app));
    g_return_if_fail (app->priv->handling_quit);
    g_return_if_fail (app->priv->delay_quit_count);

    app->priv->delay_quit_count--;

    if (!app->priv->delay_quit_count)
        md_app_do_quit (app);
}

void
md_app_cancel_quit (MdApp *app)
{
    g_return_if_fail (MD_IS_APP (app));
    g_return_if_fail (app->priv->handling_quit);

    app->priv->delay_quit_count = 0;
    app->priv->handling_quit = FALSE;

    g_signal_emit (app, signals[QUIT_CANCELLED], 0);
}


static void
md_app_install_cleanup (void)
{
    if (!md_app_data.atexit_installed)
    {
        md_app_data.atexit_installed = TRUE;
        atexit (md_app_cleanup);
    }
}

static void
md_app_cleanup (void)
{
//     if (md_app_data.input)
//     {
//         _md_app_input_free (md_app_data.input);
//         md_app_data.input = NULL;
//     }

// #ifdef MOO_USE_XDGMIME
//     xdg_mime_shutdown ();
// #endif

    moo_cleanup ();
}


#ifndef __WIN32__
static void
find_user_rc_files (MdApp *app)
{
    char *cache_dir;
    char *rc_file;

    cache_dir = g_strdup_printf ("%s/%s", g_get_user_cache_dir (), g_get_prgname ());
    rc_file = g_strdup_printf ("%s/" TMPL_RC_FILE, g_get_user_config_dir (), g_get_prgname ());

    app->priv->rc_files[MOO_PREFS_RC] = rc_file;

    if (!g_file_test (g_get_user_config_dir (), G_FILE_TEST_EXISTS))
        _moo_mkdir_with_parents (g_get_user_config_dir ());

    if (!g_file_test (cache_dir, G_FILE_TEST_EXISTS))
        _moo_mkdir_with_parents (cache_dir);

//     if (app->priv->instance_name)
//         app->priv->rc_files[MOO_PREFS_STATE] =
//             g_strdup_printf ("%s/" TMPL_STATE_FILE ".%s",
//                              cache_dir,
//                              g_get_prgname (),
//                              app->priv->instance_name);
//     else
        app->priv->rc_files[MOO_PREFS_STATE] =
            g_strdup_printf ("%s/" TMPL_STATE_FILE,
                             cache_dir,
                             g_get_prgname ());

    g_free (cache_dir);
}
#endif

static char **
find_sys_rc_files (void)
{
    char *prefix;
    char *var;
    const char *value;
    char **files = NULL;

    prefix = g_ascii_strup (g_get_prgname (), -1);
    var = g_strdup_printf ("%s_RC_FILES", prefix);
    value = g_getenv (var);

    if (value)
    {
        files = g_strsplit (value, G_SEARCHPATH_SEPARATOR_S, 0);
    }
    else
    {
        char *tmpl = g_strdup_printf (TMPL_RC_FILE, g_get_prgname ());
        files = moo_get_data_files (tmpl, MOO_DATA_SHARE, NULL);
        g_free (tmpl);
    }

    g_free (var);
    g_free (prefix);
    return files;
}

static void
md_app_load_prefs (MdApp *app)
{
    GError *error = NULL;
    char **sys_files;

#ifndef __WIN32__
    find_user_rc_files (app);
#else
    app->priv->rc_files[MOO_PREFS_RC] =
        g_strdup_printf ("%s/" TMPL_RC_FILE, g_get_user_config_dir (), g_get_prgname ());
    app->priv->rc_files[MOO_PREFS_STATE] =
        g_strdup_printf ("%s/" TMPL_STATE_FILE, g_get_user_config_dir (), g_get_prgname ());
#endif

    sys_files = find_sys_rc_files ();

    if (!moo_prefs_load (sys_files,
                         app->priv->rc_files[MOO_PREFS_RC],
                         app->priv->rc_files[MOO_PREFS_STATE],
                         &error))
    {
        g_warning ("%s: could not read config files", G_STRLOC);

        if (error)
        {
            g_warning ("%s: %s", G_STRLOC, error->message);
            g_error_free (error);
        }
    }

    g_strfreev (sys_files);
}

static void
md_app_save_prefs (MdApp *app)
{
    GError *error = NULL;

    if (!moo_prefs_save (app->priv->rc_files[MOO_PREFS_RC],
                         app->priv->rc_files[MOO_PREFS_STATE],
                         &error))
    {
        g_warning ("%s: could not save config files", G_STRLOC);

        if (error)
        {
            g_warning ("%s: %s", G_STRLOC, error->message);
            g_error_free (error);
        }
    }
}
