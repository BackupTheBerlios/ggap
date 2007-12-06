/*
 *   moowspromptblock.c
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

#include "moowspromptblock.h"
#include "mooutils/mooutils-misc.h"


struct _MooWsPromptBlockPrivate
{
    char *ps;
    char *ps2;
    char *text;
    guint ps_len;
    guint ps2_len;
    GtkTextTag *ps_tag;
    GtkTextTag *text_tag;
};


static gboolean moo_ws_iter_is_prompt                   (const GtkTextIter  *iter);
static void     moo_ws_prompt_block_get_iter_position   (MooWsPromptBlock   *pb,
                                                         const GtkTextIter  *pos,
                                                         int                *linep,
                                                         int                *characterp);


G_DEFINE_TYPE (MooWsPromptBlock, moo_ws_prompt_block, MOO_TYPE_WS_BLOCK)

enum {
    PROP_0,
    PROP_PS,
    PROP_PS2,
    PROP_TEXT
};

static void
moo_ws_prompt_block_init (MooWsPromptBlock *block)
{
    block->priv = G_TYPE_INSTANCE_GET_PRIVATE (block, MOO_TYPE_WS_PROMPT_BLOCK,
                                               MooWsPromptBlockPrivate);
    block->priv->ps = g_strdup (">>> ");
    block->priv->ps2 = g_strdup ("... ");
    block->priv->ps_len = block->priv->ps2_len = 4;
    block->priv->text = NULL;
    block->priv->text_tag = gtk_text_tag_new (NULL);
    block->priv->ps_tag = gtk_text_tag_new (NULL);
    g_object_set_data (G_OBJECT (block->priv->ps_tag), "moo-ws-prompt", GINT_TO_POINTER (TRUE));
}

static void
moo_ws_prompt_block_dispose (GObject *object)
{
    MooWsPromptBlock *block = MOO_WS_PROMPT_BLOCK (object);

    if (block->priv)
    {
        g_free (block->priv->text);
        g_free (block->priv->ps);
        g_free (block->priv->ps2);
        g_object_unref (block->priv->text_tag);
        g_object_unref (block->priv->ps_tag);
        block->priv = NULL;
    }

    G_OBJECT_CLASS (moo_ws_prompt_block_parent_class)->dispose (object);
}

static void
moo_ws_prompt_block_add (MooWsBlock *block,
                         MooWsView  *view,
                         MooWsBlock *after,
                         MooWsBlock *before)
{
    char *text;
    GtkTextTagTable *tag_table;
    MooWsPromptBlock *pb = MOO_WS_PROMPT_BLOCK (block);

    MOO_WS_BLOCK_CLASS (moo_ws_prompt_block_parent_class)->add (block, view, after, before);

    tag_table = gtk_text_buffer_get_tag_table (block->buffer);
    gtk_text_tag_table_add (tag_table, pb->priv->ps_tag);
    gtk_text_tag_table_add (tag_table, pb->priv->text_tag);

    text = pb->priv->text;
    pb->priv->text = NULL;
    moo_ws_prompt_block_set_text (pb, text);
    g_free (text);
}

static void
moo_ws_prompt_block_remove (MooWsBlock *block)
{
    MooWsPromptBlock *pb = MOO_WS_PROMPT_BLOCK (block);
    GtkTextTagTable *tag_table;

    tag_table = gtk_text_buffer_get_tag_table (block->buffer);
    gtk_text_tag_table_remove (tag_table, pb->priv->ps_tag);
    gtk_text_tag_table_remove (tag_table, pb->priv->text_tag);

    MOO_WS_BLOCK_CLASS (moo_ws_prompt_block_parent_class)->remove (block);
}

static void
place_cursor_past_prompt (MooWsPromptBlock *pb,
                          GtkTextIter      *where)
{
    int character;

    moo_ws_prompt_block_get_iter_position (pb, where, NULL, &character);

    if (character < 0)
        gtk_text_iter_forward_chars (where, -character);
}

static gboolean
moo_ws_prompt_block_insert_interactive (MooWsBlock  *block,
                                        GtkTextIter *where,
                                        const char  *text,
                                        gssize       len)
{
    char **lines, **p;
    MooWsPromptBlock *pb = MOO_WS_PROMPT_BLOCK (block);

    if (!text || !text[0])
        return FALSE;

    if (moo_ws_iter_is_prompt (where))
        place_cursor_past_prompt (pb, where);

    lines = moo_strnsplit_lines (text, len, NULL);

    for (p = lines; p && *p; ++p)
    {
        if (p != lines)
        {
            _moo_ws_block_insert (block, where, "\n", -1);
            if (pb->priv->ps2)
                _moo_ws_block_insert_with_tags (block, where, pb->priv->ps2, -1,
                                                pb->priv->ps_tag, NULL);
        }

        if (**p)
            _moo_ws_block_insert_with_tags (block, where, *p, -1,
                                            pb->priv->text_tag, NULL);
    }

#if 0
    {
        GtkTextIter s, e;
        _moo_ws_block_get_start_iter (block, &s);
        _moo_ws_block_get_end_iter (block, &e);
        g_print ("... after moo_ws_prompt_block_insert_interactive\n");
        g_print ("start: %d, %d; end: %d, %d\n",
                 gtk_text_iter_get_line (&s),
                 gtk_text_iter_get_line_offset (&s),
                 gtk_text_iter_get_line (&e),
                 gtk_text_iter_get_line_offset (&e));
        g_print ("...\n");
    }
#endif

    g_strfreev (lines);
    return TRUE;
}

static gboolean
moo_ws_prompt_block_delete_interactive (MooWsBlock  *block,
                                        GtkTextIter *start,
                                        GtkTextIter *end)
{
    int start_line, end_line;
    int start_char, end_char;
    MooWsPromptBlock *pb = MOO_WS_PROMPT_BLOCK (block);

    gtk_text_iter_order (start, end);
    moo_ws_prompt_block_get_iter_position (pb, start, &start_line, &start_char);
    moo_ws_prompt_block_get_iter_position (pb, end, &end_line, &end_char);

    if (start_char < 0)
    {
        if (start_line == 0)
        {
            gtk_text_iter_set_line_offset (start, pb->priv->ps_len);
        }
        else
        {
            gtk_text_iter_backward_line (start);
            if (!gtk_text_iter_ends_line (start))
                gtk_text_iter_forward_to_line_end (start);
        }
    }

    if (end_char < 0)
    {
        if (end_line == 0)
            gtk_text_iter_set_line_offset (end, pb->priv->ps_len);
        else
            gtk_text_iter_set_line_offset (end, pb->priv->ps2_len);
    }

    if (gtk_text_iter_compare (start, end) >= 0)
    {
        _moo_ws_view_beep (block->view);
        return FALSE;
    }

    gtk_text_buffer_delete (block->buffer, start, end);

#if 0
    {
        GtkTextIter s, e;
        _moo_ws_block_get_start_iter (block, &s);
        _moo_ws_block_get_end_iter (block, &e);
        g_print ("... after moo_ws_prompt_block_delete_interactive\n");
        g_print ("start: %d, %d; end: %d, %d\n",
                 gtk_text_iter_get_line (&s),
                 gtk_text_iter_get_line_offset (&s),
                 gtk_text_iter_get_line (&e),
                 gtk_text_iter_get_line_offset (&e));
        g_print ("...\n");
    }
#endif

    return TRUE;
}


static gboolean
moo_ws_pompt_block_check_move_cursor (MooWsBlock     *block,
                                      GtkTextIter    *pos,
                                      GtkMovementStep step,
                                      int             count,
                                      gboolean        extend_selection)
{
    MooWsPromptBlock *pb = MOO_WS_PROMPT_BLOCK (block);

    if (!moo_ws_iter_is_prompt (pos))
        return FALSE;

    switch (step)
    {
        case GTK_MOVEMENT_LOGICAL_POSITIONS: /* move by forw/back graphemes */
        case GTK_MOVEMENT_VISUAL_POSITIONS:  /* move by left/right graphemes */
            if (!extend_selection)
            {
                place_cursor_past_prompt (pb, pos);
                return TRUE;
            }
            /* fall through */
        case GTK_MOVEMENT_WORDS:             /* move by forward/back words */
            if (count > 0)
            {
                if (extend_selection && gtk_text_iter_get_line_offset (pos) == 0)
                    return FALSE;
                place_cursor_past_prompt (pb, pos);
            }
            else if (gtk_text_iter_backward_line (pos))
            {
                if (!gtk_text_iter_ends_line (pos))
                    gtk_text_iter_forward_to_line_end (pos);
            }
            else
            {
                place_cursor_past_prompt (pb, pos);
            }
            return TRUE;

        case GTK_MOVEMENT_DISPLAY_LINES:     /* move up/down lines (wrapped lines) */
        case GTK_MOVEMENT_DISPLAY_LINE_ENDS: /* move up/down lines (wrapped lines) */
        case GTK_MOVEMENT_PARAGRAPHS:        /* move up/down paragraphs (newline-ended lines) */
        case GTK_MOVEMENT_PARAGRAPH_ENDS:    /* move to either end of a paragraph */
        case GTK_MOVEMENT_PAGES:	     /* move by pages */
        case GTK_MOVEMENT_BUFFER_ENDS:       /* move to ends of the buffer */
            place_cursor_past_prompt (pb, pos);
            return TRUE;

        case GTK_MOVEMENT_HORIZONTAL_PAGES:  /* move horizontally by pages */
        default:
            return FALSE;
    }
}


static void
moo_ws_prompt_block_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
    char *tmp;
    MooWsPromptBlock *block = MOO_WS_PROMPT_BLOCK (object);

    switch (property_id)
    {
        case PROP_PS:
            tmp = block->priv->ps;
            block->priv->ps = g_value_dup_string (value);
            if (block->priv->ps)
                block->priv->ps_len = g_utf8_strlen (block->priv->ps, -1);
            else
                block->priv->ps_len = 0;
            g_free (tmp);
            break;

        case PROP_PS2:
            tmp = block->priv->ps2;
            block->priv->ps2 = g_value_dup_string (value);
            if (block->priv->ps2)
                block->priv->ps2_len = g_utf8_strlen (block->priv->ps2, -1);
            else
                block->priv->ps2_len = 0;
            g_free (tmp);
            break;

        case PROP_TEXT:
            moo_ws_prompt_block_set_text (block, g_value_get_string (value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
moo_ws_prompt_block_get_property (GObject    *object,
                                  guint       property_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
    MooWsPromptBlock *block = MOO_WS_PROMPT_BLOCK (object);

    switch (property_id)
    {
        case PROP_PS:
            g_value_set_string (value, block->priv->ps);
            break;

        case PROP_PS2:
            g_value_set_string (value, block->priv->ps2);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
moo_ws_prompt_block_class_init (MooWsPromptBlockClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    MooWsBlockClass *block_class = MOO_WS_BLOCK_CLASS (klass);

    object_class->dispose = moo_ws_prompt_block_dispose;
    object_class->get_property = moo_ws_prompt_block_get_property;
    object_class->set_property = moo_ws_prompt_block_set_property;

    block_class->add = moo_ws_prompt_block_add;
    block_class->remove = moo_ws_prompt_block_remove;
    block_class->insert_interactive = moo_ws_prompt_block_insert_interactive;
    block_class->delete_interactive = moo_ws_prompt_block_delete_interactive;
    block_class->check_move_cursor = moo_ws_pompt_block_check_move_cursor;

    g_object_class_install_property (object_class, PROP_PS,
        g_param_spec_string ("ps", "ps", "ps",
                             NULL,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (object_class, PROP_PS2,
        g_param_spec_string ("ps2", "ps2", "ps2",
                             NULL,
                             G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property (object_class, PROP_TEXT,
        g_param_spec_string ("text", "text", "text",
                             NULL,
                             G_PARAM_WRITABLE));

    g_type_class_add_private (klass, sizeof (MooWsPromptBlockPrivate));
}


MooWsBlock *
moo_ws_prompt_block_new (const char *ps,
                         const char *ps2)
{
    return g_object_new (MOO_TYPE_WS_PROMPT_BLOCK, "ps", ps, "ps2", ps2, NULL);
}


void
moo_ws_prompt_block_set_text (MooWsPromptBlock *pb,
                              const char       *text)
{
    MooWsBlock *block;
    GtkTextIter start, end;
    char **lines, **p;

    g_return_if_fail (MOO_IS_WS_PROMPT_BLOCK (pb));

    block = MOO_WS_BLOCK (pb);

    if (!block->view)
    {
        char *tmp = pb->priv->text;
        pb->priv->text = g_strdup (text);
        g_free (tmp);
        return;
    }

    _moo_ws_view_start_edit (block->view);

    _moo_ws_block_get_start_iter (block, &start);
    _moo_ws_block_get_end_iter (block, &end);
    gtk_text_buffer_delete (block->buffer, &start, &end);

    if (text && text[0])
        lines = moo_strnsplit_lines (text, -1, NULL);
    else
        lines = NULL;

    if (pb->priv->ps)
        _moo_ws_block_insert_with_tags (block, &start, pb->priv->ps, -1,
                                        pb->priv->ps_tag, NULL);
    if (lines && lines[0])
        _moo_ws_block_insert_with_tags (block, &start, lines[0], -1,
                                        pb->priv->text_tag, NULL);

    if (lines && lines[0])
    {
        for (p = lines + 1; *p; ++p)
        {
            _moo_ws_block_insert (block, &start, "\n", -1);
            if (pb->priv->ps2)
                _moo_ws_block_insert_with_tags (block, &start, pb->priv->ps2, -1,
                                                pb->priv->ps_tag, NULL);
            _moo_ws_block_insert_with_tags (block, &start, *p, -1,
                                            pb->priv->text_tag, NULL);
        }
    }

    g_strfreev (lines);

    _moo_ws_view_end_edit (block->view);
}


static gboolean
moo_ws_iter_is_prompt (const GtkTextIter *iter)
{
    GSList *tags;

    tags = gtk_text_iter_get_tags (iter);

    while (tags)
    {
        if (g_object_get_data (tags->data, "moo-ws-prompt"))
        {
            g_slist_free (tags);
            return TRUE;
        }

        tags = g_slist_delete_link (tags, tags);
    }

    return FALSE;
}

static void
moo_ws_prompt_block_get_iter_position (MooWsPromptBlock  *pb,
                                       const GtkTextIter *pos,
                                       int               *linep,
                                       int               *characterp)
{
    MooWsBlock *block = MOO_WS_BLOCK (pb);
    GtkTextIter iter = *pos;
    GtkTextIter start_iter;
    int line, first_line;

    if (linep)
        *linep = -1;

    g_assert (gtk_text_iter_is_end (&iter) || gtk_text_iter_is_cursor_position (&iter));

    _moo_ws_block_get_start_iter (block, &start_iter);
    first_line = gtk_text_iter_get_line (&start_iter);
    line = gtk_text_iter_get_line (&iter);
    g_assert (first_line <= line);

    if (first_line == line)
    {
        if (linep)
            *linep = 0;
        if (characterp)
            *characterp = gtk_text_iter_get_line_offset (&iter) - pb->priv->ps_len;
    }
    else
    {
        if (linep)
            *linep = line - first_line;
        if (characterp)
            *characterp = gtk_text_iter_get_line_offset (&iter) - pb->priv->ps2_len;
    }
}


GtkTextTag *
moo_ws_prompt_block_get_ps_tag (MooWsPromptBlock *pb)
{
    g_return_val_if_fail (MOO_IS_WS_PROMPT_BLOCK (pb), NULL);
    return pb->priv->ps_tag;
}

GtkTextTag *
moo_ws_prompt_block_get_text_tag (MooWsPromptBlock *pb)
{
    g_return_val_if_fail (MOO_IS_WS_PROMPT_BLOCK (pb), NULL);
    return pb->priv->text_tag;
}


char **
moo_ws_prompt_block_get_lines (MooWsPromptBlock *pb)
{
    GPtrArray *array;
    GtkTextIter iter;
    gboolean first_line = TRUE;

    g_return_val_if_fail (MOO_IS_WS_PROMPT_BLOCK (pb), NULL);

    array = g_ptr_array_new ();
    _moo_ws_block_get_start_iter (MOO_WS_BLOCK (pb), &iter);

    while (TRUE)
    {
        GtkTextIter end;

        if (first_line && pb->priv->ps_len)
            gtk_text_iter_forward_chars (&iter, pb->priv->ps_len);
        else if (!first_line && pb->priv->ps2_len)
            gtk_text_iter_forward_chars (&iter, pb->priv->ps2_len);

        end = iter;
        if (!gtk_text_iter_ends_line (&end))
            gtk_text_iter_forward_to_line_end (&end);

        g_ptr_array_add (array, gtk_text_iter_get_slice (&iter, &end));

        first_line = FALSE;

        if (!gtk_text_iter_forward_line (&iter) ||
            _moo_ws_iter_get_block (&iter) != MOO_WS_BLOCK (pb))
            break;
    }

    g_ptr_array_add (array, NULL);
    return (char**) g_ptr_array_free (array, FALSE);
}

char *
moo_ws_prompt_block_get_text (MooWsPromptBlock *pb)
{
    char **lines;
    char *text;

    g_return_val_if_fail (MOO_IS_WS_PROMPT_BLOCK (pb), NULL);

    lines = moo_ws_prompt_block_get_lines (pb);
    text = g_strjoinv ("\n", lines);
    g_strfreev (lines);
    return text;
}


const char *
moo_ws_prompt_block_get_ps (MooWsPromptBlock *pb)
{
    g_return_val_if_fail (MOO_IS_WS_PROMPT_BLOCK (pb), NULL);
    return pb->priv->ps;
}

const char *
moo_ws_prompt_block_get_ps2 (MooWsPromptBlock *pb)
{
    g_return_val_if_fail (MOO_IS_WS_PROMPT_BLOCK (pb), NULL);
    return pb->priv->ps2;
}


void
moo_ws_prompt_block_place_cursor (MooWsPromptBlock *pb,
                                  int               line,
                                  int               column)
{
    GtkTextIter iter;
    MooWsBlock *block;

    g_return_if_fail (MOO_IS_WS_PROMPT_BLOCK (pb));

    block = MOO_WS_BLOCK (pb);
    _moo_ws_block_get_iter_at_line (block, &iter, line, &line);

    if (column < 0)
    {
        if (!gtk_text_iter_ends_line (&iter))
            gtk_text_iter_forward_to_line_end (&iter);
    }
    else
    {
        int add, line_chars;

        if (line == 0)
            add = pb->priv->ps_len;
        else
            add = pb->priv->ps2_len;

        line_chars = gtk_text_iter_get_chars_in_line (&iter);
        column = MIN (column, line_chars - add);

        gtk_text_iter_set_line_offset (&iter, column + add);
    }

    gtk_text_buffer_place_cursor (block->buffer, &iter);
}

void
moo_ws_prompt_block_iter_set_line_offset (MooWsPromptBlock *pb,
                                          GtkTextIter      *iter,
                                          int               offset)
{
    int line;

    g_return_if_fail (MOO_IS_WS_PROMPT_BLOCK (pb));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (offset >= 0);

    moo_ws_prompt_block_get_iter_position (pb, iter, &line, NULL);
    g_return_if_fail (line >= 0);

    if (line == 0)
        gtk_text_iter_set_line_offset (iter, offset + pb->priv->ps_len);
    else
        gtk_text_iter_set_line_offset (iter, offset + pb->priv->ps2_len);
}

void
moo_ws_prompt_block_new_line (MooWsPromptBlock *pb)
{
    GtkTextIter iter;
    MooWsBlock *block;

    g_return_if_fail (MOO_IS_WS_PROMPT_BLOCK (pb));

    block = MOO_WS_BLOCK (pb);
    _moo_ws_view_start_edit (block->view);

    _moo_ws_block_get_end_iter (block, &iter);
    _moo_ws_block_insert (block, &iter, "\n", -1);

    if (pb->priv->ps2)
        _moo_ws_block_insert_with_tags (block, &iter, pb->priv->ps2, -1,
                                        pb->priv->ps_tag, NULL);

    _moo_ws_view_end_edit (block->view);
    gtk_text_buffer_place_cursor (block->buffer, &iter);
}
