/*
 *   @(#)$Id: gapeditwindow.c,v 1.1 2005/04/22 08:49:55 emuntyan Exp $
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

#include "gap/gapapp.h"
#include "gap/gapeditwindow.h"
#include "gap/gapui.h"
#include "gap/gapprefsdialog.h"
#include "gui/stock.h"
#include "gui/about.h"
#include "mooedit/mooeditwindow.h"
#include "mooui/moouiobject.h"


static void     gap_edit_window_class_init  (GapEditWindowClass *klass);

static void     gap_edit_window_init        (GapEditWindow     *window);
static void     gap_edit_window_init_actions(GapEditWindow     *window);

static void     gap_edit_window_send_to_gap (GapEditWindow     *window);


/* GAP_TYPE_EDIT_WINDOW */
G_DEFINE_TYPE (GapEditWindow, gap_edit_window, MOO_TYPE_EDIT_WINDOW)


static void gap_edit_window_class_init (GapEditWindowClass *klass)
{
}


static void gap_edit_window_init (GapEditWindow *window)
{
    MooUIXML *xml;
    GError *error = NULL;

    moo_edit_window_set_app_name (MOO_EDIT_WINDOW (window), "GGAP");
    moo_window_set_icon_from_stock  (MOO_WINDOW (window), GGAP_STOCK_APP);

    g_object_set (G_OBJECT (window),
                  "menubar-ui-name", "EditorWindow/Menubar",
                  "toolbar-ui-name", "EditorWindow/Toolbar",
                  "name", "Editor Window",
                  "id", "EditorWindow",
                  NULL);

    xml = moo_ui_object_get_ui_xml (MOO_UI_OBJECT (window));
    moo_ui_xml_add_ui_from_string (xml, GAP_UI, -1, &error);
    if (error) {
        g_error ("%s", error->message);
        g_error_free (error);
    }

    gap_edit_window_init_actions (window);
}


static void     gap_edit_window_init_actions(GapEditWindow  *window)
{
    moo_window_new_action (window, "SendToGAP",
        "name", "Send to GAP",
        "label", "Send to _GAP",
        "tooltip", "Send selected text to GAP console",
        "icon-stock-id", GTK_STOCK_GOTO_BOTTOM,
        "accel", "<ctrl>Return",
        "closure", moo_closure_new_object (G_CALLBACK (gap_edit_window_send_to_gap),
                                           window),
        NULL);

    moo_window_new_action (window, "Preferences",
        "name", "Preferences",
        "label", "Pre_ferences",
        "tooltip", "Preferences",
        "icon-stock-id", GTK_STOCK_PREFERENCES,
        "accel", "<ctrl>P",
        "closure", moo_closure_new_object (G_CALLBACK (gap_prefs_dialog),
                                           window),
        NULL);

    moo_window_new_action (window, "About",
        "name", "About GGAP",
        "label", "_About GGAP",
        "tooltip", "About GGAP",
        "icon-stock-id", GTK_STOCK_ABOUT,
        "accel", "",
        "closure", moo_closure_new_object (G_CALLBACK (ggap_about_dialog),
                                           window),
        NULL);
}


GtkWidget*  gap_edit_window_new            ()
{
    return GTK_WIDGET (g_object_new (GAP_TYPE_EDIT_WINDOW, NULL));
}


static void gap_edit_window_send_to_gap           (GapEditWindow   *window)
{
    MooEdit *edit;
    char *selection;

    edit = moo_edit_window_get_current_tab (MOO_EDIT_WINDOW (window));
    g_return_if_fail (edit != NULL);
    selection = moo_edit_get_selected_text (edit);
    g_return_if_fail (selection != NULL);
    if (selection[0])
        gap_app_feed_gap (gap_app(), selection, -1);
    else {
        char *text = moo_edit_get_text (edit);
        gap_app_feed_gap (gap_app(), text, -1);
        g_free (text);
    }
    g_free (selection);
}
