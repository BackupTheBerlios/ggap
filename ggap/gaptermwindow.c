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

#include "ggap-i18n.h"
#include "gapapp.h"
#include "gap.h"
#include "gaptermwindow.h"
#include "gapterm.h"
#include "mooutils/moofiledialog.h"
#include "mooutils/moocompat.h"
#include "mooutils/mooutils-misc.h"
#include "mooterm/mooterm-prefs.h"
#include "mooapp/mooapp.h"
#include <gtk/gtk.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


struct _GapTermWindowPrivate {
    GapTerm *term;
    guint   prefs_notify_id;
    guint   apply_prefs_idle;
};


static GObject *gap_term_window_constructor     (GType           type,
                                                 guint           n_props,
                                                 GObjectConstructParam *props);
static void     gap_term_window_destroy         (GtkObject      *object);
static GapView *gap_term_window_get_terminal    (GapWindow      *window);

static void     copy_clipboard                  (MooTerm        *term);
static void     paste_clipboard                 (MooTerm        *term);

static void     prefs_notify                    (const char     *key,
                                                 const GValue   *newval,
                                                 GapTermWindow  *window);


/* GAP_TYPE_TERM_WINDOW */
G_DEFINE_TYPE (GapTermWindow, gap_term_window, GAP_TYPE_WINDOW)


static void
gap_term_window_class_init (GapTermWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);
    GapWindowClass *gwindow_class = GAP_WINDOW_CLASS (klass);

    moo_window_class_set_id (window_class, "Terminal", "Terminal");

    object_class->constructor = gap_term_window_constructor;
    gtkobject_class->destroy = gap_term_window_destroy;
    gwindow_class->get_terminal = gap_term_window_get_terminal;

    moo_window_class_new_action (window_class, "Copy", NULL,
                                 "display-name", GTK_STOCK_COPY,
                                 "label", GTK_STOCK_COPY,
                                 "tooltip", GTK_STOCK_COPY,
                                 "stock-id", GTK_STOCK_COPY,
                                 "accel", "<alt>C",
                                 "closure-callback", copy_clipboard,
                                 "closure-proxy-func", gap_window_get_terminal,
                                 NULL);

    moo_window_class_new_action (window_class, "Paste", NULL,
                                 "display-name", GTK_STOCK_PASTE,
                                 "label", GTK_STOCK_PASTE,
                                 "tooltip", GTK_STOCK_PASTE,
                                 "stock-id", GTK_STOCK_PASTE,
                                 "accel", "<alt>V",
                                 "closure-callback", paste_clipboard,
                                 "closure-proxy-func", gap_window_get_terminal,
                                 NULL);

    moo_window_class_new_action (window_class, "SelectAll", NULL,
                                 "display-name", GTK_STOCK_SELECT_ALL,
                                 "label", GTK_STOCK_SELECT_ALL,
                                 "tooltip", GTK_STOCK_SELECT_ALL,
                                 "accel", "<alt>A",
                                 "closure-callback", moo_term_select_all,
                                 "closure-proxy-func", gap_window_get_terminal,
                                 NULL);

    g_type_class_add_private (klass, sizeof (GapTermWindowPrivate));
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

    gap_view_start_gap (GAP_VIEW (window->priv->term), NULL);

    return object;
}

static GapView *
gap_term_window_get_terminal (GapWindow *window)
{
    return GAP_VIEW (GAP_TERM_WINDOW (window)->priv->term);
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


static void
copy_clipboard (MooTerm *term)
{
    moo_term_copy_clipboard (term, GDK_SELECTION_CLIPBOARD);
}


static void
paste_clipboard (MooTerm *term)
{
    moo_term_paste_clipboard (term, GDK_SELECTION_CLIPBOARD);
}
