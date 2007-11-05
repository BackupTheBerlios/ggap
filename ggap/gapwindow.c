/*
 *   gapwindow.c
 *
 *   Copyright (C) 2004-2006 by Yevgen Muntyan <muntyan@math.tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "ggap-i18n.h"
#include "gapwindow.h"
#include "gapapp.h"
#include "gap.h"
#include "mooutils/moofiledialog.h"
#include "mooutils/moocompat.h"
#include "mooutils/mooutils-misc.h"
#include "mooapp/mooapp.h"
#include <gtk/gtk.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


struct _GapWindowPrivate {
    GType view_type;
};


static GObject *gap_window_constructor  (GType           type,
                                         guint           n_props,
                                         GObjectConstructParam *props);

static void     switch_to_editor        (void);
static void     gap_read_file           (GapWindow      *window);
static void     gap_open_workspace      (GapWindow      *window);


G_DEFINE_TYPE (GapWindow, gap_window, MOO_TYPE_WINDOW)


static void
gap_window_class_init (GapWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);

    object_class->constructor = gap_window_constructor;

    moo_window_class_set_id (window_class, "GAP", "GAP");

    moo_window_class_new_action (window_class, "SwitchToEditor", NULL,
                                 "display-name", _("Switch to Editor"),
                                 "label", _("Switch to Editor"),
                                 "tooltip", _("Switch to editor"),
                                 "stock-id", GTK_STOCK_EDIT,
                                 "closure-callback", switch_to_editor,
                                 NULL);

    moo_window_class_new_action (window_class, "GAPRead", NULL,
                                 "display-name", _("Read File"),
                                 "label", _("Read File"),
                                 "tooltip", _("Read file"),
                                 "stock-id", GTK_STOCK_OPEN,
                                 "closure-callback", gap_read_file,
                                 NULL);
    moo_window_class_new_action (window_class, "GAPOpenWorkspace", NULL,
                                 "display-name", _("Open Workspace"),
                                 "label", _("Open _Workspace"),
                                 "tooltip", _("Open workspace"),
                                 "stock-id", GTK_STOCK_OPEN,
                                 "closure-callback", gap_open_workspace,
                                 NULL);
}


static void
gap_window_init (GapWindow *window)
{
    g_object_set (G_OBJECT (window),
                  "menubar-ui-name", "GAP/Menubar",
                  "toolbar-ui-name", "GAP/Toolbar",
                  NULL);
}


static GObject *
gap_window_constructor (GType type,
                        guint n_props,
                        GObjectConstructParam *props)
{
    GapWindow *window;
    GtkWidget *scrolledwindow;
    GtkWidget *terminal;

    GObject *object = G_OBJECT_CLASS(gap_window_parent_class)->constructor (type, n_props, props);

    window = GAP_WINDOW (object);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow);
    gtk_box_pack_start (GTK_BOX (MOO_WINDOW(window)->vbox), scrolledwindow, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow),
                                         GTK_SHADOW_ETCHED_OUT);

    if (!g_type_is_a (window->view_type, GAP_TYPE_VIEW))
    {
        g_critical ("%s: oops", G_STRLOC);
        terminal = g_object_new (GTK_TYPE_TEXT_VIEW, NULL);
    }
    else
    {
        terminal = g_object_new (window->view_type, NULL);
    }

    gtk_widget_show (terminal);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), terminal);
    GTK_WIDGET_SET_FLAGS (terminal, GTK_CAN_FOCUS);
    GTK_WIDGET_SET_FLAGS (terminal, GTK_CAN_DEFAULT);

    gtk_widget_grab_focus (terminal);
    gtk_widget_grab_default (terminal);

    window->terminal = GAP_VIEW (terminal);
    gtk_widget_show (MOO_WINDOW(window)->vbox);

//     moo_term_apply_settings (window->terminal);

//     window->priv->prefs_notify_id = moo_prefs_notify_connect (MOO_TERM_PREFS_PREFIX,
//                                                               MOO_PREFS_MATCH_PREFIX,
//                                                               (MooPrefsNotify) prefs_notify,
//                                                               window, NULL);

    return object;
}


// static gboolean
// apply_prefs (GapWindow *window)
// {
//     window->priv->apply_prefs_idle = 0;
//     moo_term_apply_settings (window->terminal);
//     return FALSE;
// }
//
// static void
// prefs_notify (G_GNUC_UNUSED const char *key,
//               G_GNUC_UNUSED const GValue *newval,
//               GapWindow *window)
// {
//     g_return_if_fail (GAP_IS_TERM_WINDOW (window));
//     if (!window->priv->apply_prefs_idle)
//         window->priv->apply_prefs_idle =
//                 _moo_idle_add ((GSourceFunc) apply_prefs, window);
// }


GapView *
gap_window_get_terminal (GapWindow *window)
{
    g_return_val_if_fail (GAP_IS_WINDOW (window), NULL);
    return window->terminal;
}


// static void
// copy_clipboard (MooTerm *term)
// {
//     moo_term_copy_clipboard (term, GDK_SELECTION_CLIPBOARD);
// }
//
//
// static void
// paste_clipboard (MooTerm *term)
// {
//     moo_term_paste_clipboard (term, GDK_SELECTION_CLIPBOARD);
// }


static void
switch_to_editor (void)
{
    MooApp *app = moo_app_get_instance ();
    MooEditor *editor = moo_app_get_editor (app);
    moo_editor_present (editor, 0);
}


static void
gap_read_file (GapWindow *window)
{
    const char *file;
    char *string;

    file = moo_file_dialogp (GTK_WIDGET (window),
                             MOO_FILE_DIALOG_OPEN,
                             NULL,
                             "Read File",
                             GGAP_PREFS_PREFIX "/read_file",
                             NULL);

    if (!file)
        return;

    string = gap_read_file_string (file);
    gap_app_feed_gap (GAP_APP_INSTANCE, string);
    g_free (string);
}


static void
gap_open_workspace (GapWindow *window)
{
    const char *file;

    file = moo_file_dialogp (GTK_WIDGET (window),
                             MOO_FILE_DIALOG_OPEN,
                             NULL,
                             "Choose Saved Workspace File",
                             GGAP_PREFS_PREFIX "/open_workspace",
                             NULL);

    if (!file)
        return;

    gap_app_open_workspace (GAP_APP_INSTANCE, file);
}
