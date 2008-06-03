/*
 *   gapprocess.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "gapprocess.h"
#include "gap.h"
#include "ggap-enums.h"
#include "marshals.h"
#include <mooterm/mootermpt.h>
#include <mooutils/mooutils-debug.h>
#include <mooutils/mooutils-misc.h>
#include <gdk/gdk.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

MOO_DEBUG_INIT(gap-out, FALSE)

typedef struct {
    GMainLoop *loop;
    gboolean success;
    gboolean destroyed;
    char *output;
    gboolean finished;
} GapCommandInfo;

typedef enum {
    INPUT_NORMAL,
    INPUT_MAYBE_MAGIC,
    INPUT_DATA_TYPE,
    INPUT_DATA_FIXED_LEN,
    INPUT_DATA_FIXED,
    INPUT_DATA_VAR,
    INPUT_MAYBE_DATA_END
} InputState;

typedef enum {
    INPUT_DATA_COMMAND,
    INPUT_DATA_OUTPUT
} InputDataType;

struct GapProcessPrivate {
    MooTermPt *pt;
    gboolean in_stderr;
    GapState gap_state;

    GString *input_buf;
    GString *input_buf2;
    gsize input_data_len;
    InputState input_state;

    GapCommandInfo *cmd_info;

    gboolean loaded;
};


static void     gap_process_dispose             (GObject        *object);

static void     stop_running_command_loop       (GapCommandInfo *ci);
static gboolean has_running_command_loop        (GapProcess     *proc);


/* GAP_TYPE_PROCESS */
G_DEFINE_TYPE (GapProcess, gap_process, G_TYPE_OBJECT)


enum {
    PROP_0,
    PROP_GAP_STATE
};

enum {
    GAP_DIED,
    GAP_PROMPT,
    GAP_OUTPUT,
    GAP_GLOBALS_CHANGED,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

static void
gap_process_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
    GapProcess *proc = GAP_PROCESS (object);

    switch (prop_id)
    {
        case PROP_GAP_STATE:
            g_value_set_enum (value, proc->priv->gap_state);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
gap_process_class_init (GapProcessClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->get_property = gap_process_get_property;
    object_class->dispose = gap_process_dispose;

    signals[GAP_DIED] =
        g_signal_new ("gap-died", GAP_TYPE_PROCESS,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (GapProcessClass, gap_died),
                      NULL, NULL,
                      _ggap_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    signals[GAP_OUTPUT] =
        g_signal_new ("gap-output", GAP_TYPE_PROCESS,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (GapProcessClass, gap_output),
                      NULL, NULL,
                      _ggap_marshal_VOID__POINTER_UINT_BOOL,
                      G_TYPE_NONE, 3,
                      G_TYPE_POINTER,
                      G_TYPE_UINT,
                      G_TYPE_BOOLEAN);

    signals[GAP_PROMPT] =
        g_signal_new ("gap-prompt", GAP_TYPE_PROCESS,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (GapProcessClass, gap_prompt),
                      NULL, NULL,
                      _ggap_marshal_VOID__POINTER_UINT_BOOL,
                      G_TYPE_NONE, 3,
                      G_TYPE_POINTER,
                      G_TYPE_UINT,
                      G_TYPE_BOOLEAN);

    signals[GAP_GLOBALS_CHANGED] =
        g_signal_new ("gap-globals-changed", GAP_TYPE_PROCESS,
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (GapProcessClass, gap_globals_changed),
                      NULL, NULL,
                      _ggap_marshal_VOID__POINTER_UINT_BOOL,
                      G_TYPE_NONE, 3,
                      G_TYPE_POINTER,
                      G_TYPE_UINT,
                      G_TYPE_BOOLEAN);

    g_object_class_install_property (object_class, PROP_GAP_STATE,
        g_param_spec_enum ("gap-state", "gap-state", "gap-state",
                           GAP_TYPE_STATE, GAP_DEAD, G_PARAM_READABLE));

    g_type_class_add_private (klass, sizeof (GapProcessPrivate));
}


static void
set_state (GapProcess *proc,
           GapState    state)
{
    if (proc->priv && proc->priv->gap_state != state)
    {
        proc->priv->gap_state = state;
        g_object_notify (G_OBJECT (proc), "gap-state");
    }
}


static void
gap_process_dispose (GObject *object)
{
    GapProcess *proc = GAP_PROCESS (object);

    if (proc->priv->pt)
    {
        if (moo_term_pt_child_alive (proc->priv->pt))
            moo_term_pt_kill_child (proc->priv->pt);
        g_object_unref (proc->priv->pt);
        proc->priv->pt = NULL;
    }

    if (proc->priv->input_buf)
    {
        g_string_free (proc->priv->input_buf, TRUE);
        proc->priv->input_buf = NULL;
    }

    if (proc->priv->input_buf2)
    {
        g_string_free (proc->priv->input_buf2, TRUE);
        proc->priv->input_buf2 = NULL;
    }

    G_OBJECT_CLASS (gap_process_parent_class)->dispose (object);
}

static void
gap_process_init (GapProcess *proc)
{
    proc->priv = G_TYPE_INSTANCE_GET_PRIVATE (proc, GAP_TYPE_PROCESS, GapProcessPrivate);

    proc->priv->input_buf = NULL;
    proc->priv->input_buf2 = NULL;
    proc->priv->input_data_len = 0;
    proc->priv->input_state = INPUT_NORMAL;

    proc->priv->pt = NULL;
    proc->priv->in_stderr = FALSE;

    proc->priv->gap_state = GAP_DEAD;
}


void
gap_process_set_size (GapProcess *proc,
                      int         width,
                      int         height)
{
    g_return_if_fail (GAP_IS_PROCESS (proc));

    if (proc->priv->pt)
        moo_term_pt_set_size (proc->priv->pt, width, height);
}


GapState
gap_process_get_state (GapProcess *proc)
{
    g_return_val_if_fail (GAP_IS_PROCESS (proc), GAP_DEAD);
    return proc->priv->gap_state;
}


static void
write_log (const char *text,
           int         len,
           gboolean    in)
{
MOO_DEBUG({
    static int last = -1;

    if (last != in)
    {
        if (last != -1)
            g_print ("\n");

        if (!in)
            g_print ("<-- ");
        else
            g_print ("--> ");
    }

    last = in;

    if (len < 0)
        len = strlen (text);

    while (len)
    {
        int i;

        for (i = 0; i < len && text[i] != '\n'; ++i) ;

        g_print ("%.*s", (int) i, text);

        if (i != len)
        {
            if (!in)
                g_print ("\n<-- ");
            else
                g_print ("\n--> ");
        }

        if (i < len)
        {
            len -= i + 1;
            text += i + 1;
        }
        else
        {
            len = 0;
            text += len;
        }
    }

    fflush (stdout);
}); /* MOO_DEBUG */
}

static void
write_child (GapProcess *proc,
             const char *text)
{
    moo_term_pt_write (proc->priv->pt, text, -1);
    write_log (text, -1, FALSE);
}

void
gap_process_write_input (GapProcess  *proc,
                         char       **lines)
{
    g_return_if_fail (GAP_IS_PROCESS (proc));
    g_return_if_fail (lines && *lines);

    if (lines[1])
    {
        char *string = g_strjoinv (" ", lines);
        write_child (proc, string);
        g_free (string);
    }
    else
    {
        write_child (proc, lines[0]);
    }

    write_child (proc, "\n");
    set_state (proc, GAP_BUSY);
}


static void
child_died (GapProcess *proc)
{
    g_object_unref (proc->priv->pt);
    proc->priv->pt = NULL;
    set_state (proc, GAP_DEAD);
    g_signal_emit_by_name (proc, "gap-died");
}


static void
do_normal_text (GapProcess *proc,
                const char *data,
                guint       data_len)
{
    if (has_running_command_loop (proc))
    {
        g_critical ("%s: oops", G_STRLOC);
        stop_running_command_loop (proc->priv->cmd_info);
    }

    if (proc->priv->loaded)
        g_signal_emit (proc, signals[GAP_OUTPUT], 0,
                       data, data_len, proc->priv->in_stderr);
}

static void
do_prompt (GapProcess *proc,
           const char *string,
           gsize       len)
{
    if (has_running_command_loop (proc))
    {
        g_critical ("%s: oops", G_STRLOC);
        stop_running_command_loop (proc->priv->cmd_info);
    }

    g_signal_emit (proc, signals[GAP_PROMPT], 0,
                   string, len, !proc->priv->loaded);

    proc->priv->loaded = TRUE;
    set_state (proc, GAP_IN_PROMPT);
}

static void
do_output (GapProcess *proc,
           const char *data,
           gsize       data_len)
{
    if (has_running_command_loop (proc))
    {
        proc->priv->cmd_info->output = g_strndup (data, data_len);
        proc->priv->cmd_info->success = TRUE;
        stop_running_command_loop (proc->priv->cmd_info);
    }
    else
    {
        g_critical ("%s: output: %.*s",
                    G_STRLOC, (int) data_len, data);
    }
}

static void
do_globals (GapProcess *proc,
            const char *data,
            guint       data_len,
            gboolean    added)
{
    g_signal_emit (proc, signals[GAP_GLOBALS_CHANGED], 0,
                   data, data_len, added);
}

static void
do_data (GapProcess *proc,
         const char *data,
         gsize       data_len)
{
    if (data_len >= strlen ("prompt:") && strncmp (data, "prompt:", strlen ("prompt:")) == 0)
        do_prompt (proc, data + strlen ("prompt:"), data_len - strlen ("prompt:"));
    else if (data_len >= strlen ("output:") && strncmp (data, "output:", strlen ("output:")) == 0)
        do_output (proc, data + strlen ("output:"), data_len - strlen ("output:"));
    else if (data_len >= strlen ("globals-added:") && strncmp (data, "globals-added:", strlen ("globals-added:")) == 0)
        do_globals (proc, data + strlen ("globals-added:"), data_len - strlen ("globals-added:"), TRUE);
    else if (data_len >= strlen ("globals-deleted:") && strncmp (data, "globals-deleted:", strlen ("globals-deleted:")) == 0)
        do_globals (proc, data + strlen ("globals-deleted:"), data_len - strlen ("globals-deleted:"), FALSE);
    else
    {
        g_critical ("%s: got unknown data: '%.*s'",
                    G_STRLOC, (int) data_len, data);
    }
}

#define MAGIC       "@GGAP@"
#define MAGIC_LEN   6
#define LENGTH_LEN  8

static void
read_chars_normal (GapProcess  *proc,
                   char const **data_p,
                   gsize       *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;
    gsize i;

    g_assert (!proc->priv->input_buf);

    for (i = 0; i < data_len; ++i)
    {
        if (data[i] == '@')
        {
            gsize n = MIN (MAGIC_LEN, data_len - i);

            if (!n || strncmp (data + i, MAGIC, n) == 0)
            {
                if (i > 0)
                    do_normal_text (proc, data, i);

                if (n == MAGIC_LEN)
                {
                    proc->priv->input_state = INPUT_DATA_TYPE;
                }
                else
                {
                    proc->priv->input_state = INPUT_MAYBE_MAGIC;
                    proc->priv->input_buf = g_string_new_len (data + i, n);
                }

                *data_p = data + i + n;
                *data_len_p = data_len - i - n;

                return;
            }
        }
    }

    do_normal_text (proc, data, data_len);
    *data_p = data + data_len;
    *data_len_p = 0;
}

static void
read_chars_maybe_magic (GapProcess  *proc,
                        char const **data_p,
                        gsize       *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;
    gsize n;

    n = MIN (data_len, MAGIC_LEN - proc->priv->input_buf->len);
    g_string_append_len (proc->priv->input_buf, data, n);
    *data_p = data + n;
    *data_len_p = data_len - n;

    if (strncmp (proc->priv->input_buf->str, MAGIC, proc->priv->input_buf->len) != 0)
    {
        GString *tmp = proc->priv->input_buf;
        proc->priv->input_buf = NULL;
        proc->priv->input_state = INPUT_NORMAL;
        do_normal_text (proc, tmp->str, tmp->len);
    }
    else if (proc->priv->input_buf->len == MAGIC_LEN)
    {
        g_string_free (proc->priv->input_buf, TRUE);
        proc->priv->input_buf = NULL;
        proc->priv->input_state = INPUT_DATA_TYPE;
    }
}

static void
read_chars_data_type (GapProcess  *proc,
                      char const **data_p,
                      gsize       *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;

    g_assert (proc->priv->input_state == INPUT_DATA_TYPE);
    g_assert (proc->priv->input_buf == 0);

    g_return_if_fail (data_len != 0);

    *data_p = data + 1;
    *data_len_p = data_len - 1;

    switch (data[0])
    {
        case GGAP_DTC_FIXED:
            proc->priv->input_state = INPUT_DATA_FIXED_LEN;
            proc->priv->input_buf = g_string_new (NULL);
            break;
        case GGAP_DTC_VARIABLE:
            proc->priv->input_state = INPUT_DATA_VAR;
            proc->priv->input_buf = g_string_new (NULL);
            break;
        default:
            g_critical ("%s: unknown data type '%c'", G_STRLOC, data[0]);
            proc->priv->input_state = INPUT_NORMAL;
            break;
    }
}

static gsize
get_length (const char *str)
{
    gulong n;
    char *end;

    errno = 0;
    n = strtoul (str, &end, 16);

    if (errno != 0 || end == NULL || *end != 0)
    {
        g_critical ("%s: could not convert '%s' to a number", G_STRLOC, str);
        return 0;
    }

    return n;
}

static void
read_chars_data_fixed_len (GapProcess  *proc,
                           char const **data_p,
                           gsize       *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;
    gsize n;

    n = MIN (data_len, LENGTH_LEN - proc->priv->input_buf->len);
    g_string_append_len (proc->priv->input_buf, data, n);
    *data_p = data + n;
    *data_len_p = data_len - n;

    if (proc->priv->input_buf->len == LENGTH_LEN)
    {
        gsize input_data_len = get_length (proc->priv->input_buf->str);

        if (input_data_len == 0)
        {
            proc->priv->input_state = INPUT_NORMAL;
            g_string_free (proc->priv->input_buf, TRUE);
            proc->priv->input_buf = NULL;
        }
        else
        {
            proc->priv->input_state = INPUT_DATA_FIXED;
            g_string_truncate (proc->priv->input_buf, 0);
            proc->priv->input_data_len = input_data_len;
        }
    }
}

static void
read_chars_data_fixed (GapProcess  *proc,
                       char const **data_p,
                       gsize       *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;
    gsize remain = proc->priv->input_data_len - proc->priv->input_buf->len;
    gsize n;

    n = MIN (remain, data_len);
    g_string_append_len (proc->priv->input_buf, data, n);
    *data_p = data + n;
    *data_len_p = data_len - n;

    if (proc->priv->input_buf->len == proc->priv->input_data_len)
    {
        GString *tmp = proc->priv->input_buf;
        proc->priv->input_buf = NULL;
        proc->priv->input_data_len = 0;
        proc->priv->input_state = INPUT_NORMAL;
        do_data (proc, tmp->str, tmp->len);
        g_string_free (tmp, TRUE);
    }
}

static void
read_chars_data_inf (GapProcess  *proc,
                     char const **data_p,
                     gsize       *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;
    gsize i;

    g_assert (proc->priv->input_buf != NULL);
    g_assert (proc->priv->input_buf2 == NULL);

    for (i = 0; i < data_len; ++i)
    {
        if (data[i] == '@')
        {
            gsize n = MIN (MAGIC_LEN, data_len - i);

            if (!n || strncmp (data + i, MAGIC, n) == 0)
            {
                if (i > 0)
                    g_string_append_len (proc->priv->input_buf, data, i);

                proc->priv->input_state = INPUT_MAYBE_DATA_END;
                proc->priv->input_buf2 = g_string_new_len (data + i, n);

                *data_p = data + i + n;
                *data_len_p = data_len - i - n;

                return;
            }
        }
    }

    g_string_append_len (proc->priv->input_buf, data, data_len);
    *data_p = data + data_len;
    *data_len_p = 0;
}

static void
read_chars_maybe_data_end (GapProcess  *proc,
                           char const **data_p,
                           gsize       *data_len_p)
{
    const char *data = *data_p;
    gsize data_len = *data_len_p;

    g_assert (proc->priv->input_buf != NULL);
    g_assert (proc->priv->input_buf2 != NULL);

    if (proc->priv->input_buf2->len < MAGIC_LEN)
    {
        gsize n = MIN (data_len, MAGIC_LEN - proc->priv->input_buf2->len);

        g_string_append_len (proc->priv->input_buf2, data, n);
        data += n;
        data_len -= n;
        *data_p = data;
        *data_len_p = data_len;

        if (proc->priv->input_buf2->len < MAGIC_LEN)
            return;

        if (strncmp (proc->priv->input_buf2->str, MAGIC, proc->priv->input_buf2->len) != 0)
        {
            g_string_append_len (proc->priv->input_buf,
                                 proc->priv->input_buf2->str,
                                 proc->priv->input_buf2->len);
            g_string_free (proc->priv->input_buf2, TRUE);
            proc->priv->input_buf2 = NULL;
            proc->priv->input_state = INPUT_DATA_VAR;
            return;
        }
    }

    if (data_len == 0)
        return;

    *data_p = data + 1;
    *data_len_p = data_len - 1;

    if (data[0] == GGAP_DTC_END)
    {
        GString *tmp;

        g_string_free (proc->priv->input_buf2, TRUE);
        proc->priv->input_buf2 = NULL;
        tmp = proc->priv->input_buf;
        proc->priv->input_buf = NULL;
        proc->priv->input_state = INPUT_NORMAL;

        do_data (proc, tmp->str, tmp->len);
        g_string_free (tmp, TRUE);
    }
    else
    {
        g_message ("%s: got '%c', expecting '%c'",
                   G_STRLOC, data[0], GGAP_DTC_END);
        g_string_append_len (proc->priv->input_buf,
                             proc->priv->input_buf2->str,
                             proc->priv->input_buf2->len);
        g_string_append_c (proc->priv->input_buf, data[0]);

        proc->priv->input_state = INPUT_DATA_VAR;

        g_string_free (proc->priv->input_buf2, TRUE);
        proc->priv->input_buf2 = NULL;
    }
}

static void
io_func (const char *buf,
         gsize       len,
         gpointer    data)
{
    GapProcess *proc = data;

    g_return_if_fail (len > 0);

    write_log (buf, len, TRUE);

    g_object_ref (proc);

    while (len != 0 && proc->priv != NULL)
    {
        switch (proc->priv->input_state)
        {
            case INPUT_NORMAL:
                read_chars_normal (proc, &buf, &len);
                break;

            case INPUT_MAYBE_MAGIC:
                read_chars_maybe_magic (proc, &buf, &len);
                break;
            case INPUT_DATA_TYPE:
                read_chars_data_type (proc, &buf, &len);
                break;

            case INPUT_DATA_FIXED_LEN:
                read_chars_data_fixed_len (proc, &buf, &len);
                break;
            case INPUT_DATA_FIXED:
                read_chars_data_fixed (proc, &buf, &len);
                break;

            case INPUT_DATA_VAR:
                read_chars_data_inf (proc, &buf, &len);
                break;
            case INPUT_MAYBE_DATA_END:
                read_chars_maybe_data_end (proc, &buf, &len);
                break;
        }
    }

    g_object_unref (proc);
}


GapProcess *
gap_process_start (const char   *cmd_line,
                   int           width,
                   int           height,
                   GError      **error)
{
    MooTermPt *pt;
    MooTermCommand *cmd;
    GapProcess *proc;

    g_return_val_if_fail (cmd_line != NULL, FALSE);

    proc = g_object_new (GAP_TYPE_PROCESS, NULL);
    pt = moo_term_pt_new (io_func, proc);
    moo_term_pt_set_echo_input (pt, FALSE);
    moo_term_pt_set_priority (pt, G_PRIORITY_HIGH);

    if (width > 0 && height > 0)
        moo_term_pt_set_size (pt, width, height);

    _moo_message ("starting GAP: %s", cmd_line);

    cmd = moo_term_command_new_command_line (cmd_line, NULL, NULL);

    if (!moo_term_pt_fork_command (pt, cmd, error))
    {
        g_object_unref (proc);
        g_object_unref (pt);
        return NULL;
    }

    proc->priv->pt = pt;
    g_signal_connect_swapped (pt, "child-died", G_CALLBACK (child_died), proc);
    set_state (proc, GAP_LOADING);

    moo_term_command_free (cmd);
    return proc;
}


void
gap_process_die (GapProcess *proc)
{
    g_return_if_fail (GAP_IS_PROCESS (proc));
    if (proc->priv->pt)
        moo_term_pt_kill_child (proc->priv->pt);
}


static void
stop_running_command_loop (GapCommandInfo *ci)
{
    if (g_main_loop_is_running (ci->loop))
        g_main_loop_quit (ci->loop);
}

static gboolean
has_running_command_loop (GapProcess *proc)
{
    return proc->priv && proc->priv->cmd_info &&
            proc->priv->cmd_info->loop &&
            g_main_loop_is_running (proc->priv->cmd_info->loop);
}

static void
run_command_destroy (G_GNUC_UNUSED GapProcess *proc,
                     GapCommandInfo *ci)
{
    ci->destroyed = TRUE;
    stop_running_command_loop (ci);
}

gboolean
gap_process_run_command (GapProcess  *proc,
                         const char  *command,
                         const char  *args,
                         const char  *gap_cmd_line,
                         char       **output)
{
    char *string;
    GapCommandInfo ci;
    gulong destroy_cb_id;

    if (output)
        *output = NULL;

    g_return_val_if_fail (GAP_IS_PROCESS (proc), FALSE);
    g_return_val_if_fail (proc->priv->cmd_info == NULL, FALSE);
    g_return_val_if_fail (command != NULL, FALSE);

    string = ggap_pkg_exec_command (command, args);
    write_child (proc, string);

    if (gap_cmd_line)
        write_child (proc, gap_cmd_line);

    g_object_ref (proc);

    proc->priv->cmd_info = &ci;
    ci.success = FALSE;
    ci.loop = NULL;
    ci.destroyed = FALSE;
    ci.output = NULL;

    destroy_cb_id = g_signal_connect (proc, "close",
                                      G_CALLBACK (run_command_destroy),
                                      &ci);

    ci.loop = g_main_loop_new (NULL, FALSE);

    gdk_threads_leave ();
    g_main_loop_run (ci.loop);
    gdk_threads_enter ();

    g_main_loop_unref (ci.loop);
    ci.loop = NULL;

    if (!ci.destroyed)
        g_signal_handler_disconnect (proc, destroy_cb_id);

    if (ci.destroyed)
        ci.success = FALSE;

    *output = ci.output;
    proc->priv->cmd_info = NULL;

    g_object_unref (proc);
    return ci.success;
}


void
gap_process_ask_completions (GapProcess *proc)
{
    g_return_if_fail (GAP_IS_PROCESS (proc));

    if (proc->priv->gap_state == GAP_IN_PROMPT)
    {
        char *string = ggap_pkg_exec_command ("get-globals", NULL);
        write_child (proc, string);
        g_free (string);
    }
}
