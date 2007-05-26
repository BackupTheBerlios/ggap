%{
#include "gapparser-priv.h"
%}

%pure-parser
%name-prefix="_gap_parser_yy"
%error-verbose
%lex-param      {GapParser *parser}
%parse-param    {GapParser *parser}
%locations

%token TOK_REC
%token TOK_LOCAL TOK_FUNCTION TOK_END TOK_MAPTO
%token TOK_FOR TOK_IN TOK_DO TOK_OD TOK_WHILE TOK_REPEAT TOK_UNTIL
%token TOK_IF TOK_THEN TOK_ELSE TOK_ELIF TOK_FI
%token TOK_RETURN TOK_BREAK TOK_QUIT TOK_QUIT_CAP
%token TOK_FALSE TOK_TRUE

%token TOK_IDENTIFIER

%token TOK_CHAR_LITERAL     // 'a'
%token TOK_STRING_LITERAL   // "wefwef"
%token TOK_INTEGER

%token TOK_ASSIGN           // :=
%token TOK_REP_REC_ELM      // !.
%token TOK_REP_LIST_ELM     // ![]
%token TOK_TWO_DOTS         // ..

%token TOK_LEQ              // <=
%token TOK_GEQ              // >=
%token TOK_NEQ              // <>
%token TOK_MOD

%token TOK_NOT
%token TOK_AND
%token TOK_OR

%%

Program:        /* empty */
        |       non_empty_program
;
non_empty_program:
                ';'
        |       non_empty_program ';'
        |       non_empty_program Statement ';'
        |       Statement ';'
;
Statement:      Expression
        |       Assignment
        |       Loop
        |       Conditional
 	| 	TOK_RETURN
        |       TOK_RETURN Expression
	| 	TOK_BREAK
	| 	TOK_QUIT
	| 	TOK_QUIT_CAP
        |       error
;

ShortFunction:  TOK_IDENTIFIER TOK_MAPTO Expression
;
Function:       TOK_FUNCTION '(' funcdef_args ')'
                    funcdef_local
                    Program
                TOK_END
;
funcdef_local:  /* empty */
        |       TOK_LOCAL funcdef_arglist ';'
;
funcdef_args:   /* empty */
        |       funcdef_arglist
;
funcdef_arglist:
                TOK_IDENTIFIER
        |       funcdef_arglist ',' TOK_IDENTIFIER
;

Loop:           TOK_FOR Lvalue TOK_IN Expression TOK_DO Program TOK_OD
        |       TOK_WHILE Expression TOK_DO Program TOK_OD
        |       TOK_REPEAT Program TOK_UNTIL Expression
;

Conditional:
                TOK_IF Expression TOK_THEN Program TOK_FI
        |       TOK_IF Expression TOK_THEN Program TOK_ELSE Program TOK_FI
        |       TOK_IF Expression TOK_THEN Program elif_block TOK_FI
        |       TOK_IF Expression TOK_THEN Program elif_block TOK_ELSE Program TOK_FI
;
elif_block:
                TOK_ELIF Expression TOK_THEN Program
        |       elif_block TOK_ELIF Expression TOK_THEN Program
;

Assignment:     Lvalue TOK_ASSIGN Expression
;

Lvalue:         '~'
        |       TOK_IDENTIFIER
	|       Lvalue '.' TOK_IDENTIFIER
	|       Lvalue '.' TOK_INTEGER
	|       Lvalue '.' '(' Expression ')'
	|       Lvalue '[' Expression ']'
	|       Lvalue '{' Expression '}'
        |       FunctionCall
	|       Lvalue TOK_REP_REC_ELM TOK_IDENTIFIER
	|       Lvalue TOK_REP_REC_ELM '(' Expression ')'
	|       Lvalue TOK_REP_LIST_ELM Expression ']'
;

FunctionCall:   Lvalue '(' expr_list ')'
        |       Lvalue '(' expr_list ':' func_opt_list ')'
;
func_opt_list:  TOK_IDENTIFIER
        |       TOK_IDENTIFIER TOK_ASSIGN Expression
        |       func_opt_list ',' TOK_IDENTIFIER
        |       func_opt_list ',' TOK_IDENTIFIER TOK_ASSIGN Expression
;

Expression:     ShortFunction
        |       Logical
;

Logical:        And
        |       Logical TOK_OR And
;

And:            Rel
        |       And TOK_AND Rel
;

Rel:            Sum
        |       Sum RelOp Sum
        |       TOK_NOT Rel
;
RelOp: '=' | TOK_NEQ | '<' | '>' | TOK_LEQ | TOK_GEQ | TOK_IN;

Sum:            Product
        |       Sum SumOp Product
;
SumOp: '+' | '-';

Product:        Power
        |       Product ProductOp Power
;
ProductOp: '*' | '/' | TOK_MOD;

Power:          power_operand
        |       power_operand '^' power_operand
;
power_operand:  Atom
        |       '+' power_operand
        |       '-' power_operand
;

Atom:           TOK_INTEGER
	| 	Lvalue
	| 	'(' Expression ')'
	| 	Permutation
	| 	TOK_CHAR_LITERAL
	| 	TOK_STRING_LITERAL
	| 	Function
	| 	List
	| 	Record
        |       TOK_TRUE
        |       TOK_FALSE
;

/* () and (1,2) are permutations, while (1) is an expression in parentheses */
Permutation:    '(' perm_elms ')'
        |       Permutation '(' perm_elms ')'
;
perm_elms:      /* empty */
        |       Expression ',' expr_list
;

List:           '[' Expression TOK_TWO_DOTS Expression ']'
        |       '[' Expression ',' Expression TOK_TWO_DOTS Expression ']'
        |       '[' ',' list_elms ']'
        |       '[' Expression ',' ',' list_elms ']'
        |       '[' Expression ',' Expression ',' list_elms ']'
        |       '[' Expression ',' Expression ']'
        |       '[' Expression ']'
        |       '[' Expression ',' ']'
        |       '[' ']'
;
list_elms:      expr_or_empty
        |       list_elms ',' expr_or_empty
;
expr_or_empty:  /* empty */
        |       Expression
;

expr_list:      /* empty */
        |       non_empty_expr_list
;
non_empty_expr_list:
                Expression
        |       non_empty_expr_list ',' Expression
;

Record:         TOK_REC '(' rec_elms ')'
;
rec_elms:       /* empty */
        |       non_empty_rec_elms
        |       non_empty_rec_elms ','
;
non_empty_rec_elms:
                ',' TOK_IDENTIFIER TOK_ASSIGN Expression
        |       TOK_IDENTIFIER TOK_ASSIGN Expression
        |       non_empty_rec_elms ',' TOK_IDENTIFIER TOK_ASSIGN Expression
        |       non_empty_rec_elms ',' ',' TOK_IDENTIFIER TOK_ASSIGN Expression
;

%%
