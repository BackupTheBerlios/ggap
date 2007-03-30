/*
 *   pipehelper.c
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include "pipehelper.h"

#if PIPEHELPER_BUFSIZE > 256
#error "Oops"
#endif

#ifdef __GNUC__
#define ATTR_PRINTF __attribute__((__format__ (__printf__, 1, 2)))
#define UNUSED __attribute__((__unused__))
#else
#define ATTR_PRINTF
#define UNUSED
#endif

#if 0
ATTR_PRINTF static void
writelog (const char *format,
          ...)
{
    FILE *file;
    va_list vargs;
    time_t t;
    struct tm *tmbuf;
    char buf[1024];

    static int been_here = 0;

    if (!been_here)
    {
        been_here = 1;
        remove ("c:\\log.txt");
    }

    file = fopen ("c:\\log.txt", "a");

    if (!file)
        return;

    time (&t);
    tmbuf = localtime (&t);
    strftime (buf, sizeof buf, "%X", tmbuf);

    va_start (vargs, format);
    fprintf (file, "%s: ", buf);
    vfprintf (file, format, vargs);
    fprintf (file, "\n");
    va_end (vargs);

    fclose (file);
}
#else
static void
writelog (UNUSED const char *format,
          ...)
{
}
#endif

int main (int argc, char *argv[])
{
    HANDLE file;

    if (argc != 2)
    {
        writelog ("usage: %s <file>", argv[0]);
        exit (1);
    }

    if (!WaitNamedPipe (argv[1], NMPWAIT_WAIT_FOREVER))
    {
        writelog ("could not open pipe '%s'", argv[1]);
        exit (1);
    }

    file = CreateFile (argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (file == INVALID_HANDLE_VALUE)
    {
        writelog ("could not open file '%s'", argv[1]);
        exit (1);
    }

    writelog ("Opened pipe %s for reading", argv[1]);

    while (1)
    {
        unsigned char buf[PIPEHELPER_BUFSIZE];
        DWORD bytes_read;
        unsigned len;

        if (!ReadFile (file, buf, PIPEHELPER_BUFSIZE, &bytes_read, NULL))
        {
            writelog ("error in ReadFile");
            exit (1);
        }

        if (bytes_read < PIPEHELPER_BUFSIZE)
        {
            writelog ("got less bytes when asked");
            exit (1);
        }

        len = buf[0];
        writelog ("got %d bytes", len);

        if (len)
        {
            fwrite (buf + 1, 1, len, stdout);
            fflush (stdout);
        }
    }

    return 0;
}
