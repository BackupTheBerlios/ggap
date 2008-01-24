/*
 *   gapparser-priv.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef GAP_PARSER_PRIV_H
#define GAP_PARSER_PRIV_H

#include "gapparser.h"
#include "gapparser-yacc.h"

G_BEGIN_DECLS


int     _gap_parser_yylex   (YYSTYPE    *lvalp,
                             YYLTYPE    *locp,
                             GapParser  *parser);
void    _gap_parser_yyerror (YYLTYPE    *locp,
                             GapParser  *parser,
                             const char *string);
int     _gap_parser_yyparse (GapParser  *parser);


G_END_DECLS

#endif /* GAP_PARSER_PRIV_H */
