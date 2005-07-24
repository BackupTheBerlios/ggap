s/#include <sys\/types.h>//
s/#include <sys\/stat.h>//
s/#include <string.h>//
s/#include <stdio.h>//
s/#include <unistd.h>/#include "mooutils\/moocompat.h"/
s/#include "gapprefs.h"/#include "mooutils\/mooprefsdialog.h"/
s/#include "callbacks.h"/#include "gap\/gapprefs.h"/
s/create_window (void)/_ggap_create_gap_prefs_page (GtkWidget *page);\nGtkWidget *_ggap_create_gap_prefs_page (GtkWidget *page)/
s/GtkWidget \*window;//
s/GtkWidget \*page;//
s/window = gtk_window_new (GTK_WINDOW_TOPLEVEL);//
s/gtk_window_set_title (GTK_WINDOW (window), _("window"));//
s/page = gtk_vbox_new (FALSE, 0);//
s/gtk_widget_show (page);//
s/gtk_container_add (GTK_CONTAINER (window), page);//
s/GLADE_HOOKUP_OBJECT_NO_REF (window, window, \"window\");//
s/GLADE_HOOKUP_OBJECT (window, page, "page");/GLADE_HOOKUP_OBJECT_NO_REF (page, page, "page");/
s/GLADE_HOOKUP_OBJECT (window, /GLADE_HOOKUP_OBJECT (page, /
s/GLADE_HOOKUP_OBJECT_NO_REF (window, tooltips, \"tooltips\");/GLADE_HOOKUP_OBJECT_NO_REF (page, tooltips, \"tooltips\");/
s/[ \t]*GLADE_HOOKUP_OBJECT (page, [a-z_]*[0-9][0-9]*,.*//
s/return window;/return page;/
s/g_signal_connect ((gpointer) \([a-z_]*\), "moo_prefs_key", G_CALLBACK (\([A-Z_]*\)), NULL);/moo_prefs_dialog_page_bind_setting (MOO_PREFS_DIALOG_PAGE (page), \1, \2, NULL);/
s/g_signal_connect ((gpointer) \([a-z_]*\), "moo_sensitive", G_CALLBACK (\([a-z_]*\)), NULL);/moo_bind_sensitive (GTK_TOGGLE_BUTTON (\2), \&\1, 1, FALSE);/
s/g_signal_connect_swapped ((gpointer) \([a-z_]*\), "moo_sensitive", G_CALLBACK (\([a-z_]*\)), GTK_OBJECT (invert));/moo_bind_sensitive (GTK_TOGGLE_BUTTON (\2), \&\1, 1, TRUE);/
