#include "utils.h"

#include <stdio.h>
#include <stdarg.h>

int verbosity_level;

void log_printf(int level, char *fmt, ...)
{
    va_list args;
    int i;

    if (level <= verbosity_level) {
        va_start(args, fmt);

        for (i=0; i<5; i++) {
            fputc(i<level ? '*' : ' ', stderr);
        }
        vfprintf(stderr, fmt, args);
    }
}
