/* Which button confirms in menus (confirm.h). */
#include "confirm.h"

#include <ctype.h>

#include "pad.h"

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

void confirm_reset(ConfirmLatch *l)
{
    l->held[0] = l->held[1] = -1;
}

uint16_t confirm_apply(ConfirmLatch *l, uint16_t buttons, int swap)
{
    static const uint16_t BIT[2] = { PAD_CROSS, PAD_CIRCLE };
    uint16_t out = buttons & (uint16_t)~(PAD_CROSS | PAD_CIRCLE);
    int i;

    for (i = 0; i < 2; i++) {
        if (!(buttons & BIT[i])) {
            l->held[i] = -1;
            continue;
        }
        if (l->held[i] < 0) {
            l->held[i] = (int8_t)(swap != 0);
        }
        out |= l->held[i] ? BIT[1 - i] : BIT[i];
    }
    return out;
}
