%option noyywrap nodefault yylineno

%{
#include <stdlib.h>
#include <string.h>
#include "cscript_yacc.tab.h"

extern void yyerror(char *s, ...);
extern int fileno(FILE *stream);
extern char *strdup(const char *s);

int oldstate;
%}

%x COMMENT

%%

[1-9]+ |
0[0-7]* |
0[xX][0-9a-fA-F]*	{ yylval.intval = atoi(yytext); return INTNUM; }

[0-9]*\.[0-9]+ |
[0-9]+\.[0-9]*		{ yylval.fltval = atof(yytext); return FLTNUM; }

\'([^\']|\\[nt\'])*\' |
\"([^\"]|\\[nt\"])*\"	{
	yytext[yyleng - 1] = '\0';
	yylval.strval = (char*)strdup((char*)yytext + 1);
	return STRING;
}

"true"			{ yylval.intval = 1; return BOOL; }
"false"			{ yylval.intval = 0; return BOOL; }
"undef"			{ yylval.intval = -1; return BOOL; }

"="			{ yylval.intval = 0; return ASSIGNMENT; }
"+="			{ yylval.intval = 1; return ASSIGNMENT; }
"-="			{ yylval.intval = 2; return ASSIGNMENT; }
"*="			{ yylval.intval = 3; return ASSIGNMENT; }
"/="			{ yylval.intval = 4; return ASSIGNMENT; }

"<"			{ yylval.intval = '<'; return RELATION; }
">"			{ yylval.intval = '>'; return RELATION; }
"<="			{ yylval.intval = 'L'; return RELATION; }
">="			{ yylval.intval = 'G'; return RELATION; }
"=="			{ yylval.intval = '='; return EQUALITY; }
"!="			{ yylval.intval = '!'; return EQUALITY; }

"if"			{ return IF; }
"else"			{ return ELSE; }
"while"			{ return WHILE; }
"for"			{ return FOR; }
"do"			{ return DO; }
"return"		{ return RETURN; }
^"#import"		{ return IMPORT; }
"in"			{ return IN; }

"/*"			{ oldstate = YY_START; BEGIN COMMENT; }
<COMMENT>"*/"		{ BEGIN oldstate; }
"//".*			;
"#".*			;

"++"			{ return INCREASE; }
"--"			{ return DECREASE; }
"&&"			{ return LOGIAND; }
"||"			{ return LOGIOR; }
"<<"			{ yylval.intval = 'l'; return SHIFT; }
">>"			{ yylval.intval = 'r'; return SHIFT; }

"+" |
"-" |
"*" |
"/" |
"%" |
";" |
"," |
"." |
"{" |
"}" |
"!" |
"[" |
"]" |
"(" |
")" |
"?" |
":" |
"&" |
"|" |
"^"			{ return *yytext; }

[a-zA-Z_][0-9a-zA-Z]*	{ yylval.strval = (char*)strdup(yytext); return NAME; }

[ \t\n]			{ /* ignore whitespaces */ }


.			{ yyerror("mystery character: [%c](%d)",
				  *yytext, *yytext);
	/* the following code are used to ignore some gcc `unused' warning. */
	void *a = &yyunput; a = &input; ++a; }
