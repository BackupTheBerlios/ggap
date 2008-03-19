/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse _gap_parser_yyparse
#define yylex   _gap_parser_yylex
#define yyerror _gap_parser_yyerror
#define yylval  _gap_parser_yylval
#define yychar  _gap_parser_yychar
#define yydebug _gap_parser_yydebug
#define yynerrs _gap_parser_yynerrs
#define yylloc _gap_parser_yylloc

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
     TOK_SAVE_WORKSPACE = 281,
     TOK_IDENTIFIER = 282,
     TOK_CHAR_LITERAL = 283,
     TOK_STRING_LITERAL = 284,
     TOK_INTEGER = 285,
     TOK_ASSIGN = 286,
     TOK_REP_REC_ELM = 287,
     TOK_REP_LIST_ELM = 288,
     TOK_TWO_DOTS = 289,
     TOK_LEQ = 290,
     TOK_GEQ = 291,
     TOK_NEQ = 292,
     TOK_MOD = 293,
     TOK_NOT = 294,
     TOK_AND = 295,
     TOK_OR = 296
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
#define TOK_SAVE_WORKSPACE 281
#define TOK_IDENTIFIER 282
#define TOK_CHAR_LITERAL 283
#define TOK_STRING_LITERAL 284
#define TOK_INTEGER 285
#define TOK_ASSIGN 286
#define TOK_REP_REC_ELM 287
#define TOK_REP_LIST_ELM 288
#define TOK_TWO_DOTS 289
#define TOK_LEQ 290
#define TOK_GEQ 291
#define TOK_NEQ 292
#define TOK_MOD 293
#define TOK_NOT 294
#define TOK_AND 295
#define TOK_OR 296




/* Copy the first part of user declarations.  */
#line 1 "/home/muntyan/projects/ggap/ggap/gapparser-yacc.y"

#include "gapparser-priv.h"


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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


/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 212 "/home/muntyan/projects/ggap/ggap/gapparser-yacc.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  68
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   650

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  61
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  39
/* YYNRULES -- Number of rules.  */
#define YYNRULES  124
/* YYNRULES -- Number of states.  */
#define YYNSTATES  229

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   296

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      43,    44,    58,    56,    45,    57,    47,    59,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    52,    42,
      54,    53,    55,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    48,     2,    49,    60,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    50,     2,    51,    46,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    11,    15,    18,    20,
      22,    24,    26,    28,    31,    33,    35,    37,    39,    41,
      45,    53,    54,    58,    59,    61,    63,    67,    75,    81,
      86,    92,   100,   107,   116,   121,   127,   131,   133,   135,
     139,   143,   149,   154,   159,   161,   165,   171,   176,   181,
     188,   190,   194,   198,   204,   209,   211,   213,   215,   219,
     221,   225,   227,   231,   234,   236,   238,   240,   242,   244,
     246,   248,   250,   254,   256,   258,   260,   264,   266,   268,
     270,   272,   276,   278,   281,   284,   286,   288,   292,   294,
     296,   298,   300,   302,   304,   306,   308,   312,   317,   318,
     322,   328,   336,   341,   348,   356,   362,   366,   371,   374,
     376,   380,   381,   383,   384,   386,   388,   392,   397,   398,
     400,   403,   408,   412,   418
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      62,     0,    -1,    -1,    63,    -1,    42,    -1,    63,    42,
      -1,    63,    64,    42,    -1,    64,    42,    -1,    78,    -1,
      73,    -1,    70,    -1,    71,    -1,    20,    -1,    20,    78,
      -1,    21,    -1,    22,    -1,    23,    -1,     1,    -1,    77,
      -1,    27,     7,    78,    -1,     5,    43,    68,    44,    67,
      62,     6,    -1,    -1,     4,    69,    42,    -1,    -1,    69,
      -1,    27,    -1,    69,    45,    27,    -1,     8,    74,     9,
      78,    10,    62,    11,    -1,    12,    78,    10,    62,    11,
      -1,    13,    62,    14,    78,    -1,    15,    78,    16,    62,
      19,    -1,    15,    78,    16,    62,    17,    62,    19,    -1,
      15,    78,    16,    62,    72,    19,    -1,    15,    78,    16,
      62,    72,    17,    62,    19,    -1,    18,    78,    16,    62,
      -1,    72,    18,    78,    16,    62,    -1,    74,    31,    78,
      -1,    46,    -1,    27,    -1,    74,    47,    27,    -1,    74,
      47,    30,    -1,    74,    47,    43,    78,    44,    -1,    74,
      48,    78,    49,    -1,    74,    50,    78,    51,    -1,    75,
      -1,    74,    32,    27,    -1,    74,    32,    43,    78,    44,
      -1,    74,    33,    78,    49,    -1,    74,    43,    95,    44,
      -1,    74,    43,    95,    52,    76,    44,    -1,    27,    -1,
      27,    31,    78,    -1,    76,    45,    27,    -1,    76,    45,
      27,    31,    78,    -1,    26,    43,    78,    44,    -1,    65,
      -1,    79,    -1,    80,    -1,    79,    41,    80,    -1,    81,
      -1,    80,    40,    81,    -1,    83,    -1,    83,    82,    83,
      -1,    39,    81,    -1,    53,    -1,    37,    -1,    54,    -1,
      55,    -1,    35,    -1,    36,    -1,     9,    -1,    85,    -1,
      83,    84,    85,    -1,    56,    -1,    57,    -1,    87,    -1,
      85,    86,    87,    -1,    58,    -1,    59,    -1,    38,    -1,
      88,    -1,    88,    60,    88,    -1,    89,    -1,    56,    88,
      -1,    57,    88,    -1,    30,    -1,    74,    -1,    43,    78,
      44,    -1,    90,    -1,    28,    -1,    29,    -1,    66,    -1,
      92,    -1,    97,    -1,    25,    -1,    24,    -1,    43,    91,
      44,    -1,    90,    43,    91,    44,    -1,    -1,    78,    45,
      96,    -1,    48,    78,    34,    78,    49,    -1,    48,    78,
      45,    78,    34,    78,    49,    -1,    48,    45,    93,    49,
      -1,    48,    78,    45,    45,    93,    49,    -1,    48,    78,
      45,    78,    45,    93,    49,    -1,    48,    78,    45,    78,
      49,    -1,    48,    78,    49,    -1,    48,    78,    45,    49,
      -1,    48,    49,    -1,    94,    -1,    93,    45,    94,    -1,
      -1,    78,    -1,    -1,    96,    -1,    78,    -1,    96,    45,
      78,    -1,     3,    43,    98,    44,    -1,    -1,    99,    -1,
      99,    45,    -1,    45,    27,    31,    78,    -1,    27,    31,
      78,    -1,    99,    45,    27,    31,    78,    -1,    99,    45,
      45,    27,    31,    78,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    59,    59,    61,    64,    65,    66,    67,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    84,
      87,    92,    94,    96,    98,   101,   102,   106,   107,   108,
     112,   113,   114,   115,   118,   119,   123,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   141,   142,
     145,   146,   147,   148,   152,   156,   157,   161,   162,   166,
     167,   171,   172,   173,   175,   175,   175,   175,   175,   175,
     175,   178,   179,   181,   181,   184,   185,   187,   187,   187,
     190,   191,   194,   195,   196,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   215,   216,   218,   220,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   235,
     236,   238,   240,   243,   245,   248,   249,   253,   255,   257,
     258,   261,   262,   263,   264
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\"rec\"", "\"local\"", "\"function\"",
  "\"end\"", "\"->\"", "\"for\"", "\"in\"", "\"do\"", "\"od\"",
  "\"while\"", "\"repeat\"", "\"until\"", "\"if\"", "\"then\"", "\"else\"",
  "\"elif\"", "\"fi\"", "\"return\"", "\"break\"", "\"quit\"", "\"QUIT\"",
  "\"false\"", "\"true\"", "\"SaveWorkspace\"", "\"identifier\"",
  "\"character\"", "\"string\"", "\"integer\"", "\":=\"", "\"!.\"",
  "\"![\"", "\"..\"", "\"<=\"", "\">=\"", "\"<>\"", "\"mod\"", "\"not\"",
  "\"and\"", "\"or\"", "';'", "'('", "')'", "','", "'~'", "'.'", "'['",
  "']'", "'{'", "'}'", "':'", "'='", "'<'", "'>'", "'+'", "'-'", "'*'",
  "'/'", "'^'", "$accept", "Program", "non_empty_program", "statement",
  "ShortFunction", "Function", "funcdef_local", "funcdef_args",
  "funcdef_arglist", "Loop", "Conditional", "elif_block", "Assignment",
  "Lvalue", "FunctionCall", "func_opt_list", "save_workspace",
  "expression", "Logical", "And", "Rel", "RelOp", "Sum", "SumOp",
  "Product", "ProductOp", "Power", "power_operand", "Atom", "Permutation",
  "perm_elms", "List", "list_elms", "maybe_expression", "expr_list",
  "non_empty_expr_list", "Record", "rec_elms", "non_empty_rec_elms", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,    59,    40,    41,    44,   126,    46,    91,    93,
     123,   125,    58,    61,    60,    62,    43,    45,    42,    47,
      94
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    61,    62,    62,    63,    63,    63,    63,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    65,
      66,    67,    67,    68,    68,    69,    69,    70,    70,    70,
      71,    71,    71,    71,    72,    72,    73,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    75,    75,
      76,    76,    76,    76,    77,    78,    78,    79,    79,    80,
      80,    81,    81,    81,    82,    82,    82,    82,    82,    82,
      82,    83,    83,    84,    84,    85,    85,    86,    86,    86,
      87,    87,    88,    88,    88,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    90,    90,    91,    91,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    93,
      93,    94,    94,    95,    95,    96,    96,    97,    98,    98,
      98,    99,    99,    99,    99
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     2,     3,     2,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     1,     3,
       7,     0,     3,     0,     1,     1,     3,     7,     5,     4,
       5,     7,     6,     8,     4,     5,     3,     1,     1,     3,
       3,     5,     4,     4,     1,     3,     5,     4,     4,     6,
       1,     3,     3,     5,     4,     1,     1,     1,     3,     1,
       3,     1,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     1,     1,     3,     1,     1,     1,
       1,     3,     1,     2,     2,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     4,     0,     3,
       5,     7,     4,     6,     7,     5,     3,     4,     2,     1,
       3,     0,     1,     0,     1,     1,     3,     4,     0,     1,
       2,     4,     3,     5,     6
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    17,     0,     0,     0,     0,     0,     0,    12,    14,
      15,    16,    95,    94,     0,    38,    89,    90,    85,     0,
       4,    98,    37,     0,     0,     0,     0,     0,     0,    55,
      91,    10,    11,     9,    86,    44,    18,     8,    56,    57,
      59,    61,    71,    75,    80,    82,    88,    92,    93,   118,
      23,    38,     0,    86,     0,     0,     0,    13,     0,     0,
      63,     0,     0,   111,   108,     0,    83,    84,     1,     5,
       0,     7,     0,     0,     0,   113,     0,     0,     0,     0,
       0,    70,    68,    69,    65,    64,    66,    67,    73,    74,
       0,     0,    79,    77,    78,     0,     0,    98,     0,     0,
       0,   119,    25,     0,    24,     0,     0,     0,     0,     0,
      19,    87,     0,    96,   112,     0,   109,     0,     0,   106,
       6,    36,    45,     0,     0,   115,     0,   114,    39,    40,
       0,     0,     0,    58,    60,    62,    72,    76,    81,     0,
       0,     0,     0,   117,   120,    21,     0,     0,     0,    29,
       0,    54,    99,   111,   102,     0,   111,   107,     0,     0,
      47,    48,     0,     0,     0,    42,    43,    97,   122,     0,
       0,     0,     0,     0,    26,     0,    28,     0,     0,    30,
       0,   110,   100,     0,     0,   111,   105,    46,    50,     0,
     116,    41,   121,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    32,   103,     0,     0,     0,    49,     0,   123,
       0,    22,    20,    27,    31,     0,     0,     0,   101,   104,
      51,    52,   124,    34,    33,     0,     0,    35,    53
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    26,    27,    28,    29,    30,   173,   103,   104,    31,
      32,   180,    33,    53,    35,   189,    36,    37,    38,    39,
      40,    90,    41,    91,    42,    95,    43,    44,    45,    46,
      62,    47,   115,   116,   126,   127,    48,   100,   101
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -145
static const yytype_int16 yypact[] =
{
     282,  -145,   -32,     5,   -13,   551,   374,   551,   551,  -145,
    -145,  -145,  -145,  -145,    11,    23,  -145,  -145,  -145,   586,
    -145,   551,  -145,    10,   593,   593,    45,   232,    21,  -145,
    -145,  -145,  -145,  -145,   109,  -145,  -145,  -145,    34,    17,
    -145,    15,   -30,  -145,    27,  -145,    47,  -145,  -145,    -9,
      65,  -145,    53,   131,    85,    83,    86,  -145,   551,   551,
    -145,    60,    77,   551,  -145,    -3,  -145,  -145,  -145,  -145,
      81,  -145,   551,   -22,   551,   551,   -11,   551,   551,   586,
     586,  -145,  -145,  -145,  -145,  -145,  -145,  -145,  -145,  -145,
     593,   593,  -145,  -145,  -145,   593,   593,   551,    95,   108,
     105,   106,  -145,   110,   115,   551,   424,   551,   328,   117,
    -145,  -145,   551,  -145,  -145,    -2,  -145,   551,    88,  -145,
    -145,  -145,  -145,   551,   101,  -145,    37,   121,  -145,  -145,
     551,   104,   116,    17,  -145,    54,   -30,  -145,  -145,   123,
     118,   551,   138,  -145,    -1,   166,   145,   172,   165,  -145,
     111,  -145,   121,   551,  -145,   134,   551,  -145,    49,   140,
    -145,  -145,   158,   551,   144,  -145,  -145,  -145,  -145,   551,
     159,   167,    65,   470,  -145,   424,  -145,   516,   551,  -145,
     129,  -145,  -145,    33,   551,   551,  -145,  -145,   161,    94,
    -145,  -145,  -145,   551,   168,   -25,   190,   186,   179,   185,
     516,   551,  -145,  -145,   153,    39,   551,  -145,   177,  -145,
     551,  -145,  -145,  -145,  -145,   328,   187,   189,  -145,  -145,
    -145,   176,  -145,  -145,  -145,   328,   551,  -145,  -145
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -145,    16,  -145,   182,  -145,  -145,  -145,  -145,    38,  -145,
    -145,  -145,  -145,     0,  -145,  -145,  -145,     2,  -145,   132,
     -16,  -145,   124,  -145,   122,  -145,   125,   -23,  -145,  -145,
     120,  -145,  -144,    66,  -145,   112,  -145,  -145,  -145
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -4
static const yytype_int16 yytable[] =
{
      34,    66,    67,    60,    52,   122,    34,    54,    92,    56,
      57,    49,   183,     2,    51,     3,   128,   211,    98,   129,
     146,   123,    55,    61,    81,    65,   170,    34,    93,    94,
      59,   117,   130,    22,    12,    13,    99,    15,    16,    17,
      18,   205,   118,   153,   171,    68,   119,   154,    50,    19,
      82,    83,    84,    21,    58,    63,    22,    80,    23,    64,
     109,   110,   105,    71,   134,   114,    24,    25,    85,    86,
      87,    88,    89,   138,   121,    79,   124,   125,   153,   131,
     132,   161,   203,   184,   153,    73,    74,    96,   219,   162,
      97,     2,   102,     3,   185,   106,    75,   107,   186,   139,
      76,    77,   108,    78,   111,   112,    34,   147,    34,   149,
      88,    89,    12,    13,   125,    15,    16,    17,    18,   155,
     158,   113,   148,   120,   150,   159,   141,    19,   177,   178,
     179,    21,   164,   156,    22,   142,    23,   157,   207,   208,
      72,    73,    74,   168,    24,    25,   200,   201,   202,   143,
     160,   144,    75,   165,   145,   114,    76,    77,   114,    78,
     146,   151,   167,    73,    74,   190,   163,   166,   112,   169,
     172,   192,   174,    34,    75,    34,   176,    34,    76,    77,
     199,    78,   175,   182,   187,   188,   204,   114,   191,   196,
     193,   197,   206,   198,   194,   209,   212,   213,   214,   210,
      34,   215,   218,   217,   221,   225,   224,   226,   220,    70,
     195,   133,   222,   136,   135,    34,   216,   140,     0,   181,
     137,     0,     0,     0,   152,    34,     0,     0,   228,     0,
       0,   223,    -3,     1,     0,     2,     0,     3,    -3,     0,
       4,   227,     0,    -3,     5,     6,    -3,     7,     0,    -3,
      -3,    -3,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,     0,     0,     0,     0,     0,     0,     0,
       0,    19,     0,     0,    69,    21,     0,     0,    22,     0,
      23,     0,    -2,     1,     0,     2,     0,     3,    24,    25,
       4,     0,     0,     0,     5,     6,     0,     7,     0,     0,
       0,     0,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,     0,     0,     0,     0,     0,     0,     0,
       0,    19,     0,     0,    20,    21,     0,     0,    22,     1,
      23,     2,     0,     3,     0,     0,     4,     0,    24,    25,
       5,     6,     0,     7,     0,    -2,    -2,    -2,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,     0,
       0,     0,     0,     0,     0,     0,     0,    19,     0,     0,
      20,    21,     0,     0,    22,     1,    23,     2,     0,     3,
       0,     0,     4,     0,    24,    25,     5,     6,    -2,     7,
       0,     0,     0,     0,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,     0,     0,     0,     0,     0,
       0,     0,     0,    19,     0,     0,    20,    21,     0,     0,
      22,     0,    23,     0,     0,     1,     0,     2,     0,     3,
      24,    25,     4,     0,     0,    -2,     5,     6,     0,     7,
       0,     0,     0,     0,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,     0,     0,     0,     0,     0,
       0,     0,     0,    19,     0,     0,    20,    21,     0,     0,
      22,     1,    23,     2,     0,     3,    -2,     0,     4,     0,
      24,    25,     5,     6,     0,     7,     0,     0,     0,     0,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,     0,     0,     0,     0,     0,     0,     0,     0,    19,
       0,     0,    20,    21,     0,     0,    22,     1,    23,     2,
       0,     3,     0,     0,     4,     0,    24,    25,     5,     6,
       0,     7,     0,     0,     0,    -2,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,     0,     0,     0,
       0,     0,     0,     0,     2,    19,     3,     0,    20,    21,
       0,     0,    22,     0,    23,     0,     0,     0,     0,     0,
       0,     0,    24,    25,     0,    12,    13,     0,    15,    16,
      17,    18,     0,     0,     0,     0,     0,     0,     0,     2,
      19,     3,     0,     0,    21,     0,     2,    22,     3,    23,
       0,     0,     0,     0,     0,     0,     0,    24,    25,     0,
      12,    13,     0,    51,    16,    17,    18,    12,    13,     0,
      51,    16,    17,    18,     0,    19,     0,     0,     0,    21,
       0,     0,    22,     0,    23,     0,    21,     0,     0,    22,
       0,    23,    24,    25,     0,     0,     0,     0,     0,    24,
      25
};

static const yytype_int16 yycheck[] =
{
       0,    24,    25,    19,     4,    27,     6,     5,    38,     7,
       8,    43,   156,     3,    27,     5,    27,    42,    27,    30,
      45,    43,     6,    21,     9,    23,    27,    27,    58,    59,
       7,    34,    43,    46,    24,    25,    45,    27,    28,    29,
      30,   185,    45,    45,    45,     0,    49,    49,    43,    39,
      35,    36,    37,    43,    43,    45,    46,    40,    48,    49,
      58,    59,     9,    42,    80,    63,    56,    57,    53,    54,
      55,    56,    57,    96,    72,    41,    74,    75,    45,    77,
      78,    44,    49,    34,    45,    32,    33,    60,    49,    52,
      43,     3,    27,     5,    45,    10,    43,    14,    49,    97,
      47,    48,    16,    50,    44,    45,   106,   105,   108,   107,
      56,    57,    24,    25,   112,    27,    28,    29,    30,   117,
     118,    44,   106,    42,   108,   123,    31,    39,    17,    18,
      19,    43,   130,    45,    46,    27,    48,    49,    44,    45,
      31,    32,    33,   141,    56,    57,    17,    18,    19,    44,
      49,    45,    43,    49,    44,   153,    47,    48,   156,    50,
      45,    44,    44,    32,    33,   163,    45,    51,    45,    31,
       4,   169,    27,   173,    43,   175,    11,   177,    47,    48,
     178,    50,    10,    49,    44,    27,   184,   185,    44,   173,
      31,   175,    31,   177,    27,   193,     6,    11,    19,    31,
     200,    16,    49,   201,    27,    16,    19,    31,   206,    27,
     172,    79,   210,    91,    90,   215,   200,    97,    -1,   153,
      95,    -1,    -1,    -1,   112,   225,    -1,    -1,   226,    -1,
      -1,   215,     0,     1,    -1,     3,    -1,     5,     6,    -1,
       8,   225,    -1,    11,    12,    13,    14,    15,    -1,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    -1,    -1,    42,    43,    -1,    -1,    46,    -1,
      48,    -1,     0,     1,    -1,     3,    -1,     5,    56,    57,
       8,    -1,    -1,    -1,    12,    13,    -1,    15,    -1,    -1,
      -1,    -1,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    -1,    -1,    42,    43,    -1,    -1,    46,     1,
      48,     3,    -1,     5,    -1,    -1,     8,    -1,    56,    57,
      12,    13,    -1,    15,    -1,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    39,    -1,    -1,
      42,    43,    -1,    -1,    46,     1,    48,     3,    -1,     5,
      -1,    -1,     8,    -1,    56,    57,    12,    13,    14,    15,
      -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    39,    -1,    -1,    42,    43,    -1,    -1,
      46,    -1,    48,    -1,    -1,     1,    -1,     3,    -1,     5,
      56,    57,     8,    -1,    -1,    11,    12,    13,    -1,    15,
      -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    39,    -1,    -1,    42,    43,    -1,    -1,
      46,     1,    48,     3,    -1,     5,     6,    -1,     8,    -1,
      56,    57,    12,    13,    -1,    15,    -1,    -1,    -1,    -1,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    39,
      -1,    -1,    42,    43,    -1,    -1,    46,     1,    48,     3,
      -1,     5,    -1,    -1,     8,    -1,    56,    57,    12,    13,
      -1,    15,    -1,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,    39,     5,    -1,    42,    43,
      -1,    -1,    46,    -1,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    56,    57,    -1,    24,    25,    -1,    27,    28,
      29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
      39,     5,    -1,    -1,    43,    -1,     3,    46,     5,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,    57,    -1,
      24,    25,    -1,    27,    28,    29,    30,    24,    25,    -1,
      27,    28,    29,    30,    -1,    39,    -1,    -1,    -1,    43,
      -1,    -1,    46,    -1,    48,    -1,    43,    -1,    -1,    46,
      -1,    48,    56,    57,    -1,    -1,    -1,    -1,    -1,    56,
      57
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     5,     8,    12,    13,    15,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    39,
      42,    43,    46,    48,    56,    57,    62,    63,    64,    65,
      66,    70,    71,    73,    74,    75,    77,    78,    79,    80,
      81,    83,    85,    87,    88,    89,    90,    92,    97,    43,
      43,    27,    74,    74,    78,    62,    78,    78,    43,     7,
      81,    78,    91,    45,    49,    78,    88,    88,     0,    42,
      64,    42,    31,    32,    33,    43,    47,    48,    50,    41,
      40,     9,    35,    36,    37,    53,    54,    55,    56,    57,
      82,    84,    38,    58,    59,    86,    60,    43,    27,    45,
      98,    99,    27,    68,    69,     9,    10,    14,    16,    78,
      78,    44,    45,    44,    78,    93,    94,    34,    45,    49,
      42,    78,    27,    43,    78,    78,    95,    96,    27,    30,
      43,    78,    78,    80,    81,    83,    85,    87,    88,    78,
      91,    31,    27,    44,    45,    44,    45,    78,    62,    78,
      62,    44,    96,    45,    49,    78,    45,    49,    78,    78,
      49,    44,    52,    45,    78,    49,    51,    44,    78,    31,
      27,    45,     4,    67,    27,    10,    11,    17,    18,    19,
      72,    94,    49,    93,    34,    45,    49,    44,    27,    76,
      78,    44,    78,    31,    27,    69,    62,    62,    62,    78,
      17,    18,    19,    49,    78,    93,    31,    44,    45,    78,
      31,    42,     6,    11,    19,    16,    62,    78,    49,    49,
      78,    27,    78,    62,    19,    16,    31,    62,    78
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (&yylloc, parser, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, parser)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, parser); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, GapParser *parser)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, parser)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    GapParser *parser;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (parser);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, GapParser *parser)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, parser)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    GapParser *parser;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, parser);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, GapParser *parser)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, parser)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    GapParser *parser;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , parser);
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, parser); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, GapParser *parser)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, parser)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    GapParser *parser;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (parser);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (GapParser *parser);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (GapParser *parser)
#else
int
yyparse (parser)
    GapParser *parser;
#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;
#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
      
/* Line 1267 of yacc.c.  */
#line 1738 "/home/muntyan/projects/ggap/ggap/gapparser-yacc.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, parser, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (&yylloc, parser, yymsg);
	  }
	else
	  {
	    yyerror (&yylloc, parser, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, parser);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, parser);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, parser, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, parser);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, parser);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 267 "/home/muntyan/projects/ggap/ggap/gapparser-yacc.y"


