#include <stdio.h>
#include "logger.h"

void log_info(const char *msg)
{
    printf("%s\n", msg);
}

void log_error(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}
