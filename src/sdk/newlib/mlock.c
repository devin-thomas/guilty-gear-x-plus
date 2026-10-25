/* Adapted from nathanialf/ico (MIT; e0881204, sce/libc/stdlib/mlock.c): ee-gcc 2.9's newlib/libgcc as linked by both GGX discs. */
/* libc.a member mlock.o */
#include "reent.h"
#include "libc_internal.h"

void __malloc_lock(Reent *ptr) {}

void __malloc_unlock(Reent *ptr) {}