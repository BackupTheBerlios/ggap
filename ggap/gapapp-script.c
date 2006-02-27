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
#include "mooutils/mooscript/mooscript-zenity.h"


#define ADD_FUNC(ctx_, factory_, name_)         \
G_STMT_START {                                  \
    MSFunc *func_;                              \
    func_ = factory_ ();                        \
    ms_context_set_func (ctx_, name_, func_);   \
    g_object_unref (func_);                     \
} G_STMT_END;

#define ADD_CONSTANT(ctx_, func_, name_)        \
G_STMT_START {                                  \
    MSVariable *var_;                           \
    MSValue *val_;                              \
    val_ = func_ ();                            \
    var_ = ms_variable_new_value (val_);        \
    ms_context_set_var (ctx_, name_, var_);     \
    ms_variable_unref (var_);                   \
    ms_value_unref (val_);                      \
} G_STMT_END;


gpointer
gap_app_get_editor_context (MooWindow *window)
{
    MSContext *ctx;

    ctx = g_object_new (MS_TYPE_CONTEXT, "window", window, NULL);

    return ctx;
}


gpointer
gap_app_get_terminal_context (MooWindow *window)
{
    return gap_app_get_editor_context (window);
}
