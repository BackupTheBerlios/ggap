#include <mooui/mdmanager.h>
#include <gtk/gtk.h>

int main (int argc, char *argv[])
{
    MdManager *mgr;

    gtk_init (&argc, &argv);

    moo_window_class_set_id (g_type_class_ref (MD_TYPE_WINDOW), "Worksheet", "Worksheet");

    mgr = g_object_new (MD_TYPE_MANAGER, NULL);
    _md_manager_action_new_window (mgr);

//     window = g_object_new (MD_TYPE_WINDOW, NULL);
//     gtk_widget_show (window);
//     g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    gtk_main ();

    return 0;
}
