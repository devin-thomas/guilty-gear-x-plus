/* newlib 1.8.2 libc/stdlib/malloc.c (Cygnus; https://sourceware.org/pub/newlib/newlib-1.8.2.tar.gz),
   the two non-reentrant wrappers, spelled with the headers adapted from nathanialf/ico (MIT). */
#include "reent.h"
#include "stdlib.h"

void *malloc(int nbytes)
{
    return _malloc_r(_impure_ptr, nbytes);
}

void free(void *aptr)
{
    _free_r(_impure_ptr, aptr);
}