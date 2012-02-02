%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "cscript_parser_yacc.h"

int emit(const char *s, ...);
void yyerror(const char *s, ...);

extern int yylex(void);

extern int yylineno;


#ifdef _GNUC_
# define inline __inline__
#else
# define inline
#endif

%}


%glr-parser
%expect 2


%union {
	int intval;
	double fltval;
	char *strval;
/*				struct cs_bt *btree;*/
};

/* type declaration */
%token <intval> INTNUM
%token <fltval> FLTNUM
%token <strval> STRING
%token <strval> NAME
%token <intval> EQUALITY RELATION
%token <intval> ASSIGNMENT
%token <intval> SHIFT

/* key word */
%token IF ELSE WHILE FOR DO SWITCH CASE
%token DEFAULT BREAK CONTINUE REDO
%token RETURN TYPEOF IN STRUCT ENUM LAMBDA IMPORT XNULL
%token AUTO EXTERN

/* operator */
%token INCREASE DECREASE LOGIAND LOGIOR ARROW POINT
/*     ++       --       &&      ||    */

/* conflinct operator */
/*%token ADDRESS INDIRECT MULIPLY BITAND PLUS MINUS*/

/* internal types */
%token STR INT FLOAT



%nonassoc LOWEST

/* operator precedence (low->high) */
%left ','
%right ASSIGNMENT '='
%right '?' COND_OP /* conditional operator */
%left LOGIOR
%left LOGIAND
%left '|'
%left '^'
%left '&'
%left EQUALITY
%left RELATION IN /* added */
%left SHIFT
%left '+' '-'
%left '*' '/' '%'
%right INCREASE DECREASE '!' '~' UMINUS UPLUS TYPE_CAST ADDR_OP INDI_OP
/* ^^  ++      --         !   ~  -      +     (int)     *       & */
%left INC_POST DEC_POST
%left '(' '[' POINT ARROW

/* pseudo precedences */
%nonassoc SOLE_IF
%nonassoc ELSE

%nonassoc HIGHEST

%code top {
#define EMIT0(type) cscript_emit0(type);
#define EMIT1(type,a) cscript_emit1(type,a);
#define EMIT2(type,a,b) cscript_emit2(type,a,b);
#define EMIT3(type,a,b,c) cscript_emit3(type,a,b,c);
}

%%

main_rule:
	|	main_rule statement
	|	main_rule function_defination
	;

/* literal */
literal:	string
	|	integer
	|	floatnumber
	;

string:		STRING	{ EMIT1(_STRING, $1); }
	;

integer:	INTNUM	{ emit("A INT(%d)", $1); }
	;

floatnumber:	FLTNUM	{ emit("A FLOAT(%g)", $1); }
	;

/* name */
expr:		NAME		{ emit("A NAME(%s)", $1); }
	|	expr ARROW NAME { emit("MEMBER ACCESS(->)"); }
	|	expr POINT NAME { emit("MEMBER ACCESS(.)"); }
	|	expr '[' expr ']'
	;


/* assignment */
assignment:	expr '=' expr %prec ASSIGNMENT {
			emit("ASGN(=)");
		}
	|	expr ASSIGNMENT expr {
			emit("ASGN(%c)", $2);
		}
	;


/* declaration */
declaration:	function_declaration
	|	variable_declaration
	;

/* function declaration */
function_declaration:
		EXTERN NAME '(' parameter_list ')' {
			emit("DECL FUNCTION(%s)", $2);
		}
	;
parameter_list:	name_list
	;

name_list:
	|	NAME
	|	name_list ',' NAME	{ emit("GLUE"); }
	;

/* variable declaration */
variable_declaration:
		AUTO decl_list { emit("DECLARE VARIABLES"); }
	;

decl_list:	decl_list_item
	|	decl_list ',' decl_list_item	{ emit("GLUE"); }
	;

decl_list_item:	NAME		{ emit("A VARIABLE(%s)", $1); }
	|	NAME '=' expr	{
			emit("A VARIABLE WITH INITIALIZATION(%s)", $1);
		}
	;


/* binary operation */
binary_operation:
		expr '+' expr {
			emit("PLUS");
		}
	|	expr '-' expr {
			emit("MINUS");
		}
	|	expr '*' expr {
			emit("MULTIPLY");
		}
	|	expr '/' expr {
			emit("DIVIDE");
		}
	|	expr '%' expr {
			emit("MOD");
		}
	|	expr SHIFT expr {
			emit("SHIFT(%c)", $2);
		}
	|	expr EQUALITY expr {
			emit("EQUALITY(%c)", $2);
		}
	|	expr RELATION expr {
			emit("RELATION(%c)", $2);
		}
	|	expr '&' expr {
			emit("BIT AND");
		}
	|	expr '|' expr {
			emit("BIT OR");
		}
	|	expr '^' expr {
			emit("BIT XOR");
		}
	|	expr LOGIAND expr {
			emit("LOGI AND");
		}
	|	expr LOGIOR expr {
			emit("LOGI OR");
		}
	|	expr IN expr %dprec 1 {
			emit("IN");
		}
	;

/* unary operation */
unary_operation:
		INCREASE expr {
			emit("PREFIX INCREASE");
		}
	|	DECREASE expr {
			emit("PREFIX DECREASE");
		}
	|	expr INCREASE %prec INC_POST {
			emit("POSTFIX INCREASE");
		}
	|	expr DECREASE %prec DEC_POST {
			emit("POSTFIX DECREASE");
		}
	|	'!' expr {
			emit("LOGI NEGATION");
		}
	|	'~' expr {
			emit("BIT COMPLEMENT");
		}
	|	'-' expr %prec UMINUS {
			emit("UNARY MINUS");
		}
	|	'+' expr %prec UPLUS {
			emit("UNARY PLUS");
		}
	|	'(' type ')' expr %prec TYPE_CAST {
			emit("TYPE CAST");
		}
	|	'&' expr %prec ADDR_OP {
			emit("GET ADDRESS");
		}
	|	'*' expr %prec INDI_OP {
			emit("INDIRECT OPERATION");
		}
	;

/* type decalaration */
type:		INT	{ emit("TYPE INT"); }
	|	STR	{ emit("TYPE STR"); }
	|	FLOAT	{ emit("TYPE FLOAT"); }
	|	type '*' %prec HIGHEST { emit("AS A POINTER"); }
	|	STRUCT NAME	{ emit("AS A STRUCT"); }
/*	|	NAME		{ emit("AS A TYPE"); }*/
	;

/* trenary operation */
trenary_operation:
		expr '?' expr ':' expr %prec COND_OP {
			emit("CONDITIONING OPERATOR");
		}
	;


/* branch statements */
branch_statement:
		if_statement
	|	while_statement
	|	for_statement
	|	for_in_statement
	|	do_while_statement
	|	switch_statement
	;
/* if */
if_statement:	if_part %prec SOLE_IF {
			emit("AS IF STATEMENT(IF)");
		}
	|	if_part else_part {
			emit("AS IF STATEMENT(IF-ELSE)");
		}
	;

if_part:	IF '(' expr ')' statement {
			emit("AS IF PART(STATEMENT)");
		}
	;

else_part:	ELSE statement {
			emit("AS ELSE PART(STATEMENT)");
		}
	;

/* block */
block:		'{' raw_block '}'

raw_block:	statements
	|	raw_statement
	|	statements raw_statement {
			emit("GLUE");
		}
	|	{
			emit("AS A EMPTY BLOCK");
		}
	;

statements:	statement
	|	statements statement	{
			emit("GLUE");
		}
	;

/* while */
while_statement:
		WHILE '(' expr ')' statement { emit("AS A WHILE WITH STATE"); }
	;

/* for and for-in statement */
for_statement:	FOR '(' raw_statement ';' raw_statement ';' raw_statement ')'
		statement { emit("AS A FOR WITH STATEMENT"); }
	;

for_in_statement:
		FOR '(' NAME IN expr ')' statement %dprec 2 {
			emit("AS A FOR IN STATEMENT WITH STATEMENT");
		}
	;
/* do-while */
do_while_statement:
		DO statement WHILE '(' expr ')' {
			emit("DO_WHILE WITH STATEMENT");
		}
	;

/* loop contorl */
loop_control_statement:
		continue_statement
	|	break_statement
	|	redo_statement
	;
continue_statement:
		CONTINUE
	|	CONTINUE INTNUM
	;
break_statement:
		BREAK
	|	BREAK INTNUM
	;
redo_statement:	REDO
	|	REDO INTNUM
	;


/* switch statement */
switch_statement:
		SWITCH '(' expr ')' '{' switch_content '}'
	|	SWITCH '(' expr ')' '{' '}'
	;

switch_content:	switch_item
	|	switch_content switch_item
	;

switch_item:	CASE expr ':' statements
	|	DEFAULT ':' statements
	;

/* function calling */
function_call:	expr '(' expr_list ')' { emit("F CALL"); }
	;

expr_list:	{ emit("EMPTY EXPR"); }
	|	expr
	|	expr_list ',' expr { emit("GLUE"); }
	;


/* function defination */
function_defination:
		NAME '(' parameter_list ')' '{' raw_block '}' {
			emit("DEFINING FUNCTION(%s)", $1);
		}
	;

/* expression */
expr:		literal
	|	assignment
	|	binary_operation
	|	unary_operation
	|	trenary_operation
	|	'(' expr ')'
	|	function_call
	;

/* statement */
statement:	expr ';'
	|	declaration ';'
	|	branch_statement
	|	loop_control_statement ';'
	|	';'
	|	block
	;

raw_statement:	expr
	|	declaration
	;


%%

#ifdef EMIT0
# undef EMIT0
# undef EMIT1
# undef EMIT2
# undef EMIT3
#endif


int emit(const char *s, ...)
{
    va_list ap;
    va_start(ap, s);

    printf("rpn: ");
    vfprintf(stdout, s, ap);
    printf("\n");

    va_end(ap);

    return 0;
}

void yyerror(const char *s, ...)
{
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "Error[%d]: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");

    va_end(ap);
}

int main()
{
    if (!yyparse())
	printf("cscript parse worked.\n");
    else
	printf("cscript parse failed.\n");

    return 0;
}

