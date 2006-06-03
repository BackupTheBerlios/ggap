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
#include "gapoutput.h"
#include <mooscript/mooscript-context.h>
#include <mooutils/mooglade.h>
#include <gtk/gtk.h>
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

    if (len >= 128 * 128)
    {
        g_critical ("%s: string too long", G_STRLOC);
        return;
    }

    g_string_append_c (data, GAP_DATA_STRING);
    g_string_append_c (data, len / 128);
    g_string_append_c (data, len % 128);
    g_string_append_len (data, string, len);
}


void
gap_data_add_int (GString *data,
                  int      val)
{
    if (ABS (val) >= 128 * 128)
    {
        g_critical ("%s: value too big", G_STRLOC);
        return;
    }

    g_string_append_c (data, GAP_DATA_INT);
    g_string_append_c (data, val >= 0 ? 0 : 1);
    g_string_append_c (data, ABS(val) / 128);
    g_string_append_c (data, ABS(val) % 128);
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


void
gap_data_add_command_exec_file (GString    *data,
                                const char *filename)
{
    gap_data_add_command (data);
    gap_data_add_pair (data);
    gap_data_add_small_int (data, GAP_COMMAND_EXEC_FILE);
    gap_data_add_string (data, filename, -1);
}


void
gap_data_add_command_close_window (GString    *data,
                                   const char *window_id)
{
    gap_data_add_command (data);
    gap_data_add_pair (data);
    gap_data_add_small_int (data, GAP_COMMAND_CLOSE_WINDOW);
    gap_data_add_string (data, window_id, -1);
}


void
gap_data_add_command_signal (GString    *data,
                             const char *object_id,
                             const char *signal,
                             guint       n_args)
{
    gap_data_add_command (data);
    gap_data_add_list (data, n_args + 3);
    gap_data_add_small_int (data, GAP_COMMAND_SIGNAL);
    gap_data_add_string (data, object_id, -1);
    gap_data_add_string (data, signal, -1);
}


/**************************************************************************/
/* GAP <-> GGAP communication
 */


static gpointer send_error (MSContext  *ctx,
                            const char *format,
                            ...) G_GNUC_PRINTF (2, 3);


void
gap_data_send (GString *data)
{
    g_return_if_fail (data != NULL);
    gap_app_output_write (data->str, data->len);
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
send_bool_result (MSContext  *ctx,
                  char        status,
                  gboolean    val)
{
    const char *stamp;
    GString *data;

    stamp = g_object_get_data (G_OBJECT (ctx), "gap-stamp");
    g_return_if_fail (stamp != NULL);

    data = g_string_new (NULL);

    gap_data_add_triple (data);
    gap_data_add_string (data, stamp, -1);
    gap_data_add_small_int (data, status);
    gap_data_add_bool (data, val);

    gap_data_send (data);
    g_string_free (data, TRUE);
}


static void
send_int_result (MSContext  *ctx,
                 char        status,
                 int         val)
{
    const char *stamp;
    GString *data;

    stamp = g_object_get_data (G_OBJECT (ctx), "gap-stamp");
    g_return_if_fail (stamp != NULL);

    data = g_string_new (NULL);

    gap_data_add_triple (data);
    gap_data_add_string (data, stamp, -1);
    gap_data_add_small_int (data, status);
    gap_data_add_int (data, val);

    gap_data_send (data);
    g_string_free (data, TRUE);
}


static void
send_string_result (MSContext  *ctx,
                    char        status,
                    const char *string1,
                    const char *string2)
{
    const char *stamp;
    GString *data;

    g_return_if_fail (string1 || !string2);

    stamp = g_object_get_data (G_OBJECT (ctx), "gap-stamp");
    g_return_if_fail (stamp != NULL);

    data = g_string_new (NULL);

    if (string2)
        gap_data_add_list (data, 4);
    else if (string1)
        gap_data_add_triple (data);
    else
        gap_data_add_pair (data);

    gap_data_add_string (data, stamp, -1);
    gap_data_add_small_int (data, status);

    if (string1)
        gap_data_add_string (data, string1, -1);
    if (string2)
        gap_data_add_string (data, string2, -1);

    gap_data_send (data);

    g_string_free (data, TRUE);
}


static gpointer
send_error (MSContext  *ctx,
            const char *format,
            ...)
{
    va_list var_args;
    char *message;
    const char *stamp;
    GString *data;

    g_return_val_if_fail (format != NULL, NULL);

    stamp = g_object_get_data (G_OBJECT (ctx), "gap-stamp");
    g_return_val_if_fail (stamp != NULL, NULL);

    va_start (var_args, format);
    message = g_strdup_vprintf (format, var_args);
    va_end (var_args);

    data = g_string_new (NULL);
    gap_data_add_triple (data);
    gap_data_add_string (data, stamp, -1);
    gap_data_add_small_int (data, GAP_STATUS_ERROR);
    gap_data_add_string (data, message, -1);

    gap_data_send (data);
    ms_context_set_error (ctx, MS_ERROR_RUNTIME, message);

    g_string_free (data, TRUE);
    g_free (message);

    return NULL;
}


/****************************************************************************/
/* GAP api
 */

#define CHECK_SESSION()                                                 \
G_STMT_START {                                                          \
    if (!GAP_APP_INSTANCE->session)                                     \
        return ms_context_set_error (ctx, MS_ERROR_RUNTIME,             \
                                     "GAP not running");                \
} G_STMT_END

#define GET_OBJECT_BY_ID(wrapper_, arg_)                                \
G_STMT_START {                                                          \
    char *id_;                                                          \
                                                                        \
    id_ = ms_value_print (arg_);                                        \
    wrapper_ = gap_session_find_object (GAP_APP_INSTANCE->session, id_);\
                                                                        \
    if (!wrapper_ || wrapper_->dead)                                    \
    {                                                                   \
        send_error (ctx, "object <%s> doesn't exist", id_);             \
        g_free (id_);                                                   \
        return NULL;                                                    \
    }                                                                   \
                                                                        \
    g_free (id_);                                                       \
} G_STMT_END

#define GET_OBJECT(wrapper_, arg_)                                      \
G_STMT_START {                                                          \
    wrapper_ = ms_value_get_object (arg_);                              \
    g_return_val_if_fail (GAP_IS_OBJECT (wrapper_), NULL);              \
} G_STMT_END


static MSValue *
create_glade_window_func (MSValue   *arg1,
                          MSValue   *arg2,
                          MSContext *ctx)
{
    GapObject *wrapper;
    GtkWidget *window;
    char *file, *root;
    MooGladeXML *xml;
    MSValue *retval = NULL;

    CHECK_SESSION ();

    file = ms_value_print (arg1);
    root = ms_value_print (arg2);
    xml = moo_glade_xml_new (file, root && root[0] ? root : NULL);

    if (!xml)
    {
        send_error (ctx, "Error loading glade file");
        goto out;
    }

    window = moo_glade_xml_get_root (xml);
    g_return_val_if_fail (window != NULL, NULL);
    gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (GAP_APP_INSTANCE->term_window));

    wrapper = gap_session_add_object (GAP_APP_INSTANCE->session, window,
                                      "GladeWindow", TRUE);
    g_object_set_data_full (G_OBJECT (window), "moo-glade-xml", xml, g_object_unref);

    if (!wrapper)
    {
        send_error (ctx, "oops");
    }
    else
    {
        send_string_result (ctx, GAP_STATUS_OK, wrapper->id, NULL);
        retval = ms_value_none ();
    }

out:
    g_free (file);
    g_free (root);
    return retval;
}


static MSValue *
destroy_func (MSValue   *arg,
              MSContext *ctx)
{
    GapObject *wrapper;

    CHECK_SESSION ();
    GET_OBJECT (wrapper, arg);
    gap_object_destroy (wrapper);
    send_string_result (ctx, GAP_STATUS_OK, NULL, NULL);

    return ms_value_none ();
}


static void
menu_item_activated (GapObject *wrapper)
{
    GString *data = g_string_new (NULL);
    gap_data_add_command_signal (data, wrapper->id, "activate", 0);
    gap_data_send (data);
    g_string_free (data, TRUE);
}


static gulong
connect_activate (GapObject  *wrapper,
                  char      **error_message)
{
    if (GTK_IS_MENU_ITEM (wrapper->obj))
        return g_signal_connect_swapped (wrapper->obj, "activate",
                                         G_CALLBACK (menu_item_activated),
                                         wrapper);

    *error_message = g_strdup_printf ("Signal 'activate' is invalid for object '%s'",
                                      wrapper->id);
    return 0;
}


static void
button_clicked (GapObject *wrapper)
{
    GString *data = g_string_new (NULL);
    gap_data_add_command_signal (data, wrapper->id, "clicked", 0);
    gap_data_send (data);
    g_string_free (data, TRUE);
}


static gulong
connect_clicked (GapObject  *wrapper,
                  char      **error_message)
{
    if (GTK_IS_BUTTON (wrapper->obj))
        return g_signal_connect_swapped (wrapper->obj, "clicked",
                                         G_CALLBACK (button_clicked),
                                         wrapper);

    *error_message = g_strdup_printf ("Signal 'clicked' is invalid for object '%s'",
                                      wrapper->id);
    return 0;
}


static void
dialog_response (GapObject *wrapper,
                 int        response)
{
    GString *data = g_string_new (NULL);
    gap_data_add_command_signal (data, wrapper->id, "response", 1);
    gap_data_add_int (data, response);
    gap_data_send (data);
    g_string_free (data, TRUE);
}


static gulong
connect_response (GapObject  *wrapper,
                  char      **error_message)
{
    if (GTK_IS_DIALOG (wrapper->obj))
        return g_signal_connect_swapped (wrapper->obj, "response",
                                         G_CALLBACK (dialog_response),
                                         wrapper);

    *error_message = g_strdup_printf ("Signal 'response' is invalid for object '%s'",
                                      wrapper->id);
    return 0;
}


static gulong
do_connect_signal (GapObject  *wrapper,
                   const char *signal,
                   char      **error_message)
{
    if (!strcmp (signal, "activate"))
        return connect_activate (wrapper, error_message);
    if (!strcmp (signal, "clicked"))
        return connect_clicked (wrapper, error_message);
    if (!strcmp (signal, "response"))
        return connect_response (wrapper, error_message);

    *error_message = g_strdup_printf ("Uknown signal '%s'", signal);
    return 0;
}


static MSValue *
connect_func (MSValue   *arg1,
              MSValue   *arg2,
              MSValue   *arg3,
              MSContext *ctx)
{
    GapObject *wrapper;
    char *signal, *gap_id, *msg = NULL;
    gulong handler_id;
    MSValue *retval = NULL;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg1);
    signal = ms_value_print (arg2);
    gap_id = ms_value_print (arg3);

    handler_id = do_connect_signal (wrapper, signal, &msg);

    if (!handler_id)
    {
        send_error (ctx, "%s", msg);
        goto out;
    }

    gap_object_connect (wrapper, gap_id, handler_id);
    send_string_result (ctx, GAP_STATUS_OK, NULL, NULL);
    retval = ms_value_none ();

out:
    g_free (signal);
    g_free (gap_id);
    g_free (msg);
    return retval;
}


static MSValue *
disconnect_func (MSValue  **args,
                 guint      n_args,
                 MSContext *ctx)
{
    guint i;
    GapObject *wrapper;

    CHECK_SESSION ();

    if (n_args < 2)
        return ms_context_set_error (ctx, MS_ERROR_VALUE,
                                     "Too few arguments in Disconnect");

    GET_OBJECT (wrapper, args[0]);

    for (i = 1; i < n_args; ++i)
    {
        char *gap_id = ms_value_print (args[i]);
        gap_object_disconnect (wrapper, gap_id);
        g_free (gap_id);
    }

    send_string_result (ctx, GAP_STATUS_OK, "", NULL);
    return ms_value_none ();
}


static MSValue *
glade_lookup_func (MSValue   *arg1,
                   MSValue   *arg2,
                   MSContext *ctx)
{
    GapObject *wrapper, *new_wrapper;
    gpointer widget;
    char *widget_name;
    MooGladeXML *xml;
    MSValue *retval = NULL;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg1);
    widget_name = ms_value_print (arg2);

    xml = g_object_get_data (wrapper->obj, "moo-glade-xml");

    if (!xml)
    {
        send_error (ctx, "object <%s> is not a glade object", wrapper->id);
        goto out;
    }

    widget = moo_glade_xml_get_widget (xml, widget_name);

    if (!widget)
    {
        send_string_result (ctx, GAP_STATUS_OK, "", NULL);
        retval = ms_value_none ();
        goto out;
    }

    new_wrapper = gap_session_add_object (GAP_APP_INSTANCE->session,
                                          widget, NULL, FALSE);

    if (!new_wrapper)
    {
        g_critical ("oops");
        send_error (ctx, "oops");
        goto out;
    }

    send_string_result (ctx, GAP_STATUS_OK, new_wrapper->id, new_wrapper->type);
    retval = ms_value_none ();

out:
    g_free (widget_name);
    return retval;
}


static MSValue *
run_dialog_func (MSValue   *arg,
                 MSContext *ctx)
{
    GapObject *wrapper;
    int response;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg);

    if (!GTK_IS_DIALOG (wrapper->obj))
    {
        send_error (ctx, "object <%s> is not a dialog", wrapper->id);
        return NULL;
    }

    response = gtk_dialog_run (wrapper->obj);
    send_int_result (ctx, GAP_STATUS_OK, response);
    return ms_value_none ();
}


static MSValue *
show_func (MSValue   *arg,
           MSContext *ctx)
{
    GapObject *wrapper;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg);

    if (GTK_IS_WIDGET (wrapper->obj))
    {
        gtk_widget_show (wrapper->obj);
        send_string_result (ctx, GAP_STATUS_OK, NULL, NULL);
    }
    else
    {
        return send_error (ctx, "Show() is not applicable to object <%s>",
                           wrapper->id);
    }

    return ms_value_none ();
}


static MSValue *
hide_func (MSValue   *arg,
           MSContext *ctx)
{
    GapObject *wrapper;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg);

    if (GTK_IS_WIDGET (wrapper->obj))
    {
        gtk_widget_hide (wrapper->obj);
        send_string_result (ctx, GAP_STATUS_OK, NULL, NULL);
    }
    else
    {
        return send_error (ctx, "Hide() is not applicable to object <%s>",
                           wrapper->id);
    }

    return ms_value_none ();
}


static MSValue *
is_visible_func (MSValue   *arg,
                 MSContext *ctx)
{
    GapObject *wrapper;
    gboolean visible;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg);

    if (GTK_IS_WIDGET (wrapper->obj))
    {
        visible = GTK_WIDGET_VISIBLE (wrapper->obj);
    }
    else
    {
        return send_error (ctx, "IsVisible() is not applicable to object <%s>",
                           wrapper->id);
    }

    send_bool_result (ctx, GAP_STATUS_OK, visible);
    return ms_value_none ();
}


static MSValue *
is_active_func (MSValue   *arg,
                MSContext *ctx)
{
    GapObject *wrapper;
    gboolean active;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg);

    if (GTK_IS_TOGGLE_BUTTON (wrapper->obj))
    {
        active = gtk_toggle_button_get_active (wrapper->obj);
    }
    else if (GTK_IS_CHECK_MENU_ITEM (wrapper->obj))
    {
        active = gtk_check_menu_item_get_active (wrapper->obj);
    }
    else
    {
        return send_error (ctx, "IsActive() is not applicable to object <%s>",
                           wrapper->id);
    }

    send_bool_result (ctx, GAP_STATUS_OK, active);
    return ms_value_none ();
}


static MSValue *
get_text_func (MSValue   *arg,
               MSContext *ctx)
{
    char *freeme = NULL;
    const char *text;
    GapObject *wrapper;
    MSValue *retval = NULL;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg);

    if (GTK_IS_EDITABLE (wrapper->obj))
    {
        freeme = gtk_editable_get_chars (wrapper->obj, 0, -1);
        text = freeme;
    }
    else if (GTK_IS_TEXT_BUFFER (wrapper->obj))
    {
        GtkTextIter start, end;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer (wrapper->obj);
        gtk_text_buffer_get_bounds (buffer, &start, &end);
        freeme = gtk_text_buffer_get_text (buffer, &start, &end, TRUE);
        text = freeme;
    }
    else
    {
        send_error (ctx, "GetText() is not applicable to object <%s>",
                    wrapper->id);
        goto out;
    }

    send_string_result (ctx, GAP_STATUS_OK, text, NULL);
    retval = ms_value_none ();

out:
    g_free (freeme);
    return retval;
}


static MSValue *
set_text_func (MSValue   *arg1,
               MSValue   *arg2,
               MSContext *ctx)
{
    char *text;
    GapObject *wrapper;
    MSValue *retval = NULL;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg1);
    text = ms_value_print (arg2);

    if (GTK_IS_ENTRY (wrapper->obj))
    {
        gtk_entry_set_text (wrapper->obj, text);
    }
    else if (GTK_IS_TEXT_VIEW (wrapper->obj))
    {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer (wrapper->obj);
        gtk_text_buffer_set_text (buffer, text, -1);
    }
    else if (GTK_IS_TEXT_BUFFER (wrapper->obj))
    {
        gtk_text_buffer_set_text (wrapper->obj, text, -1);
    }
    else
    {
        send_error (ctx, "GetText() is not applicable to object <%s>",
                    wrapper->id);
        goto out;
    }

    send_string_result (ctx, GAP_STATUS_OK, NULL, NULL);
    retval = ms_value_none ();

out:
    g_free (text);
    return retval;
}


static MSValue *
set_active_func (MSValue   *arg1,
                 MSValue   *arg2,
                 MSContext *ctx)
{
    gboolean active;
    GapObject *wrapper;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg1);
    active = ms_value_get_bool (arg2);

    if (GTK_IS_TOGGLE_BUTTON (wrapper->obj))
    {
        gtk_toggle_button_set_active (wrapper->obj, active);
    }
    else if (GTK_IS_CHECK_MENU_ITEM (wrapper->obj))
    {
        gtk_check_menu_item_set_active (wrapper->obj, active);
    }
    else
    {
        return send_error (ctx, "SetActive() is not applicable to object <%s>",
                           wrapper->id);
    }

    send_string_result (ctx, GAP_STATUS_OK, NULL, NULL);
    return ms_value_none ();
}


static MSValue *
gobject_func (MSValue   *arg,
              MSContext *ctx)
{
    GapObject *wrapper;
    CHECK_SESSION ();
    GET_OBJECT_BY_ID (wrapper, arg);
    return ms_value_object (wrapper);
}


static void
setup_gap_context (MSContext *ctx)
{
    MSFunc *func;

#define ADD_FUNC(func__,typ__,name__)           \
G_STMT_START {                                  \
    func = typ__ (func__);                      \
    ms_context_set_func (ctx, name__, func);    \
    g_object_unref (func);                      \
} G_STMT_END

    ADD_FUNC (set_stamp_func, ms_cfunc_new_1, "SetStamp");
    ADD_FUNC (get_stamp_func, ms_cfunc_new_0, "GetStamp");
    ADD_FUNC (destroy_func, ms_cfunc_new_1, "Destroy");
    ADD_FUNC (connect_func, ms_cfunc_new_3, "Connect");
    ADD_FUNC (disconnect_func, ms_cfunc_new_var, "Disconnect");
    ADD_FUNC (gobject_func, ms_cfunc_new_1, "GObject");

    ADD_FUNC (create_glade_window_func, ms_cfunc_new_2, "CreateGladeWindow");
    ADD_FUNC (glade_lookup_func, ms_cfunc_new_2, "GladeLookup");

    ADD_FUNC (run_dialog_func, ms_cfunc_new_1, "RunDialog");

    ADD_FUNC (show_func, ms_cfunc_new_1, "Show");
    ADD_FUNC (hide_func, ms_cfunc_new_1, "Hide");
    ADD_FUNC (is_visible_func, ms_cfunc_new_1, "IsVisible");
    ADD_FUNC (is_active_func, ms_cfunc_new_1, "IsActive");
    ADD_FUNC (get_text_func, ms_cfunc_new_1, "GetText");
    ADD_FUNC (set_text_func, ms_cfunc_new_2, "SetText");
    ADD_FUNC (set_active_func, ms_cfunc_new_2, "SetActive");
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
