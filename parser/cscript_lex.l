%{
#include <stdlib.h>
#include <string.h>
#include "cscript_yacc.tab.h"

void yyerror(char *s, ...);

int oldstate;
%}

%x COMMENT

%%

/* number */
[1-9]* |
0[0-7]* |
0[xX][0-9a-fA-F]*	{ yylval.intval = atoi(yytext); return INTNUM; }

[0-9]*.[0-9]+ |
[0-9]+.[0-9]*		{ yylval.fltval = atof(yytext); return FLTNUM; }

/* string */
\'([^\']|\\[nt\'])*\' |
\"([^\"]|\\[nt\"])*\"	{
	yytext[yyleng - 2] = '\0';
	yylval.strval = strdup((char*)yytext + 1);
	return STRING;
}

/* bool */
true			{ yylval.intval = 1; return BOOL; }
false			{ yylval.intval = 0; return BOOL; }
undef			{ yylval.intval = -1; return BOOL; }

/* name */
[a-zA-Z_][0-9a-zA-Z]*	{ yylval.strval = strdup(yytext); return STRING; }

/* comparison */
"<"			{ yylval.intval = 3; return RALATION; }
">"			{ yylval.intval = 4; return RALATION; }
"<="			{ yylval.intval = 5; return RALATION; }
">="			{ yylval.intval = 6; return RALATION; }
"=="			{ yylval.intval = 1; return EQUALITY; }
"!="			{ yylval.intval = 2; return EQUALITY; }

/* assignment */
"="			{ yylval.intval = 0; return ASSIGNMENT; }
"+="			{ yylval.intval = 1; return ASSIGNMENT; }
"-="			{ yylval.intval = 2; return ASSIGNMENT; }
"*="			{ yylval.intval = 3; return ASSIGNMENT; }
"/="			{ yylval.intval = 4; return ASSIGNMENT; }

/* keyword */
"if"			{ return IF; }
"elsif"			{ return ELSIF; }
"else"			{ return ELSE; }
"while"			{ return WHILE; }
"for"			{ return FOR; }
"do"			{ return DO; }
"return"		{ return RETURN; }
"import"		{ return IMPORT; }
"in"			{ return IN; }

/* comment */
"/*"			{ oldstate = YY_START; BEGIN COMMENT; }
<CCOMMENT>"*/"		{ BEGIN oldstate; }
"//".*			;
"#".*			;

/* operator */
"++"			{ return INCREASE; }
"--"			{ return DECREASE; }
"&&"			{ return LOGIAND; }
"||"			{ return LOGIOR; }
"<<"			{ yylval.intval = 1; return SHIFT; }
">>"			{ yylval.intval = 2; return SHIFT; }

/* symbol */
"+"|
"-"|
"*"|
"/"|
"%"|
";"|
","|
"."|
"{"|
"}"|
"!"|
"["|
"]"|
"?"|
":"|
"&"|
"|"|
"^"			{ return *yytext; }

