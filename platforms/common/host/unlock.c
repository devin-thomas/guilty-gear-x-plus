/* Unlocks (unlock.h). */
#include "unlock.h"

#include <stdio.h>
#include <string.h>

/* USER_SETTING (type-inventory.csv): offsets in userinfo */
#define U_PHASE 0x6D         /* char */
#define U_USECOL 0x430       /* u_short[18] */
#define U_GCHECK 0x1214      /* int[128] */
#define U_GSTART 0x1414      /* int[128] */
#define NCHARS 18
#define NPICTURES 98         /* gallery pictures 0-97 */
#define ALL_COLOURS 0x7FF
#define FULL_PHASE 5
#define FULL_LIMIT 5         /* SetLimitStatus for phase 5 */

/* job.c's jobs where the unlocks are read afresh: TitleInit to TitleMain, OptionMenuInit and Main */
enum { JOB_TITLE_FIRST = 6, JOB_TITLE_LAST = 8, JOB_OPTION_INIT = 47, JOB_OPTION_MAIN = 48 };
#define OPT_PASSWORD 0x40    /* opt.thdflg: OPTIONS > PASSWORD is open */
#define PASS_CHECK 8         /* Opt_Password's passidx: the field it checks the entry */
#define PASS_FAILED 15
#define PASS_EX2_DONE 13     /* EX PASSWORD 2 INPUT COMPLETED */

static const char *const NAMES[4] = { "characters", "colours", "gallery", "challenges" };

/* G O H A N as the screen's letter indices (1 = A) */
static const int GOHAN[5] = { 7, 15, 8, 1, 14 };

static int gohan_pending;

const char *unlock_name(int item)
{
    int i;

    for (i = 0; i < 4; i++) {
        if (item == 1 << i) {
            return NAMES[i];
        }
    }
    return "?";
}

int unlock_parse(const char *s)
{
    int mask = 0, i;

    if (!strcmp(s, "all")) {
        return UNLOCK_ALL;
    }
    if (!strcmp(s, "none") || !*s) {
        return 0;
    }
    while (*s) {
        size_t n = strcspn(s, ",");

        for (i = 0; i < 4; i++) {
            if (strlen(NAMES[i]) == n && !strncmp(s, NAMES[i], n)) {
                mask |= 1 << i;
                break;
            }
        }
        if (i == 4) {
            return -1;
        }
        s += n + (s[n] == ',');
    }
    return mask;
}

static uint8_t r8(CoreHost *h, uint32_t a)
{
    return (uint8_t)(ee_r32(h, a & ~3u) >> (8 * (a & 3)));
}

static uint16_t r16(CoreHost *h, uint32_t a)
{
    return ee_r16(h, a);
}

/* writes through ee_ptr, which marks the page changed (core_state.h), and logs them */
static int w8(CoreHost *h, uint32_t a, uint8_t v, const char *what)
{
    uint8_t old = r8(h, a);

    if (old == v) {
        return 0;
    }
    *ee_ptr(h, a, 1) = v;
    shim_log(h, "unlock: %s @%08x %u -> %u", what, a, old, v);
    return 1;
}

static int w16(CoreHost *h, uint32_t a, uint16_t v, const char *what)
{
    uint16_t old = r16(h, a);

    if (old == v) {
        return 0;
    }
    memcpy(ee_ptr(h, a, 2), &v, 2);
    shim_log(h, "unlock: %s @%08x 0x%04x -> 0x%04x", what, a, old, v);
    return 1;
}

static int w32(CoreHost *h, uint32_t a, uint32_t v, const char *what)
{
    uint32_t old = ee_r32(h, a);

    if (old == v) {
        return 0;
    }
    ee_w32(h, a, v);
    shim_log(h, "unlock: %s @%08x %u -> %u", what, a, old, v);
    return 1;
}

int unlock_state(CoreHost *h)
{
    uint32_t u = ggx_sym_userinfo;
    int st = UNLOCK_ALL, i;

    if (r8(h, u + U_PHASE) < FULL_PHASE) {
        st &= ~UNLOCK_CHARACTERS;
    }
    for (i = 0; i < NCHARS; i++) {
        if ((r16(h, u + U_USECOL + 2u * (uint32_t)i) & ALL_COLOURS) != ALL_COLOURS) {
            st &= ~UNLOCK_COLOURS;
        }
    }
    for (i = 0; i < NPICTURES; i++) {
        if (!ee_r32(h, u + U_GSTART + 4u * (uint32_t)i) ||
            (i >= 40 && !ee_r32(h, u + U_GCHECK + 4u * (uint32_t)i))) {
            st &= ~UNLOCK_GALLERY;
        }
        if (i < 32 && !ee_r32(h, u + U_GCHECK + 4u * (uint32_t)i)) {
            st &= ~UNLOCK_CHALLENGES;
        }
    }
    return st;
}

int unlock_safe(CoreHost *h)
{
    uint32_t job = ee_r32(h, ggx_sym_JobMode);

    return (job >= JOB_TITLE_FIRST && job <= JOB_TITLE_LAST) || job == JOB_OPTION_INIT || job == JOB_OPTION_MAIN;
}

int unlock_apply(CoreHost *h, int items, const char *why)
{
    uint32_t u = ggx_sym_userinfo;
    char what[48];
    int n = 0, i;

    shim_log(h, "unlock: %s:%s%s%s%s", why, items & UNLOCK_CHARACTERS ? " characters" : "",
             items & UNLOCK_COLOURS ? " colours" : "", items & UNLOCK_GALLERY ? " gallery" : "",
             items & UNLOCK_CHALLENGES ? " challenges" : "");
    if (items & UNLOCK_CHARACTERS) {
        if (r8(h, u + U_PHASE) < FULL_PHASE) {
            n += w8(h, u + U_PHASE, FULL_PHASE, "userinfo.phase");
        }
        /* what SetLimitStatus() makes of phase 5 */
        if ((int32_t)ee_r32(h, ggx_sym_LimitStatus) < FULL_LIMIT) {
            n += w32(h, ggx_sym_LimitStatus, FULL_LIMIT, "LimitStatus");
        }
    }
    for (i = 0; (items & UNLOCK_COLOURS) && i < NCHARS; i++) {
        uint32_t a = u + U_USECOL + 2u * (uint32_t)i;

        snprintf(what, sizeof(what), "userinfo.usecol[%d]", i);
        n += w16(h, a, (uint16_t)(r16(h, a) | ALL_COLOURS), what);
    }
    for (i = 0; (items & UNLOCK_GALLERY) && i < NPICTURES; i++) {
        snprintf(what, sizeof(what), "userinfo.gstart[%d]", i);
        if (!ee_r32(h, u + U_GSTART + 4u * (uint32_t)i)) {
            n += w32(h, u + U_GSTART + 4u * (uint32_t)i, 1, what);
        }
        if (i >= 40 && !ee_r32(h, u + U_GCHECK + 4u * (uint32_t)i)) {
            snprintf(what, sizeof(what), "userinfo.gcheck[%d]", i);
            n += w32(h, u + U_GCHECK + 4u * (uint32_t)i, 1, what);
        }
    }
    for (i = 0; (items & UNLOCK_CHALLENGES) && i < 32; i++) {
        snprintf(what, sizeof(what), "userinfo.gcheck[%d]", i);
        if (!ee_r32(h, u + U_GCHECK + 4u * (uint32_t)i)) {
            n += w32(h, u + U_GCHECK + 4u * (uint32_t)i, 1, what);
        }
    }
    shim_log(h, "unlock: %d writes", n);
    return n;
}

void unlock_before(CoreHost *h)
{
    uint32_t b = ggx_static_Opt_Password_passbuf;
    int i;

    gohan_pending = 0;
    if (ee_r32(h, ggx_sym_JobMode) != JOB_OPTION_MAIN || !(ee_r32(h, ggx_sym_opt) & OPT_PASSWORD) ||
        ee_r32(h, ggx_static_Opt_Password_passmode) != 1 || ee_r32(h, ggx_static_Opt_Password_passidx) != PASS_CHECK) {
        return;
    }
    for (i = 0; i < 5; i++) {
        if ((int32_t)ee_r32(h, b + 4u * (uint32_t)i) != GOHAN[i]) {
            return;
        }
    }
    gohan_pending = 1;
}

int unlock_after(CoreHost *h)
{
    if (!gohan_pending) {
        return 0;
    }
    gohan_pending = 0;
    if (ee_r32(h, ggx_static_Opt_Password_passidx) != PASS_FAILED) {
        return 0;     /* the game took it some other way: leave it */
    }
    shim_log(h, "unlock: GOHAN on the password screen");
    w32(h, ggx_static_Opt_Password_passidx, PASS_EX2_DONE, "Opt_Password.passidx (EX PASSWORD 2 INPUT COMPLETED)");
    unlock_apply(h, UNLOCK_ALL, "GOHAN");
    return 1;
}
