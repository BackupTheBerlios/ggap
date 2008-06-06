/*
 *   gapwscompletion.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "gapwscompletion.h"
#include "moows/moowsblock.h"


G_DEFINE_TYPE (GapWsCompletion, gap_ws_completion, MOO_TYPE_COMPLETION_SIMPLE)


static void
gap_ws_completion_replace_text (G_GNUC_UNUSED MooTextCompletion *cmpl,
                                GtkTextIter       *start,
                                GtkTextIter       *end,
                                const char        *text)
{
    MooWsBlock *block;

    block = _moo_ws_iter_get_block (start);
    g_return_if_fail (block != NULL);
    g_return_if_fail (block == _moo_ws_iter_get_block (end));

    if (!gtk_text_iter_equal (start, end))
        if (!_moo_ws_block_delete_interactive (block, start, end))
            return;

    if (text && text[0])
        _moo_ws_block_insert_interactive (block, start, text, -1);
}

static void
gap_ws_completion_class_init (GapWsCompletionClass *klass)
{
    MOO_TEXT_COMPLETION_CLASS(klass)->replace_text = gap_ws_completion_replace_text;
}

static void
gap_ws_completion_init (G_GNUC_UNUSED GapWsCompletion *cmpl)
{
}
