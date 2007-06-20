/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOK_REC = 258,
     TOK_LOCAL = 259,
     TOK_FUNCTION = 260,
     TOK_END = 261,
     TOK_MAPTO = 262,
     TOK_FOR = 263,
     TOK_IN = 264,
     TOK_DO = 265,
     TOK_OD = 266,
     TOK_WHILE = 267,
     TOK_REPEAT = 268,
     TOK_UNTIL = 269,
     TOK_IF = 270,
     TOK_THEN = 271,
     TOK_ELSE = 272,
     TOK_ELIF = 273,
     TOK_FI = 274,
     TOK_RETURN = 275,
     TOK_BREAK = 276,
     TOK_QUIT = 277,
     TOK_QUIT_CAP = 278,
     TOK_FALSE = 279,
     TOK_TRUE = 280,
     TOK_IDENTIFIER = 281,
     TOK_CHAR_LITERAL = 282,
     TOK_STRING_LITERAL = 283,
     TOK_INTEGER = 284,
     TOK_ASSIGN = 285,
     TOK_REP_REC_ELM = 286,
     TOK_REP_LIST_ELM = 287,
     TOK_TWO_DOTS = 288,
     TOK_LEQ = 289,
     TOK_GEQ = 290,
     TOK_NEQ = 291,
     TOK_MOD = 292,
     TOK_NOT = 293,
     TOK_AND = 294,
     TOK_OR = 295
   };
#endif
/* Tokens.  */
#define TOK_REC 258
#define TOK_LOCAL 259
#define TOK_FUNCTION 260
#define TOK_END 261
#define TOK_MAPTO 262
#define TOK_FOR 263
#define TOK_IN 264
#define TOK_DO 265
#define TOK_OD 266
#define TOK_WHILE 267
#define TOK_REPEAT 268
#define TOK_UNTIL 269
#define TOK_IF 270
#define TOK_THEN 271
#define TOK_ELSE 272
#define TOK_ELIF 273
#define TOK_FI 274
#define TOK_RETURN 275
#define TOK_BREAK 276
#define TOK_QUIT 277
#define TOK_QUIT_CAP 278
#define TOK_FALSE 279
#define TOK_TRUE 280
#define TOK_IDENTIFIER 281
#define TOK_CHAR_LITERAL 282
#define TOK_STRING_LITERAL 283
#define TOK_INTEGER 284
#define TOK_ASSIGN 285
#define TOK_REP_REC_ELM 286
#define TOK_REP_LIST_ELM 287
#define TOK_TWO_DOTS 288
#define TOK_LEQ 289
#define TOK_GEQ 290
#define TOK_NEQ 291
#define TOK_MOD 292
#define TOK_NOT 293
#define TOK_AND 294
#define TOK_OR 295




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


