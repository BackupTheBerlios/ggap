/*
 *   gapoutput.c
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __WIN32__
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else /* !__WIN32__ */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/poll.h>
#endif /* !__WIN32__ */
#include <gtk/gtk.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "gapoutput.h"
#include "pipehelper.h"


typedef struct
{
    guint        ref_count;

    char        *pipe_name;
    gboolean     ready;

#ifdef __WIN32__
    HANDLE       pipe;
#else
    int          in;
    int          out;
#endif
} GapAppOutput;

static GapAppOutput *output;

static gboolean output_start    (GapAppOutput *ch);
static void     output_shutdown (GapAppOutput *ch);
static void     output_write    (GapAppOutput *ch,
                                 const char   *data,
                                 gssize        len);


const char *
gap_app_output_get_name (void)
{
    if (output)
        return output->pipe_name;
    else
        return NULL;
}


gboolean
gap_app_output_start (void)
{
    g_return_val_if_fail (output == NULL, FALSE);

    output = g_new0 (GapAppOutput, 1);

#ifndef __WIN32__
    output->in = -1;
    output->out = -1;
#endif

    return output_start (output);
}


void
gap_app_output_shutdown (void)
{
    if (output)
    {
        output_shutdown (output);
        g_free (output);
        output = NULL;
    }
}


gboolean
gap_app_output_restart (void)
{
    g_return_val_if_fail (output != NULL, FALSE);
    output_shutdown (output);
    return output_start (output);
}


void
gap_app_output_write (const char *data,
                      gssize      len)
{
    g_return_if_fail (output != NULL);
    g_return_if_fail (data != NULL);
    output_write (output, data, len);
}


static void
output_shutdown (GapAppOutput *ch)
{
    g_return_if_fail (ch != NULL);

#ifdef __WIN32__
    if (ch->pipe)
    {
        DisconnectNamedPipe (ch->pipe);
        CloseHandle (ch->pipe);
        ch->pipe = NULL;
    }
#endif /* __WIN32__ */

#ifndef __WIN32__
    if (ch->out >= 0)
        close (ch->out);
    if (ch->in >= 0)
        close (ch->in);
    ch->out = -1;
    ch->in = -1;

    if (ch->pipe_name)
        unlink (ch->pipe_name);
#endif /* ! __WIN32__ */

    if (ch->pipe_name)
    {
        g_free (ch->pipe_name);
        ch->pipe_name = NULL;
    }

    ch->ready = FALSE;
}


/****************************************************************************/
/* WIN32
 */
#ifdef __WIN32__

static gboolean
output_start (GapAppOutput *ch)
{
    g_return_val_if_fail (!ch->ready, FALSE);

    ch->pipe_name = g_strdup_printf ("\\\\.\\pipe\\ggap_out_%ld",
                                     GetCurrentProcessId());
    ch->pipe = CreateNamedPipe (ch->pipe_name,
                                PIPE_ACCESS_DUPLEX,
                                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                                PIPE_UNLIMITED_INSTANCES,
                                0, 0, 200, NULL);

    if (ch->pipe == INVALID_HANDLE_VALUE)
    {
        char *msg = g_win32_error_message (GetLastError ());
        g_critical ("%s: %s", G_STRLOC, msg);
        g_free (msg);
        g_free (ch->pipe_name);
        ch->pipe_name = NULL;
        return FALSE;
    }

    g_message ("created output pipe %s", ch->pipe_name);
    ch->ready = TRUE;
    return TRUE;
}


#define DATASIZE (PIPEHELPER_BUFSIZE - 1)

#if PIPEHELPER_BUFSIZE > 256
#error "Oops"
#endif

static void
output_write (GapAppOutput *ch,
              const char   *data,
              gssize        len)
{
    g_return_if_fail (ch != NULL);
    g_return_if_fail (ch->ready);
    g_return_if_fail (data != NULL);

    if (len < 0)
        len = strlen (data);

    while (len)
    {
        unsigned chunk_len, data_len;
        guchar buf[PIPEHELPER_BUFSIZE];
        BOOL result;
        DWORD written;

        chunk_len = MIN ((guint) len, DATASIZE);
        buf[0] = len;
        memcpy (buf + 1, data, chunk_len);
        data_len = chunk_len + 1;

        result = WriteFile (ch->pipe, buf, PIPEHELPER_BUFSIZE, &written, NULL);

        if (!result)
        {
            char *msg = g_win32_error_message (GetLastError ());
            g_critical ("%s: %s", G_STRLOC, msg);
            g_free (msg);
            return;
        }
        else if (written < PIPEHELPER_BUFSIZE)
        {
            g_critical ("%s: oops", G_STRLOC);
            return;
        }

        data += chunk_len;
        len -= chunk_len;
    }

    g_message ("sent some data");
}

#endif /* __WIN32__ */


/****************************************************************************/
/* UNIX
 */
#ifndef __WIN32__

static char *
get_pipe_dir (void)
{
    GdkDisplay *display;
    char *display_name;
    char *user_name;
    char *name;
    const char *appname;

    appname = g_get_prgname ();
    g_return_val_if_fail (appname != NULL, NULL);

    display = gdk_display_get_default ();
    g_return_val_if_fail (display != NULL, NULL);

    display_name = g_strcanon (g_strdup (gdk_display_get_name (display)),
                               G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS,
                               '-');
    user_name = g_strcanon (g_strdup (g_get_user_name ()),
                            G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS,
                            '-');

    name = g_strdup_printf ("%s/%s-%s-%s", g_get_tmp_dir (), appname, user_name,
                            display_name[0] == '-' ? &display_name[1] : display_name);

    mkdir (name, S_IRWXU);

    g_free (display_name);
    g_free (user_name);
    return name;
}

static gboolean
output_start (GapAppOutput *ch)
{
    char *pipe_dir;

    g_return_val_if_fail (!ch->ready, FALSE);

    pipe_dir = get_pipe_dir ();
    g_return_val_if_fail (pipe_dir != NULL, FALSE);

    ch->pipe_name = g_strdup_printf ("%s/out-%ld", pipe_dir, (long) getpid ());
    unlink (ch->pipe_name);
    g_free (pipe_dir);

    if (mkfifo (ch->pipe_name, S_IRUSR | S_IWUSR))
    {
        perror ("mkfifo");
        goto error;
    }

    ch->in = open (ch->pipe_name, O_RDONLY | O_NONBLOCK);

    if (ch->in == -1)
    {
        perror ("open");
        goto error;
    }

    ch->out = open (ch->pipe_name, O_WRONLY | O_NONBLOCK);

    if (ch->out == -1)
    {
        perror ("open");
        goto error;
    }

    ch->ready = TRUE;
    return TRUE;

error:
    g_free (ch->pipe_name);
    ch->pipe_name = NULL;
    if (ch->in >= 0)
        close (ch->in);
    if (ch->out >= 0)
        close (ch->out);
    ch->in = -1;
    ch->out = -1;
    return FALSE;
}


static void
output_write (GapAppOutput   *ch,
              const char     *data,
              gssize          len)
{
    g_return_if_fail (ch != NULL && data != NULL);
    g_return_if_fail (ch->ready);

    if (!len)
        return;

    if (len < 0)
        len = strlen (data);

    /* XXX make a buffer, and so on */

    while (len)
    {
        gssize result = write (ch->out, data, len);

        if (result < 0)
        {
            int err = errno;

            switch (err)
            {
                case EAGAIN:
                    perror ("write");
                    return;

                case EINTR:
                    continue;

                default:
                    perror ("write");
                    return;
            }
        }
        else
        {
            data += result;
            len -= result;
        }
    }
}

#endif /* ! __WIN32__ */
