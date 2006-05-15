/*
 *   gapapp-script.c
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

#include "gapapp.h"
#include "gapapp-script.h"
#include "mooscript/mooscript-context.h"
#include <string.h>


/**************************************************************************/
/* Common functions
 */

static MSValue*
gap_func (MSValue   **args,
          guint       n_args,
          G_GNUC_UNUSED MSContext *ctx)
{
    guint i;

    for (i = 0; i < n_args; ++i)
    {
        char *s = ms_value_print (args[i]);
        gap_app_feed_gap (GAP_APP_INSTANCE, s);
        g_free (s);
    }

    return ms_value_none ();
}


static void
setup_editor_context (MSContext *ctx)
{
    MSFunc *func;

    g_return_if_fail (MS_IS_CONTEXT (ctx));

    func = ms_cfunc_new_var (gap_func);
    ms_context_set_func (ctx, "GAP", func);
    g_object_unref (func);
}


static void
setup_terminal_context (MSContext *ctx)
{
    return setup_editor_context (ctx);
}


void
gap_app_setup_command (MooCommand *cmd,
                       GtkWindow  *window)
{
    if (MOO_IS_EDIT_WINDOW (window))
        setup_editor_context (cmd->context);
    else
        setup_terminal_context (cmd->context);
}


/**************************************************************************/
/* Serializing data sent to GAP
 */

void
gap_data_add_string (GString    *data,
                     const char *string,
                     int         len)
{
    if (!string)
    {
        string = "";
        len = 0;
    }
    else if (len < 0)
    {
        len = strlen (string);
    }

    if (len >= 256 * 256)
    {
        g_critical ("%s: string too long", G_STRLOC);
        return;
    }

    g_string_append_c (data, GAP_DATA_STRING);
    g_string_append_c (data, len / 256);
    g_string_append_c (data, len % 256);
    g_string_append_len (data, string, len);
}


void
gap_data_add_command_object_died (GString    *data,
                                  const char *id)
{
    gap_data_add_command (data);
    gap_data_add_pair (data);
    gap_data_add_small_int (data, GAP_COMMAND_OBJECT_DIED);
    gap_data_add_string (data, id, -1);
}


/**************************************************************************/
/* GAP <-> GGAP communication
 */

void
gap_data_send (GString *data)
{
    g_return_if_fail (data != NULL);
    moo_app_write_output (data->str, data->len);
}


static MSValue *
set_stamp_func (MSValue   *arg,
                MSContext *ctx)
{
    char *stamp = ms_value_print (arg);
    g_object_set_data_full (G_OBJECT (ctx), "gap-stamp", stamp, g_free);
    return ms_value_none ();
}


static MSValue *
get_stamp_func (MSContext *ctx)
{
    char *stamp = g_object_get_data (G_OBJECT (ctx), "gap-stamp");
    return ms_value_string (stamp);
}


static void
send_string_result (MSContext  *ctx,
                    char        status,
                    const char *string,
                    int         len)
{
    const char *stamp;
    GString *data;

    stamp = g_object_get_data (G_OBJECT (ctx), "gap-stamp");
    g_return_if_fail (stamp != NULL);

    data = g_string_new (NULL);
    gap_data_add_triple (data);
    gap_data_add_string (data, stamp, -1);
    gap_data_add_small_int (data, status);
    gap_data_add_string (data, string, len);

    gap_data_send (data);

    g_string_free (data, TRUE);
}


static MSValue *
create_window_func (MSContext *ctx)
{
    GapObject *wrapper;
    GtkWidget *window;

    if (!GAP_APP_INSTANCE->session)
        return ms_context_set_error (ctx, MS_ERROR_RUNTIME,
                                     "GAP not running");

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_show (window);
    wrapper = gap_session_add_object (GAP_APP_INSTANCE->session, window, TRUE);

    if (!wrapper)
        send_string_result (ctx, GAP_STATUS_ERROR, "Error", -1);
    else
        send_string_result (ctx, GAP_STATUS_OK, wrapper->id, -1);

    return ms_value_none ();
}


static MSValue *
destroy_func (MSValue   *arg,
              MSContext *ctx)
{
    char *id;
    GapObject *wrapper;

    if (!GAP_APP_INSTANCE->session)
        return ms_context_set_error (ctx, MS_ERROR_RUNTIME,
                                     "GAP not running");

    id = ms_value_print (arg);
    wrapper = gap_session_find_object (GAP_APP_INSTANCE->session, id);

    if (!wrapper)
    {
        char *msg = g_strdup_printf ("Object <%s> doesn't exist", id);
        send_string_result (ctx, GAP_STATUS_ERROR, msg, -1);
        g_free (msg);
    }
    else
    {
        gap_object_destroy (wrapper);
        send_string_result (ctx, GAP_STATUS_OK, NULL, -1);
    }

    g_free (id);
    return ms_value_none ();
}


static void
setup_gap_context (MSContext *ctx)
{
    MSFunc *func;

    func = ms_cfunc_new_0 (create_window_func);
    ms_context_set_func (ctx, "CreateWindow", func);
    g_object_unref (func);

    func = ms_cfunc_new_1 (set_stamp_func);
    ms_context_set_func (ctx, "SetStamp", func);
    g_object_unref (func);

    func = ms_cfunc_new_1 (destroy_func);
    ms_context_set_func (ctx, "Destroy", func);
    g_object_unref (func);
}


void
gap_app_exec_command (GapApp     *app,
                      const char *data)
{
    MSContext *ctx;
    MSValue *val;

    ctx = ms_context_new (app->term_window);
    setup_terminal_context (ctx);
    setup_gap_context (ctx);

    val = ms_context_run_script (ctx, data);

    if (!val)
        g_warning ("%s", ms_context_get_error_msg (ctx));

    ms_value_unref (val);
    g_object_unref (ctx);
}
