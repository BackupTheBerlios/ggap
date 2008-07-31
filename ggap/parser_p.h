#ifndef GAP_PARSER_P_H
#define GAP_PARSER_P_H

#include "ggap/parser.h"
typedef QChar YYSTYPE;
#define YYSTYPE_IS_DECLARED
#include "ggap/parser-yacc.h"

namespace ggap {
namespace parser {
class Parser;
const char *translate(const char *what);
}
}

int    _gap_parser_yylex    (YYSTYPE              *lvalp,
                             YYLTYPE              *locp,
                             ggap::parser::Parser *parser);
void   _gap_parser_yyerror  (YYLTYPE              *locp,
                             ggap::parser::Parser *parser,
                             const char           *string);
int    _gap_parser_yyparse  (ggap::parser::Parser *parser);

#endif // GAP_PARSER_P_H
