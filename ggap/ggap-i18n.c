/*
 *   ggap-i18n.c
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

#include "ggap-i18n.h"

#ifdef ENABLE_NLS

const char *
_ggap_gettext (const char *string)
{
    const char *translated;

    translated = gettext (string);

    if (translated == string)
        translated = D_(string, "moo");
    if (translated == string)
        translated = D_(string, "gtk20");

    return translated;
}

const char *
_ggap_Q_gettext (const char *string)
{
    const char *translated;

    translated = gettext (string);

    if (translated == string)
        translated = QD_(string, "moo");
    else
        translated = g_strip_context (string, translated);

    return translated;
}

#endif /* ENABLE_NLS */
