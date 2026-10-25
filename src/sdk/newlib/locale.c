/* Adapted from nathanialf/ico (MIT; e0881204, sce/libc/locale/locale.c): ee-gcc 2.9's newlib/libgcc as linked by both GGX discs. */
/* libc.a member locale.o */
#include "stdlib.h"
#include "string.h"
#include "reent.h"
#include "locale.h"

extern int __mb_cur_max; /* DATA_EXTERN: defined with an initializer in the original; bytes kept in the bins */

/* STATIC_EXTERN: the C locale's conventions (".", then "" fields and CHAR_MAX
   counts), kept in the bins */
extern const struct lconv lconv;

/* RODATA_EXTERN: the "C" and "" literals _setlocale_r compares against, which
   the original shares with lconv's initializer strings in this member's
   .rodata (a per-function build can't reproduce that pooling) */
extern const char locale_C[];
extern const char locale_empty[];

/* (a typedef so the build's C splitter, which skips chunks that open with
   `struct`, sees these two as functions) */
typedef struct lconv lconv_t;


char *_setlocale_r(struct Reent *data, int category, const char *locale)
{
    /* newlib's lc_ctype and last_lc_ctype statics ("C") are only used with
       MB_CAPABLE; unreferenced here, their .data stays in the bins */

    if (locale == 0)
        goto no_check;
    if (strcmp(locale, locale_C) == 0)
        goto found;
    if (strcmp(locale, locale_empty) != 0)
        return 0;
found:
    data->current_category = category;
    data->current_locale = locale;
no_check:
    return (char *)locale_C;
}

lconv_t *_localeconv_r(struct Reent *data)
{
    return (lconv_t *)&lconv;
}

char *setlocale(int category, const char *locale)
{
    return _setlocale_r(_impure_ptr, category, locale);
}

lconv_t *localeconv(void)
{
    return _localeconv_r(_impure_ptr);
}