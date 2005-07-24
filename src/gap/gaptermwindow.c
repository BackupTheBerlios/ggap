/*
 *   @(#)$Id: gaptermwindow.c,v 1.4 2005/06/06 14:45:55 emuntyan Exp $
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

#include "gap/gaptermwindow.h"
#include "gap/gapapp.h"
#include "gap/gapui.h"
#include "gap/gapprefsdialog.h"
#include "gap/gapprefs.h"
#include "mooutils/moostock.h"
#include "mooutils/moodialogs.h"
#include "mooui/moouiobject-impl.h"
#include "mooedit/mooeditprefs.h"
#include "mooutils/moocompat.h"
#include "mooutils/eggregex.h"
#include "mooutils/moowin.h"


static void         gap_term_window_class_init          (GapTermWindowClass *klass);
static void         gap_term_window_init                (GapTermWindow     *window);
GObject            *gap_term_window_constructor         (GType                  type,
                                                         guint                  n_props,
                                                         GObjectConstructParam *props);
static void         gap_term_window_finalize            (GObject        *window);

static gboolean     gap_term_window_close               (GapTermWindow *window);
static void         gap_term_window_new_editor_window   (GapTermWindow *window);
static void         gap_term_window_open_in_editor      (GapTermWindow *window);
static void         gap_term_window_gap_read            (GapTermWindow *window);
static void         gap_term_window_copy_last_output    (GapTermWindow *window);
static void         gap_term_window_python_execute_file (GapTermWindow *window);

static EggRegex *gap_prompt = NULL;


/* GAP_TYPE_TERM_WINDOW */
G_DEFINE_TYPE (GapTermWindow, gap_term_window, MOO_TYPE_TERM_WINDOW)


static void gap_term_window_class_init (GapTermWindowClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    MooWindowClass *window_class = MOO_WINDOW_CLASS (klass);

    gobject_class->constructor = gap_term_window_constructor;
    gobject_class->finalize = gap_term_window_finalize;
    window_class->close = (gboolean (*) (MooWindow*))gap_term_window_close;

    moo_ui_object_class_init (gobject_class, "Terminal", "Terminal");

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "NewEditor",
                                    "name", "New Editor",
                                    "label", "_New Editor",
                                    "tooltip", "Open new editor window",
                                    "icon-stock-id", GTK_STOCK_EDIT,
                                    "accel", "<alt>N",
                                    "closure::callback", gap_term_window_new_editor_window,
                                    NULL);

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "OpenInEditor",
                                    "name", "Open in Editor",
                                    "label", "_Open in Editor",
                                    "tooltip", "Open document in new editor window",
                                    "icon-stock-id", GTK_STOCK_OPEN,
                                    "accel", "<alt>O",
                                    "closure::callback", gap_term_window_open_in_editor,
                                    NULL);

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "Quit",
                                    "name", "Quit",
                                    "label", "_Quit",
                                    "tooltip", "Quit",
                                    "icon-stock-id", GTK_STOCK_QUIT,
                                    "accel", "<alt>Q",
                                    "closure::callback", gap_term_window_close,
                                    NULL);

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "CopyLastOutput",
                                    "name", "Copy Last Output",
                                    "label", "Copy _Last Output",
                                    "tooltip", "Copy last GAP output",
                                    "icon-stock-id", GTK_STOCK_COPY,
                                    "accel", "<alt><shift>C",
                                    "closure::callback", gap_term_window_copy_last_output,
                                    NULL);

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "Preferences",
                                    "name", "Preferences",
                                    "label", "_Preferences",
                                    "tooltip", "Open GGAP Preferences dialog",
                                    "accel", "<alt>P",
                                    "icon-stock-id", GTK_STOCK_PREFERENCES,
                                    "closure::callback", gap_prefs_dialog,
                                    NULL);

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "GAPRead",
                                    "name", "GAP Read",
                                    "label", "Rea_d",
                                    "tooltip", "Read",
                                    "accel", "",
                                    "icon-stock-id", GTK_STOCK_OPEN,
                                    "closure::callback", gap_term_window_gap_read,
                                    NULL);

    moo_ui_object_class_new_action (gobject_class,
                                    "id", "Restart",
                                    "name", "Restart",
                                    "label", "_Restart",
                                    "tooltip", "Restart GAP",
                                    "accel", "<alt>R",
                                    "icon-stock-id", GTK_STOCK_REFRESH,
                                    "closure::callback", gap_app_restart_gap,
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
                                    "closure::callback", gap_term_window_python_execute_file,
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


static void gap_term_window_init (G_GNUC_UNUSED GapTermWindow *window)
{
}


GObject            *gap_term_window_constructor         (GType                  type,
                                                         guint                  n_props,
                                                         GObjectConstructParam *props)
{
    const char *pattern;
    MooUIXML *xml;
    GError *error = NULL;
    GtkWidget *popup;
    GapTermWindow *window;

    GObject *object =
            G_OBJECT_CLASS(gap_term_window_parent_class)->constructor (type, n_props, props);

    window = GAP_TERM_WINDOW (object);

    gtk_window_set_title (GTK_WINDOW (window), "GGAP");
    moo_window_set_icon_from_stock  (GTK_WINDOW (window), MOO_STOCK_APP);

    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

    pattern = "^(gap>|brk>|brk_[0-9][0-9]>) ";
    moo_term_set_prompt (MOO_TERM_WINDOW(window)->terminal, pattern, "^> ");
    gap_prompt = egg_regex_new (pattern,
                                (EggRegexCompileFlags) 0,
                                (EggRegexMatchFlags) 0,
                                &error);
    if (error) {
        g_critical ("%s: %s", G_STRLOC, error->message);
        egg_regex_free (gap_prompt);
        gap_prompt = NULL;
        g_error_free (error);
        error = NULL;
    }

    xml = moo_ui_object_get_ui_xml (MOO_UI_OBJECT (window));
    moo_ui_xml_add_ui_from_string (xml, GAP_UI, -1, &error);
    if (error) {
        g_error ("%s", error->message);
        g_error_free (error);
    }

    gap_term_window_apply_settings (window);
    popup = moo_ui_xml_create_widget (xml, "Terminal/Popup",
                                      moo_ui_object_get_actions (MOO_UI_OBJECT (window)),
                                      MOO_WINDOW (window)->accel_group,
                                      MOO_WINDOW (window)->tooltips);
    if (popup) moo_term_set_popup_menu (MOO_TERM_WINDOW (window)->terminal, popup);

    return object;
}


void        gap_term_window_apply_settings  (GapTermWindow     *window)
{
    g_return_if_fail (GAP_IS_TERM_WINDOW (window));
    g_signal_emit_by_name (MOO_TERM_WINDOW(window)->terminal, "apply_settings", NULL);
}


static void gap_term_window_finalize (GObject *object)
{
    if (gap_prompt) egg_regex_unref (gap_prompt);
    G_OBJECT_CLASS (gap_term_window_parent_class)->finalize (object);
}


GapTermWindow *gap_term_window_new (void)
{
    GapTermWindow *win = GAP_TERM_WINDOW (g_object_new (GAP_TYPE_TERM_WINDOW, NULL));
    gtk_widget_show (GTK_WIDGET (win));
    return win;
}


static gboolean gap_term_window_close (G_GNUC_UNUSED GapTermWindow *window)
{
    return moo_app_quit (moo_app_get_instance ());
}


/****************************************************************************/


static void gap_term_window_new_editor_window (G_GNUC_UNUSED GapTermWindow *window)
{
    moo_editor_new_window (moo_app_get_editor (moo_app_get_instance ()));
}

static void gap_term_window_open_in_editor (G_GNUC_UNUSED GapTermWindow *self)
{
    moo_editor_open (moo_app_get_editor (moo_app_get_instance ()),
                     NULL, NULL, NULL);
}


static void gap_term_window_gap_read (GapTermWindow *self)
{
    const char *file;

    file = moo_file_dialogp (GTK_WIDGET (self),
                             MOO_DIALOG_FILE_OPEN_EXISTING,
                             "Choose File",
                             GAP_PREFS_DIALOGS_GAP_READ,
                             NULL);

    if (file) {
        char *escaped = g_strescape (file, "");
        char *cmd = g_strdup_printf ("Read(\"%s\");\n", escaped);
        gap_app_feed_gap (gap_app(), cmd, -1);
        g_free (cmd);
        g_free (escaped);
    }
}


static void gap_term_window_copy_last_output (GapTermWindow *self)
{
    int prev, last;
    char *text = NULL;

    g_return_if_fail (gap_prompt != NULL);
    last = moo_term_get_cursor_row_abs (MOO_TERM_WINDOW(self)->terminal);

    while (last >= 0) {
        const char *line = moo_term_get_line (MOO_TERM_WINDOW(self)->terminal, last, NULL);
        egg_regex_clear (gap_prompt);
        if (egg_regex_match (gap_prompt, line, -1, 0) >= 1) {
            break;
        }
        else
            --last;
    }
    g_return_if_fail (last > 0);

    prev = last - 1;
    while (prev >= 0) {
        const char *line = moo_term_get_line (MOO_TERM_WINDOW(self)->terminal, prev, NULL);
        egg_regex_clear (gap_prompt);
        if (egg_regex_match (gap_prompt, line, -1, 0) >= 1) {
            break;
        }
        else
            --prev;
    }
    if (prev < 0) prev = 0;

    if (last == prev + 1) return;
    text = moo_term_get_range (MOO_TERM_WINDOW(self)->terminal, prev + 1, 0, last, 0);
    if (text) {
        GtkClipboard *cb = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
        gtk_clipboard_set_text (cb, text, -1);
        g_free (text);
    }
}



static void gap_term_window_python_execute_file (GapTermWindow *self)
{
    moo_app_python_execute_file (GTK_WINDOW (self));
}

