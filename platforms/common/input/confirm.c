/* Which button confirms in menus (confirm.h). */
#include "confirm.h"

#include <ctype.h>

int confirm_parse(const char *s)
{
    const char *x = "x", *c = "circle";
    const char *p;

    for (p = s; *p && *x && tolower((unsigned char)*p) == *x; p++, x++) {
    }
    if (!*p && !*x) {
        return CONFIRM_X;
    }
    for (p = s; *p && *c && tolower((unsigned char)*p) == *c; p++, c++) {
    }
    return !*p && !*c ? CONFIRM_CIRCLE : -1;
}

const char *confirm_name(int confirm)
{
    return confirm == CONFIRM_X ? "x" : "circle";
}
