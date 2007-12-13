/*
 *   gapwswindow.c
 *
 *   Copyright (C) 2004-2007 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "gapwswindow.h"
#include "gapworksheet.h"
#include "gapapp.h"
#include "ggap-i18n.h"
#include "mooutils/moodialogs.h"
#include "mooutils/moofiledialog.h"
#include <gtk/gtk.h>

struct GapWsWindowPrivate {
    GtkStatusbar *statusbar;
//     GapWorksheet *ws;
};

G_DEFINE_TYPE (GapWsWindow, gap_ws_window, MD_TYPE_WINDOW)

static GObject *gap_ws_window_constructor   (GType   type,
                                             guint   n_props,
                                             GObjectConstructParam *props);

static void     action_open_worksheet       (GapWsWindow    *window);
static void     action_save_worksheet       (GapWsWindow    *window);
static void     action_save_worksheet_as    (GapWsWindow    *window);

static void
gap_ws_window_class_init (GapWsWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
//     MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);

    object_class->constructor = gap_ws_window_constructor;

    moo_window_class_set_id (MOO_WINDOW_CLASS (klass), "Worksheet", "Worksheet");
    g_type_class_add_private (klass, sizeof (GapWsWindowPrivate));

//     moo_window_class_new_action (window_class, "OpenWorksheet", NULL,
//                                  "display-name", _("Open Worksheet"),
//                                  "label", _("_Open Worksheet..."),
//                                  "tooltip", _("Open worksheet..."),
//                                  "stock-id", GTK_STOCK_OPEN,
//                                  "accel", "<Ctrl>O",
//                                  "closure-callback", action_open_worksheet,
//                                  NULL);
//
//     moo_window_class_new_action (window_class, "SaveWorksheet", NULL,
//                                  "display-name", _("Save Worksheet"),
//                                  "label", _("_Save Worksheet"),
//                                  "tooltip", _("Save worksheet"),
//                                  "stock-id", GTK_STOCK_SAVE,
//                                  "accel", "<Ctrl>S",
//                                  "closure-callback", action_save_worksheet,
//                                  NULL);
//
//     moo_window_class_new_action (window_class, "SaveWorksheetAs", NULL,
//                                  "display-name", _("Save Worksheet As"),
//                                  "label", _("_Save Worksheet As..."),
//                                  "tooltip", _("Save worksheet as..."),
//                                  "stock-id", GTK_STOCK_SAVE_AS,
//                                  "accel", "<Shift><Ctrl>S",
//                                  "closure-callback", action_save_worksheet_as,
//                                  NULL);

//     moo_window_class_new_action (window_class, "Cut", NULL,
//                                  "display-name", GTK_STOCK_CUT,
//                                  "label", GTK_STOCK_CUT,
//                                  "tooltip", GTK_STOCK_CUT,
//                                  "stock-id", GTK_STOCK_COPY,
//                                  "accel", "<Ctrl>C",
//                                  "closure-callback", copy_clipboard,
//                                  "closure-proxy-func", gap_term_window_get_terminal,
//                                  NULL);
//
//     moo_window_class_new_action (window_class, "Copy", NULL,
//                                  "display-name", GTK_STOCK_COPY,
//                                  "label", GTK_STOCK_COPY,
//                                  "tooltip", GTK_STOCK_COPY,
//                                  "stock-id", GTK_STOCK_COPY,
//                                  "accel", "<alt>C",
//                                  "closure-callback", copy_clipboard,
//                                  "closure-proxy-func", gap_term_window_get_terminal,
//                                  NULL);
//
//     moo_window_class_new_action (window_class, "Paste", NULL,
//                                  "display-name", GTK_STOCK_PASTE,
//                                  "label", GTK_STOCK_PASTE,
//                                  "tooltip", GTK_STOCK_PASTE,
//                                  "stock-id", GTK_STOCK_PASTE,
//                                  "accel", "<alt>V",
//                                  "closure-callback", paste_clipboard,
//                                  "closure-proxy-func", gap_term_window_get_terminal,
//                                  NULL);
//
//     moo_window_class_new_action (window_class, "SelectAll", NULL,
//                                  "display-name", GTK_STOCK_SELECT_ALL,
//                                  "label", GTK_STOCK_SELECT_ALL,
//                                  "tooltip", GTK_STOCK_SELECT_ALL,
//                                  "accel", "<alt>A",
//                                  "closure-callback", moo_term_select_all,
//                                  "closure-proxy-func", gap_term_window_get_terminal,
//                                  NULL);
}


static void
gap_ws_window_init (GapWsWindow *window)
{
    window->priv = G_TYPE_INSTANCE_GET_PRIVATE (window, GAP_TYPE_WS_WINDOW, GapWsWindowPrivate);

    g_object_set (G_OBJECT (window),
                  "menubar-ui-name", "Worksheet/Menubar",
                  "toolbar-ui-name", "Worksheet/Toolbar",
                  NULL);

    moo_window_set_global_accels (MOO_WINDOW (window), FALSE);
}


static void
set_statusbar_text (GapWsWindow *window,
                    const char  *text)
{
    gtk_statusbar_pop (window->priv->statusbar, 0);
    if (text && text[0])
        gtk_statusbar_push (window->priv->statusbar, 0, text);
}

static void
gap_state_changed (GapWorksheet *ws,
                   G_GNUC_UNUSED GParamSpec *pspec,
                   GapWsWindow  *window)
{
    GapState state;

    g_object_get (ws, "gap-state", &state, NULL);

    switch (state)
    {
        case GAP_BUSY:
        case GAP_BUSY_INTERNAL:
            set_statusbar_text (window, "Busy");
            break;

        case GAP_DEAD:
        case GAP_IN_PROMPT:
            set_statusbar_text (window, NULL);
            break;

        case GAP_LOADING:
            set_statusbar_text (window, "Loading");
            break;
    }
}

// static void
// filename_changed (GapWorksheet *ws,
//                   G_GNUC_UNUSED GParamSpec *pspec,
//                   GapWsWindow  *window)
// {
//     const char *filename;
//     char *display_name;
//     char *title;
//
//     filename = gap_worksheet_get_filename (ws);
//     display_name = filename ? g_filename_display_name (filename) : g_strdup ("Untitled");
//
//     if (gap_worksheet_get_modified (ws))
//         title = g_strdup_printf ("ggap - %s [modified]", display_name);
//     else
//         title = g_strdup_printf ("ggap - %s", display_name);
//
//     gtk_window_set_title (GTK_WINDOW (window), title);
//
//     g_free (title);
//     g_free (display_name);
// }

static GObject *
gap_ws_window_constructor (GType type,
                           guint n_props,
                           GObjectConstructParam *props)
{
    GapWsWindow *window;
    GtkWidget *statusbar;
//     GtkWidget *swin;
//     GapWorksheet *ws;

    GObject *object = G_OBJECT_CLASS(gap_ws_window_parent_class)->constructor (type, n_props, props);
    window = GAP_WS_WINDOW (object);

//     swin = gtk_scrolled_window_new (NULL, NULL);
//     gtk_box_pack_start (GTK_BOX (MOO_WINDOW(window)->vbox), swin, TRUE, TRUE, 0);
//     gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),
//                                     GTK_POLICY_AUTOMATIC,
//                                     GTK_POLICY_ALWAYS);
//     gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (swin),
//                                          GTK_SHADOW_ETCHED_OUT);

//     window->priv->ws = ws = g_object_new (GAP_TYPE_WORKSHEET, NULL);

//     gtk_container_add (GTK_CONTAINER (swin), GTK_WIDGET (ws));
//     GTK_WIDGET_SET_FLAGS (GTK_WIDGET (ws), GTK_CAN_FOCUS);
//     GTK_WIDGET_SET_FLAGS (GTK_WIDGET (ws), GTK_CAN_DEFAULT);
//     gtk_widget_grab_focus (GTK_WIDGET (ws));
//     gtk_widget_grab_default (GTK_WIDGET (ws));

    statusbar = gtk_statusbar_new ();
    gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (statusbar), TRUE);
    gtk_box_pack_start (GTK_BOX (MOO_WINDOW (window)->vbox), statusbar, FALSE, FALSE, 0);
    window->priv->statusbar = GTK_STATUSBAR (statusbar);

    gtk_widget_show_all (MOO_WINDOW(window)->vbox);

//     g_signal_connect (ws, "notify::gap-state",
//                       G_CALLBACK (gap_state_changed),
//                       window);
//     g_signal_connect (ws, "notify::filename",
//                       G_CALLBACK (filename_changed),
//                       window);
//     g_signal_connect (ws, "notify::modified",
//                       G_CALLBACK (filename_changed),
//                       window);
//
//     filename_changed (ws, NULL, window);
//     gap_state_changed (ws, NULL, window);

    return object;
}


#define FILENAME "/tmp/ggap-file.gws"
#define BINFILENAME "/tmp/ggap-file-binary"

// static gboolean
// save_worksheet (GapWsWindow  *window,
//                 GapWorksheet *ws,
//                 const char   *filename)
// {
//     GError *error = NULL;
//
//     if (!filename)
//         filename = moo_file_dialogp (GTK_WIDGET (window),
//                                      MOO_FILE_DIALOG_SAVE,
//                                      "untitled.gws",
//                                      "Choose Filename",
//                                      GGAP_PREFS_PREFIX "/worksheet",
//                                      NULL);
//
//     if (!filename)
//         return FALSE;
//
//     if (!gap_worksheet_save (ws, filename, TRUE, &error))
//     {
//         char *text = g_strdup_printf ("Could not save file %s", filename);
//         moo_error_dialog (GTK_WIDGET (window), text, error ? error->message : "failed");
//         g_free (text);
//         g_error_free (error);
//         return FALSE;
//     }
//
//     return TRUE;
// }

// static gboolean
// ask_close_worksheet (GapWsWindow  *window,
//                      GapWorksheet *ws)
// {
//     MooSaveChangesDialogResponse response;
//     const char *filename;
//     char *display_name;
//
//     if (!gap_worksheet_get_modified (ws))
//         return TRUE;
//
//     filename = gap_worksheet_get_filename (ws);
//     display_name = filename ? g_filename_display_basename (filename) : NULL;
//     response = moo_save_changes_dialog (display_name, GTK_WIDGET (window));
//     g_free (display_name);
//
//     if (response == MOO_SAVE_CHANGES_RESPONSE_SAVE)
//         return save_worksheet (window, ws, filename);
//     else if (response == MOO_SAVE_CHANGES_RESPONSE_CANCEL)
//         return FALSE;
//     else
//         return TRUE;
// }

// static void
// set_worksheet (GapWsWindow  *window,
//                GapWorksheet *ws)
// {
//     GtkWidget *swin;
//
//     g_signal_handlers_disconnect_by_func (window->priv->ws, (gpointer) gap_state_changed, window);
//     g_signal_handlers_disconnect_by_func (window->priv->ws, (gpointer) filename_changed, window);
//
//     swin = GTK_WIDGET (window->priv->ws)->parent;
//     gtk_container_remove (GTK_CONTAINER (swin), GTK_WIDGET (window->priv->ws));
//     gtk_container_add (GTK_CONTAINER (swin), GTK_WIDGET (ws));
//     window->priv->ws = ws;
//
//     g_signal_connect (ws, "notify::gap-state",
//                       G_CALLBACK (gap_state_changed),
//                       window);
//     g_signal_connect (ws, "notify::filename",
//                       G_CALLBACK (filename_changed),
//                       window);
//     g_signal_connect (ws, "notify::modified",
//                       G_CALLBACK (filename_changed),
//                       window);
//
//     gtk_widget_grab_focus (GTK_WIDGET (ws));
//     gtk_widget_show (GTK_WIDGET (ws));
//     filename_changed (ws, NULL, window);
// }

// static void
// action_open_worksheet (GapWsWindow *window)
// {
//     const char *filename;
//     gboolean do_close;
//     GError *error = NULL;
//
//     if (!gap_worksheet_get_empty (window->priv->ws))
//     {
//         if (!ask_close_worksheet (window, window->priv->ws))
//             return;
//         do_close = TRUE;
//     }
//
//     filename = moo_file_dialogp (GTK_WIDGET (window),
//                                  MOO_FILE_DIALOG_OPEN,
//                                  NULL,
//                                  "Choose Worksheet",
//                                  GGAP_PREFS_PREFIX "/worksheet",
//                                  NULL);
//
//     if (!filename)
//         return;
//
//     if (do_close)
//     {
//         GapWorksheet *ws = g_object_new (GAP_TYPE_WORKSHEET, NULL);
//         set_worksheet (window, ws);
//     }
//
//     if (!gap_worksheet_load (window->priv->ws, filename, &error))
//     {
//         char *text = g_strdup_printf ("Could not open file %s", filename);
//         moo_error_dialog (GTK_WIDGET (window), text, error ? error->message : "failed");
//         g_free (text);
//     }
// }

// static void
// action_save_worksheet (GapWsWindow *window)
// {
//     save_worksheet (window, window->priv->ws,
//                     gap_worksheet_get_filename (window->priv->ws));
// }
//
// static void
// action_save_worksheet_as (GapWsWindow *window)
// {
//     save_worksheet (window, window->priv->ws, NULL);
// }
