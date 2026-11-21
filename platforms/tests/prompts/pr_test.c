/* ggx-prompts-test: the button prompts (prompts/pr.h) on synthetic font
 * pictures, laid out as the HUD font's 0x43 and 0x44 are (12x16 cells, five
 * a row) but drawn here, so no disc or game data is needed. Each cell is
 * filled with a code for the character it holds, so the test can tell which
 * cell went where:
 *
 *   - no substitution while every letter is 0 (a PlayStation pad), or for
 *     another upload;
 *   - each symbol cell gets its letter's cell, from either picture, with
 *     the digits picture's colours carried into the symbols picture's CLUT;
 *   - the rest of the picture is untouched;
 *   - KEY CONFIG's columns (x < 384 and above, y < 400) take players 1 and
 *     2, the help lines lower down the shared player's help letters;
 *   - a symbol drawn in another symbol's cell (PORT-019: X confirms).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gs_mem.h"
#include "pr.h"

static int failures;

#define CHECK(c, ...)                                                                                                  \
    do {                                                                                                               \
        if (!(c)) {                                                                                                    \
            fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__);                                                       \
            fprintf(stderr, __VA_ARGS__);                                                                              \
            fprintf(stderr, "\n");                                                                                     \
            failures++;                                                                                                \
        }                                                                                                              \
    } while (0)

static int texel(const uint8_t *t, int x, int y)
{
    int i = y * 64 + x;

    return (t[i / 2] >> (i & 1 ? 4 : 0)) & 0xF;
}

static void put(uint8_t *t, int x, int y, int v)
{
    int i = y * 64 + x;

    t[i / 2] = (uint8_t)(i & 1 ? (t[i / 2] & 0x0F) | v << 4 : (t[i / 2] & 0xF0) | v);
}

/* Cell n of a picture: index v in its left half, 0 (clear) in its right. */
static void fill(uint8_t *t, int n, int v)
{
    int x, y;

    for (y = 0; y < 16; y++) {
        for (x = 0; x < 12; x++) {
            put(t, (n % 5) * 12 + x, (n / 5) * 16 + y, x < 6 ? v : 0);
        }
    }
}

static int cell_is(const uint8_t *t, int n, int v)
{
    int x, y;

    for (y = 0; y < 16; y++) {
        for (x = 0; x < 12; x++) {
            if (texel(t, (n % 5) * 12 + x, (n / 5) * 16 + y) != (x < 6 ? v : 0)) {
                return 0;
            }
        }
    }
    return 1;
}

int main(void)
{
    static uint8_t dig[2048], sym[2048];
    TlRawPicture d, s;
    Prompts *p;
    const uint8_t *out;
    char err[128];
    const char none[4] = { 0, 0, 0, 0 };
    const char xbox[4] = { 'B', 'A', 'X', 'Y' };       /* circle, cross, square, triangle */
    const char keys[4] = { 'X', 'Z', 'A', 'S' };
    const char part[4] = { 'B', '\n', 0, 'Y' };        /* a key with no letter, a PS shape */
    uint32_t v1, v2, v;
    int i;

    memset(&d, 0, sizeof(d));
    memset(&s, 0, sizeof(s));
    d.psm = s.psm = GS_PSMT4;
    d.w = d.h = s.w = s.h = 64;
    d.texels = dig;
    s.texels = sym;
    s.hash = 0x1234;
    d.hash = 0x5678;
    /* the same colours in another order: digits' index i (1-15) is symbols' 16 - i; 0 is clear in both */
    for (i = 1; i < 16; i++) {
        d.clut[i] = 0x80000000u | (uint32_t)i * 0x0F0B07u;
        s.clut[16 - i] = d.clut[i];
    }
    /* digits picture: cells 0-19 are 0-9 and A-J, index 1 + n % 14; symbols: K-Z, then the four shapes */
    for (i = 0; i < 20; i++) {
        fill(dig, i, 1 + i % 14);
        fill(sym, i, 1 + (i + 3) % 14);
    }

    p = pr_open(&d, &s, err, sizeof(err));
    CHECK(p != NULL, "pr_open: %s", err);
    if (!p) {
        return 1;
    }

    /* a PlayStation pad: no variant, no substitution */
    pr_set(p, 0, none);
    pr_set(p, 1, none);
    CHECK(pr_variant(p, 0x1234, 256, 200) == 0, "PS pad: a variant");
    CHECK(pr_subst(p, 0x1234, GS_PSMT4, 64, 64, sym, 0) == NULL, "variant 0 substitutes");

    /* player 1 an Xbox pad, player 2 the keyboard */
    pr_set(p, 0, xbox);
    pr_set(p, 1, keys);
    pr_set_help(p, 0, xbox);
    pr_set_help(p, 1, keys);
    v1 = pr_variant(p, 0x1234, 255, 138);
    v2 = pr_variant(p, 0x1234, 511, 138);
    CHECK(v1 && v2 && v1 != v2, "KEY CONFIG's columns: %08x %08x", v1, v2);
    CHECK(pr_variant(p, 0x9999, 255, 138) == 0, "another upload has a variant");
    pr_set_shared(p, 1);
    CHECK(pr_variant(p, 0x1234, 100, 432) == v2, "the help line doesn't follow the shared player 2");
    pr_set_shared(p, 0);
    CHECK(pr_variant(p, 0x1234, 600, 432) == v1, "the help line doesn't follow the shared player 1");

    out = pr_subst(p, 0x1234, GS_PSMT4, 64, 64, sym, v1);
    CHECK(out != NULL, "no substitution for the Xbox letters");
    if (out) {
        /* circle (cell 16) <- B (digits' cell 11), cross (17) <- A (digits 10), in the symbols' CLUT */
        CHECK(cell_is(out, 16, 16 - (1 + 11 % 14)), "circle isn't B");
        CHECK(cell_is(out, 17, 16 - (1 + 10 % 14)), "cross isn't A");
        /* square (18) <- X (symbols' cell 13), triangle (19) <- Y (symbols' 14), as they are */
        CHECK(cell_is(out, 18, 1 + (13 + 3) % 14), "square isn't X");
        CHECK(cell_is(out, 19, 1 + (14 + 3) % 14), "triangle isn't Y");
        for (i = 0; i < 16; i++) {
            CHECK(cell_is(out, i, 1 + (i + 3) % 14), "letter cell %d changed", i);
        }
    }
    out = pr_subst(p, 0x1234, GS_PSMT4, 64, 64, sym, v2);
    if (out) {
        CHECK(cell_is(out, 16, 1 + (13 + 3) % 14), "keyboard circle isn't X");
        CHECK(cell_is(out, 17, 1 + (15 + 3) % 14), "keyboard cross isn't Z");
        CHECK(cell_is(out, 18, 16 - (1 + 10 % 14)), "keyboard square isn't A");
        CHECK(cell_is(out, 19, 1 + (8 + 3) % 14), "keyboard triangle isn't S");
    }

    /* buttons without a letter keep their symbols */
    pr_set(p, 0, part);
    v = pr_variant(p, 0x1234, 255, 138);
    out = pr_subst(p, 0x1234, GS_PSMT4, 64, 64, sym, v);
    CHECK(out != NULL, "no substitution with two letters");
    if (out) {
        CHECK(cell_is(out, 17, 1 + (17 + 3) % 14), "cross (no letter) changed");
        CHECK(cell_is(out, 18, 1 + (18 + 3) % 14), "square (no letter) changed");
        CHECK(cell_is(out, 19, 1 + (14 + 3) % 14), "triangle isn't Y");
    }
    CHECK(pr_subst(p, 0x1234, GS_PSMT8, 64, 64, sym, v) == NULL, "another format substituted");
    CHECK(pr_has_letter('Q') && pr_has_letter('7') && !pr_has_letter('a') && !pr_has_letter('-'), "pr_has_letter");

    /* PORT-019: a PlayStation pad where X confirms: the help line shows X's symbol in Circle's place and Circle's
       in X's; KEY CONFIG (above y 400) keeps the plain symbols */
    {
        const char swapped[4] = { PR_SYMBOL(PR_CROSS), PR_SYMBOL(PR_CIRCLE), 0, PR_SYMBOL(PR_TRIANGLE) };

        pr_set(p, 0, none);
        pr_set_help(p, 0, swapped);
        pr_set_shared(p, 0);
        CHECK(pr_variant(p, 0x1234, 255, 138) == 0, "KEY CONFIG with symbols isn't plain");
        v = pr_variant(p, 0x1234, 100, 432);
        out = pr_subst(p, 0x1234, GS_PSMT4, 64, 64, sym, v);
        CHECK(v && out, "no help-line variant");
        if (out) {
            CHECK(cell_is(out, 16, 1 + (17 + 3) % 14), "help line's Circle cell isn't X's symbol");
            CHECK(cell_is(out, 17, 1 + (16 + 3) % 14), "help line's X cell isn't Circle's symbol");
            CHECK(cell_is(out, 18, 1 + (18 + 3) % 14) && cell_is(out, 19, 1 + (19 + 3) % 14), "square, triangle");
        }
    }

    pr_free(p);
    printf("prompts: %s (%d failed)\n", failures ? "FAILED" : "ok", failures);
    return failures != 0;
}
