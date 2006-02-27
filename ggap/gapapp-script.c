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
#include "mooutils/mooscript/mooscript-context.h"


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


gpointer
gap_app_get_editor_context (MooWindow *window)
{
    MSContext *ctx;
    MSFunc *func;

    ctx = g_object_new (MS_TYPE_CONTEXT, "window", window, NULL);

    func = ms_cfunc_new_var (gap_func);
    ms_context_set_func (ctx, "GAP", func);
    g_object_unref (func);

    return ctx;
}


gpointer
gap_app_get_terminal_context (MooWindow *window)
{
    return gap_app_get_editor_context (window);
}
