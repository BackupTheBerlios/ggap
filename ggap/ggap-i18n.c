/*
 *   ggap-i18n.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
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
