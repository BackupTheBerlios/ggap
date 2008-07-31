%{
#include "ggap/parser_p.h"
using namespace ggap::parser;
#define YY_(s) ggap::parser::translate(s)
%}

%pure-parser
%name-prefix="_gap_parser_yy"
%error-verbose
%lex-param   {Parser *parser}
%parse-param {Parser *parser}
%locations

%token TOK_REC              "rec"
%token TOK_LOCAL            "local"
%token TOK_FUNCTION         "function"
%token TOK_END              "end"
%token TOK_MAPTO            "->"
%token TOK_FOR              "for"
%token TOK_IN               "in"
%token TOK_DO               "do"
%token TOK_OD               "od"
%token TOK_WHILE            "while"
%token TOK_REPEAT           "repeat"
%token TOK_UNTIL            "until"
%token TOK_IF               "if"
%token TOK_THEN             "then"
%token TOK_ELSE             "else"
%token TOK_ELIF             "elif"
%token TOK_FI               "fi"
%token TOK_RETURN           "return"
%token TOK_BREAK            "break"
%token TOK_QUIT             "quit"
%token TOK_QUIT_CAP         "QUIT"
%token TOK_FALSE            "false"
%token TOK_TRUE             "true"

%token TOK_SAVE_WORKSPACE   "SaveWorkspace"
%token TOK_UNBIND           "Unbind"

%token TOK_IDENTIFIER       "identifier"
%token TOK_CHAR_LITERAL     "character" // 'a'
%token TOK_STRING_LITERAL   "string"    // "wefwef"
%token TOK_INTEGER          "integer"

%token TOK_ASSIGN           ":="
%token TOK_REP_REC_ELM      "!."
%token TOK_REP_LIST_ELM     "!["
%token TOK_TWO_DOTS         ".."

%token TOK_LEQ              "<="
%token TOK_GEQ              ">="
%token TOK_NEQ              "<>"
%token TOK_MOD              "mod"

%token TOK_NOT              "not"
%token TOK_AND              "and"
%token TOK_OR               "or"

%%

Program
        : /* empty */
        | non_empty_program
        ;
non_empty_program
        : ';'
        | non_empty_program ';'
        | non_empty_program statement ';'
        | non_empty_program error ';'// { yyerrok; }
        | statement ';'
        | error ';' // { yyerrok; }
        ;
statement
        : expression
        | Assignment
        | Loop
        | Conditional
 	| "return"
        | "return" expression
	| "break"
	| "quit"
	| "QUIT"
        | save_workspace
        | unbind
        ;

ShortFunction
        : TOK_IDENTIFIER "->" expression
        ;
Function
        : "function" '(' funcdef_args ')'
            funcdef_local
            Program
          "end"
        ;
funcdef_local
        : /* empty */
        | "local" funcdef_arglist ';'
        ;
funcdef_args
        : /* empty */
        | funcdef_arglist
        ;
funcdef_arglist
        : TOK_IDENTIFIER
        | funcdef_arglist ',' TOK_IDENTIFIER
        ;

Loop
        : "for" Lvalue "in" expression "do" Program "od"
        | "while" expression "do" Program "od"
        | "repeat" Program "until" expression
        ;

Conditional
        : "if" expression "then" Program "fi"
        | "if" expression "then" Program "else" Program "fi"
        | "if" expression "then" Program elif_block "fi"
        | "if" expression "then" Program elif_block "else" Program "fi"
        ;
elif_block
        : "elif" expression "then" Program
        | elif_block "elif" expression "then" Program
        ;

Assignment
        : Lvalue ":=" expression
        ;

Lvalue
        : '~'
        | TOK_IDENTIFIER
	| Lvalue '.' TOK_IDENTIFIER
	| Lvalue '.' TOK_INTEGER
	| Lvalue '.' '(' expression ')'
	| Lvalue '[' expression ']'
	| Lvalue '{' expression '}'
        | FunctionCall
	| Lvalue "!." TOK_IDENTIFIER
	| Lvalue "!." '(' expression ')'
	| Lvalue "![" expression ']'
        ;

FunctionCall
        : Lvalue '(' expr_list ')'
        | Lvalue '(' expr_list ':' func_opt_list ')'
        ;
func_opt_list
        : TOK_IDENTIFIER
        | TOK_IDENTIFIER ":=" expression
        | func_opt_list ',' TOK_IDENTIFIER
        | func_opt_list ',' TOK_IDENTIFIER ":=" expression
        ;

unbind_expression
        : Lvalue '.' TOK_IDENTIFIER
	| Lvalue '.' TOK_INTEGER
	| Lvalue '.' '(' expression ')'
	| Lvalue '[' expression ']'
	| Lvalue "!." TOK_IDENTIFIER
	| Lvalue "!." '(' expression ')'
	| Lvalue "![" expression ']'
        ;
unbind
        : "Unbind" '(' unbind_expression ')'
        ;

save_workspace
        : "SaveWorkspace" '(' expression ')'
        ;

expression
        : ShortFunction
        | Logical
        ;

Logical
        : And
        | Logical "or" And
        ;

And
        : Rel
        | And "and" Rel
        ;

Rel
        : Sum
        | Sum RelOp Sum
        | "not" Rel
        ;
RelOp: '=' | "<>" | '<' | '>' | "<=" | ">=" | "in";

Sum
        : Product
        | Sum SumOp Product
        ;
SumOp: '+' | '-';

Product
        : Power
        | Product ProductOp Power
        ;
ProductOp: '*' | '/' | "mod";

Power
        : power_operand
        | power_operand '^' power_operand
        ;
power_operand
        : Atom
        | '+' power_operand
        | '-' power_operand
        ;

Atom
        : TOK_INTEGER
	| Lvalue
	| '(' expression ')'
	| Permutation
	| TOK_CHAR_LITERAL
	| TOK_STRING_LITERAL
	| Function
	| List
	| Record
        | "true"
        | "false"
        ;

/* () and (1,2) are permutations, while (1) is an expression in parentheses */
Permutation
        : '(' perm_elms ')'
        | Permutation '(' perm_elms ')'
        ;
perm_elms
        : /* empty */
        | expression ',' non_empty_expr_list
        ;

List
        : '[' expression ".." expression ']'
        | '[' expression ',' expression TOK_TWO_DOTS expression ']'
        | '[' ',' list_elms ']'
        | '[' expression ',' ',' list_elms ']'
        | '[' expression ',' expression ',' list_elms ']'
        | '[' expression ',' expression ']'
        | '[' expression ']'
        | '[' expression ',' ']'
        | '[' ']'
        ;
list_elms
        : maybe_expression
        | list_elms ',' maybe_expression
        ;
maybe_expression
        : /* empty */
        | expression
        ;

expr_list
        : /* empty */
        | non_empty_expr_list
        ;
non_empty_expr_list
        : expression
        | non_empty_expr_list ',' expression
        ;

Record
        : "rec" '(' rec_elms ')'
        ;
rec_elms
        : /* empty */
        | non_empty_rec_elms
        | non_empty_rec_elms ','
        ;
non_empty_rec_elms
        : ',' TOK_IDENTIFIER ":=" expression
        | TOK_IDENTIFIER ":=" expression
        | non_empty_rec_elms ',' TOK_IDENTIFIER ":=" expression
        | non_empty_rec_elms ',' ',' TOK_IDENTIFIER ":=" expression
;

%%
