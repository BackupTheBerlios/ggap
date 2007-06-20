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
#line 210 "/home/muntyan/projects/ggap/ggap/gapparser-yacc.c"

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
#define YYFINAL  65
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   660

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  60
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  38
/* YYNRULES -- Number of rules.  */
#define YYNRULES  122
/* YYNRULES -- Number of states.  */
#define YYNSTATES  224

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   295

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      42,    43,    57,    55,    44,    56,    46,    58,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    51,    41,
      53,    52,    54,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    47,     2,    48,    59,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    49,     2,    50,    45,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    11,    15,    18,    20,
      22,    24,    26,    28,    31,    33,    35,    37,    39,    43,
      51,    52,    56,    57,    59,    61,    65,    73,    79,    84,
      90,    98,   105,   114,   119,   125,   129,   131,   133,   137,
     141,   147,   152,   157,   159,   163,   169,   174,   179,   186,
     188,   192,   196,   202,   204,   206,   208,   212,   214,   218,
     220,   224,   227,   229,   231,   233,   235,   237,   239,   241,
     243,   247,   249,   251,   253,   257,   259,   261,   263,   265,
     269,   271,   274,   277,   279,   281,   285,   287,   289,   291,
     293,   295,   297,   299,   301,   305,   310,   311,   315,   321,
     329,   334,   341,   349,   355,   359,   364,   367,   369,   373,
     374,   376,   377,   379,   381,   385,   390,   391,   393,   396,
     401,   405,   411
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      61,     0,    -1,    -1,    62,    -1,    41,    -1,    62,    41,
      -1,    62,    63,    41,    -1,    63,    41,    -1,    76,    -1,
      72,    -1,    69,    -1,    70,    -1,    20,    -1,    20,    76,
      -1,    21,    -1,    22,    -1,    23,    -1,     1,    -1,    26,
       7,    76,    -1,     5,    42,    67,    43,    66,    61,     6,
      -1,    -1,     4,    68,    41,    -1,    -1,    68,    -1,    26,
      -1,    68,    44,    26,    -1,     8,    73,     9,    76,    10,
      61,    11,    -1,    12,    76,    10,    61,    11,    -1,    13,
      61,    14,    76,    -1,    15,    76,    16,    61,    19,    -1,
      15,    76,    16,    61,    17,    61,    19,    -1,    15,    76,
      16,    61,    71,    19,    -1,    15,    76,    16,    61,    71,
      17,    61,    19,    -1,    18,    76,    16,    61,    -1,    71,
      18,    76,    16,    61,    -1,    73,    30,    76,    -1,    45,
      -1,    26,    -1,    73,    46,    26,    -1,    73,    46,    29,
      -1,    73,    46,    42,    76,    43,    -1,    73,    47,    76,
      48,    -1,    73,    49,    76,    50,    -1,    74,    -1,    73,
      31,    26,    -1,    73,    31,    42,    76,    43,    -1,    73,
      32,    76,    48,    -1,    73,    42,    93,    43,    -1,    73,
      42,    93,    51,    75,    43,    -1,    26,    -1,    26,    30,
      76,    -1,    75,    44,    26,    -1,    75,    44,    26,    30,
      76,    -1,    64,    -1,    77,    -1,    78,    -1,    77,    40,
      78,    -1,    79,    -1,    78,    39,    79,    -1,    81,    -1,
      81,    80,    81,    -1,    38,    79,    -1,    52,    -1,    36,
      -1,    53,    -1,    54,    -1,    34,    -1,    35,    -1,     9,
      -1,    83,    -1,    81,    82,    83,    -1,    55,    -1,    56,
      -1,    85,    -1,    83,    84,    85,    -1,    57,    -1,    58,
      -1,    37,    -1,    86,    -1,    86,    59,    86,    -1,    87,
      -1,    55,    86,    -1,    56,    86,    -1,    29,    -1,    73,
      -1,    42,    76,    43,    -1,    88,    -1,    27,    -1,    28,
      -1,    65,    -1,    90,    -1,    95,    -1,    25,    -1,    24,
      -1,    42,    89,    43,    -1,    88,    42,    89,    43,    -1,
      -1,    76,    44,    94,    -1,    47,    76,    33,    76,    48,
      -1,    47,    76,    44,    76,    33,    76,    48,    -1,    47,
      44,    91,    48,    -1,    47,    76,    44,    44,    91,    48,
      -1,    47,    76,    44,    76,    44,    91,    48,    -1,    47,
      76,    44,    76,    48,    -1,    47,    76,    48,    -1,    47,
      76,    44,    48,    -1,    47,    48,    -1,    92,    -1,    91,
      44,    92,    -1,    -1,    76,    -1,    -1,    94,    -1,    76,
      -1,    94,    44,    76,    -1,     3,    42,    96,    43,    -1,
      -1,    97,    -1,    97,    44,    -1,    44,    26,    30,    76,
      -1,    26,    30,    76,    -1,    97,    44,    26,    30,    76,
      -1,    97,    44,    44,    26,    30,    76,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    41,    41,    42,    45,    46,    47,    48,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    62,    64,
      69,    70,    72,    73,    76,    77,    80,    81,    82,    86,
      87,    88,    89,    92,    93,    96,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   112,   113,   115,
     116,   117,   118,   121,   122,   125,   126,   129,   130,   133,
     134,   135,   137,   137,   137,   137,   137,   137,   137,   139,
     140,   142,   142,   144,   145,   147,   147,   147,   149,   150,
     152,   153,   154,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   171,   172,   174,   175,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   188,   189,   191,
     192,   195,   196,   199,   200,   203,   205,   206,   207,   210,
     211,   212,   213
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOK_REC", "TOK_LOCAL", "TOK_FUNCTION",
  "TOK_END", "TOK_MAPTO", "TOK_FOR", "TOK_IN", "TOK_DO", "TOK_OD",
  "TOK_WHILE", "TOK_REPEAT", "TOK_UNTIL", "TOK_IF", "TOK_THEN", "TOK_ELSE",
  "TOK_ELIF", "TOK_FI", "TOK_RETURN", "TOK_BREAK", "TOK_QUIT",
  "TOK_QUIT_CAP", "TOK_FALSE", "TOK_TRUE", "TOK_IDENTIFIER",
  "TOK_CHAR_LITERAL", "TOK_STRING_LITERAL", "TOK_INTEGER", "TOK_ASSIGN",
  "TOK_REP_REC_ELM", "TOK_REP_LIST_ELM", "TOK_TWO_DOTS", "TOK_LEQ",
  "TOK_GEQ", "TOK_NEQ", "TOK_MOD", "TOK_NOT", "TOK_AND", "TOK_OR", "';'",
  "'('", "')'", "','", "'~'", "'.'", "'['", "']'", "'{'", "'}'", "':'",
  "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'^'", "$accept",
  "Program", "non_empty_program", "Statement", "ShortFunction", "Function",
  "funcdef_local", "funcdef_args", "funcdef_arglist", "Loop",
  "Conditional", "elif_block", "Assignment", "Lvalue", "FunctionCall",
  "func_opt_list", "Expression", "Logical", "And", "Rel", "RelOp", "Sum",
  "SumOp", "Product", "ProductOp", "Power", "power_operand", "Atom",
  "Permutation", "perm_elms", "List", "list_elms", "expr_or_empty",
  "expr_list", "non_empty_expr_list", "Record", "rec_elms",
  "non_empty_rec_elms", 0
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
     295,    59,    40,    41,    44,   126,    46,    91,    93,   123,
     125,    58,    61,    60,    62,    43,    45,    42,    47,    94
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    60,    61,    61,    62,    62,    62,    62,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    64,    65,
      66,    66,    67,    67,    68,    68,    69,    69,    69,    70,
      70,    70,    70,    71,    71,    72,    73,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    74,    74,    75,
      75,    75,    75,    76,    76,    77,    77,    78,    78,    79,
      79,    79,    80,    80,    80,    80,    80,    80,    80,    81,
      81,    82,    82,    83,    83,    84,    84,    84,    85,    85,
      86,    86,    86,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    88,    88,    89,    89,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    91,    91,    92,
      92,    93,    93,    94,    94,    95,    96,    96,    96,    97,
      97,    97,    97
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     2,     3,     2,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     3,     7,
       0,     3,     0,     1,     1,     3,     7,     5,     4,     5,
       7,     6,     8,     4,     5,     3,     1,     1,     3,     3,
       5,     4,     4,     1,     3,     5,     4,     4,     6,     1,
       3,     3,     5,     1,     1,     1,     3,     1,     3,     1,
       3,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     3,     1,     1,     1,     1,     3,
       1,     2,     2,     1,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     4,     0,     3,     5,     7,
       4,     6,     7,     5,     3,     4,     2,     1,     3,     0,
       1,     0,     1,     1,     3,     4,     0,     1,     2,     4,
       3,     5,     6
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    17,     0,     0,     0,     0,     0,     0,    12,    14,
      15,    16,    93,    92,    37,    87,    88,    83,     0,     4,
      96,    36,     0,     0,     0,     0,     0,     0,    53,    89,
      10,    11,     9,    84,    43,     8,    54,    55,    57,    59,
      69,    73,    78,    80,    86,    90,    91,   116,    22,    37,
       0,    84,     0,     0,     0,    13,     0,    61,     0,     0,
     109,   106,     0,    81,    82,     1,     5,     0,     7,     0,
       0,     0,   111,     0,     0,     0,     0,     0,    68,    66,
      67,    63,    62,    64,    65,    71,    72,     0,     0,    77,
      75,    76,     0,     0,    96,     0,     0,     0,   117,    24,
       0,    23,     0,     0,     0,     0,    18,    85,     0,    94,
     110,     0,   107,     0,     0,   104,     6,    35,    44,     0,
       0,   113,     0,   112,    38,    39,     0,     0,     0,    56,
      58,    60,    70,    74,    79,     0,     0,     0,     0,   115,
     118,    20,     0,     0,     0,    28,     0,    97,   109,   100,
       0,   109,   105,     0,     0,    46,    47,     0,     0,     0,
      41,    42,    95,   120,     0,     0,     0,     0,     0,    25,
       0,    27,     0,     0,    29,     0,   108,    98,     0,     0,
     109,   103,    45,    49,     0,   114,    40,   119,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    31,   101,     0,
       0,     0,    48,     0,   121,     0,    21,    19,    26,    30,
       0,     0,     0,    99,   102,    50,    51,   122,    33,    32,
       0,     0,    34,    52
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    25,    26,    27,    28,    29,   168,   100,   101,    30,
      31,   175,    32,    51,    34,   184,    35,    36,    37,    38,
      87,    39,    88,    40,    92,    41,    42,    43,    44,    59,
      45,   111,   112,   122,   123,    46,    97,    98
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -137
static const yytype_int16 yypact[] =
{
     273,  -137,   -11,     7,    -6,   109,   363,   109,   109,  -137,
    -137,  -137,  -137,  -137,    52,  -137,  -137,  -137,   570,  -137,
     109,  -137,     8,   604,   604,    51,   224,    34,  -137,  -137,
    -137,  -137,  -137,    48,  -137,  -137,    49,    54,  -137,    31,
      11,  -137,    32,  -137,    77,  -137,  -137,    -3,    94,  -137,
      -4,    76,   114,   112,   111,  -137,   109,  -137,   -25,   102,
     109,  -137,    69,  -137,  -137,  -137,  -137,   105,  -137,   109,
      28,   109,   109,   -12,   109,   109,   570,   570,  -137,  -137,
    -137,  -137,  -137,  -137,  -137,  -137,  -137,   604,   604,  -137,
    -137,  -137,   604,   604,   109,   118,   123,   115,   108,  -137,
     116,   117,   109,   412,   109,   318,  -137,  -137,   109,  -137,
    -137,   -23,  -137,   109,   536,  -137,  -137,  -137,  -137,   109,
     107,  -137,   -35,   119,  -137,  -137,   109,   121,   127,    54,
    -137,    26,    11,  -137,  -137,   129,   128,   109,   132,  -137,
       3,   153,   141,   169,   172,  -137,   113,   119,   109,  -137,
     136,   109,  -137,    96,   137,  -137,  -137,   159,   109,   143,
    -137,  -137,  -137,  -137,   109,   157,   162,    94,   457,  -137,
     412,  -137,   502,   109,  -137,   124,  -137,  -137,    13,   109,
     109,  -137,  -137,   161,    56,  -137,  -137,  -137,   109,   163,
      57,   183,   181,   175,   180,   502,   109,  -137,  -137,   149,
      44,   109,  -137,   173,  -137,   109,  -137,  -137,  -137,  -137,
     318,   185,   184,  -137,  -137,  -137,   176,  -137,  -137,  -137,
     318,   109,  -137,  -137
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -137,     6,  -137,   179,  -137,  -137,  -137,  -137,    35,  -137,
    -137,  -137,  -137,     0,  -137,  -137,     2,  -137,   133,   -17,
    -137,   125,  -137,   120,  -137,   122,   -21,  -137,  -137,   134,
    -137,  -136,    63,  -137,   110,  -137,  -137,  -137
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -4
static const yytype_int16 yytable[] =
{
      33,    57,    63,    64,    50,   102,    33,    52,   156,    54,
      55,     2,    53,     3,   124,   178,   157,   125,   107,   108,
      49,   148,    58,    95,    62,   149,    33,    70,    71,   165,
     126,    47,    12,    13,    14,    15,    16,    17,    72,    21,
      78,    96,    73,    74,   200,    75,    18,   166,    89,    48,
      20,    65,    60,    21,   118,    22,    61,   148,   106,    56,
     130,   198,   110,    23,    24,    79,    80,    81,    90,    91,
     119,   117,   134,   120,   121,    68,   127,   128,    69,    70,
      71,    85,    86,    82,    83,    84,    85,    86,   148,    76,
      72,    93,   214,    77,    73,    74,   135,    75,   206,   202,
     203,   142,   113,    33,   143,    33,   145,    70,    71,   144,
     121,   146,     2,   114,     3,   150,   153,   115,    72,    94,
      99,   154,    73,    74,   103,    75,   104,   105,   159,   179,
     172,   173,   174,    12,    13,    14,    15,    16,    17,   163,
     180,   195,   196,   197,   181,   109,   116,    18,   137,   138,
     110,    20,   140,   110,    21,   155,    22,   167,   139,   141,
     185,   142,   164,   158,    23,    24,   187,   169,    33,   160,
      33,   162,    33,   108,   191,   194,   192,   161,   193,   170,
     182,   199,   110,   171,   177,   183,   186,   188,   189,   207,
     204,   201,   208,   205,   209,    33,   210,   213,   212,   216,
     220,   211,   190,   215,   219,    67,   221,   217,   132,   129,
      33,   176,   131,     0,   133,     0,   218,     0,   147,     0,
      33,     0,     0,   223,    -3,     1,   222,     2,   136,     3,
      -3,     0,     4,     0,     0,    -3,     5,     6,    -3,     7,
       0,    -3,    -3,    -3,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,     0,     0,     0,     0,     0,     0,
       0,     0,    18,     0,     0,    66,    20,     0,     0,    21,
       0,    22,     0,    -2,     1,     0,     2,     0,     3,    23,
      24,     4,     0,     0,     0,     5,     6,     0,     7,     0,
       0,     0,     0,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,     0,     0,     0,     0,     0,     0,     0,
       0,    18,     0,     0,    19,    20,     0,     0,    21,     1,
      22,     2,     0,     3,     0,     0,     4,     0,    23,    24,
       5,     6,     0,     7,     0,    -2,    -2,    -2,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,     0,     0,
       0,     0,     0,     0,     0,     0,    18,     0,     0,    19,
      20,     0,     0,    21,     1,    22,     2,     0,     3,     0,
       0,     4,     0,    23,    24,     5,     6,    -2,     7,     0,
       0,     0,     0,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,     0,     0,     0,     0,     0,     0,     0,
       0,    18,     0,     0,    19,    20,     0,     0,    21,     0,
      22,     0,     0,     1,     0,     2,     0,     3,    23,    24,
       4,     0,     0,    -2,     5,     6,     0,     7,     0,     0,
       0,     0,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
      18,     0,     0,    19,    20,     0,     0,    21,     1,    22,
       2,     0,     3,    -2,     0,     4,     0,    23,    24,     5,
       6,     0,     7,     0,     0,     0,     0,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,     0,     0,     0,
       0,     0,     0,     0,     0,    18,     0,     0,    19,    20,
       0,     0,    21,     1,    22,     2,     0,     3,     0,     0,
       4,     0,    23,    24,     5,     6,     0,     7,     0,     0,
       0,    -2,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     2,
      18,     3,     0,    19,    20,     0,     0,    21,     0,    22,
       0,     0,     0,     0,     0,     0,     0,    23,    24,     0,
      12,    13,    14,    15,    16,    17,     0,     0,     0,     0,
       0,     0,     0,     2,    18,     3,     0,     0,    20,     0,
     151,    21,     0,    22,   152,     0,     0,     0,     0,     0,
       0,    23,    24,     0,    12,    13,    49,    15,    16,    17,
       0,     0,     0,     0,     0,     0,     0,     2,    18,     3,
       0,     0,    20,     0,     0,    21,     0,    22,     0,     0,
       0,     0,     0,     0,     0,    23,    24,     0,    12,    13,
      49,    15,    16,    17,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    20,     0,     0,    21,
       0,    22,     0,     0,     0,     0,     0,     0,     0,    23,
      24
};

static const yytype_int16 yycheck[] =
{
       0,    18,    23,    24,     4,     9,     6,     5,    43,     7,
       8,     3,     6,     5,    26,   151,    51,    29,    43,    44,
      26,    44,    20,    26,    22,    48,    26,    31,    32,    26,
      42,    42,    24,    25,    26,    27,    28,    29,    42,    45,
       9,    44,    46,    47,   180,    49,    38,    44,    37,    42,
      42,     0,    44,    45,    26,    47,    48,    44,    56,     7,
      77,    48,    60,    55,    56,    34,    35,    36,    57,    58,
      42,    69,    93,    71,    72,    41,    74,    75,    30,    31,
      32,    55,    56,    52,    53,    54,    55,    56,    44,    40,
      42,    59,    48,    39,    46,    47,    94,    49,    41,    43,
      44,    44,    33,   103,   102,   105,   104,    31,    32,   103,
     108,   105,     3,    44,     5,   113,   114,    48,    42,    42,
      26,   119,    46,    47,    10,    49,    14,    16,   126,    33,
      17,    18,    19,    24,    25,    26,    27,    28,    29,   137,
      44,    17,    18,    19,    48,    43,    41,    38,    30,    26,
     148,    42,    44,   151,    45,    48,    47,     4,    43,    43,
     158,    44,    30,    44,    55,    56,   164,    26,   168,    48,
     170,    43,   172,    44,   168,   173,   170,    50,   172,    10,
      43,   179,   180,    11,    48,    26,    43,    30,    26,     6,
     188,    30,    11,    30,    19,   195,    16,    48,   196,    26,
      16,   195,   167,   201,    19,    26,    30,   205,    88,    76,
     210,   148,    87,    -1,    92,    -1,   210,    -1,   108,    -1,
     220,    -1,    -1,   221,     0,     1,   220,     3,    94,     5,
       6,    -1,     8,    -1,    -1,    11,    12,    13,    14,    15,
      -1,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    38,    -1,    -1,    41,    42,    -1,    -1,    45,
      -1,    47,    -1,     0,     1,    -1,     3,    -1,     5,    55,
      56,     8,    -1,    -1,    -1,    12,    13,    -1,    15,    -1,
      -1,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    -1,    -1,    41,    42,    -1,    -1,    45,     1,
      47,     3,    -1,     5,    -1,    -1,     8,    -1,    55,    56,
      12,    13,    -1,    15,    -1,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,    -1,    41,
      42,    -1,    -1,    45,     1,    47,     3,    -1,     5,    -1,
      -1,     8,    -1,    55,    56,    12,    13,    14,    15,    -1,
      -1,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    -1,    -1,    41,    42,    -1,    -1,    45,    -1,
      47,    -1,    -1,     1,    -1,     3,    -1,     5,    55,    56,
       8,    -1,    -1,    11,    12,    13,    -1,    15,    -1,    -1,
      -1,    -1,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    -1,    -1,    41,    42,    -1,    -1,    45,     1,    47,
       3,    -1,     5,     6,    -1,     8,    -1,    55,    56,    12,
      13,    -1,    15,    -1,    -1,    -1,    -1,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    38,    -1,    -1,    41,    42,
      -1,    -1,    45,     1,    47,     3,    -1,     5,    -1,    -1,
       8,    -1,    55,    56,    12,    13,    -1,    15,    -1,    -1,
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
      38,     5,    -1,    41,    42,    -1,    -1,    45,    -1,    47,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,
      24,    25,    26,    27,    28,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,    38,     5,    -1,    -1,    42,    -1,
      44,    45,    -1,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    56,    -1,    24,    25,    26,    27,    28,    29,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,    38,     5,
      -1,    -1,    42,    -1,    -1,    45,    -1,    47,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    24,    25,
      26,    27,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    42,    -1,    -1,    45,
      -1,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      56
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     5,     8,    12,    13,    15,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    38,    41,
      42,    45,    47,    55,    56,    61,    62,    63,    64,    65,
      69,    70,    72,    73,    74,    76,    77,    78,    79,    81,
      83,    85,    86,    87,    88,    90,    95,    42,    42,    26,
      73,    73,    76,    61,    76,    76,     7,    79,    76,    89,
      44,    48,    76,    86,    86,     0,    41,    63,    41,    30,
      31,    32,    42,    46,    47,    49,    40,    39,     9,    34,
      35,    36,    52,    53,    54,    55,    56,    80,    82,    37,
      57,    58,    84,    59,    42,    26,    44,    96,    97,    26,
      67,    68,     9,    10,    14,    16,    76,    43,    44,    43,
      76,    91,    92,    33,    44,    48,    41,    76,    26,    42,
      76,    76,    93,    94,    26,    29,    42,    76,    76,    78,
      79,    81,    83,    85,    86,    76,    89,    30,    26,    43,
      44,    43,    44,    76,    61,    76,    61,    94,    44,    48,
      76,    44,    48,    76,    76,    48,    43,    51,    44,    76,
      48,    50,    43,    76,    30,    26,    44,     4,    66,    26,
      10,    11,    17,    18,    19,    71,    92,    48,    91,    33,
      44,    48,    43,    26,    75,    76,    43,    76,    30,    26,
      68,    61,    61,    61,    76,    17,    18,    19,    48,    76,
      91,    30,    43,    44,    76,    30,    41,     6,    11,    19,
      16,    61,    76,    48,    48,    76,    26,    76,    61,    19,
      16,    30,    61,    76
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
#line 1737 "/home/muntyan/projects/ggap/ggap/gapparser-yacc.c"
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


#line 216 "/home/muntyan/projects/ggap/ggap/gapparser-yacc.y"


