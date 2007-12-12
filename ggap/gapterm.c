/*
 *   gapterm.c
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

#include "gapterm.h"
#include "gapview.h"
#include "gapapp.h"
#include "gapeditwindow.h"
#include "mooterm/mooterm-text.h"
#include "mooutils/mooutils-misc.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <glib/gregex.h>
#include <gdk/gdkkeysyms.h>

#define ERROR_TAG "gap-syntax-error"

struct _GapTermPrivate {
    guint analyze_idle_id;
    MooTermTag *error_tag;
    GRegex *error_regex;
    int last_line_checked;
};

typedef struct {
    char *file;
    char *info;
    int line;
} ErrInfo;


static void     gap_term_class_init     (GapTermClass   *klass);
static void     gap_term_view_init      (GapViewIface   *iface);
static void     gap_term_init           (GapTerm        *window);
static GObject *gap_term_constructor    (GType           type,
                                         guint           n_props,
                                         GObjectConstructParam *props);
static void     gap_term_destroy        (GtkObject      *object);

static gboolean gap_term_key_press      (GtkWidget      *widget,
                                         GdkEventKey    *event);
static gboolean gap_term_button_press   (GtkWidget      *widget,
                                         GdkEventButton *event);
static void     gap_term_new_line       (MooTerm        *term);
static void     gap_term_reset          (MooTerm        *term);
static void     gap_term_child_died     (MooTerm        *term);
static gboolean do_analyze              (GapTerm        *term);

static ErrInfo *err_info_new            (const char         *file,
                                         const char         *info,
                                         int                 line);
static void     err_info_free           (ErrInfo            *err);


/* GAP_TYPE_TERM */
G_DEFINE_TYPE_WITH_CODE (GapTerm, gap_term, MOO_TYPE_TERM,
                         G_IMPLEMENT_INTERFACE (GAP_TYPE_VIEW, gap_term_view_init))


static void
gap_term_class_init (GapTermClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
    GtkWidgetClass *gtkwidget_class = GTK_WIDGET_CLASS (klass);
    MooTermClass *term_class = MOO_TERM_CLASS (klass);

    object_class->constructor = gap_term_constructor;
    gtkobject_class->destroy = gap_term_destroy;
    gtkwidget_class->button_press_event = gap_term_button_press;
    gtkwidget_class->key_press_event = gap_term_key_press;
    term_class->new_line = gap_term_new_line;
    term_class->reset = gap_term_reset;
    term_class->child_died = gap_term_child_died;

    g_type_class_add_private (klass, sizeof (GapTermPrivate));
}


static gboolean
gap_term_view_start_gap (GapView    *view,
                         const char *cmd_line)
{
    if (moo_prefs_get_bool (GGAP_PREFS_GAP_CLEAR_TERMINAL))
        moo_term_reset (MOO_TERM (view));

    return moo_term_fork_command_line (MOO_TERM (view), cmd_line,
                                       NULL, NULL, NULL);
}

static void
gap_term_view_stop_gap (GapView *view)
{
    moo_term_kill_child (MOO_TERM (view));
}

static void
gap_term_view_feed_gap (GapView    *view,
                        const char *text)
{
    moo_term_feed_child (MOO_TERM (view), text, -1);
}

static gboolean
gap_term_view_child_alive (GapView *view)
{
    return moo_term_child_alive (MOO_TERM (view));
}

static void
gap_term_view_send_intr (GapView *view)
{
    moo_term_ctrl_c (MOO_TERM (view));
}

static void
gap_term_view_init (GapViewIface *iface)
{
    iface->start_gap = gap_term_view_start_gap;
    iface->stop_gap = gap_term_view_stop_gap;
    iface->feed_gap = gap_term_view_feed_gap;
    iface->child_alive = gap_term_view_child_alive;
    iface->send_intr = gap_term_view_send_intr;
    iface->get_gap_flags = NULL;
}


static void
gap_term_init (GapTerm *term)
{
    term->priv = G_TYPE_INSTANCE_GET_PRIVATE (term, GAP_TYPE_TERM,
                                              GapTermPrivate);
    term->priv->last_line_checked = -1;

    term->priv->error_regex = g_regex_new ("Syntax error: (.*) in (.*) line (\\d+)",
                                           G_REGEX_ANCHORED, 0, NULL);
    g_return_if_fail (term->priv->error_regex != NULL);
}


static GObject*
gap_term_constructor (GType               type,
                      guint               n_props,
                      GObjectConstructParam *props)
{
    MooTermTextAttr attr;
    GObject *object;
    GapTerm *term;

    object = G_OBJECT_CLASS(gap_term_parent_class)->constructor (type, n_props, props);
    term = GAP_TERM (object);

    term->priv->error_tag = moo_term_create_tag (MOO_TERM (term), ERROR_TAG);

    attr.mask = MOO_TERM_TEXT_UNDERLINE | MOO_TERM_TEXT_FOREGROUND;
    attr.foreground = MOO_TERM_RED;
    moo_term_tag_set_attr (term->priv->error_tag, &attr);

    return object;
}


static void
gap_term_destroy (GtkObject *object)
{
    GapTerm *term = GAP_TERM (object);

    if (term->priv)
    {
        if (term->priv->analyze_idle_id)
            g_source_remove (term->priv->analyze_idle_id);
        term->priv->analyze_idle_id = 0;
        g_regex_unref (term->priv->error_regex);

        term->priv = NULL;
    }

    GTK_OBJECT_CLASS(gap_term_parent_class)->destroy (object);
}


static void
gap_term_child_died (MooTerm *term)
{
    if (MOO_TERM_CLASS(gap_term_parent_class)->child_died)
        MOO_TERM_CLASS(gap_term_parent_class)->child_died (term);
    g_signal_emit_by_name (term, "gap-exited");
}


static void
gap_term_reset (MooTerm *term)
{
    GAP_TERM (term)->priv->last_line_checked = -1;
    MOO_TERM_CLASS(gap_term_parent_class)->reset (term);
}

static void
gap_term_new_line (MooTerm *moo_term)
{
    GapTerm *term = GAP_TERM (moo_term);

    if (!term->priv->analyze_idle_id)
        term->priv->analyze_idle_id = g_idle_add ((GSourceFunc) do_analyze, term);
}


static gboolean
do_analyze (GapTerm *term)
{
    MooTermIter start, end;
    int cursor_row, i;
    MooTerm *moo_term = MOO_TERM (term);

    term->priv->analyze_idle_id = 0;

    moo_term_get_iter_at_cursor (moo_term, &start);
    cursor_row = start.row;

    if (cursor_row <= term->priv->last_line_checked)
        return FALSE;

    for (i = MAX(term->priv->last_line_checked, 0); i < cursor_row; ++i)
    {
        MooTermLine *line;
        char *text;
        guint width;
        int j, k;
        GString *message;
        GMatchInfo *match_info = NULL;

        line = moo_term_get_line (moo_term, i);
        g_return_val_if_fail (line != NULL, FALSE);

        if (moo_term_line_get_char (line, 0) != 'S')
            continue;

        text = moo_term_line_get_text (line, 0, strlen ("Syntax error:"));

        if (strcmp (text, "Syntax error:"))
        {
            g_free (text);
            continue;
        }

        g_free (text);
        j = i;
        moo_term_get_screen_size (MOO_TERM (term), &width, NULL);

        while (j < cursor_row)
        {
            line = moo_term_get_line (moo_term, j);
            if (moo_term_line_get_char (line, width - 1) == ' ' &&
                moo_term_line_get_char (line, width - 2) == '\\')
            {
                ++j;
            }
            else
            {
                break;
            }
        }

        if (j == cursor_row)
        {
            term->priv->last_line_checked = i - 1;
            return FALSE;
        }

        message = g_string_new (NULL);

        for (k = i; k < j; ++k)
        {
            line = moo_term_get_line (moo_term, k);
            text = moo_term_line_get_text (line, 0, width - 2);
            g_string_append (message, text);
            g_free (text);
        }

        line = moo_term_get_line (moo_term, j);
        for (k = width - 2; k >= 0 && moo_term_line_get_char (line, k) == ' '; --k) ;
        text = moo_term_line_get_text (line, 0, k + 1);
        g_string_append (message, text);
        g_free (text);

        if (g_regex_match (term->priv->error_regex, message->str, 0, &match_info))
        {
            char *file_string, *info_string, *line_string;
            long line_no;

            info_string = g_match_info_fetch (match_info, 1);
            file_string = g_match_info_fetch (match_info, 2);
            line_string = g_match_info_fetch (match_info, 3);

            g_assert (info_string && file_string && line_string);

            errno = 0;
            line_no = strtol (line_string, NULL, 10);

            if (errno)
            {
                perror ("strtol");
                g_critical ("%s: could not convert '%s' to number",
                            G_STRLOC, line_string);
            }
            else if (line_no < 1 || line_no > G_MAXINT)
            {
                g_critical ("%s: invalid line number '%s'", G_STRLOC, line_string);
            }
            else
            {
                moo_term_get_iter_at_line (moo_term, &start, i);
                moo_term_get_iter_at_line_offset (moo_term, &end, j, k + 1);
                moo_term_apply_tag (moo_term, term->priv->error_tag, &start, &end);

                if (strcmp (file_string, "*errin*") && strcmp (file_string, "stream"))
                {
                    for (k = i; k <= j; ++k)
                    {
                        line = moo_term_get_line (moo_term, k);
                        moo_term_set_line_data_full (moo_term, line, ERROR_TAG,
                                                     err_info_new (file_string, info_string, line_no - 1),
                                                     (GDestroyNotify) err_info_free);
                    }
                }
            }

            g_free (file_string);
            g_free (info_string);
            g_free (line_string);
        }

        if (match_info)
            g_match_info_free (match_info);

        g_string_free (message, TRUE);
        i = j;
    }

    term->priv->last_line_checked = cursor_row - 1;
    return FALSE;
}


static ErrInfo*
err_info_new (const char         *file,
              const char         *info,
              int                 line)
{
    ErrInfo *err_info;
    g_return_val_if_fail (file != NULL && info != NULL && line >= 0, NULL);
    err_info = moo_new (ErrInfo);
    err_info->file = g_strdup (file);
    err_info->info = g_strdup (info);
    err_info->line = line;
    return err_info;
}


static void
err_info_free (ErrInfo *err)
{
    if (err)
    {
        g_free (err->file);
        g_free (err->info);
        moo_free (ErrInfo, err);
    }
}


static gboolean
gap_term_button_press (GtkWidget      *widget,
                       GdkEventButton *event)
{
    int buf_x, buf_y;
    MooTermIter iter;
    GapTerm *term = GAP_TERM (widget);

    if (event->button != 1 || event->type != GDK_BUTTON_PRESS)
        goto call_parent;

    moo_term_window_to_buffer_coords (MOO_TERM (term), event->x, event->y, &buf_x, &buf_y);
    moo_term_get_iter_at_pos (MOO_TERM (term), &iter, buf_x, buf_y);

    if (moo_term_iter_has_tag (&iter, term->priv->error_tag))
    {
        ErrInfo *err = moo_term_get_line_data (MOO_TERM (term), iter.line, ERROR_TAG);

        if (err)
        {
            gap_edit_window_open_file (err->file, err->line, widget);
            return TRUE;
        }
    }

call_parent:
    return GTK_WIDGET_CLASS(gap_term_parent_class)->button_press_event (widget, event);
}


#define CTRL_A "\001"
#define CTRL_B "\002"
#define CTRL_D "\004"
#define CTRL_E "\005"
#define CTRL_F "\006"
#define CTRL_H "\010"
#define ESC_B "\033b"
#define ESC_D "\033d"
#define ESC_F "\033f"
#define ESC_DEL "\033\177"


static gboolean
gap_term_key_press (GtkWidget      *widget,
                    GdkEventKey    *event)
{
    GdkModifierType state;
    const char *s = NULL;

    state = event->state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK);

    if (!state)
    {
        switch (event->keyval)
        {
            case GDK_Home:
            case GDK_KP_Home:
                s = CTRL_A;
                break;

            case GDK_End:
            case GDK_KP_End:
                s = CTRL_E;
                break;

            case GDK_Left:
            case GDK_KP_Left:
                s = CTRL_B;
                break;

            case GDK_Right:
            case GDK_KP_Right:
                s = CTRL_F;
                break;

            case GDK_BackSpace:
                s = CTRL_H;
                break;

//             case GDK_Delete:
//             case GDK_KP_Delete:
//                 "\033[3"
//                 s = CTRL_D;
//                 break;
        }
    }
    else if (state == GDK_CONTROL_MASK)
    {
        switch (event->keyval)
        {
            case GDK_Left:
            case GDK_KP_Left:
                s = ESC_B;
                break;

            case GDK_Right:
            case GDK_KP_Right:
                s = ESC_F;
                break;

            case GDK_BackSpace:
                s = ESC_DEL;
                break;

            case GDK_Delete:
            case GDK_KP_Delete:
                s = ESC_D;
                break;
        }
    }

    if (s)
    {
        gap_app_feed_gap (GAP_APP_INSTANCE, s);
        return TRUE;
    }
    else
    {
        return GTK_WIDGET_CLASS(gap_term_parent_class)->key_press_event (widget, event);
    }
}
