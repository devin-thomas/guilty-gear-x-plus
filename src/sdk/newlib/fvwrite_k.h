/* fvwrite.c's GETIOV macro, in a header: the build's C splitter would take a
   preprocessor chunk ending in '}' for a function. */
#define GETIOV(extra_work)                                                                         \
    while (len == 0) {                                                                             \
        extra_work;                                                                                \
        p = iov->iov_base;                                                                         \
        len = iov->iov_len;                                                                        \
        iov++;                                                                                     \
    }
