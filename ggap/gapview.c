/*
 *   gapview.c
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

#include "gapview.h"
#include "gapapp.h"
#include "gap.h"
#include "gapoutput.h"
#include "mooutils/moomarshals.h"
#include "mooutils/mooutils-misc.h"
#include <gtk/gtkwidget.h>


static void     gap_view_base_init      (GapViewIface   *iface);
static void     gap_view_class_init     (GapViewIface   *iface);
static void     gap_exited              (GapView        *view);


GType
gap_view_get_type (void)
{
    static GType type = 0;

    if (G_UNLIKELY (!type))
    {
        const GTypeInfo info = {
            sizeof (GapViewIface),
            (GBaseInitFunc) gap_view_base_init,
            NULL,
            (GClassInitFunc) gap_view_class_init,
            NULL,
            NULL,
            0,
            0,
            NULL,
            NULL
        };

        type = g_type_register_static (G_TYPE_INTERFACE, "GapView", &info, 0);
        g_type_interface_add_prerequisite (type, GTK_TYPE_WIDGET);
    }

    return type;
}


static void
gap_view_base_init (G_GNUC_UNUSED GapViewIface *iface)
{
    static gboolean been_here;

    if (!been_here)
    {
        g_signal_new ("gap-exited",
                      GAP_TYPE_VIEW,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (GapViewIface, gap_exited),
                      NULL, NULL,
                      _moo_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

        been_here = TRUE;
    }
}


static void
gap_view_class_init (GapViewIface *iface)
{
    iface->gap_exited = gap_exited;
}


static GString *
make_command_line (const char *cmd_base,
                   const char *flags,
                   const char *custom_wsp,
                   guint       session_id,
                   gboolean    fancy)
{
    gboolean init_pkg, save_workspace;
    gboolean wsp_already_saved = FALSE;
    char *wsp_file = NULL;
    const char *init_file = NULL;
    GString *cmd;

    init_pkg = moo_prefs_get_bool (GGAP_PREFS_GAP_INIT_PKG);
    save_workspace = moo_prefs_get_bool (GGAP_PREFS_GAP_SAVE_WORKSPACE);

    cmd = g_string_new (cmd_base);

    if (flags)
        g_string_append_printf (cmd, " %s", flags);

    if (!custom_wsp && save_workspace)
    {
        wsp_file = gap_saved_workspace_filename ();

        g_return_val_if_fail (wsp_file != NULL, cmd);

        wsp_already_saved = g_file_test (wsp_file, G_FILE_TEST_EXISTS);

        if (!wsp_already_saved)
        {
            char *gzipped = g_strdup_printf ("%s.gz", wsp_file);
            wsp_already_saved = g_file_test (gzipped, G_FILE_TEST_EXISTS);
            g_free (gzipped);
        }
    }

    if (custom_wsp)
    {
        g_string_append_printf (cmd, " -L \"%s\"", custom_wsp);
    }
    else if (save_workspace)
    {
        if (wsp_already_saved)
            g_string_append_printf (cmd, " -L \"%s\"", wsp_file);

        if (!wsp_already_saved && !moo_make_user_data_dir (NULL))
            g_critical ("%s: could not create user data dir", G_STRLOC);

        if (!wsp_already_saved || init_pkg)
            init_file = gap_init_file (wsp_already_saved ? NULL : wsp_file,
                                       init_pkg, session_id, fancy);
    }

    if (init_pkg && !init_file)
        init_file = gap_init_file (NULL, TRUE, session_id, fancy);

    if (init_pkg)
    {
        char **dirs;
        guint i, n_dirs = 0;

        dirs = moo_get_data_dirs (MOO_DATA_SHARE, &n_dirs);

        for (i = 0; i < n_dirs; ++i)
            g_string_append_printf (cmd, " -l \"%s\";", dirs[i]);

        g_strfreev (dirs);
    }

    if (init_file)
        g_string_append_printf (cmd, " \"%s\"", init_file);

    g_free (wsp_file);
    return cmd;
}


static void
gap_view_start_gap_real (GapView    *view,
                         const char *workspace)
{
    static guint session_id;
    const char *cmd_base;
    char *flags = NULL;
    GString *cmd;
    gboolean result;
    gboolean fancy = FALSE;
    GapSession *session;

    g_return_if_fail (GAP_IS_VIEW (view));
    g_return_if_fail (!gap_view_child_alive (view));

    cmd_base = moo_prefs_get_string (GGAP_PREFS_GAP_COMMAND);
    g_return_if_fail (cmd_base && cmd_base[0]);

#ifdef __WIN32__
    {
        static char *saved_bin_dir;
        static const char *saved_path;
        char *bin_dir;

        if (gap_parse_cmd_line (cmd_base, &bin_dir, NULL))
        {
            if (!saved_bin_dir || strcmp (saved_bin_dir, bin_dir))
            {
                char *path;

                if (!saved_path)
                    saved_path = g_getenv ("PATH");

                if (saved_path)
                    path = g_strdup_printf ("%s;%s", bin_dir, saved_path);
                else
                    path = g_strdup (bin_dir);

                g_setenv ("PATH", path, TRUE);
                g_free (path);

                saved_bin_dir = bin_dir;
                bin_dir = NULL;
            }

            g_free (bin_dir);
        }
        else
        {
            g_warning ("%s: could not parse command line `%s`",
                       G_STRLOC, cmd_base);
        }
    }
#endif

    if (session_id++ >= 9999)
        session_id = 1;

    if (GAP_VIEW_GET_IFACE (view)->get_gap_flags)
        GAP_VIEW_GET_IFACE (view)->get_gap_flags (view, &flags, &fancy);

    cmd = make_command_line (cmd_base, flags, workspace, session_id, fancy);

    g_message ("starting GAP: %s", cmd->str);

    g_return_if_fail (GAP_VIEW_GET_IFACE (view)->start_gap != NULL);
    result = GAP_VIEW_GET_IFACE (view)->start_gap (view, cmd->str);

    if (!result)
        g_critical ("%s: could not start gap", G_STRLOC);

    if ((session = gap_session_new (gap_app_output_get_name (), session_id, view)))
        g_object_set_data_full (G_OBJECT (view), "gap-session", session, g_object_unref);

    g_free (flags);
    g_string_free (cmd, TRUE);
}


GapSession *
gap_view_get_session (GapView *view)
{
    g_return_val_if_fail (GAP_IS_VIEW (view), NULL);
    return g_object_get_data (G_OBJECT (view), "gap-session");
}

static void
gap_exited (GapView *view)
{
    GapSession *session;

    session = gap_view_get_session (view);

    if (session)
    {
        gap_session_shutdown (session);
        g_object_set_data (G_OBJECT (view), "gap-session", NULL);
    }

    gap_app_output_restart ();
}


void
gap_view_start_gap (GapView    *view,
                    const char *workspace)
{
    g_return_if_fail (GAP_IS_VIEW (view));
    gap_view_start_gap_real (view, workspace);
}


void
gap_view_stop_gap (GapView *view)
{
    g_return_if_fail (GAP_IS_VIEW (view));
    g_return_if_fail (GAP_VIEW_GET_IFACE(view)->stop_gap != NULL);

    if (gap_view_child_alive (view))
        GAP_VIEW_GET_IFACE (view)->stop_gap (view);
}


void
gap_view_feed_gap (GapView    *view,
                   const char *text)
{
    g_return_if_fail (GAP_IS_VIEW (view));
    g_return_if_fail (GAP_VIEW_GET_IFACE(view)->feed_gap != NULL);
    g_return_if_fail (text != NULL);
    g_return_if_fail (gap_view_child_alive (view));
    GAP_VIEW_GET_IFACE(view)->feed_gap (view, text);
}


gboolean
gap_view_child_alive (GapView *view)
{
    g_return_val_if_fail (GAP_IS_VIEW (view), FALSE);
    g_return_val_if_fail (GAP_VIEW_GET_IFACE (view)->child_alive != NULL, FALSE);
    return GAP_VIEW_GET_IFACE (view)->child_alive (view);
}


void
gap_view_send_intr (GapView *view)
{
    g_return_if_fail (GAP_IS_VIEW (view));
    g_return_if_fail (GAP_VIEW_GET_IFACE (view)->send_intr != NULL);

    if (gap_view_child_alive (view))
        GAP_VIEW_GET_IFACE (view)->send_intr (view);
}
