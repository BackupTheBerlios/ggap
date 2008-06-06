/*
 *   moows-file.c
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#include "moows-file.h"
#include "moows-private.h"
#include "moowspromptblock.h"
#include "moowstextblock.h"
#include "mooutils/mooutils-misc.h"
#include "mooutils/mooutils-gobject.h"
#include "mooutils/moomarkup.h"

#define MOO_WORKSHEET_FILE_ERROR (g_quark_from_static_string ("moo-worksheet-file-error"))
#define MOO_WORKSHEET_FILE_ERROR_FORMAT 0

#define ELM_WORKSHEET   "ggap-worksheet"
#define ELM_CONTENT     "content"
#define ELM_INPUT       "input"
#define ELM_OUTPUT      "output"

#define PROP_VERSION    "version"
#define PROP_VERSION_VALUE "1.0"

#define PROP_PS         "ps"
#define PROP_PS2        "ps2"
#define PROP_TYPE       "type"

#define PROP_OUTPUT_TYPE_STDOUT "out"
#define PROP_OUTPUT_TYPE_STDERR "err"


static MooWsBuffer *
get_buffer (MooWorksheet *ws)
{
    return MOO_WS_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (ws)));
}


static void
load_input (MooWorksheet  *ws,
            MooMarkupNode *elm)
{
    const char *ps = NULL, *ps2 = NULL;
    MooWsBlock *block;

    ps = moo_markup_get_prop (elm, PROP_PS);
    ps2 = moo_markup_get_prop (elm, PROP_PS2);

    if (!ps)
        g_critical ("%s: %s property missing", G_STRLOC, PROP_PS);
    if (!ps2)
        g_critical ("%s: %s property missing", G_STRLOC, PROP_PS2);

    block = moo_worksheet_create_prompt_block (ws, ps, ps2, moo_markup_get_content (elm));
    g_return_if_fail (block != NULL);

    moo_ws_buffer_append_block (get_buffer (ws), block);
}

static MooWsBlock *
create_output_block (MooWsOutputType out_type)
{
    MooWsTextBlock *block;

    block = moo_ws_text_block_new ();

    if (out_type == MOO_WS_OUTPUT_ERR)
    {
        g_object_set_data (G_OBJECT (block), "moo-worksheet-stderr",
                           GINT_TO_POINTER (TRUE));
        g_object_set (MOO_WS_BLOCK (block)->tag,
                      "foreground", "red",
#if 0
                      "background", "green",
#endif
                      NULL);
    }
    else
    {
#if 0
        g_object_set (MOO_WS_BLOCK (block)->tag,
                      "background", "yellow",
                      NULL);
#endif
    }

    return MOO_WS_BLOCK (block);
}

static void
load_output (MooWorksheet  *ws,
             MooMarkupNode *elm)
{
    MooWsBlock *block;
    const char *type;
    MooWsOutputType out_type = MOO_WS_OUTPUT_OUT;

    type = moo_markup_get_prop (elm, PROP_TYPE);
    if (!type)
    {
        g_critical ("%s: %s property missing", G_STRLOC, PROP_TYPE);
        type = PROP_OUTPUT_TYPE_STDOUT;
    }

    if (!strcmp (type, PROP_OUTPUT_TYPE_STDERR))
        out_type = MOO_WS_OUTPUT_ERR;

    block = create_output_block (out_type);
    g_return_if_fail (block != NULL);

    moo_ws_text_block_set_text (MOO_WS_TEXT_BLOCK (block),
                                moo_markup_get_content (elm));
    moo_ws_buffer_append_block (get_buffer (ws), block);
}

gboolean
moo_worksheet_load_xml (MooWorksheet   *ws,
                        const char     *text,
                        gsize           text_len,
                        GError        **error)
{
    MooMarkupDoc *doc;
    MooMarkupNode *root, *elm, *child;
    const char *version;

    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), FALSE);
    g_return_val_if_fail (text != NULL, FALSE);

    if (!(doc = moo_markup_parse_memory (text, text_len, error)))
        return FALSE;

    if (!(root = moo_markup_get_root_element (doc, ELM_WORKSHEET)))
    {
        g_set_error (error, MOO_WORKSHEET_FILE_ERROR,
                     MOO_WORKSHEET_FILE_ERROR_FORMAT,
                     "%s element missing", ELM_WORKSHEET);
        goto error;
    }

    version = moo_markup_get_prop (root, PROP_VERSION);
    if (!version || strcmp (version, PROP_VERSION_VALUE) != 0)
    {
        g_set_error (error, MOO_WORKSHEET_FILE_ERROR,
                     MOO_WORKSHEET_FILE_ERROR_FORMAT,
                     "bad version '%s'", version ? version : "");
        goto error;
    }

    if (!(elm = moo_markup_get_element (root, ELM_CONTENT)))
    {
        g_set_error (error, MOO_WORKSHEET_FILE_ERROR,
                     MOO_WORKSHEET_FILE_ERROR_FORMAT,
                     "%s element missing", ELM_CONTENT);
        goto error;
    }

    moo_worksheet_reset (ws);

    for (child = elm->children; child != NULL; child = child->next)
    {
        if (!MOO_MARKUP_IS_ELEMENT (child))
            continue;

        if (!strcmp (child->name, ELM_INPUT))
            load_input (ws, child);
        else if (!strcmp (child->name, ELM_OUTPUT))
            load_output (ws, child);
        else
            g_critical ("%s: unknown element %s", G_STRFUNC, child->name);
    }

    moo_markup_doc_unref (doc);
    return TRUE;

error:
    moo_markup_doc_unref (doc);
    return FALSE;
}


gboolean
moo_worksheet_format (MooWorksheet  *ws,
                      MooFileWriter *writer)
{
    MooMarkupDoc *doc;
    MooMarkupNode *root, *content;
    MooWsBlock *block;
    gboolean retval;

    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), FALSE);

    doc = moo_markup_doc_new ("moo-worksheet");
    root = moo_markup_create_root_element (doc, ELM_WORKSHEET);
    moo_markup_set_prop (root, PROP_VERSION, PROP_VERSION_VALUE);
    content = moo_markup_create_element (root, ELM_CONTENT);

    for (block = _moo_ws_buffer_get_first_block (get_buffer (ws));
         block != NULL; block = block->next)
    {
        MooMarkupNode *elm;

        if (MOO_IS_WS_PROMPT_BLOCK (block))
        {
            const char *ps, *ps2;
            char *text;

            ps = moo_ws_prompt_block_get_ps (MOO_WS_PROMPT_BLOCK (block));
            ps2 = moo_ws_prompt_block_get_ps2 (MOO_WS_PROMPT_BLOCK (block));
            text = moo_ws_prompt_block_get_text (MOO_WS_PROMPT_BLOCK (block));

            elm = moo_markup_create_text_element (content, ELM_INPUT, text);

            moo_markup_set_prop (elm, PROP_PS, ps);
            moo_markup_set_prop (elm, PROP_PS2, ps2);

            g_free (text);
        }
        else if (MOO_IS_WS_TEXT_BLOCK (block))
        {
            char *text;

            text = moo_ws_text_block_get_text (MOO_WS_TEXT_BLOCK (block));
            elm = moo_markup_create_text_element (content, ELM_OUTPUT, text);

            if (!g_object_get_data (G_OBJECT (block), "moo-worksheet-stderr"))
                moo_markup_set_prop (elm, PROP_TYPE, PROP_OUTPUT_TYPE_STDOUT);
            else
                moo_markup_set_prop (elm, PROP_TYPE, PROP_OUTPUT_TYPE_STDERR);

            g_free (text);
        }
        else
        {
            g_critical ("%s: unknown block", G_STRLOC);
        }
    }

    retval = moo_markup_write_pretty (doc, writer, 2);
    moo_markup_doc_unref (doc);
    return retval;
}

char *
moo_worksheet_get_input_text (MooWorksheet *ws)
{
    MooWsBlock *block;
    GString *text;

    g_return_val_if_fail (MOO_IS_WORKSHEET (ws), NULL);

    text = g_string_new (NULL);

    for (block = _moo_ws_buffer_get_first_block (get_buffer (ws));
         block != NULL; block = block->next)
    {
        char *block_text;

        if (!MOO_IS_WS_PROMPT_BLOCK (block))
            continue;

        block_text = moo_ws_prompt_block_get_text (MOO_WS_PROMPT_BLOCK (block));

        if (text->len)
            g_string_append (text, "\n");

        g_string_append (text, block_text);
        g_free (block_text);
    }

    return g_string_free (text, FALSE);
}
