%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int emit(char *s, ...);
void yyerror(char *s, ...);
%}

%union {
	int intval;
	double fltval;
	char *strval;
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
%token IF ELSE ELSIF UNLESS WHILE FOR DO RETURN IMPORT IN

/* operator */
%token INCREASE DECREASE LOGIAND LOGIOR
/*     ++       --       &&      ||    */


%type <strval> name string

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
%left FUNC_CALL INC_POST DEC_POST

%%


/* block */
block:	'{' stmt_list '}'	{ emit("AS BLOCK"); }
	;

stmt_list:	stmt
	|	stmt_list stmt
	;

block_stmt:	block
	|	stmt
	;



/* literal */
number:		INTNUM		{ emit("INT(%d)", $1); }
	|	FLTNUM		{ emit("FLOAT(%g)", $1); }
	;

string:		STRING		{ emit("STR(%s)", $1); $$ = $1; }
	;

literal:	number
	|	string
	;

/* assignment */
asgn_exp:	name ASSIGNMENT exp	{ emit("ASGN(%d)", $2); }
	;

/* if statements */
if_stmt:	if_part				{ emit("IF STMT"); }
	|	if_part else_part		{ emit("IF/ELSE STMT"); }
	|	if_part elsif_parts		{ emit("IF/ELSIF STMT"); }
	|	if_part elsif_parts else_part	{ emit("IF/ELSIF/ELSE STMT"); }
	;

if_part:	IF '(' exp ')' block_stmt	{ emit("AS IF PART"); }
	;

else_part:	ELSE block_stmt			{ emit("AS ELSE PART"); }
	;

elsif_part:	ELSIF '(' exp ')' block_stmt	{ emit("AS ELSIF PART"); }
	;

elsif_parts:	elsif_part
	|	elsif_parts elsif_part
	;


/* unless statement */
unless_stmt:	unless_part		{ emit("UNLESS STMT"); }
	|	unless_part else_part	{ emit("UNLESS/ELSE STMT"); }
	;

unless_part:	UNLESS '(' exp ')' block_stmt	{ emit("AS UNLESS PART"); }
	;


/* while */
while_stmt:	WHILE '(' exp ')' block_stmt		{ emit("WHILE STMT"); }
		DO block_stmt WHILE '(' exp ')' ';'	{ emit("DO_WH STMT"); }
	;


/* for */
for_stmt:	FOR '(' for_args ')' block_stmt	{ emit("FOR STMT"); }
	;

for_args:	smpl_stmt ';' exp ';' smpl_stmt	{ emit("AS FOR_TYPICAL ARG"); }
	|	name IN exp			{ emit("AS FOR_IN ARG"); }
	;


/* array */
array:		'[' array_list ']'
	|	'[' ']'
	;

array_list:	array_item
	|	array_list ',' array_item
	;

array_item:	exp
	;

/* hash */
hash:		'[' hash_list ']'
	;

hash_list:	hash_item
	|	hash_list hash_item
	;

hash_item:	hash_key ':' exp
	;

hash_key:	name
	|	string
	|	number
	;

/* function call */
func_call:	name '(' arg_list ')'	{ emit("FUNC CALL(%s)", $1); }
	|	block '(' arg_list ')'	{ emit("FUNC CALL(NAKED BLOCK)"); }
	;

arg_list:	exp		{ emit("AS ARG"); }
	|	arg_list exp	{ emit("AS ARG"); }
	;

/* arithmetic expression */
arith_exp:	exp '+' exp	{ emit("ARITH(+)"); }
	|	exp '-' exp	{ emit("ARITH(-)"); }
	|	exp '*' exp	{ emit("ARITH(*)"); }
	|	exp '/' exp	{ emit("ARITH(/)"); }
	|	INCREASE exp %prec INC_PRE	{ emit("ARITH(++ PRE)"); }
	|	DECREASE exp %prec DEC_PRE	{ emit("ARITH(-- PRE)"); }
	|	exp INCREASE %prec INC_POST	{ emit("ARITH(++ POST)"); }
	|	exp DECREASE %prec DEC_POST	{ emit("ARITH(-- POST)"); }
	|	'-' exp %prec UMINUS	{ emit("ARITH(UMINUS)"); }
	|	'+' exp %prec UPLUS	{ emit("ARITH(UPLUS)"); }
	;

/* logical expression */
logi_exp:	exp RELATION exp	{ emit("LOGI(rel %d)", $2); }
	|	exp EQUALITY exp	{ emit("LOGI(equ %d)", $2); }
	|	'!' exp			{ emit("LOGI(not)"); }
	|	exp LOGIAND exp		{ emit("LOGI(and)"); }
	|	exp LOGIOR exp		{ emit("LOGI(or)"); }
	;


/* bitwise expression */
bit_exp:	exp SHIFT exp		{ emit("BIT(shift %d)", $2); }
	|	exp '&' exp		{ emit("BIT(and)"); }
	|	exp '^' exp		{ emit("BIT(xor)"); }
	|	exp '|' exp		{ emit("BIT(or)"); }
	;

/* cond_op expression */
cond_op_exp:	exp '?' exp ':' exp %prec COND_OP	{ emit("COND_OP"); }
	;

/* comma expression */
comma_exp:	exp ',' comma_list	{ emit("AS COMMA EXPRESSION"); }
	;

comma_list:	exp
	|	comma_list ',' exp
	;

/* scalar/list/hash */
scalar:		name
	|	string
	|	number
	|	block
	;

name:		NAME		{ emit("TOKNAME(%s)", $1); $$ = $1 }
	;


/* expression */
exp_or_empty:	/* nothing */
	|	exp
	;
exp:		name		{ emit("EXP NAME"); }
	|	func_call	{ emit("EXP FUNCCALL"); }
	|	asgn_exp	{ emit("EXP ASGN"); }
	|	arith_exp	{ emit("EXP ARITH"); }
	|	logi_exp	{ emit("EXP LOGI"); }
	|	bit_exp		{ emit("EXP BIT"); }
	|	cond_op_exp	{ emit("EXP COND_OP"); }
	|	comma_exp	{ emit("EXP COMMA"); }
	|	scalar		{ emit("EXP SCALAR"); }
	|	array		{ emit("EXP ARRAY"); }
	|	hash		{ emit("EXP HASH"); }
	;

exp:		
	;

useless:;
/* statement */
smpl_stmt:	exp	{ emit("EXP_STMT"); }
	;
stmt:		';'	{ emit("NOP"); }
	|	smpl_stmt ';'
	|	if_stmt
	|	unless_stmt
	|	while_stmt
	|	for_stmt
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

    fprintf(stderr, "Error: ");
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
}

