#include <stdarg.h>
#include <stdio.h>

void log_message(const char *format, ...)
{
	FILE *logfile = fopen("log", "a");
	va_list args;
	va_start(args, format);
	vfprintf(logfile, format, args);
	va_end(args);
	fclose(logfile);
}
