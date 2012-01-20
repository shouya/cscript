%{
#include <stdarg.h>
%}

%%

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

