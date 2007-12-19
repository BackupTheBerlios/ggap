#include <mooui/mdapp.h>
#include <gtk/gtk.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    MdApp *app;
    GError *error = NULL;
    GOptionContext *context;

    g_type_init ();

    if (!(app = g_object_new (MD_TYPE_APP, NULL)))
        return EXIT_FAILURE;

    context = g_option_context_new (NULL);
    md_app_setup_option_context (app, context);
    g_option_context_add_group (context, gtk_get_option_group (FALSE));

    if (!g_option_context_parse (context, &argc, &argv, &error))
    {
        g_printerr ("%s\n", error->message);
        exit (EXIT_FAILURE);
    }

    md_app_run (app, argc, argv);

    /* never reached */
    return EXIT_SUCCESS;
}
