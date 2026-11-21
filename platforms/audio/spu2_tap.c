/* Reading SPU2 taps (spu2_tap.h). */
#include "spu2_tap.h"

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

#define TAP_MAX_WORDS 0x100000u

FILE *spu2_tap_file(const char *path, int *piped)
{
    size_t n = strlen(path);
    char cmd[1200];

    *piped = n > 4 && !strcmp(path + n - 4, ".zst");
    if (!*piped) {
        return fopen(path, "rb");
    }
    if (strchr(path, '"') || n > 1000) {
        return NULL;
    }
    /* (a reader that stops early closes the pipe on zstd: its complaint is no news; a bad file reads as a cut-short
       tap) */
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd), "zstd -dcq \"%s\" 2>NUL", path);
    return popen(cmd, "rb");
#else
    snprintf(cmd, sizeof(cmd), "zstd -dcq \"%s\" 2>/dev/null", path);
    return popen(cmd, "r");
#endif
}

void spu2_tap_file_close(FILE *f, int piped)
{
    if (!f) {
        return;
    }
    if (piped) {
        pclose(f);
    } else {
        fclose(f);
    }
}

static int u32r(FILE *f, uint32_t *v)
{
    uint8_t b[4];

    if (fread(b, 1, 4, f) != 4) {
        return -1;
    }
    *v = (uint32_t)b[0] | (uint32_t)b[1] << 8 | (uint32_t)b[2] << 16 | (uint32_t)b[3] << 24;
    return 0;
}

static int words_r(FILE *f, uint16_t *w, uint32_t n)
{
    uint32_t i;

    if (fread(w, 2, n, f) != n) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        const uint8_t *b = (const uint8_t *)&w[i];
        w[i] = (uint16_t)(b[0] | b[1] << 8);
    }
    return 0;
}

int spu2_tap_open(Spu2Tap *t, const char *path)
{
    char magic[8];
    uint32_t version;

    memset(t, 0, sizeof(*t));
    if (!(t->f = spu2_tap_file(path, &t->piped)) || fread(magic, 1, 8, t->f) != 8 || memcmp(magic, "GGSPU2T", 8) != 0 ||
        u32r(t->f, &version) || version != 1 || !(t->buf = malloc(TAP_MAX_WORDS * 2))) {
        spu2_tap_close(t);
        return -1;
    }
    return 0;
}

int spu2_tap_next(Spu2Tap *t, Spu2TapRecord *r)
{
    int tag = fgetc(t->f);
    uint16_t v;

    memset(r, 0, sizeof(*r));
    if (tag == EOF) {
        return 0;
    }
    r->tag = tag;
    if (u32r(t->f, &r->tick)) {
        return -1;
    }
    switch (tag) {
    case 'E':
        return 0;
    case 'K':
        return 1;
    case 'S':
        if (words_r(t->f, t->buf, 0x400)) {
            return -1;
        }
        r->data = t->buf;
        return 1;
    case 'W':
        if (u32r(t->f, &r->addr) || words_r(t->f, &v, 1)) {
            return -1;
        }
        r->value = v;
        return 1;
    case 'M':
        if (u32r(t->f, &r->addr) || u32r(t->f, &r->words) || r->words > TAP_MAX_WORDS ||
            words_r(t->f, t->buf, r->words)) {
            return -1;
        }
        r->data = t->buf;
        return 1;
    default:
        return -1;
    }
}

void spu2_tap_close(Spu2Tap *t)
{
    spu2_tap_file_close(t->f, t->piped);
    free(t->buf);
    memset(t, 0, sizeof(*t));
}
