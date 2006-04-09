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
#include "mooscript/mooscript-context.h"


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
