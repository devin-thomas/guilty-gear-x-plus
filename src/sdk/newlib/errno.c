/* Adapted from nathanialf/ico (MIT; e0881204, sce/libc/errno/errno.c): ee-gcc 2.9's newlib/libgcc as linked by both GGX discs. */
/* libc.a member errno.o */
#include "reent.h"
#include "errno.h"

int *__errno(void)
{
    return &_impure_ptr->err;
}