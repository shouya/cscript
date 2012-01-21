#include "cscript_common.c"

FILE* cscript_outfile;

/* private functions declaratioin */
void _cscript_output(char *s, va_list var_args, const char* type);

/* global functions implementation */

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

void _cscript_output(char *s, va_list var_args, const char* type)
{
	if (!cscrip_log_file) {
		return;
	}

	fprintf(cscript_outfile, type);
	vfprintf(cscript_outfile, s, ap);
	fprintf(cscript_outfile, "\n");
}
