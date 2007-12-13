/*
 *   gaptermwindow.c
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

#include "config.h"
#include "ggap-i18n.h"
#include "gapapp.h"
#include "gap.h"
#include "gaptermwindow.h"
#include "gapterm.h"
#include "mooutils/moofiledialog.h"
#include "mooutils/moocompat.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/mooeditops.h"
#include "mooutils/moostock.h"
#include "mooterm/mooterm-prefs.h"
#include "mooterm/mooterm-text.h"
#include "mooapp/mooapp.h"
#include <gtk/gtk.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


struct GapTermWindowPrivate {
    GapTerm *term;
    guint   prefs_notify_id;
    guint   apply_prefs_idle;
};


static GObject *gap_term_window_constructor     (GType           type,
                                                 guint           n_props,
                                                 GObjectConstructParam *props);
static void     gap_term_window_destroy         (GtkObject      *object);

static void     prefs_notify                    (const char     *key,
                                                 const GValue   *newval,
                                                 GapTermWindow  *window);

static void     action_switch_to_editor         (void);
static void     action_read_file                (GapTermWindow  *window);
static void     action_open_workspace           (GapTermWindow  *window);
static void     action_restart_gap              (GapTermWindow  *window);
static void     action_send_intr                (GapTermWindow  *window);


G_DEFINE_TYPE (GapTermWindow, gap_term_window, MOO_TYPE_WINDOW)


static void
gap_term_window_class_init (GapTermWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);

    moo_window_class_set_id (window_class, "Terminal", "Terminal");

    object_class->constructor = gap_term_window_constructor;
    gtkobject_class->destroy = gap_term_window_destroy;

    g_type_class_add_private (klass, sizeof (GapTermWindowPrivate));

    moo_window_class_new_action (window_class, "SwitchToEditor", NULL,
                                 "display-name", _("Switch to Editor"),
                                 "label", _("Switch to Editor"),
                                 "tooltip", _("Switch to editor"),
                                 "stock-id", GTK_STOCK_EDIT,
                                 "closure-callback", action_switch_to_editor,
                                 NULL);

    moo_window_class_new_action (window_class, "GAPRead", NULL,
                                 "display-name", _("Read File"),
                                 "label", _("Read File"),
                                 "tooltip", _("Read file"),
                                 "stock-id", GTK_STOCK_OPEN,
                                 "closure-callback", action_read_file,
                                 NULL);
    moo_window_class_new_action (window_class, "GAPOpenWorkspace", NULL,
                                 "display-name", _("Open Workspace"),
                                 "label", _("Open _Workspace"),
                                 "tooltip", _("Open workspace"),
                                 "stock-id", GTK_STOCK_OPEN,
                                 "closure-callback", action_open_workspace,
                                 NULL);

    moo_window_class_new_action (window_class, "Restart", NULL,
                                 "display-name", _("Restart"),
                                 "label", _("_Restart"),
                                 "tooltip", _("Restart GAP"),
                                 "stock-id", MOO_STOCK_RESTART,
                                 "accel", "<alt>R",
                                 "closure-callback", action_restart_gap,
                                 NULL);

    moo_window_class_new_action (window_class, "Interrupt", NULL,
                                 "display-name", _("Interrupt"),
                                 "label", _("_Interrupt"),
                                 "tooltip", _("Interrupt computation"),
                                 "stock-id", GTK_STOCK_STOP,
                                 "closure-callback", action_send_intr,
                                 NULL);
}


static void
gap_term_window_init (GapTermWindow *window)
{
    window->priv = G_TYPE_INSTANCE_GET_PRIVATE (window, GAP_TYPE_TERM_WINDOW,
                                                GapTermWindowPrivate);

    g_object_set (G_OBJECT (window),
                  "menubar-ui-name", "Terminal/Menubar",
                  "toolbar-ui-name", "Terminal/Toolbar",
                  NULL);

    moo_window_set_global_accels (MOO_WINDOW (window), TRUE);
}


static GObject *
gap_term_window_constructor (GType type,
                             guint n_props,
                             GObjectConstructParam *props)
{
    GObject *object;
    GapTermWindow *window;
    GtkWidget *swin;

    object = G_OBJECT_CLASS(gap_term_window_parent_class)->constructor (type, n_props, props);
    window = GAP_TERM_WINDOW (object);

    swin = gtk_scrolled_window_new (NULL, NULL);
    gtk_box_pack_start (GTK_BOX (MOO_WINDOW(window)->vbox), swin, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swin),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_ALWAYS);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (swin),
                                         GTK_SHADOW_ETCHED_OUT);

    window->priv->term = g_object_new (GAP_TYPE_TERM, NULL);

    gtk_container_add (GTK_CONTAINER (swin), GTK_WIDGET (window->priv->term));
    GTK_WIDGET_SET_FLAGS (GTK_WIDGET (window->priv->term), GTK_CAN_FOCUS);
    GTK_WIDGET_SET_FLAGS (GTK_WIDGET (window->priv->term), GTK_CAN_DEFAULT);

    gtk_widget_grab_focus (GTK_WIDGET (window->priv->term));
    gtk_widget_grab_default (GTK_WIDGET (window->priv->term));

    gtk_widget_show_all (MOO_WINDOW(window)->vbox);

    moo_term_apply_settings (MOO_TERM (window->priv->term));
    window->priv->prefs_notify_id = moo_prefs_notify_connect (MOO_TERM_PREFS_PREFIX,
                                                              MOO_PREFS_MATCH_PREFIX,
                                                              (MooPrefsNotify) prefs_notify,
                                                              window, NULL);

    moo_window_set_edit_ops_widget (MOO_WINDOW (window), GTK_WIDGET (window->priv->term));
    gap_term_start_gap (window->priv->term, NULL);

    return object;
}


static void
gap_term_window_destroy (GtkObject *object)
{
    GapTermWindow *window = GAP_TERM_WINDOW (object);

    if (window->priv->prefs_notify_id)
        moo_prefs_notify_disconnect (window->priv->prefs_notify_id);
    if (window->priv->apply_prefs_idle)
        g_source_remove (window->priv->apply_prefs_idle);

    window->priv->prefs_notify_id = 0;
    window->priv->apply_prefs_idle = 0;

    GTK_OBJECT_CLASS (gap_term_window_parent_class)->destroy (object);
}


static gboolean
apply_prefs (GapTermWindow *window)
{
    window->priv->apply_prefs_idle = 0;
    moo_term_apply_settings (MOO_TERM (window->priv->term));
    return FALSE;
}

static void
prefs_notify (G_GNUC_UNUSED const char *key,
              G_GNUC_UNUSED const GValue *newval,
              GapTermWindow  *window)
{
    g_return_if_fail (GAP_IS_TERM_WINDOW (window));
    if (!window->priv->apply_prefs_idle)
        window->priv->apply_prefs_idle =
                moo_idle_add ((GSourceFunc) apply_prefs, window);
}


void
gap_term_window_feed_gap (GapTermWindow *window,
                          const char    *string)
{
    g_return_if_fail (GAP_IS_TERM_WINDOW (window));
    g_return_if_fail (string != NULL);
    gap_term_feed_gap (window->priv->term, string);
}


static void
action_switch_to_editor (void)
{
    MooApp *app = moo_app_get_instance ();
    MooEditor *editor = moo_app_get_editor (app);
    moo_editor_present (editor, 0);
}

static void
action_read_file (GapTermWindow *window)
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
    gap_term_feed_gap (window->priv->term, string);
    g_free (string);
}

static void
action_open_workspace (GapTermWindow *window)
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

    gap_term_stop_gap (window->priv->term);
    gap_term_start_gap (window->priv->term, file);
}

static void
action_restart_gap (GapTermWindow *window)
{
    gap_term_stop_gap (window->priv->term);
    g_usleep (100000);
    gap_term_start_gap (window->priv->term, NULL);
}

static void
action_send_intr (GapTermWindow *window)
{
    if (moo_term_child_alive (MOO_TERM (window->priv->term)))
        gap_term_send_intr (window->priv->term);
}
