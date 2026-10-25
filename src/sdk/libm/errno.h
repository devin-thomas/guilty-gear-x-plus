/* errno.h for GGX's libm wrappers: the part of newlib 1.8.2's
   libc/include/sys/errno.h they use. */
#ifndef GGX_ERRNO_H
#define GGX_ERRNO_H
#define errno (*__errno())
extern int *__errno(void);
#define EDOM 33   /* Math arg out of domain of func */
#define ERANGE 34 /* Math result not representable */
#endif
