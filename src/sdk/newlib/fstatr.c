/* Adapted from nathanialf/ico (MIT; e0881204, sce/libc/reent/fstatr.c): ee-gcc 2.9's newlib/libgcc as linked by both GGX discs. */
/* libc.a member fstatr.o */
#include "stdlib.h"
#include "string.h"
#include "reent.h"
#include "errno.h"
#include "stat.h"

int _fstat_r(Reent *ptr, int fd, struct stat *pstat)
{
    int ret;
    errno = 0;
    ret = fstat(fd, pstat);
    if (ret == -1) {
        if (errno != 0) {
            ptr->err = errno;
        }
    }
    return ret;
}