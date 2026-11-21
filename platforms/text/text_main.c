/* ggx-text: proof sheets of the English text layer (PORT-014).
 *
 *   ggx-text [--data-dir <dir> | --disc <image>] [--text-dir <dir>] [--lang <file>]
 *            [--out <dir>] [--fid N] [--quiet]
 *
 * Reads the translation files (default <text dir>/en/main.lang) and the
 * player's disc as ggx does, draws every English picture, and checks each
 * one: its picture is on this disc, and the English fits. With --out it
 * writes, per disc file, <out>/<release>-<fid>.png: each original above its
 * English, on a dark background, for proofreading. Those sheets are game
 * imagery: keep them out of git. Exit status 1 if a picture is missing or
 * an English line doesn't fit.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_host.h"
#include "fsys.h"
#include "png_write.h"
#include "tl_host.h"

#if defined(VERSION_KR)
#define RELEASE "KR"
#define RELEASE_ID TL_KR
#else
#define RELEASE "JP"
#define RELEASE_ID TL_JP
#endif

#ifndef GGX_TEXT_SOURCE_DIR
#define GGX_TEXT_SOURCE_DIR "text"
#endif

#define SHEET_W 1100
#define GAP 6
#define BG 0xFF462828u   /* dark blue, RGBA little-endian */

static void usage(void)
{
    fprintf(stderr, "usage: ggx-text [--data-dir <dir> | --disc <image>] [--text-dir <dir>] [--lang <file>]\n"
                    "                [--out <dir>] [--fid N] [--quiet]\n");
    exit(2);
}

static int texel(const TlPicture *p, const uint8_t *buf, uint32_t x, uint32_t y)
{
    size_t i = (size_t)y * p->w + x;

    return p->psm == 0x13 ? buf[i] : (buf[i >> 1] >> ((i & 1) * 4)) & 15;
}

/* A picture over the background, as the GS would blend it at full alpha
   (0x80 = 1.0). */
static void blit(uint32_t *sheet, uint32_t sw, uint32_t sh, int ox, int oy, const TlPicture *p, const uint8_t *buf)
{
    uint32_t x, y;

    for (y = 0; y < p->h; y++) {
        for (x = 0; x < p->w; x++) {
            uint32_t c = p->clut[texel(p, buf, x, y)], a = c >> 24, k;
            uint32_t *d;
            uint32_t out = 0xFF000000u;

            if ((uint32_t)ox + x >= sw || (uint32_t)oy + y >= sh) {
                continue;
            }
            d = &sheet[(size_t)(oy + (int)y) * sw + (size_t)ox + x];
            a = a > 0x80 ? 0x80 : a;
            for (k = 0; k < 24; k += 8) {
                uint32_t s = (c >> k) & 0xFF, b = (*d >> k) & 0xFF;

                out |= ((s * a + b * (0x80 - a)) >> 7) << k;
            }
            *d = out;
        }
    }
}

static int write_sheet(const char *dir, TextLayer *t, uint32_t fid, uint32_t *idx, uint32_t n)
{
    uint32_t i, h = GAP, x = 0, rowh = 0, *px;
    int rc;
    char name[64], path[1100];
    TlPicture p;

    /* pairs (original above English) in rows across the sheet */
    for (i = 0; i < n; i++) {
        tl_picture(t, idx[i], &p);
        if (x && x + p.w > SHEET_W) {
            h += rowh + GAP;
            x = 0;
            rowh = 0;
        }
        x += p.w + GAP;
        rowh = 2 * p.h + 2 > rowh ? 2 * p.h + 2 : rowh;
    }
    h += rowh + GAP;
    px = malloc((size_t)SHEET_W * h * 4);
    if (!px) {
        return -1;
    }
    for (i = 0; i < (size_t)SHEET_W * h; i++) {
        px[i] = BG;
    }
    x = 0;
    rowh = 0;
    h = GAP;
    for (i = 0; i < n; i++) {
        uint32_t y;

        tl_picture(t, idx[i], &p);
        if (x && x + p.w > SHEET_W) {
            h += rowh + GAP;
            x = 0;
            rowh = 0;
        }
        blit(px, SHEET_W, (uint32_t)-1 / SHEET_W, (int)x, (int)h, &p, p.orig);
        if (p.english) {
            blit(px, SHEET_W, (uint32_t)-1 / SHEET_W, (int)x, (int)(h + p.h + 2), &p, p.english);
        }
        /* a frame: grey, red if it didn't fit, yellow if marked for review */
        for (y = 0; y < 2 * p.h + 2; y++) {
            uint32_t edge = p.fit == TL_FIT_NOT ? 0xFF2020E0u : p.review ? 0xFF20D0E0u : 0xFF706060u, k;

            px[(size_t)(h + y) * SHEET_W + x] = edge;
            px[(size_t)(h + y) * SHEET_W + x + p.w - 1] = edge;
            if (y == 0 || y == p.h || y == 2 * p.h + 1) {
                for (k = 0; k < p.w; k++) {
                    px[(size_t)(h + y) * SHEET_W + x + k] = edge;
                }
            }
        }
        x += p.w + GAP;
        rowh = 2 * p.h + 2 > rowh ? 2 * p.h + 2 : rowh;
    }
    h += rowh + GAP;
    snprintf(name, sizeof(name), "%s-%03u.png", RELEASE, fid);
    rc = fs_join(path, sizeof(path), dir, "/", name) == 0 ? png_write_rgba(path, (uint8_t *)px, SHEET_W, h) : -1;
    free(px);
    return rc;
}

int main(int argc, char **argv)
{
    CoreHostConfig cfg;
    static CoreHost host;
    const char *text_dir = GGX_TEXT_SOURCE_DIR, *lang = NULL, *out = NULL;
    char pref[1024], lang_buf[1100], fonts[1100], err[8192];
    long only = -1;
    int i, bound, quiet = 0, rc = 0;
    uint32_t n, k, missing = 0, smaller = 0, overflow = 0, review = 0, *idx;
    TextLayer *t;

    memset(&cfg, 0, sizeof(cfg));
    cfg.cards[0] = 1;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--quiet")) {
            quiet = 1;
            continue;
        }
        if (i + 1 >= argc) {
            usage();
        }
        if (!strcmp(argv[i], "--data-dir")) {
            cfg.data_dir = argv[++i];
        } else if (!strcmp(argv[i], "--disc")) {
            cfg.disc_path = argv[++i];
        } else if (!strcmp(argv[i], "--text-dir")) {
            text_dir = argv[++i];
        } else if (!strcmp(argv[i], "--lang")) {
            lang = argv[++i];
        } else if (!strcmp(argv[i], "--out")) {
            out = argv[++i];
        } else if (!strcmp(argv[i], "--fid")) {
            only = strtol(argv[++i], NULL, 10);
        } else {
            usage();
        }
    }
    if (cfg.disc_path && cfg.data_dir) {
        usage();
    }
    if (!cfg.disc_path && !cfg.data_dir) {
        if (fs_pref_dir(pref, sizeof(pref)) != 0) {
            fprintf(stderr, "ggx-text: no user data directory: set HOME, or pass --data-dir\n");
            return 2;
        }
        cfg.data_dir = pref;
    }
    if (!lang) {
        if (fs_join(lang_buf, sizeof(lang_buf), text_dir, "/", "en/main.lang") != 0) {
            usage();
        }
        lang = lang_buf;
    }
    if (fs_join(fonts, sizeof(fonts), text_dir, "/", "fonts") != 0) {
        usage();
    }
    t = tl_open(RELEASE_ID, lang, fonts, err, sizeof(err));
    if (!t) {
        fprintf(stderr, "ggx-text: %s\n", err);
        return 2;
    }
    if (core_host_open(&host, &cfg) != 0) {
        return 2;
    }
    bound = tl_bind_host(t, &host, err, sizeof(err));
    if (bound < 0) {
        fprintf(stderr, "ggx-text: %s\n", err);
        return 2;
    }
    if (err[0]) {
        fprintf(stderr, "%s", err);
        rc = 1;
    }
    n = tl_count(t);
    idx = malloc(sizeof(uint32_t) * (n + 1));
    if (!idx) {
        return 2;
    }
    for (k = 0; k < n; k++) {
        TlPicture p;

        if (tl_picture(t, k, &p) != 0) {
            missing++;
            continue;
        }
        if (only >= 0 && p.fid != (uint32_t)only) {
            continue;
        }
        review += p.review != 0;
        if (p.fit == TL_FIT_SMALLER) {
            smaller++;
        } else if (p.fit == TL_FIT_NOT) {
            overflow++;
            rc = 1;
            fprintf(stderr, "%u/%s: the English doesn't fit\n", p.fid, p.path);
        }
        if (!quiet) {
            printf("%u/%s\t%s%s\t%s\n", p.fid, p.path, p.fit == TL_FIT ? "fits" : p.fit == TL_FIT_SMALLER ? "smaller" : "OVERFLOW",
                   p.review ? " review" : "", p.text);
        }
    }
    if (out) {
        uint32_t fid, m;

        fs_mkdirs(out);
        for (fid = 0; fid < 1024; fid++) {
            m = 0;
            for (k = 0; k < n; k++) {
                TlPicture p;

                if (tl_picture(t, k, &p) == 0 && p.fid == fid && (only < 0 || (long)fid == only)) {
                    idx[m++] = k;
                }
            }
            if (m && write_sheet(out, t, fid, idx, m) != 0) {
                fprintf(stderr, "ggx-text: cannot write a sheet in %s\n", out);
                rc = 2;
            }
        }
    }
    fprintf(stderr, "ggx-text: %s: %u entries, %d pictures bound, %u not found, %u drawn smaller, %u don't fit, %u marked for review\n",
            RELEASE, n, bound, missing, smaller, overflow, review);
    free(idx);
    tl_free(t);
    core_host_close(&host);
    return rc;
}
