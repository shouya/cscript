%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "cscript_parser.h"

int emit(char *s, ...);
void yyerror(char *s, ...);

extern int yylex(void);

extern int yylineno;
%}

%union {
	int intval;
	double fltval;
	char *strval;
	struct cs_bt *btree;
};

/* type declaration */
%token <intval> INTNUM
%token <fltval> FLTNUM
%token <strval> STRING
%token <strval> NAME
%token <intval> EQUALITY RELATION
%token <intval> ASSIGNMENT
%token <intval> SHIFT
%token <intval> BOOL

/* key word */
%token IF ELSE UNLESS WHILE FOR DO RETURN IMPORT IN

/* operator */
%token INCREASE DECREASE LOGIAND LOGIOR
/*     ++       --       &&      ||    */


/* operator precedence (low->high) */

%left COMMA
%right ASSIGNMENT
%right COND_OP /* conditional operator */
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
%right INC_PRE DEC_PRE '!' UMINUS UPLUS
%left FUNC_CALL INC_POST DEC_POST '[' '(' '{'

/* rugulation type definition */
%type <btree> number string name scalar
%type <btree> block_stmt block stmt_list
%type <btree> if_stmt unless_stmt while_stmt for_stmt
%type <btree> array array_list array_item /**/ hash
%type <btree> func_call arg_list
%type <btree> asgn_exp arith_exp logi_exp bit_exp cond_op_exp
%type <btree> comma_exp comma_list
%type <btree> exp
%type <btree> smpl_stmt stmt

%%

/* main regulation */
main_reg:	/* empty statement */
	|	stmt
	|	main_reg stmt;
	;


/* block */
block:	'{' stmt_list '}'	{ $$= mkblock($2); }
	;

stmt_list:			{ $$= mkcode(NULL, NULL); }
	|	stmt		{ $$= mkcode(NULL, $1); }
	|	stmt_list stmt	{ $$= mkcode($1, $2); } 
	;

block_stmt:	block		{ $$= mkstmt($1); }
	|	stmt		{ $$= $1; }
	;



/* literal */
number:		INTNUM		{ $$= mkint($1); }
	|	FLTNUM		{ $$= mkflt($1); }
	;

string:		STRING		{ $$= mkstr($1); }
	;

/* assignment */
asgn_exp:	name ASSIGNMENT exp	{ $$= mkasgn($2, $1, $3); }
	;

/* if statements */
if_stmt:	IF '(' exp ')' block_stmt	{ $$= mkif($3, $5, NULL); }
	|	IF '(' exp ')' block_stmt ELSE block_stmt {
		$$= mkif($3, $5, $7);
		}
	;


/* unless statement */
unless_stmt:	UNLESS '(' exp ')' block_stmt	{ $$= mkunless($3, $5, NULL); }
	|	UNLESS '(' exp ')' block_stmt ELSE block_stmt {
		$$= mkunless($3, $5, $7);
		}
	;

/* while */
while_stmt:	WHILE '(' exp ')' block_stmt		{
			$$= mkwhile(CS_WHILE, $3, $5);
		}
	|	DO block_stmt WHILE '(' exp ')' ';'	{
			$$= mkwhile(CS_DO_WHILE, $5, $2);
		}
	;


/* for */
for_stmt:	FOR '(' smpl_stmt ';' exp ';' smpl_stmt ')' block_stmt	{
			$$= mkfor($3, $5, $7, $9);
		}
	|	FOR '(' name IN exp ')' block_stmt {
			$$= mkforin($3, $5, $7);
		}
	;


/* array */
array:		'[' array_list ']'	{ $$= mklist($2); }
	;

array_list:				{ $$= mkexplst(NULL, NULL); }
	|	array_item		{ $$= mkexplst(NULL, $1); }
	|	array_list ',' array_item { $$= mkexplst($1, $3); }
	;

array_item:	exp	{ $$= $1; }
	;

/* hash */
hash:		'[' hash_list ']'	{ $$= mkhash(); /*TODO*/ }
	;

hash_list:	hash_item
	|	hash_list hash_item
	;

hash_item:	hash_key ':' exp
	;

hash_key:	exp
	;

/* function call */
func_call:	name '(' arg_list ')'	{ $$= mkfcall($1, $3); }
	|	block '(' arg_list ')'	{ $$= mkfcall($1, $3); }
	;

arg_list:			{ $$= mkexplst(NULL, NULL); }
	|	exp		{ $$= mkexplst(NULL, $1); }
	|	arg_list ',' exp	{ $$= mkexplst($1, $3); }
	;

/* arithmetic expression */
arith_exp:	exp '+' exp	{ $$= mkbin('+', $1, $3); }
	|	exp '-' exp	{ $$= mkbin('-', $1, $3); }
	|	exp '*' exp	{ $$= mkbin('*', $1, $3); }
	|	exp '/' exp	{ $$= mkbin('/', $1, $3); }
	|	INCREASE exp %prec INC_PRE	{ $$= mkunary('i', $2); }
	|	DECREASE exp %prec DEC_PRE	{ $$= mkunary('d', $2); }
	|	exp INCREASE %prec INC_POST	{ $$= mkunary('I', $1); }
	|	exp DECREASE %prec DEC_POST	{ $$= mkunary('D', $1); }
	|	'-'exp %prec UMINUS	{ $$= mkunary('-', $2); }
	|	'+'exp %prec UPLUS	{ $$= mkunary('+', $2); }
	;

/* logical expression */
logi_exp:	exp RELATION exp	{ $$= mkbin($2, $1, $3); }
	|	exp EQUALITY exp	{ $$= mkbin($2, $1, $3); }
	|	'!'exp			{ $$= mkunary('!', $2); }
	|	exp LOGIAND exp		{ $$= mkbin('A', $1, $3); }
	|	exp LOGIOR exp		{ $$= mkbin('O', $1, $3); }
	;


/* bitwise expression */
bit_exp:	exp SHIFT exp		{ $$= mkbin($2, $1, $3); }
	|	exp '&' exp		{ $$= mkbin('&', $1, $3); }
	|	exp '^' exp		{ $$= mkbin('^', $1, $3); }
	|	exp '|' exp		{ $$= mkbin('|', $1, $3); }
	;

/* cond_op expression */
cond_op_exp:	exp '?' exp ':' exp %prec COND_OP	{
			$$= mktern('?', $1, $3, $5);
		}
	;

/* comma expression */
comma_exp:	'(' comma_list ',' exp ')' %prec COMMA	{
			$$= mkexplst($2, $4);
		}
	;

comma_list:	exp			{ $$= mkexplst(NULL, $1); }
	|	comma_list ',' exp	{ $$= mkexplst($1, $3); }
	;

/* scalar/list/hash */
scalar:		name		{ $$= $1; }
	|	string		{ $$= $1; }
	|	number		{ $$= $1; }
	|	block		{ $$= $1; }
	;

name:		NAME		{ $$= mktok($1); }
	;


/* expression */
exp:		func_call	{ $$= $1; }
	|	asgn_exp	{ $$= $1; }
	|	arith_exp	{ $$= $1; }
	|	logi_exp	{ $$= $1; }
	|	bit_exp		{ $$= $1; }
	|	cond_op_exp	{ $$= $1; }
	|	scalar		{ $$= $1; }
	|	array		{ $$= $1; }
	|	hash		{ $$= $1; }
	|	comma_exp	{ $$= $1; }
	|	'(' exp ')'	{ $$= $2; }
	;



/* statement */
smpl_stmt:	exp	{ $$= mkstmt($1); }
	;
stmt:		';'	{ $$= mkstmt(NULL); }
	|	smpl_stmt ';'	{ $$= $1; }
	|	if_stmt		{ $$= mkstmt($1); }
	|	unless_stmt	{ $$= mkstmt($1); }
	|	while_stmt	{ $$= mkstmt($1); }
	|	for_stmt	{ $$= mkstmt($1); }
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

