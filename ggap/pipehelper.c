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
#include "pipehelper.h"


int main (int argc, char *argv[])
{
    HANDLE file;

    if (argc != 2)
    {
        fprintf (stderr, "usage: %s <file>\n", argv[0]);
        return 1;
    }

    if (!WaitNamedPipe (argv[1], NMPWAIT_WAIT_FOREVER))
    {
        fprintf (stderr, "could not open pipe '%s'\n", argv[1]);
        return 1;
    }

    file = CreateFile (argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (file == INVALID_HANDLE_VALUE)
    {
        fprintf (stderr, "could not open file '%s'\n", argv[1]);
        return 1;
    }

    while (1)
    {
        char buf[PIPEHELPER_BUFSIZE];
        DWORD bytes_read;
        unsigned len;

        if (!ReadFile (file, buf, PIPEHELPER_BUFSIZE, &bytes_read, NULL))
        {
            fprintf (stderr, "error in ReadFile\n");
            return 1;
        }

        if (bytes_read < PIPEHELPER_BUFSIZE)
        {
            fprintf (stderr, "got less bytes when asked\n");
            return 1;
        }

        memcpy (&len, buf, sizeof (len));

        if (len)
        {
            fwrite (buf + sizeof (len), 1, len, stdout);
            fflush (stdout);
        }
    }

    MessageBox (NULL, "bye", "bye",
                MB_ICONERROR | MB_APPLMODAL | MB_SETFOREGROUND);

    return 0;
}
