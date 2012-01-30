#include <stdio.h>
#include <stdarg.h>

#include "cscript_common.h"

FILE* cscript_outfile;

/* private functions declaratioin */
void _cscript_output(char *s, va_list ap, const char* type);

/* global functions implementation */

void cscript_init_logfile(char *filename)
{
	if (cscript_outfile && cscript_outfile != stdout) {
		fclose(cscript_outfile);
	}
	if (filename == NULL) {
		cscript_outfile = stdout;
		return;
	}
	
	if ((cscript_outfile = fopen(filename, "w")) != NULL)
		return;
	/* cannot open file */
	return;
}


void cscript_error(char *s, ...)
{
	va_list ap;
	va_start(ap, s);

	_cscript_output(s, ap, "ERR: ");

	va_end(ap);
}

void cscript_warning(char *s, ...)
{
	va_list ap;
	va_start(ap, s);

	_cscript_output(s, ap, "WARN: ");

	va_end(ap);
}

void cscript_log(char *s, ...)
{
	va_list ap;
	va_start(ap, s);

	_cscript_output(s, ap, "LOG: ");

	va_end(ap);
}

/* private functions implementation */

void _cscript_output(char *s, va_list ap, const char* type)
{
	if (!cscript_outfile) {
		return;
	}

	fprintf(cscript_outfile, type);
	vfprintf(cscript_outfile, s, ap);
	fprintf(cscript_outfile, "\n");
}
