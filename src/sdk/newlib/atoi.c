/* Adapted from nathanialf/ico (MIT; e0881204, sce/libc/stdlib/atoi.c): ee-gcc 2.9's newlib/libgcc as linked by both GGX discs. */
/* libc.a member atoi.o */
#include "stdlib.h"
#include "string.h"
#include "reent.h"

int atoi(void *str)
{
    return (int)strtol(str, 0, 0xA);
}