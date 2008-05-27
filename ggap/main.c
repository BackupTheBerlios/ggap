/*
 *   ggap/main.c
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

#include "gapapp.h"

int main (int argc, char *argv[])
{
    moo_app_main (gap_app_get_type, argc, argv);
}


#if defined(__WIN32__) && !defined(__GNUC__)

#include <windows.h>

int __stdcall
WinMain (HINSTANCE hInstance,
         HINSTANCE hPrevInstance,
         char     *lpszCmdLine,
         int       nCmdShow)
{
	return main (__argc, __argv);
}

#endif
