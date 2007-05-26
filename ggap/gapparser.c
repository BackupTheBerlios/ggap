/*
 *   gapparser.c
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

#include "gapparser-priv.h"
#include <string.h>


typedef struct {
    const char *input;
    guint _ptr;
    guint len;
    guint line_no;
    guint line_ptr;
} GapLex;

typedef struct {
    char *message;
    YYLTYPE location;
} GapParseError;

struct _GapParser {
    GapLex *lex;
    GapParseResult result;
    GSList *errors;
    guint eof : 1;
    guint lex_error : 1;
};


static GapParser       *gap_parser_new          (void);
static void             gap_parser_free         (GapParser      *parser);
static GapParseResult   gap_parser_parse        (GapParser      *parser,
                                                 const char     *string);
static void             gap_parser_cleanup      (GapParser      *parser);

static GapLex          *gap_lex_new             (const char     *string);
static void             gap_lex_free            (GapLex         *lex);

static GapParseError   *gap_parse_error_new     (const char     *message,
                                                 YYLTYPE        *location);
static void             gap_parse_error_free    (GapParseError  *error);
static void             gap_parser_add_error    (GapParser      *parser,
                                                 const char     *message,
                                                 YYLTYPE        *location);


GapParseResult
gap_parse (const char *string)
{
    GapParser *parser;
    GapParseResult result;

    g_return_val_if_fail (string != NULL, GAP_PARSE_ERROR);

    parser = gap_parser_new ();
    result = gap_parser_parse (parser, string);
    gap_parser_free (parser);

    return result;
}


static GapParser *
gap_parser_new (void)
{
    return g_new0 (GapParser, 1);
}

static void
gap_parser_free (GapParser *parser)
{
    if (parser)
    {
        gap_parser_cleanup (parser);
        g_free (parser);
    }
}

static GapParseResult
gap_parser_parse (GapParser  *parser,
                  const char *string)
{
    gap_parser_cleanup (parser);
    parser->lex = gap_lex_new (string);

    _gap_parser_yyparse (parser);

    return parser->result;
}

static void
gap_parser_cleanup (GapParser *parser)
{
    gap_lex_free (parser->lex);
    parser->lex = NULL;
    parser->result = GAP_PARSE_OK;
    parser->eof = FALSE;
    g_slist_foreach (parser->errors, (GFunc) gap_parse_error_free, NULL);
    parser->errors = NULL;
}


static GapLex *
gap_lex_new (const char *string)
{
    GapLex *lex = g_new0 (GapLex, 1);
    lex->input = string;
    lex->_ptr = 0;
    lex->len = strlen (string);
    lex->line_no = 0;
    lex->line_ptr = 0;
    return lex;
}

static void
gap_lex_free (GapLex *lex)
{
    g_free (lex);
}


static GapParseError *
gap_parse_error_new (const char *message,
                     YYLTYPE    *location)
{
    GapParseError *error;

    error = g_new0 (GapParseError, 1);
    error->message = g_strdup (message);
    error->location = *location;

    return error;
}

static void
gap_parse_error_free (GapParseError *error)
{
    if (error)
    {
        g_free (error->message);
        g_free (error);
    }
}


#define IS_EOL(c__)         ((c__) == '\r' || (c__) == '\n')
#define IS_SPACE(c__)       ((c__) == ' ' || (c__) == '\t' || IS_EOL (c__))
#define IS_LETTER(c__)      (('a' <= (c__) && (c__) <= 'z') || ('A' <= (c__) && (c__) <= 'Z'))
#define IS_OCTAL_DIGIT(c__) ('0' <= (c__) && (c__) <= '7')
#define IS_DIGIT(c__)       ('0' <= (c__) && (c__) <= '9')
#define IS_WORD(c__)        (IS_LETTER (c__) || IS_DIGIT (c__) || (c__) == '_' || (c__) == '$')
#define IS_ASCII(c__)       (((c__) & 0x7F) == (c__))

#define IS_STRING_DIGIT(c__,oct__) (((oct__) && IS_OCTAL_DIGIT (c__)) || (!(oct__) && IS_DIGIT (c__)))

#define THIS            (lex->input[lex->_ptr])
#define NEXT            (lex->_ptr + 1 < lex->len ?     \
                            lex->input[lex->_ptr+1] :   \
                            '\0')
#define NEXT2           (lex->_ptr + 2 < lex->len ?     \
                            lex->input[lex->_ptr+2] :   \
                            '\0')

#define CHECK1(c_, what_)           \
G_STMT_START {                      \
    if (THIS == c_)                 \
    {                               \
        lex->_ptr += 1;             \
        return what_;               \
    }                               \
} G_STMT_END

#define SET_LOCATION(first,last)                        \
G_STMT_START {                                          \
    locp->first_line = locp->last_line = lex->line_no;  \
    locp->first_column = first - lex->line_ptr;         \
    locp->last_column = last - lex->line_ptr;           \
} G_STMT_END

#define CHECK2(c1_, c2_, what_)                         \
G_STMT_START {                                          \
    if (THIS == c1_ && NEXT == c2_)                     \
    {                                                   \
        SET_LOCATION (lex->_ptr, lex->_ptr + 1);        \
        lex->_ptr += 2;                                 \
        return what_;                                   \
    }                                                   \
} G_STMT_END

static int  gap_lex_parse_string    (GapLex     *lex,
                                     GapParser  *parser,
                                     YYLTYPE    *locp);
static int  gap_lex_parse_char      (GapLex     *lex,
                                     GapParser  *parser,
                                     YYLTYPE    *locp);
static int  gap_lex_parse_word      (GapLex     *lex,
                                     GapParser  *parser,
                                     YYLTYPE    *locp);


static void
skip_eol (GapLex *lex)
{
    g_assert (IS_EOL (THIS));

    lex->line_ptr += 1;
    lex->line_no += 1;

    if (THIS == '\r' && NEXT == '\n')
    {
        lex->line_ptr += 1;
        lex->_ptr += 1;
    }

    lex->_ptr += 1;
}


#define LEX_OOPS() gap_lex_error(parser, NULL, -1, -1)
#define LEX_ERROR(msg,start,end) gap_lex_error(parser, msg, start, end)

static int
gap_lex_error (GapParser  *parser,
               const char *message,
               int         start_ptr,
               int         end_ptr)
{
    YYLTYPE location;

    if (start_ptr < 0)
        start_ptr = parser->lex->_ptr - 1;
    if (end_ptr < 0)
        end_ptr = start_ptr;

    location.first_line = location.last_line = parser->lex->line_no;
    location.first_column = start_ptr - parser->lex->line_ptr;
    location.last_column = end_ptr - parser->lex->line_ptr;

    parser->lex_error = TRUE;
    message = message ? message : "syntax error";
    gap_parser_add_error (parser, message, &location);

    return -1;
}


static gboolean
gap_lex_parse_escaped_char (GapLex    *lex,
                            GapParser *parser,
                            gboolean   need_octal)
{
    const char *string = lex->input + lex->_ptr;
    guint len = lex->len - lex->_ptr;

    g_assert (*string == '\\');
    g_assert (len > 0);

    if (len == 1)
    {
        LEX_ERROR ("unterminated escape sequence", lex->_ptr, lex->_ptr);
        return FALSE;
    }

    if (IS_STRING_DIGIT (string[1], need_octal))
    {
        if (len < 4)
        {
            LEX_ERROR ("unterminated escape sequence",
                       lex->_ptr, lex->len - 1);
            return FALSE;
        }

        if (!IS_STRING_DIGIT (string[2], need_octal) ||
            !IS_STRING_DIGIT (string[3], need_octal))
        {
            LEX_ERROR ("expecting three octal digits after \\",
                       lex->_ptr, lex->_ptr + 3);
            return FALSE;
        }

        lex->_ptr += 4;
        return TRUE;
    }

    if (IS_EOL (string[1]))
    {
        LEX_OOPS ();
        return FALSE;
    }

    lex->_ptr += 2;
    return TRUE;
}

static int
gap_lex_parse_char (GapLex    *lex,
                    GapParser *parser,
                    YYLTYPE   *locp)
{
    guint start;

    g_assert (THIS == '\'');

    start = lex->_ptr;

    if (++lex->_ptr == lex->len)
    {
        LEX_ERROR ("unterminated character constant",
                   lex->_ptr - 1, lex->_ptr - 1);
        return -1;
    }

    if (THIS == '\\')
    {
        if (!gap_lex_parse_escaped_char (lex, parser, TRUE))
            return -1;
    }
    else if (IS_EOL (THIS))
    {
        return LEX_OOPS ();
    }
    else
    {
        lex->_ptr += 1;
    }

    if (lex->_ptr == lex->len || THIS != '\'')
        return LEX_ERROR ("expecting single character in a character constant",
                          lex->_ptr - 2, lex->_ptr - 1);

    lex->_ptr += 1;
    SET_LOCATION (start, lex->_ptr - 1);
    return TOK_CHAR_LITERAL;
}

static int
gap_lex_parse_string (GapLex    *lex,
                      GapParser *parser,
                      YYLTYPE   *locp)
{
    guint start;

    g_assert (lex->input[lex->_ptr] == '"');

    start = lex->_ptr;
    lex->_ptr += 1;

    while (lex->_ptr < lex->len)
    {
        if (THIS == '"')
        {
            lex->_ptr += 1;
            SET_LOCATION (start, lex->_ptr - 1);
            return TOK_STRING_LITERAL;
        }

        if (THIS == '\\')
        {
            if (IS_EOL (NEXT))
            {
                lex->_ptr += 1;
                skip_eol (lex);
            }
            else if (!gap_lex_parse_escaped_char (lex, parser, FALSE))
            {
                return -1;
            }
        }
        else if (IS_EOL (THIS))
        {
            return LEX_OOPS ();
        }
        else
        {
            lex->_ptr += 1;
        }
    }

    return LEX_ERROR ("expecting quote", start, lex->_ptr);
}


static struct {
    const char *kw;
    guint len;
    int token;
} keywords[] = {
    { "rec",      3, TOK_REC },
    { "local",    5, TOK_LOCAL },
    { "function", 8, TOK_FUNCTION },
    { "end",      3, TOK_END },
    { "for",      3, TOK_FOR },
    { "in",       2, TOK_IN },
    { "do",       2, TOK_DO },
    { "od",       2, TOK_OD },
    { "while",    5, TOK_WHILE },
    { "repeat",   6, TOK_REPEAT },
    { "until",    5, TOK_UNTIL },
    { "if",       2, TOK_IF },
    { "then",     4, TOK_THEN },
    { "else",     4, TOK_ELSE },
    { "elif",     4, TOK_ELIF },
    { "fi",       2, TOK_FI },
    { "return",   6, TOK_RETURN },
    { "break",    5, TOK_BREAK },
    { "quit",     4, TOK_QUIT },
    { "QUIT",     4, TOK_QUIT_CAP },
    { "not",      3, TOK_NOT },
    { "and",      3, TOK_AND },
    { "or",       2, TOK_OR },
    { "mod",      3, TOK_MOD },
    { "true",     4, TOK_TRUE },
    { "false",    5, TOK_FALSE },
};

#define CHECK_KEYWORD(i) \
    (len == keywords[i].len && !strncmp (keywords[i].kw, start, len))

static int
gap_lex_parse_word (GapLex    *lex,
                    GapParser *parser,
                    YYLTYPE   *locp)
{
    gboolean seen_escape = FALSE;
    gboolean seen_non_digit = FALSE;
    guint len, start_ptr;
    const char *start;

    start_ptr = lex->_ptr;
    start = &lex->input[lex->_ptr];
    len = 0;

    while (lex->_ptr < lex->len)
    {
        if (THIS == '\\')
        {
            if (lex->_ptr + 1 == lex->len)
                return LEX_ERROR ("unterminated escape sequence",
                                  lex->_ptr, lex->_ptr);
            if (IS_EOL (NEXT))
            {
                lex->_ptr += 1;
                skip_eol (lex);
            }
            else
            {
                if (!IS_DIGIT (NEXT))
                    seen_non_digit = TRUE;
                seen_escape = TRUE;
                lex->_ptr += 2;
                len += 2;
            }
        }
        else if (IS_WORD (THIS))
        {
            if (!IS_DIGIT (THIS))
                seen_non_digit = TRUE;
            lex->_ptr += 1;
            len += 1;
        }
        else
        {
            break;
        }
    }

    if (!seen_escape)
    {
        guint i;
        for (i = 0; i < G_N_ELEMENTS (keywords); ++i)
            if (CHECK_KEYWORD (i))
                return keywords[i].token;
    }

    SET_LOCATION (start_ptr, lex->_ptr - 1);
    return seen_non_digit ? TOK_IDENTIFIER : TOK_INTEGER;
}


int
_gap_parser_yylex (YYSTYPE   *lvalp,
                   YYLTYPE   *locp,
                   GapParser *parser)
{
    GapLex *lex = parser->lex;
    char c;

    while (lex->_ptr < lex->len && IS_SPACE (THIS))
    {
        if (IS_EOL (THIS))
            skip_eol (lex);
        else
            lex->_ptr++;
    }

    if (lex->_ptr == lex->len)
    {
        parser->eof = TRUE;
        return 0;
    }

    c = lex->input[lex->_ptr];
    SET_LOCATION (lex->_ptr, lex->_ptr);

    if (!IS_ASCII (c))
        return LEX_ERROR ("invalid character", lex->_ptr, lex->_ptr);

    if (c == '#')
    {
        while (lex->_ptr < lex->len && !IS_EOL (THIS))
            lex->_ptr++;

        if (IS_EOL (THIS))
            skip_eol (lex);

        return _gap_parser_yylex (lvalp, locp, parser);
    }

    if (c == '\\' && IS_EOL (NEXT))
    {
        lex->_ptr += 1;
        skip_eol (lex);
        return _gap_parser_yylex (lvalp, locp, parser);
    }

    if (c == '"')
        return gap_lex_parse_string (lex, parser, locp);

    if (c == '\'')
        return gap_lex_parse_char (lex, parser, locp);

    if (IS_WORD (c) || c == '\\')
        return gap_lex_parse_word (lex, parser, locp);

    CHECK2 (':', '=', TOK_ASSIGN);
    CHECK2 ('!', '.', TOK_REP_REC_ELM);
    CHECK2 ('!', '[', TOK_REP_LIST_ELM);
    CHECK2 ('.', '.', TOK_TWO_DOTS);
    CHECK2 ('<', '=', TOK_LEQ);
    CHECK2 ('>', '=', TOK_GEQ);
    CHECK2 ('<', '>', TOK_NEQ);
    CHECK2 ('-', '>', TOK_MAPTO);

    lex->_ptr++;
    return c;
}


void
_gap_parser_yyerror (YYLTYPE    *locp,
                     GapParser  *parser,
                     const char *string)
{
    if (parser->eof)
    {
        parser->result = GAP_PARSE_INCOMPLETE;
        return;
    }

    if (parser->lex_error)
        parser->lex_error = FALSE;
    else
        gap_parser_add_error (parser, string, locp);
}

// static char *
// get_line (const char *string)
// {
//     guint le, len;
//
//     g_return_val_if_fail (string != NULL, g_strdup (string));
//
//     len = strlen (string);
//     for (le = 0; string[le] != '\n' && string[le] != '\r' && le < len; ++le) ;
//
//     return g_strndup (string, le);
// }

static void
print_error (const char *message,
             YYLTYPE    *locp)
{
    g_print ("On line %d, chars %d-%d: %s\n", locp->first_line + 1,
             locp->first_column + 1, locp->last_column + 1, message);
}

static void
gap_parser_add_error (GapParser   *parser,
                      const char  *message,
                      YYLTYPE     *location)
{
    parser->result = GAP_PARSE_ERROR;
    parser->errors = g_slist_prepend (parser->errors,
                                      gap_parse_error_new (message, location));

    print_error (message, location);
}
