/*
 *   @(#)$Id: gapoutchannel.c,v 1.6 2005/05/31 23:57:53 emuntyan Exp $
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

#ifdef __WIN32__
#include <windows.h>
#include <io.h>
#endif /* __WIN32__ */

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif /* HAVE_ERRNO_H */
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif /* HAVE_SYS_STAT_H */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_SYS_POLL_H
#include <sys/poll.h>
#endif /* HAVE_SYS_POLL_H */

#include <string.h>

#include "gap/gapoutchannel.h"
#include "mooutils/moomarshals.h"
#include "mooutils/moocompat.h"

#define MAX_BUFFER_SIZE 4096


static void     gap_out_channel_class_init  (GapOutChannelClass *klass);
static void     gap_out_channel_init        (GapOutChannel      *ch);
static void     gap_out_channel_finalize    (GObject            *object);

static gboolean gap_out_channel_io_error    (GIOChannel         *source,
                                             GIOCondition        condition,
                                             GapOutChannel      *ch);


/* GAP_TYPE_OUT_CHANNEL */
G_DEFINE_TYPE (GapOutChannel, gap_out_channel, G_TYPE_OBJECT)
enum {
    IO_ERROR,
    LAST_SIGNAL
};
static guint signals[LAST_SIGNAL] = {0};


static void     gap_out_channel_class_init  (GapOutChannelClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->finalize = gap_out_channel_finalize;

    signals[IO_ERROR] = g_signal_new ("io-error",
                                      G_OBJECT_CLASS_TYPE (klass),
                                      G_SIGNAL_ACTION | G_SIGNAL_RUN_FIRST,
                                      G_STRUCT_OFFSET (GapOutChannelClass, io_error),
                                      NULL, NULL,
                                      _moo_marshal_VOID__VOID,
                                      G_TYPE_NONE, 0);
}


static void     gap_out_channel_init        (GapOutChannel      *ch)
{
#ifdef __WIN32__
    ch->pipe = INVALID_HANDLE_VALUE;
#else /* ! __WIN32__ */
    ch->pipe = -1;
#endif /* ! __WIN32__ */
    ch->pipe_name = NULL;
    ch->io = NULL;
    ch->io_watch = 0;
    ch->ready = FALSE;
}


static void     gap_out_channel_finalize    (GObject            *object)
{
    GapOutChannel *ch = GAP_OUT_CHANNEL (object);
    gap_out_channel_shutdown (ch);
}


void            gap_out_channel_shutdown    (GapOutChannel  *ch)
{
    if (ch->io_watch) {
        g_source_remove (ch->io_watch);
        ch->io_watch = 0;
    }
    if (ch->io) {
        g_io_channel_shutdown (ch->io, TRUE, NULL);
        g_io_channel_unref (ch->io);
        ch->io = NULL;
    }

#ifdef __WIN32__
    if (ch->pipe != INVALID_HANDLE_VALUE) {
        CloseHandle (ch->pipe);
        ch->pipe = INVALID_HANDLE_VALUE;
    }
#else /* !__WIN32__ */
    if (ch->pipe != -1) {
        close (ch->pipe);
        ch->pipe = -1;
    }
    if (ch->pipe_name) {
        unlink (ch->pipe_name);
        g_free (ch->pipe_name);
        ch->pipe_name = NULL;
    }
#endif /* !__WIN32__ */

    ch->ready = FALSE;
}


/****************************************************************************/
/* WIN32                                                                    */
/*                                                                          */

#ifdef __WIN32__

gboolean        gap_out_channel_start       (GapOutChannel  *ch)
{
    g_return_val_if_fail (!ch->ready, FALSE);

    if (!ch->pipe_name)
        ch->pipe_name =
                g_strdup_printf ("\\\\.\\pipe\\ggap_out_%ld", GetCurrentProcessId());

    ch->pipe = CreateNamedPipe (ch->pipe_name,
                                PIPE_ACCESS_DUPLEX,
                                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                                PIPE_UNLIMITED_INSTANCES,
                                0, 0, 200, NULL);

    if (ch->pipe == INVALID_HANDLE_VALUE) {
        g_critical ("%s: could not create output pipe", G_STRLOC);
        g_free (ch->pipe_name);
        ch->pipe_name = NULL;
        return FALSE;
    }
    else
        g_message ("%s: opened pipe %s", G_STRLOC, ch->pipe_name);

    ch->ready = TRUE;
    return TRUE;
}

#endif /* __WIN32__ */


/****************************************************************************/
/* UNIX                                                                     */
/***/
#ifndef __WIN32__

gboolean        gap_out_channel_start       (GapOutChannel  *ch)
{
    g_return_val_if_fail (GAP_IS_OUT_CHANNEL (ch), FALSE);
    g_return_val_if_fail (!ch->ready, TRUE);

    const char *name = gap_out_channel_get_name (ch);
    unlink (name);

    if (mkfifo (name, S_IRUSR | S_IWUSR)) {
        g_critical ("%s: error in mkfifo: %s", G_STRLOC, g_strerror (errno));
        return FALSE;
    }

    ch->pipe = open (name, O_RDWR | O_NONBLOCK);
    if (ch->pipe == -1) {
        g_critical ("%s: error in open: %s", G_STRLOC, g_strerror (errno));
        unlink (name);
        return FALSE;
    }

    ch->io = g_io_channel_unix_new (ch->pipe);
    g_io_channel_set_encoding (ch->io, NULL, NULL);
    ch->io_watch = g_io_add_watch (ch->io, G_IO_ERR | G_IO_HUP,
                                   (GIOFunc)gap_out_channel_io_error, ch);

    ch->ready = TRUE;
    return TRUE;
}


static gboolean gap_out_channel_io_error    (G_GNUC_UNUSED GIOChannel         *source,
                                             G_GNUC_UNUSED GIOCondition        condition,
                                             GapOutChannel      *ch)
{
    g_assert (source == ch->io && (condition & (G_IO_HUP | G_IO_ERR)));
    gap_out_channel_shutdown (ch);
    g_signal_emit_by_name (ch, "io-error", NULL);
    return FALSE;
}

#endif /* !__WIN32__ */



GapOutChannel  *gap_out_channel_new         (const char     *fifo_basename)
{
    GapOutChannel *ch = GAP_OUT_CHANNEL (
        g_object_new (GAP_TYPE_OUT_CHANNEL, NULL));

    if (fifo_basename)
#ifdef __WIN32__
        ch->pipe_name = g_strdup_printf ("\\\\.\\pipe\\%s", fifo_basename);
#else /* !__WIN32__ */
        ch->pipe_name = g_strdup_printf ("%s/%s", g_get_tmp_dir (), fifo_basename);
#endif /* !__WIN32__ */

    return ch;
}


const char     *gap_out_channel_get_name    (GapOutChannel  *ch)
{
    g_return_val_if_fail (GAP_IS_OUT_CHANNEL (ch), NULL);
    if (!ch->pipe_name) {
#ifdef __WIN32__
        ch->pipe_name = g_strdup_printf ("\\\\.\\pipe\\ggap_out.%ld", GetCurrentProcessId());
#else /* !__WIN32__ */
        ch->pipe_name = g_strdup_printf ("%s/ggap_out.%d", g_get_tmp_dir (), getpid ());
#endif /* !__WIN32__ */
    }
    return ch->pipe_name;
}


gboolean        gap_out_channel_restart     (GapOutChannel  *ch)
{
    g_return_val_if_fail (GAP_IS_OUT_CHANNEL (ch), FALSE);
    if (ch->ready) gap_out_channel_shutdown (ch);
    return gap_out_channel_start (ch);
}


gboolean        gap_out_channel_ready       (GapOutChannel  *ch)
{
    g_return_val_if_fail (GAP_IS_OUT_CHANNEL (ch), FALSE);
    return ch->ready;
}


gboolean        gap_out_channel_write       (GapOutChannel  *ch,
                                             const char     *data,
                                             gssize          len)
{
    g_return_val_if_fail (GAP_IS_OUT_CHANNEL (ch), FALSE);
    g_return_val_if_fail (ch->ready, FALSE);
    g_return_val_if_fail (data != NULL, FALSE);

    if (len < 0) len = strlen (data);
#ifndef __WIN32__
    return (write (ch->pipe, data, len) == len);
#else /* __WIN32__ */
    /* TODO TODO TODO */
    return FALSE;
#endif /* __WIN32__ */
}
