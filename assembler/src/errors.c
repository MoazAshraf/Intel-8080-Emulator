// errors.c: printing error messages

#include <stdarg.h>
#include <stdio.h>

extern char *prog;

// printerr: prints a formatted error message.
void printerr(char *fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    fprintf(stderr, "%s: ", prog);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}