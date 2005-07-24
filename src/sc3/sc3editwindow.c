/*
 *   @(#)$Id: sc3editwindow.c,v 1.2 2005/05/29 01:52:39 emuntyan Exp $
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

#include "sc3/sc3app.h"
#include "sc3/sc3editwindow.h"
#include "sc3/sc3ui.h"
#include "sc3/sc3prefsdialog.h"
#include "gui/stock.h"
#include "gui/about.h"
#include "mooedit/mooeditwindow.h"
#include "mooui/moouiobject.h"


static void     sc3_edit_window_class_init  (Sc3EditWindowClass *klass);

static void     sc3_edit_window_init        (Sc3EditWindow     *window);
static void     sc3_edit_window_init_actions(Sc3EditWindow     *window);

static void     sc3_edit_window_send_to_sclang (Sc3EditWindow     *window);


/* SC3_TYPE_EDIT_WINDOW */
G_DEFINE_TYPE (Sc3EditWindow, sc3_edit_window, MOO_TYPE_EDIT_WINDOW)


static void sc3_edit_window_class_init (Sc3EditWindowClass *klass)
{
}


static void sc3_edit_window_init (Sc3EditWindow *window)
{
    MooUIXML *xml;
    GError *error = NULL;

    moo_edit_window_set_app_name (MOO_EDIT_WINDOW (window), "Sclang");
    moo_window_set_icon_from_stock  (MOO_WINDOW (window), GGAP_STOCK_APP);

    g_object_set (G_OBJECT (window),
                  "menubar-ui-name", "EditorWindow/Menubar",
                  "toolbar-ui-name", "EditorWindow/Toolbar",
                  "name", "Editor Window",
                  "id", "EditorWindow",
                  NULL);

    xml = moo_ui_object_get_ui_xml (MOO_UI_OBJECT (window));
    moo_ui_xml_add_ui_from_string (xml, SC3_UI, -1, &error);
    if (error) {
        g_error ("%s", error->message);
        g_error_free (error);
    }

    sc3_edit_window_init_actions (window);
}


static void     sc3_edit_window_init_actions(Sc3EditWindow  *window)
{
    moo_window_new_action (window, "SendToSclang",
        "name", "Send to Sclang",
        "label", "Send to _Sclang",
        "tooltip", "Send selected text to sclang console",
        "icon-stock-id", GTK_STOCK_GOTO_BOTTOM,
        "accel", "<ctrl>Return",
        "closure", moo_closure_new_object (G_CALLBACK (sc3_edit_window_send_to_sclang),
                                           window),
        NULL);

    moo_window_new_action (window, "Preferences",
        "name", "Preferences",
        "label", "Pre_ferences",
        "tooltip", "Preferences",
        "icon-stock-id", GTK_STOCK_PREFERENCES,
        "accel", "<ctrl>P",
        "closure", moo_closure_new_object (G_CALLBACK (sc3_prefs_dialog),
                                           window),
        NULL);

    moo_window_new_action (window, "About",
        "name", "About Sclang",
        "label", "_About Sclang",
        "tooltip", "About Sclang",
        "icon-stock-id", GTK_STOCK_ABOUT,
        "accel", "",
        "closure", moo_closure_new_object (G_CALLBACK (ggap_about_dialog),
                                           window),
        NULL);

    moo_window_new_action (window, "StopServer",
        "name", "Stop server",
        "label", "Stop _Server",
        "tooltip", "Stop server",
        "icon-stock-id", GTK_STOCK_STOP,
        "accel", "<ctrl>period",
        "closure", moo_closure_new_object (G_CALLBACK (sc3_app_stop_server), sc3_app()),
        NULL);

    moo_window_new_action (window, "RunSelected",
        "name", "Run",
        "label", "Run",
        "tooltip", "Run",
        "icon-stock-id", GTK_STOCK_GOTO_BOTTOM,
        "accel", "<ctrl>Return",
        "closure", moo_closure_new_object (G_CALLBACK (sc3_edit_window_send_to_sclang), window),
        NULL);

    moo_window_new_action (window, "EvaluateAll",
        "name", "Evaluate all",
        "label", "Evaluate All",
        "tooltip", "Evaluate all",
        "icon-stock-id", GTK_STOCK_EXECUTE,
        "accel", "<ctrl>bracketleft",
        "closure", moo_closure_new_object (G_CALLBACK (sc3_app_eval_all), sc3_app()),
        NULL);
}


GtkWidget*  sc3_edit_window_new            ()
{
    return GTK_WIDGET (g_object_new (SC3_TYPE_EDIT_WINDOW, NULL));
}


static void sc3_edit_window_send_to_sclang           (Sc3EditWindow   *window)
{
    MooEdit *edit;
    char *selection;

    edit = moo_edit_window_get_current_tab (MOO_EDIT_WINDOW (window));
    g_return_if_fail (edit != NULL);
    selection = moo_edit_get_selected_text (edit);
    g_return_if_fail (selection != NULL);
    if (selection && selection[0]) {
        sc3_app_feed_sclang (sc3_app(), selection, -1);
    }
    sc3_app_feed_sclang (sc3_app(), "\14\n", 2);
    g_free (selection);
}
