/*
 *   @(#)$Id: sc3termwindow.c,v 1.7 2005/06/06 14:45:55 emuntyan Exp $
 *
 *   Copyright (C) 2004-2005 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sc3/sc3termwindow.h"
#include "sc3/sc3app.h"
#include "sc3/sc3ui.h"
#include "sc3/sc3prefsdialog.h"
#include "sc3/sc3prefs.h"
#include "mooutils/moostock.h"
#include "mooapp/mooappabout.h"
#include "mooutils/moodialogs.h"
#include "mooui/moouiobject-impl.h"
#include "mooedit/mooeditprefs.h"
#include "mooutils/moocompat.h"
#include "mooutils/moowin.h"


static void         sc3_term_window_class_init          (Sc3TermWindowClass *klass);
static void         sc3_term_window_init                (Sc3TermWindow     *window);
GObject            *sc3_term_window_constructor         (GType                  type,
                                                         guint                  n_props,
                                                         GObjectConstructParam *props);
static void         sc3_term_window_finalize            (GObject        *window);

static gboolean     sc3_term_window_close               (Sc3TermWindow *window);
static void         sc3_term_window_new_editor_window   (Sc3TermWindow *window);
static void         sc3_term_window_open_in_editor      (Sc3TermWindow *window);
static void         sc3_term_window_python_execute_file (Sc3TermWindow *window);

/* SC3_TYPE_TERM_WINDOW */
G_DEFINE_TYPE (Sc3TermWindow, sc3_term_window, MOO_TYPE_TERM_WINDOW)


static void sc3_term_window_class_init (Sc3TermWindowClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);

    gobject_class->constructor = sc3_term_window_constructor;
    gobject_class->finalize = sc3_term_window_finalize;
    window_class->close = (gboolean (*) (MooWindow*))sc3_term_window_close;

    moo_ui_object_class_init (gobject_class, "Terminal", "Terminal");

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "NewEditor",
                                    "name", "New Editor",
                                    "label", "_New Editor",
                                    "tooltip", "Open new editor window",
                                    "icon-stock-id", GTK_STOCK_EDIT,
                                    "accel", "<alt>N",
                                    "closure::callback", sc3_term_window_new_editor_window,
                                    NULL);

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "OpenInEditor",
                                    "name", "Open in Editor",
                                    "label", "_Open in Editor",
                                    "tooltip", "Open document in new editor window",
                                    "icon-stock-id", GTK_STOCK_OPEN,
                                    "accel", "<alt>O",
                                    "closure::callback", sc3_term_window_open_in_editor,
                                    NULL);

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "Quit",
                                    "name", "Quit",
                                    "label", "_Quit",
                                    "tooltip", "Quit",
                                    "icon-stock-id", GTK_STOCK_QUIT,
                                    "accel", "<alt>Q",
                                    "closure::callback", sc3_term_window_close,
                                    NULL);

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "Preferences",
                                    "name", "Preferences",
                                    "label", "_Preferences",
                                    "tooltip", "Open GGAP Preferences dialog",
                                    "accel", "<alt>P",
                                    "icon-stock-id", GTK_STOCK_PREFERENCES,
                                    "closure::callback", sc3_prefs_dialog,
                                    NULL);

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "Restart",
                                    "name", "Restart",
                                    "label", "_Restart",
                                    "tooltip", "Restart GAP",
                                    "accel", "<alt>R",
                                    "icon-stock-id", GTK_STOCK_REFRESH,
                                    "closure::callback", sc3_app_restart_sclang,
                                    "closure::proxy-func", moo_app_get_instance,
                                    NULL);

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "About",
                                    "name", "About GGAP",
                                    "label", "_About GGAP",
                                    "tooltip", "About GGAP",
                                    "accel", "",
                                    "icon-stock-id", GTK_STOCK_ABOUT,
                                    "closure::callback", moo_app_about_dialog,
                                    NULL);

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "StopServer",
                                    "name", "Stop server",
                                    "label", "Stop Server",
                                    "tooltip", "Stop server",
                                    "icon-stock-id", GTK_STOCK_STOP,
                                    "accel", "<ctrl>period",
                                    "closure::callback", sc3_app_stop_server,
                                    "closure::proxy-func", sc3_app,
                                    NULL);

    /************************************************************************/
    /* Python actions                                                       */
    /***/

#ifdef USE_PYTHON
    moo_ui_object_class_new_action (gobject_class,
                                    "id", "PythonMenu",
                                    "name", "Python Menu",
                                    "label", "P_ython",
                                    "visible", TRUE,
                                    "no-accel", TRUE,
                                    NULL);
#else /* !USE_PYTHON */
    moo_ui_object_class_new_action (gobject_class,
                                    "id", "PythonMenu",
                                    "dead", TRUE,
                                    NULL);
#endif /* !USE_PYTHON */

    /* this one can be compiled in since it defined and does nothing when !USE_PYTHON */
    moo_ui_object_class_new_action (gobject_class,
                                    "id", "ExecuteScript",
                                    "name", "Execute Script",
                                    "label", "_Execute Script",
                                    "tooltip", "Execute Script",
                                    "accel", "",
                                    "icon-stock-id", GTK_STOCK_EXECUTE,
                                    "closure::callback", sc3_term_window_python_execute_file,
                                    NULL);

#ifdef USE_PYTHON
    moo_ui_object_class_new_action (gobject_class,
                                    "id", "ShowConsole",
                                    "name", "Show Console",
                                    "label", "Show Conso_le",
                                    "tooltip", "Show Console",
                                    "accel", "<alt>L",
                                    "closure::callback", moo_app_show_python_console,
                                    "closure::proxy-func", moo_app_get_instance,
                                    NULL);
#endif /* USE_PYTHON */
}


static void sc3_term_window_init (G_GNUC_UNUSED Sc3TermWindow *window)
{
}


GObject            *sc3_term_window_constructor         (GType                  type,
                                                         guint                  n_props,
                                                         GObjectConstructParam *props)
{
    MooUIXML *xml;
    GError *error = NULL;
    GtkWidget *popup;
    Sc3TermWindow *window;

    GObject *object =
            G_OBJECT_CLASS(sc3_term_window_parent_class)->constructor (type, n_props, props);

    window = SC3_TERM_WINDOW (object);

    gtk_window_set_title (GTK_WINDOW (window), "Sclang");
    moo_window_set_icon_from_stock  (GTK_WINDOW (window), MOO_STOCK_APP);

    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    xml = moo_ui_object_get_ui_xml (MOO_UI_OBJECT (window));
    moo_ui_xml_add_ui_from_string (xml, SC3_UI, -1, &error);
    if (error) {
        g_error ("%s", error->message);
        g_error_free (error);
    }

    sc3_term_window_apply_settings (window);
    popup = moo_ui_xml_create_widget (xml, "Terminal/Popup",
                                      moo_ui_object_get_actions (MOO_UI_OBJECT (window)),
                                      MOO_WINDOW (window)->accel_group,
                                      MOO_WINDOW (window)->tooltips);
    if (popup) moo_term_set_popup_menu (MOO_TERM_WINDOW (window)->terminal, popup);

    return object;
}


void        sc3_term_window_apply_settings  (Sc3TermWindow     *window)
{
    g_return_if_fail (SC3_IS_TERM_WINDOW (window));
    g_signal_emit_by_name (MOO_TERM_WINDOW(window)->terminal, "apply_settings", NULL);
}


static void sc3_term_window_finalize (GObject *object)
{
    G_OBJECT_CLASS (sc3_term_window_parent_class)->finalize (object);
}


Sc3TermWindow *sc3_term_window_new (void)
{
    Sc3TermWindow *win = SC3_TERM_WINDOW (g_object_new (SC3_TYPE_TERM_WINDOW, NULL));
    gtk_widget_show (GTK_WIDGET (win));
    return win;
}


static gboolean sc3_term_window_close (G_GNUC_UNUSED Sc3TermWindow *window)
{
    return moo_app_quit (moo_app_get_instance ());
}


/****************************************************************************/


static void sc3_term_window_new_editor_window (G_GNUC_UNUSED Sc3TermWindow *window)
{
    moo_editor_new_window (moo_app_get_editor (moo_app_get_instance ()));
}

static void sc3_term_window_open_in_editor (G_GNUC_UNUSED Sc3TermWindow *self)
{
    moo_editor_open (moo_app_get_editor (moo_app_get_instance ()),
                     NULL, NULL, NULL);
}


static void sc3_term_window_python_execute_file (Sc3TermWindow *self)
{
    moo_app_python_execute_file (moo_app_get_instance (), GTK_WINDOW (self), NULL);
}

