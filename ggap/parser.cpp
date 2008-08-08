#include "ggap/parser-p.h"
#include "moo-macros.h"
#include <string.h>
#include <QHash>

const char *ggap::parser::translate(const char *s)
{
    m_return_val_if_fail(s != 0, 0);

    if (!strcmp(s, "syntax error: cannot back up"))
        return "Syntax error";
    else if (!strcmp(s, "syntax error, unexpected %s"))
        return "Syntax error, unexpected %s";
    else if (!strcmp(s, "syntax error, unexpected %s, expecting %s"))
        return "Syntax error, unexpected %s, expecting %s";
    else if (!strcmp(s, "syntax error, unexpected %s, expecting %s or %s"))
        return "Syntax error, unexpected %s, expecting %s or %s";
    else if (!strcmp(s, "syntax error, unexpected %s, expecting %s or %s or %s"))
        return "Syntax error, unexpected %s, expecting %s or %s or %s";
    else if (!strcmp(s, "syntax error, unexpected %s, expecting %s or %s or %s or %s"))
        return "Syntax error, unexpected %s, expecting %s or %s or %s or %s";
    else if (!strcmp(s, "syntax error"))
        return "Syntax error";
    else
        return s;
}

namespace ggap {
namespace parser {

struct Lex {
    const QChar *input;
    uint _ptr;
    uint len;
    uint line_no;
    uint line_ptr;
    QStringList lines;
    uint continue_line : 1;

    Lex(const QString &text);
    void commit_line(bool backslash);
};

struct Parser {
    Lex lex;
    Result result;
    QList<ErrorInfo> errors;
    uint eof : 1;
    uint lex_error : 1;

    Parser(const QString &text);
    void add_error(const char *message, const YYLTYPE *location);
};

}
}

using namespace ggap::parser;


Parser::Parser(const QString &text) :
    lex(text),
    result(Success),
    errors(),
    eof(false),
    lex_error(false)
{
}

Result ggap::parser::parse(const QStringList &input, Text *output, QList<ErrorInfo> *errors)
{
    QString in = input.join("\n");

    Parser p(in);
    _gap_parser_yyparse(&p);

    Text data = {input, p.lex.lines, p.lex.lines.join(" ")};
    *output = data;

    *errors = p.errors;
    return p.result;
}


Lex::Lex(const QString &text)
{
    input = text.unicode();
    _ptr = 0;
    len = text.length();
    line_no = 0;
    line_ptr = 0;
    continue_line = false;
}

static ErrorInfo make_error(const char    *message,
                            const YYLTYPE *location)
{
    ErrorInfo error;

    error.message = message;
    error.line = location->first_line;
    error.firstColumn = location->first_column;
    error.lastColumn = location->last_column;

    return error;
}


#define IS_EOL(c__)         ((c__) == '\r' || (c__) == '\n')
#define IS_SPACE(c__)       ((c__) == ' ' || (c__) == '\t' || IS_EOL (c__))
#define IS_LETTER(c__)      (('a' <= (c__) && (c__) <= 'z') || ('A' <= (c__) && (c__) <= 'Z'))
#define IS_OCTAL_DIGIT(c__) ('0' <= (c__) && (c__) <= '7')
#define IS_DIGIT(c__)       ('0' <= (c__) && (c__) <= '9')
#define IS_WORD(c__)        (IS_LETTER (c__) || IS_DIGIT (c__) || (c__) == '_' || (c__) == '$')
#define IS_ASCII(c__)       (((c__.unicode()) & 0x7F) == (c__.unicode())) // XXX

#define IS_STRING_DIGIT(c__,oct__) (((oct__) && IS_OCTAL_DIGIT (c__)) || (!(oct__) && IS_DIGIT (c__)))

#define THIS            (lex->input[lex->_ptr])
#define NEXT            (lex->_ptr + 1 < lex->len ?     \
                            lex->input[lex->_ptr+1] :   \
                            '\0')
#define NEXT2           (lex->_ptr + 2 < lex->len ?     \
                            lex->input[lex->_ptr+2] :   \
                            '\0')

#define SET_LOCATION(first,last)                        \
M_STMT_START {                                          \
    locp->first_line = locp->last_line = lex->line_no;  \
    locp->first_column = first - lex->line_ptr;         \
    locp->last_column = last - lex->line_ptr;           \
} M_STMT_END

#define CHECK1(c_, what_)                               \
M_STMT_START {                                          \
    if (THIS == c_)                                     \
    {                                                   \
        SET_LOCATION (lex->_ptr, lex->_ptr);            \
        lex->_ptr += 1;                                 \
        return what_;                                   \
    }                                                   \
} M_STMT_END

#define CHECK2(c1_, c2_, what_)                         \
M_STMT_START {                                          \
    if (THIS == c1_ && NEXT == c2_)                     \
    {                                                   \
        SET_LOCATION (lex->_ptr, lex->_ptr + 1);        \
        lex->_ptr += 2;                                 \
        return what_;                                   \
    }                                                   \
} M_STMT_END

void Lex::commit_line(bool backslash)
{
    QString new_line(input + line_ptr,
                     backslash ? _ptr - line_ptr - 1 : _ptr - line_ptr);

    if (continue_line)
        lines.last() += new_line;
    else
        lines << new_line;

    continue_line = backslash;
}

static void skip_eol(Lex *lex)
{
    M_ASSERT(IS_EOL (THIS));

    if (THIS == '\r' && NEXT == '\n')
        lex->_ptr += 1;

    lex->_ptr += 1;
    lex->line_no += 1;
    lex->line_ptr = lex->_ptr;
}


#define LEX_OOPS() gap_lex_error(parser, NULL, -1, -1)
#define LEX_ERROR(msg,start,end) gap_lex_error(parser, msg, start, end)

static int gap_lex_error(Parser     *parser,
                         const char *message,
                         int         start_ptr,
                         int         end_ptr)
{
    YYLTYPE location;

    if (start_ptr < 0)
        start_ptr = parser->lex._ptr - 1;
    if (end_ptr < 0)
        end_ptr = start_ptr;

    location.first_line = location.last_line = parser->lex.line_no;
    location.first_column = start_ptr - parser->lex.line_ptr;
    location.last_column = end_ptr - parser->lex.line_ptr;

    parser->lex_error = true;
    message = message ? message : "syntax error";
    parser->add_error(message, &location);

    return -1;
}


static bool parse_escaped_char(Lex    *lex,
                               Parser *parser,
                               bool    need_octal)
{
    const QChar *string = lex->input + lex->_ptr;
    uint len = lex->len - lex->_ptr;

    M_ASSERT(*string == '\\');
    M_ASSERT(len > 0);

    if (len == 1)
    {
        LEX_ERROR ("unterminated escape sequence", lex->_ptr, lex->_ptr);
        return false;
    }

    if (IS_STRING_DIGIT (string[1], need_octal))
    {
        if (len < 4)
        {
            LEX_ERROR ("unterminated escape sequence",
                       lex->_ptr, lex->len - 1);
            return false;
        }

        if (!IS_STRING_DIGIT (string[2], need_octal) ||
            !IS_STRING_DIGIT (string[3], need_octal))
        {
            LEX_ERROR ("expecting three octal digits after \\",
                       lex->_ptr, lex->_ptr + 3);
            return false;
        }

        lex->_ptr += 4;
        return true;
    }

    if (IS_EOL (string[1]))
    {
        LEX_OOPS ();
        return false;
    }

    lex->_ptr += 2;
    return true;
}

static int parse_char (Lex     *lex,
                       Parser  *parser,
                       YYLTYPE *locp)
{
    uint start;

    M_ASSERT (THIS == '\'');

    start = lex->_ptr;

    if (++lex->_ptr == lex->len)
    {
        LEX_ERROR ("unterminated character constant",
                   lex->_ptr - 1, lex->_ptr - 1);
        return -1;
    }

    if (THIS == '\\')
    {
        if (!parse_escaped_char (lex, parser, true))
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

static int parse_string (Lex     *lex,
                         Parser  *parser,
                         YYLTYPE *locp)
{
    uint start;

    M_ASSERT (lex->input[lex->_ptr] == '"');

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
                lex->commit_line(true);
                skip_eol (lex);
            }
            else if (!parse_escaped_char (lex, parser, false))
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


namespace {

class KeywordMap {
    QHash<QString, int> data;

    KeywordMap()
    {
        struct {
            const char *kw;
            int token;
        } const keywords[] = {
            { "rec",            TOK_REC },
            { "local",          TOK_LOCAL },
            { "function",       TOK_FUNCTION },
            { "end",            TOK_END },
            { "for",            TOK_FOR },
            { "in",             TOK_IN },
            { "do",             TOK_DO },
            { "od",             TOK_OD },
            { "while",          TOK_WHILE },
            { "repeat",         TOK_REPEAT },
            { "until",          TOK_UNTIL },
            { "if",             TOK_IF },
            { "then",           TOK_THEN },
            { "else",           TOK_ELSE },
            { "elif",           TOK_ELIF },
            { "fi",             TOK_FI },
            { "return",         TOK_RETURN },
            { "break",          TOK_BREAK },
            { "quit",           TOK_QUIT },
            { "QUIT",           TOK_QUIT_CAP },
            { "not",            TOK_NOT },
            { "and",            TOK_AND },
            { "or",             TOK_OR },
            { "mod",            TOK_MOD },
            { "true",           TOK_TRUE },
            { "false",          TOK_FALSE },
            { "SaveWorkspace",  TOK_SAVE_WORKSPACE },
            { "Unbind",         TOK_UNBIND },
            { "IsBound",        TOK_IS_BOUND },
        };

        for (uint i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i)
            data[keywords[i].kw] = keywords[i].token;
    }

public:
    static bool checkWord(const QChar *text, uint len, int *token)
    {
        static KeywordMap km;
        QHash<QString,int>::const_iterator iter;
        iter = km.data.find(QString(text, len));
        if (iter == km.data.constEnd())
            return false;
        *token = iter.value();
        return true;
    }

};

}

static int parse_word(Lex     *lex,
                      Parser  *parser,
                      YYLTYPE *locp)
{
    bool seen_escape = false;
    bool seen_non_digit = false;
    uint len, start_ptr;
    const QChar *start;

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
                lex->commit_line(true);
                skip_eol (lex);
            }
            else
            {
                if (!IS_DIGIT (NEXT))
                    seen_non_digit = true;
                seen_escape = true;
                lex->_ptr += 2;
                len += 2;
            }
        }
        else if (IS_WORD (THIS))
        {
            if (!IS_DIGIT (THIS))
                seen_non_digit = true;
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
        int tok;
        if (KeywordMap::checkWord(start, len, &tok))
            return tok;
    }

    SET_LOCATION (start_ptr, lex->_ptr - 1);
    return seen_non_digit ? TOK_IDENTIFIER : TOK_INTEGER;
}


int _gap_parser_yylex(YYSTYPE *lvalp,
                      YYLTYPE *locp,
                      Parser  *parser)
{
    Lex *lex = &parser->lex;

    while (lex->_ptr < lex->len && IS_SPACE (THIS))
    {
        if (IS_EOL (THIS))
        {
            lex->commit_line(false);
            skip_eol (lex);
        }
        else
            lex->_ptr++;
    }

    if (lex->_ptr == lex->len)
    {
        lex->commit_line(false);
        parser->eof = true;
        return 0;
    }

    QChar c = lex->input[lex->_ptr];
    SET_LOCATION (lex->_ptr, lex->_ptr);

    if (!IS_ASCII (c))
        return LEX_ERROR ("invalid character", lex->_ptr, lex->_ptr);

    if (c == '#')
    {
        lex->commit_line(false);

        while (lex->_ptr < lex->len && !IS_EOL (THIS))
            lex->_ptr++;

        if (IS_EOL (THIS))
        {
            skip_eol (lex);
            return _gap_parser_yylex (lvalp, locp, parser);
        }
        else
        {
            parser->eof = true;
            return 0;
        }
    }

    if (c == '\\' && IS_EOL (NEXT))
    {
        lex->_ptr += 1;
        lex->commit_line(true);
        skip_eol (lex);
        return _gap_parser_yylex (lvalp, locp, parser);
    }

    if (c == '"')
        return parse_string (lex, parser, locp);

    if (c == '\'')
        return parse_char (lex, parser, locp);

    if (IS_WORD (c) || c == '\\')
        return parse_word (lex, parser, locp);

    CHECK2 (':', '=', TOK_ASSIGN);
    CHECK2 ('!', '.', TOK_REP_REC_ELM);
    CHECK2 ('!', '[', TOK_REP_LIST_ELM);
    CHECK2 ('.', '.', TOK_TWO_DOTS);
    CHECK2 ('<', '=', TOK_LEQ);
    CHECK2 ('>', '=', TOK_GEQ);
    CHECK2 ('<', '>', TOK_NEQ);
    CHECK2 ('-', '>', TOK_MAPTO);

    lex->_ptr++;
    return c.unicode();
}


void _gap_parser_yyerror(YYLTYPE    *locp,
                         Parser     *parser,
                         const char *string)
{
    if (parser->eof)
    {
        parser->result = Incomplete;
        return;
    }

    if (parser->lex_error)
        parser->lex_error = false;
    else
        parser->add_error(string, locp);
}

void Parser::add_error(const char    *message,
                       const YYLTYPE *location)
{
    result = Error;
    errors.append(make_error(message, location));
}
