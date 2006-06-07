/*
 *   gap-script.c
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
#include "gap-script.h"
#include "gap-widgets.h"
#include "gapoutput.h"
#include <mooscript/mooscript-context.h>
#include <mooscript/mooscript-parser.h>
#include <mooutils/mooglade.h>
#include <mooutils/mooutils-misc.h>
#include <gtk/gtk.h>
#include <string.h>


#define STAMP_LEN 4


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
gap_data_add_list (GString *data,
                   guint    len)
{
    if (len >= 128 * 128)
    {
        g_critical ("%s: list too long", G_STRLOC);
        return;
    }

    g_string_append_c (data, GAP_DATA_LIST);
    g_string_append_c (data, len / 128);
    g_string_append_c (data, len % 128);
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
gap_data_add_tree_path (GString     *data,
                        GtkTreePath *path)
{
    if (path)
    {
        int depth, i;
        int *indices;

        depth = gtk_tree_path_get_depth (path);
        indices = gtk_tree_path_get_indices (path);

        gap_data_add_list (data, depth);

        for (i = 0; i < depth; ++i)
            gap_data_add_int (data, indices[i] + 1);
    }
    else
    {
        gap_data_add_list (data, 1);
        gap_data_add_small_int (data, 0);
    }
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
                             const char *signal,
                             guint       n_args)
{
    gap_data_add_command (data);
    gap_data_add_list (data, n_args + 2);
    gap_data_add_small_int (data, GAP_COMMAND_SIGNAL);
    gap_data_add_string (data, signal, -1);
}


static GtkTreePath *
tree_path_from_value (MSValue *val)
{
    guint i;
    GtkTreePath *ret;

    g_return_val_if_fail (val != NULL, NULL);
    g_return_val_if_fail (MS_VALUE_TYPE (val) == MS_VALUE_LIST, NULL);

    ret = gtk_tree_path_new ();

    for (i = 0; i < val->list.n_elms; ++i)
    {
        MSValue *elm = val->list.elms[i];

        if (MS_VALUE_TYPE (val) != MS_VALUE_INT)
        {
            gtk_tree_path_free (ret);
            g_return_val_if_reached (NULL);
        }

        gtk_tree_path_append_index (ret, elm->ival - 1);
    }

    return ret;
}


static gboolean
gap_data_add_boxed (GString *data,
                    GType    type,
                    gpointer value)
{
    if (type == GTK_TYPE_TREE_PATH)
    {
        gap_data_add_tree_path (data, value);
        return TRUE;
    }

    return FALSE;
}


static gboolean
gap_data_add_object (GString   *data,
                     gpointer   object)
{
    GapObject *wrapper = object;

    if (!GAP_IS_OBJECT (object))
    {
        wrapper = gap_session_add_object (GAP_APP_INSTANCE->session,
                                          object, NULL, FALSE);
        g_return_val_if_fail (wrapper != NULL, FALSE);
    }

    g_string_append_c (data, GAP_DATA_OBJECT);
    gap_data_add_string (data, wrapper->type, -1);
    gap_data_add_string (data, wrapper->id, -1);
    return TRUE;
}


static gboolean
gap_data_add_gvalue (GString      *data,
                     const GValue *value)
{
    int ival;
    long lval;
    const char *string;

    g_return_val_if_fail (G_IS_VALUE (value), FALSE);

    switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (value)))
    {
        case G_TYPE_CHAR:
            ival = g_value_get_char (value);
            gap_data_add_small_int (data, ival);
            return TRUE;

        case G_TYPE_UCHAR:
            ival = g_value_get_uchar (value);
            gap_data_add_small_int (data, ival);
            return TRUE;

        case G_TYPE_BOOLEAN:
            gap_data_add_bool (data, g_value_get_boolean (value));
            return TRUE;

        case G_TYPE_INT:
            lval = g_value_get_int (value);
            gap_data_add_int (data, ival);
            return TRUE;
        case G_TYPE_UINT:
            lval = g_value_get_uint (value);
            gap_data_add_int (data, ival);
            return TRUE;
        case G_TYPE_LONG:
            lval = g_value_get_long (value);
            gap_data_add_int (data, ival);
            return TRUE;
        case G_TYPE_ULONG:
            lval = g_value_get_ulong (value);
            gap_data_add_int (data, ival);
            return TRUE;

        case G_TYPE_STRING:
            string = g_value_get_string (value);
            if (string)
                gap_data_add_string (data, string, -1);
            else
                gap_data_add_none (data);
            return TRUE;

        case G_TYPE_BOXED:
            return gap_data_add_boxed (data,
                                       G_VALUE_TYPE (value),
                                       g_value_get_boxed (value));

        case G_TYPE_OBJECT:
            return gap_data_add_object (data, g_value_get_object (value));
    }

    g_return_val_if_reached (FALSE);
}


/**************************************************************************/
/* GAP <-> GGAP communication
 */


static gpointer ctx_send_error (MSContext  *ctx,
                                const char *format,
                                ...) G_GNUC_PRINTF (2, 3);


void
gap_data_send (GString *data)
{
    g_return_if_fail (data != NULL);
    gap_app_output_write (data->str, data->len);
}


static GString *
make_result_data (MSContext  *ctx,
                  char        status,
                  guint       n_args)
{
    const char *stamp;
    GString *data;

    stamp = g_object_get_data (G_OBJECT (ctx), "gap-stamp");
    g_return_val_if_fail (stamp != NULL, g_string_new (NULL));

    data = g_string_new (NULL);

    if (!n_args)
        gap_data_add_pair (data);
    else if (n_args == 1)
        gap_data_add_triple (data);
    else
        gap_data_add_list (data, n_args + 2);

    gap_data_add_string (data, stamp, -1);
    gap_data_add_small_int (data, status);

    return data;
}


// static void
// send_bool_result (MSContext  *ctx,
//                   char        status,
//                   gboolean    val)
// {
//     GString *data;
//
//     data = make_result_data (ctx, status, 1);
//     gap_data_add_bool (data, val);
//
//     gap_data_send (data);
//     g_string_free (data, TRUE);
// }


static void
send_int_result (MSContext  *ctx,
                 char        status,
                 int         val)
{
    GString *data;

    data = make_result_data (ctx, status, 1);
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
    GString *data;
    guint n_args = 0;

    g_return_if_fail (string1 || !string2);

    if (string1)
        n_args++;
    if (string2)
        n_args++;

    data = make_result_data (ctx, status, n_args);

    if (string1)
        gap_data_add_string (data, string1, -1);
    if (string2)
        gap_data_add_string (data, string2, -1);

    gap_data_send (data);
    g_string_free (data, TRUE);
}


// static void
// send_list_result (MSContext  *ctx,
//                   char        status,
//                   MSValue   **elms,
//                   guint       n_elms)
// {
//     GString *data;
//     guint i;
//
//     g_return_if_fail (!n_elms || elms);
//
//     data = make_result_data (ctx, status, 1);
//
//     gap_data_add_list (data, n_elms);
//
//     for (i = 0; i < n_elms; ++i)
//     {
//         char *string = ms_value_print (elms[i]);
//         gap_data_add_string (data, string, -1);
//         g_free (string);
//     }
//
//     gap_data_send (data);
//     g_string_free (data, TRUE);
// }


static gboolean
ctx_error_sent (MSContext *ctx)
{
    return g_object_get_data (G_OBJECT (ctx), "gap-context-error-sent") != NULL;
}


static gpointer
ctx_send_error (MSContext  *ctx,
                const char *format,
                ...)
{
    va_list var_args;
    char *message;
    GString *data;

    g_return_val_if_fail (format != NULL, NULL);
    g_return_val_if_fail (!ctx_error_sent (ctx), NULL);

    va_start (var_args, format);
    message = g_strdup_vprintf (format, var_args);
    va_end (var_args);

    data = make_result_data (ctx, GAP_STATUS_ERROR, 1);
    gap_data_add_string (data, message, -1);

    gap_data_send (data);
    ms_context_set_error (ctx, MS_ERROR_RUNTIME, message);
    g_object_set_data (G_OBJECT (ctx), "gap-context-error-sent",
                       GINT_TO_POINTER (TRUE));

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
        ctx_send_error (ctx, "object <%s> doesn't exist", id_);         \
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
                          MSValue   *arg3,
                          MSContext *ctx)
{
    GapObject *wrapper;
    GtkWidget *window;
    char *file = NULL, *root = NULL;
    MooGladeXML *xml;
    MSValue *retval = NULL;
    GSList *callbacks = NULL;

    CHECK_SESSION ();

    if (ms_value_is_none (arg2))
        arg2 = NULL;
    if (ms_value_is_none (arg3))
        arg3 = NULL;

    if (arg3 && MS_VALUE_TYPE (arg3) != MS_VALUE_DICT)
    {
        char *s = ms_value_print (arg3);
        ctx_send_error (ctx, "invalid argument '%s'", s);
        g_free (s);
        goto out;
    }

    file = ms_value_print (arg1);

    if (arg2)
        root = ms_value_print (arg2);

    if (!gap_glade_xml_new (file, root, arg3, &xml, &callbacks))
    {
        ctx_send_error (ctx, "Error loading glade file");
        goto out;
    }

    window = moo_glade_xml_get_root (xml);
    g_return_val_if_fail (window != NULL, NULL);
//     gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (GAP_APP_INSTANCE->term_window));

    wrapper = gap_session_add_object (GAP_APP_INSTANCE->session, window,
                                      "GladeWindow", TRUE);
    g_object_set_data_full (G_OBJECT (window), "moo-glade-xml", xml, g_object_unref);

    if (!wrapper)
    {
        ctx_send_error (ctx, "oops");
    }
    else
    {
        send_string_result (ctx, GAP_STATUS_OK, wrapper->id, NULL);
        retval = ms_value_none ();
    }

out:
    g_slist_foreach (callbacks, (GFunc) gap_callback_free, NULL);
    g_slist_free (callbacks);
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
tree_view_row_activated (GapObject   *wrapper,
                         GtkTreePath *path)
{
    GString *data;

    data = g_string_new (NULL);
    gap_data_add_command_signal (data, "row-activated", 2);
    gap_data_add_object (data, wrapper);
    gap_data_add_tree_path (data, path);

    gap_data_send (data);
    g_string_free (data, TRUE);
}


static gulong
connect_special_signal (GapObject  *wrapper,
                        const char *signal)
{
    if (!strcmp (signal, "row-activated") && GAP_IS_TREE_VIEW (wrapper->obj))
        return g_signal_connect_swapped (wrapper->obj, "row-activated",
                                         G_CALLBACK (tree_view_row_activated),
                                         wrapper);
//     if (!strcmp (signal, "selection-changed") && GAP_IS_TREE_VIEW (wrapper->obj))
//         return g_signal_connect_swapped (wrapper->obj, "selection-changed",
//                                          G_CALLBACK (tree_view_selection_changed),
//                                          wrapper);
    return 0;
}


static void
gap_callback_marshal (GClosure      *closure,
                      G_GNUC_UNUSED GValue *return_value,
                      guint          n_param_values,
                      const GValue  *param_values)
{
    GString *data;
    GapObject *wrapper;
    guint i;

    g_return_if_fail (n_param_values > 0);

    wrapper = gap_session_find_wrapper (GAP_APP_INSTANCE->session,
                                        g_value_get_object (&param_values[0]));
    g_return_if_fail (GAP_IS_OBJECT (wrapper));

    data = g_string_new (NULL);
    gap_data_add_command_signal (data, closure->data, n_param_values);

    for (i = 0; i < n_param_values; ++i)
    {
        if (!gap_data_add_gvalue (data, &param_values[i]))
        {
            g_warning ("%s: could not pass value of type '%s' to GAP",
                       G_STRLOC, g_type_name (G_VALUE_TYPE (&param_values[i])));
            g_string_free (data, TRUE);
            return;
        }
    }

    gap_data_send (data);
    g_string_free (data, TRUE);
}


static gulong
do_connect_signal (GapObject  *wrapper,
                   const char *signal,
                   char      **error_message)
{
    GClosure *closure;
    gulong handler_id;
    guint signal_id;
    GSignalQuery query;
    char *signal_copy;

    if ((handler_id = connect_special_signal (wrapper, signal)))
        return handler_id;

    signal_id = g_signal_lookup (signal, G_OBJECT_TYPE (wrapper->obj));

    if (!signal_id)
    {
        *error_message = g_strdup_printf ("No signal '%s' for object '%s'",
                                          signal, wrapper->id);
        return 0;
    }

    g_signal_query (signal_id, &query);
    query.return_type &= ~G_SIGNAL_TYPE_STATIC_SCOPE;

    if (query.return_type != G_TYPE_NONE)
    {
        *error_message = g_strdup_printf ("Can't connect signal '%s' for object '%s'",
                                          signal, wrapper->id);
        return 0;
    }

    signal_copy = g_strdup (signal);
    closure = g_closure_new_simple (sizeof (GClosure), signal_copy);
    g_closure_sink (g_closure_ref (closure));
    g_closure_add_finalize_notifier (closure, signal_copy, (GClosureNotify) g_free);

    g_closure_set_marshal (closure, (GClosureMarshal) gap_callback_marshal);
    handler_id = g_signal_connect_closure (wrapper->obj, signal, closure, FALSE);

    if (!handler_id)
        *error_message = g_strdup ("Error in g_signal_connect_closure");

    g_closure_unref (closure);
    return handler_id;
}


static MSValue *
connect_func (MSValue   *arg1,
              MSValue   *arg2,
              MSValue   *arg3,
              MSContext *ctx)
{
    GapObject *wrapper;
    char *signal, *gap_id, *msg = NULL;
    char *norm_signal;
    gulong handler_id;
    MSValue *retval = NULL;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg1);
    signal = ms_value_print (arg2);
    gap_id = ms_value_print (arg3);

    norm_signal = g_strdelimit (g_strdup (signal), "_", '-');
    handler_id = do_connect_signal (wrapper, norm_signal, &msg);

    if (!handler_id)
    {
        ctx_send_error (ctx, "%s", msg);
        goto out;
    }

    gap_object_connect (wrapper, gap_id, handler_id);
    send_string_result (ctx, GAP_STATUS_OK, NULL, NULL);
    retval = ms_value_none ();

out:
    g_free (signal);
    g_free (norm_signal);
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


static void
set_one_property (const char *prop,
                  MSValue    *value,
                  gpointer    user_data)
{
    char *msg = NULL;

    struct {
        gboolean error;
        GapObject *wrapper;
        MSContext *ctx;
    } *data = user_data;

    if (data->error)
        return;

    if (!gap_set_property (data->wrapper->obj, prop, value, &msg))
    {
        if (msg)
            ms_context_set_error (data->ctx, MS_ERROR_RUNTIME, msg);
        else
            ms_context_format_error (data->ctx, MS_ERROR_RUNTIME,
                                     "could not set property '%s' of object <%s>",
                                     prop, data->wrapper->id);
        g_free (msg);
        data->error = TRUE;
    }
}

static MSValue *
set_property_func (MSValue   *obj,
                   MSValue   *dict,
                   MSContext *ctx)
{
    GapObject *wrapper;

    struct {
        gboolean error;
        GapObject *wrapper;
        MSContext *ctx;
    } data = {FALSE, NULL, ctx};

    CHECK_SESSION ();
    GET_OBJECT (wrapper, obj);

    if (MS_VALUE_TYPE (dict) != MS_VALUE_DICT)
        return ms_context_set_error (ctx, MS_ERROR_VALUE,
                                     "Invalid argument in SetProperty");

    data.wrapper = wrapper;
    g_hash_table_foreach (dict->hash, (GHFunc) set_one_property, &data);

    if (!data.error)
        send_string_result (ctx, GAP_STATUS_OK, NULL, NULL);

    return data.error ? NULL : ms_value_none ();
}


static MSValue *
get_property_func (MSValue   *arg1,
                   MSValue   *arg2,
                   MSContext *ctx)
{
    GapObject *wrapper;
    char *property, *msg = NULL;
    GValue value;
    GString *data;

    CHECK_SESSION ();
    GET_OBJECT (wrapper, arg1);

    property = ms_value_print (arg2);
    value.g_type = 0;

    if (!gap_get_property (wrapper->obj, property, &value, &msg))
    {
        if (msg)
            ms_context_set_error (ctx, MS_ERROR_RUNTIME, msg);
        else
            ms_context_format_error (ctx, MS_ERROR_RUNTIME,
                                     "GetProperty: could not get value of property '%s' for object <%s>",
                                     property, wrapper->id);
        g_free (property);
        g_free (msg);
        return NULL;
    }

    data = make_result_data (ctx, GAP_STATUS_OK, 1);

    if (!gap_data_add_gvalue (data, &value))
    {
        ms_context_format_error (ctx, MS_ERROR_RUNTIME,
                                 "GetProperty: property '%s' for object <%s> is not supported",
                                 property, wrapper->id);
        g_string_free (data, TRUE);
        g_free (property);
        g_value_unset (&value);
        return NULL;
    }

    gap_data_send (data);
    g_string_free (data, TRUE);
    g_free (property);
    g_value_unset (&value);

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
        ctx_send_error (ctx, "object <%s> is not a glade object", wrapper->id);
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
        ctx_send_error (ctx, "oops");
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
        ctx_send_error (ctx, "object <%s> is not a dialog", wrapper->id);
        return NULL;
    }

    response = gtk_dialog_run (wrapper->obj);
    send_int_result (ctx, GAP_STATUS_OK, response);
    return ms_value_none ();
}


static MSValue *
run_dialog_message_func (MSValue   *arg_type,
                         MSValue   *arg_text,
                         MSValue   *arg_secondary,
                         MSValue   *params,
                         MSContext *ctx)
{
    int response, type, default_response = G_MAXINT;
    GtkButtonsType buttons = GTK_BUTTONS_CLOSE;
    char *primary = NULL, *secondary = NULL;
    char *title = NULL;
    GtkWidget *dialog;
    GtkWindow *parent = moo_get_toplevel_window ();;

    CHECK_SESSION ();

    if (!ms_value_get_int (arg_type, &type))
        return ctx_send_error (ctx, "RunDialogMessage: first argument must be an integer");

    if (!ms_value_is_none (params) && MS_VALUE_TYPE (params) != MS_VALUE_DICT)
        return ctx_send_error (ctx, "RunDialogMessage: params must be fail or a record");

    primary = ms_value_print (arg_text);

    if (!ms_value_is_none (arg_secondary))
        secondary = ms_value_print (arg_secondary);

    if (!ms_value_is_none (params))
    {
        MSValue *val;

        if ((val = ms_value_dict_get_elm (params, "title")))
        {
            title = ms_value_print (val);
            ms_value_unref (val);
        }

        if ((val = ms_value_dict_get_elm (params, "buttons")))
        {
            if (MS_VALUE_TYPE (val) == MS_VALUE_INT)
            {
                buttons = val->ival;
            }
            else
            {
                char *string = ms_value_print (val);
                g_warning ("%s: invalid buttons value: %s", G_STRLOC, string);
                g_free (string);
            }

            ms_value_unref (val);
        }

        if ((val = ms_value_dict_get_elm (params, "parent")))
        {
            GapObject *wrapper = ms_value_get_object (val);

            if (wrapper && GTK_IS_WINDOW (wrapper->obj))
            {
                parent = GTK_WINDOW (wrapper->obj);
            }
            else
            {
                g_warning ("%s: invalid parent value", G_STRLOC);
            }

            ms_value_unref (val);
        }

        if ((val = ms_value_dict_get_elm (params, "default")))
        {
            if (MS_VALUE_TYPE (val) == MS_VALUE_INT)
            {
                default_response = val->ival;
            }
            else
            {
                char *string = ms_value_print (val);
                g_warning ("%s: invalid default response: %s", G_STRLOC, string);
                g_free (string);
            }

            ms_value_unref (val);
        }
    }

    dialog = gtk_message_dialog_new (parent, 0, type,
                                     buttons, "%s", primary);
    g_return_val_if_fail (dialog != NULL, NULL);

    if (title)
        gtk_window_set_title (GTK_WINDOW (dialog), title);

    if (secondary)
        gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
                                                  "%s", secondary);

    if (default_response < G_MAXINT)
        gtk_dialog_set_default_response (GTK_DIALOG (dialog),
                                         default_response);

    response = gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);

    send_int_result (ctx, GAP_STATUS_OK, response);

    g_free (primary);
    g_free (secondary);
    g_free (title);
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


static MSValue *
get_selected_row_func (MSValue   *arg,
                       MSContext *ctx)
{
    GapObject *wrapper;
    GString *data;
    GtkTreePath *path;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg);

    if (!GAP_IS_TREE_VIEW (wrapper->obj))
    {
        return ctx_send_error (ctx, "GetSelectedRow is not applicable to object <%s>",
                               wrapper->id);
    }

    path = gap_tree_view_get_selected_row (wrapper->obj);
    data = make_result_data (ctx, GAP_STATUS_OK, 1);
    gap_data_add_tree_path (data, path);
    gap_data_send (data);

    g_string_free (data, TRUE);
    gtk_tree_path_free (path);
    return ms_value_none ();
}


static MSValue *
select_row_func (MSValue   *arg1,
                 MSValue   *arg2,
                 MSContext *ctx)
{
    GapObject *wrapper;
    GtkTreePath *path;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg1);
    path = tree_path_from_value (arg2);

    if (!GAP_IS_TREE_VIEW (wrapper->obj))
        return ctx_send_error (ctx, "SelectRow is not applicable to object <%s>",
                               wrapper->id);
    if (!path)
        return ctx_send_error (ctx, "SelectRow: invalid argument");

    gap_tree_view_select_row (wrapper->obj, path);
    send_string_result (ctx, GAP_STATUS_OK, NULL, NULL);

    gtk_tree_path_free (path);
    return ms_value_none ();
}


static MSValue *
unselect_row_func (MSValue   *arg1,
                   MSValue   *arg2,
                   MSContext *ctx)
{
    GapObject *wrapper;
    GtkTreePath *path;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg1);
    path = tree_path_from_value (arg2);

    if (!GAP_IS_TREE_VIEW (wrapper->obj))
        return ctx_send_error (ctx, "UnselectRow is not applicable to object <%s>",
                               wrapper->id);
    if (!path)
        return ctx_send_error (ctx, "UnselectRow: invalid argument");

    gap_tree_view_unselect_row (wrapper->obj, path);
    send_string_result (ctx, GAP_STATUS_OK, NULL, NULL);

    gtk_tree_path_free (path);
    return ms_value_none ();
}


static MSValue *
select_all_rows_func (MSValue   *arg,
                      MSContext *ctx)
{
    GapObject *wrapper;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg);

    if (!GAP_IS_TREE_VIEW (wrapper->obj))
        return ctx_send_error (ctx, "SelectAllRows is not applicable to object <%s>",
                               wrapper->id);

    gap_tree_view_select_all (wrapper->obj);
    send_string_result (ctx, GAP_STATUS_OK, NULL, NULL);

    return ms_value_none ();
}


static MSValue *
unselect_all_rows_func (MSValue   *arg,
                        MSContext *ctx)
{
    GapObject *wrapper;

    CHECK_SESSION ();

    GET_OBJECT (wrapper, arg);

    if (!GAP_IS_TREE_VIEW (wrapper->obj))
        return ctx_send_error (ctx, "UnselectAllRows is not applicable to object <%s>",
                               wrapper->id);

    gap_tree_view_unselect_all (wrapper->obj);
    send_string_result (ctx, GAP_STATUS_OK, NULL, NULL);

    return ms_value_none ();
}


static void
setup_gap_context (MSContext  *ctx,
                   const char *stamp)
{
    MSFunc *func;

    g_object_set_data_full (G_OBJECT (ctx), "gap-stamp",
                            g_strndup (stamp, STAMP_LEN),
                            g_free);

#define ADD_FUNC(func__,typ__,name__)           \
G_STMT_START {                                  \
    func = typ__ (func__);                      \
    ms_context_set_func (ctx, name__, func);    \
    g_object_unref (func);                      \
} G_STMT_END

    ADD_FUNC (destroy_func, ms_cfunc_new_1, "GapDestroy");
    ADD_FUNC (connect_func, ms_cfunc_new_3, "GapConnect");
    ADD_FUNC (disconnect_func, ms_cfunc_new_var, "GapDisconnect");
    ADD_FUNC (gobject_func, ms_cfunc_new_1, "GapGObject");
    ADD_FUNC (set_property_func, ms_cfunc_new_2, "GapSetProperty");
    ADD_FUNC (get_property_func, ms_cfunc_new_2, "GapGetProperty");

    ADD_FUNC (create_glade_window_func, ms_cfunc_new_3, "GapCreateGladeWindow");
    ADD_FUNC (glade_lookup_func, ms_cfunc_new_2, "GapGladeLookup");

    ADD_FUNC (run_dialog_func, ms_cfunc_new_1, "GapRunDialog");
    ADD_FUNC (run_dialog_message_func, ms_cfunc_new_4, "GapRunDialogMessage");

    ADD_FUNC (get_selected_row_func, ms_cfunc_new_1, "GapGetSelectedRow");
    ADD_FUNC (select_row_func, ms_cfunc_new_2, "GapSelectRow");
    ADD_FUNC (unselect_row_func, ms_cfunc_new_2, "GapUnselectRow");
    ADD_FUNC (select_all_rows_func, ms_cfunc_new_1, "GapSelectAllRows");
    ADD_FUNC (unselect_all_rows_func, ms_cfunc_new_1, "GapUnselectAllRows");
}


void
gap_app_exec_command (GapApp     *app,
                      const char *data,
                      guint       len)
{
    MSContext *ctx;
    MSNode *node;
    char *script;

    if (len < STAMP_LEN)
    {
        g_critical ("%s: could not get stamp", G_STRLOC);
        return;
    }

    {
        char *stamp = g_strndup (data, STAMP_LEN);
        g_print ("gap_app_exec_command: %s\n%s\n-----\n",
                 stamp, data + STAMP_LEN);
        g_free (stamp);
    }

    script = g_strdup (data + STAMP_LEN);
    ctx = ms_context_new (app->term_window);
    setup_terminal_context (ctx);
    setup_gap_context (ctx, data);

    node = ms_script_parse (script);

    if (!node)
    {
        g_critical ("%s: oops", G_STRLOC);
        ctx_send_error (ctx, "internal GGAP error: could not parse script");
    }
    else
    {
        MSValue *val = ms_top_node_eval (node, ctx);

        if (!val)
        {
            g_warning ("%s", ms_context_get_error_msg (ctx));

            if (!ctx_error_sent (ctx))
                ctx_send_error (ctx, "internal GGAP error: %s",
                                ms_context_get_error_msg (ctx));
        }

        ms_node_unref (node);
        ms_value_unref (val);
    }

    g_free (script);
    g_object_unref (ctx);
}
