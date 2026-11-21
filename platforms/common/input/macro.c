/* Button macros (macro.h). */
#include "macro.h"

#include <string.h>

#include "pad.h"

/* the eight buttons KEY CONFIG can give a function (opt_cfg.c disp_activebtn) */
#define CONFIGURABLE (PAD_SQUARE | PAD_CROSS | PAD_TRIANGLE | PAD_CIRCLE | PAD_L1 | PAD_R1 | PAD_L2 | PAD_R2)

static const struct {
    const char *name, *config;
    int parts;     /* 1 P, 2 K, 4 S, 8 HS */
} MACROS[MACRO_COUNT] = {
    { "P AND K", "p_and_k", 1 | 2 },
    { "S AND HS", "s_and_hs", 4 | 8 },
    { "P K S", "p_k_s", 1 | 2 | 4 },
    { "P K S HS", "p_k_s_hs", 1 | 2 | 4 | 8 },
};

void macro_default_keys(MacroKeys *k)
{
    k->punch = PAD_SQUARE;
    k->kick = PAD_CROSS;
    k->slash = PAD_TRIANGLE;
    k->hslash = PAD_CIRCLE;
}

const char *macro_name(int m)
{
    return m >= 0 && m < MACRO_COUNT ? MACROS[m].name : NULL;
}

const char *macro_config_name(int m)
{
    return m >= 0 && m < MACRO_COUNT ? MACROS[m].config : NULL;
}

int macro_by_config_name(const char *name)
{
    int m;

    for (m = 0; m < MACRO_COUNT; m++) {
        if (!strcmp(name, MACROS[m].config)) {
            return m;
        }
    }
    return -1;
}

/* One part's button: the game's, if it is one of the eight, else the default. */
static uint16_t part(uint32_t game, uint32_t fallback)
{
    return (uint16_t)(game && !(game & (game - 1)) && (game & CONFIGURABLE) ? game : fallback);
}

uint16_t macro_buttons(int m, const MacroKeys *k)
{
    MacroKeys d;
    uint16_t b = 0;
    int parts;

    if (m < 0 || m >= MACRO_COUNT) {
        return 0;
    }
    macro_default_keys(&d);
    parts = MACROS[m].parts;
    if (parts & 1) {
        b |= part(k->punch, d.punch);
    }
    if (parts & 2) {
        b |= part(k->kick, d.kick);
    }
    if (parts & 4) {
        b |= part(k->slash, d.slash);
    }
    if (parts & 8) {
        b |= part(k->hslash, d.hslash);
    }
    return b;
}
