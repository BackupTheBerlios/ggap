/*
 *   gapparser.h
 *
 *   Copyright (C) 2004-2008 by Yevgen Muntyan <muntyan@tamu.edu>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License version 2.1 as published by the Free Software Foundation.
 *
 *   See COPYING file that comes with this distribution.
 */

#ifndef GAP_PARSER_H
#define GAP_PARSER_H

#include <glib.h>

G_BEGIN_DECLS


typedef struct _GapParser GapParser;
typedef struct _GapParseError GapParseError;

typedef enum {
    GAP_PARSE_OK,
    GAP_PARSE_ERROR,
    GAP_PARSE_INCOMPLETE
} GapParseResult;

struct _GapParseError {
    int line;
    int first_column;
    int last_column;
    char *message;
};


GapParseResult  gap_parse               (const char *string);

GapParser      *gap_parser_new          (void);
void            gap_parser_free         (GapParser  *parser);
GapParseResult  gap_parser_parse        (GapParser  *parser,
                                         const char *string);
GSList         *gap_parser_get_errors   (GapParser  *parser);


G_END_DECLS

#endif /* GAP_PARSER_H */
