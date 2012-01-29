%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int emit(char *s, ...);
void yyerror(char *s, ...);

extern int yylex(void);

extern int yylineno;
%}

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
%token DEFAULT BREAK CONTINUE
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
%left RELATION
%left SHIFT
%left '+' '-'
%left '*' '/' '%'
%right INCREASE DECREASE '!' '~' UMINUS UPLUS TYPE_CAST ADDR_OP INDI_OP
/* ^^  ++      --         !   ~  -      +     (int)     *       & */
%left INC_POST DEC_POST
%left FUNC_CALL ARRAY_SUB MEMBER_ACC POINT ARROW

/* pseudo precedences */
%nonassoc SOLE_IF
%nonassoc ELSE


%nonassoc HIGHEST

%%

main_rule:
	|	main_rule statement
	;

/* literal */
literal:	string
	|	integer
	|	floatnumber
	;

string:		STRING	{ emit("A STRING(\"%s\")", $1); }
	;

integer:	INTNUM	{ emit("A INT(%d)", $1); }
	;

floatnumber:	FLTNUM	{ emit("A FLOAT(%g)", $1); }
	;

/* name */
name:		NAME			{ emit("A NAME(%s)", $1); }
	|	expr ARROW NAME	%prec MEMBER_ACC { emit("MEMBER ACCESS(->)"); }
	|	expr POINT NAME %prec MEMBER_ACC { emit("MEMBER ACCESS(.)"); }
	|	name '[' expr ']' %prec ARRAY_SUB
/*	|	'*' name %prec ADDR_OP*/
	;

/* assignment */
assignment:	name '=' expr %prec ASSIGNMENT {
			emit("ASGN(=)");
		}
	|	name ASSIGNMENT expr {
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
	|	name_list ',' NAME	{ emit("STICKY"); }
	;

/* variable declaration */
variable_declaration:
		AUTO decl_list { emit("DECLARE VARIABLES"); }
	;

decl_list:	decl_list_item
	|	decl_list ',' decl_list_item	{ emit("STICKY"); }
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
/*	|	'&' name %prec ADDR_OP {
			emit("GET ADDRESS");
		}*/
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
	|	IF '(' expr ')' block {
			emit("AS IF PART(BLOCK)");
		}
	;

else_part:	ELSE statement {
			emit("AS ELSE PART(STATEMENT)");
		}
	|	ELSE block {
			emit("AS ELSE PART(BLOCK)");
		}
	;

/* block */
block:		'{' statements '}'	{
			emit("MAKE A BLOCK");
		}
	|	'{' statements raw_statement '}' {
			emit("MAKE A BLOCK ENDED WITH A RAW STATEMENT");
		}
	|	'{' '}' {
			emit("AS A EMPTY BLOCK");
		}
	;

statements:	statement
	|	statements statement	{
			emit("STICKY");
		}
	;

/* while */
while_statement:
		WHILE '(' expr ')' statement { emit("AS A WHILE WITH STATE"); }
	|	WHILE '(' expr ')' block     { emit("AS A WHILE WITH BLOCK"); }
	;

/* for and for-in statement */
for_statement:	FOR '(' raw_statement ';' raw_statement ';' raw_statement ')'
		statement { emit("AS A FOR WITH STATEMENT"); }
	|	FOR '(' raw_statement ';' raw_statement ';' raw_statement ')'
		block { emit("AS A FOR WITH BLOCK"); }
	;

for_in_statement:
		FOR '(' NAME IN expr ')' statement {
			emit("AS A FOR IN STATEMENT WITH STATEMENT");
		}
	|	FOR '(' NAME IN expr ')' block {
			emit("AS A FOR IN STATEMENT WITH BLOCK");
		}
	;

/* do-while */
do_while_statement:
		DO statement WHILE '(' expr ')' {
			emit("DO_WHILE WITH STATEMENT");
		}
	|	DO block WHILE '(' expr ')'	{
			emit("DO_WHILE WITH BLOCK");
		}
	;


/* expression */
expr:		literal
	|	name
	|	assignment
	|	binary_operation
	|	unary_operation
	|	trenary_operation
	|	'(' expr ')'
	;

/* statement */
statement:	expr ';'
	|	declaration ';'
	|	branch_statement
	|	';'
	;

raw_statement:	expr
	|	declaration
	;


%%

int emit(char *s, ...)
{
    va_list ap;
    va_start(ap, s);

    printf("rpn: ");
    vfprintf(stdout, s, ap);
    printf("\n");

    va_end(ap);

    return 0;
}

void yyerror(char *s, ...)
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

