#include "iso9660.h"

#include <ctype.h>
#include <string.h>

/* deep enough for any disc; X Plus has no subdirectories */
#define MAX_DEPTH 8

static uint32_t le32(const uint8_t *p)
{
    return p[0] | (p[1] << 8) | (p[2] << 16) | ((uint32_t)p[3] << 24);
}

int iso_volume(SectorRead rd, void *src, char *volume_id, uint32_t *sectors)
{
    uint8_t sec[2048];
    int n;

    if (rd(src, 16, 1, sec) != 0 || sec[0] != 1 || memcmp(sec + 1, "CD001", 5) != 0) {
        return -1;
    }
    if (volume_id) {
        for (n = 32; n > 0 && sec[40 + n - 1] == ' '; n--) {
        }
        memcpy(volume_id, sec + 40, (size_t)n);
        volume_id[n] = 0;
    }
    if (sectors) {
        *sectors = le32(sec + 80);
    }
    return 0;
}

/* a path component's length without its ";version" or a trailing '.' */
static size_t stem_len(const char *s, size_t n)
{
    const char *semi = memchr(s, ';', n);

    if (semi) {
        n = (size_t)(semi - s);
    }
    if (n && s[n - 1] == '.') {
        n--;    /* "FILE." for a name without an extension */
    }
    return n;
}

static int part_eq(const char *a, size_t an, const char *b, size_t bn)
{
    size_t i;

    an = stem_len(a, an);
    bn = stem_len(b, bn);
    if (an != bn) {
        return 0;
    }
    for (i = 0; i < an; i++) {
        if (toupper((unsigned char)a[i]) != toupper((unsigned char)b[i])) {
            return 0;
        }
    }
    return 1;
}

static int is_sep(char c)
{
    return c == '\\' || c == '/';
}

int iso_path_eq(const char *a, const char *b)
{
    while (is_sep(*a)) {
        a++;
    }
    while (is_sep(*b)) {
        b++;
    }
    while (*a || *b) {
        const char *ae = a, *be = b;

        while (*ae && !is_sep(*ae)) {
            ae++;
        }
        while (*be && !is_sep(*be)) {
            be++;
        }
        if (!part_eq(a, (size_t)(ae - a), b, (size_t)(be - b))) {
            return 0;
        }
        a = *ae ? ae + 1 : ae;
        b = *be ? be + 1 : be;
    }
    return 1;
}

static void root(const uint8_t *pvd, uint32_t *lsn, uint32_t *size)
{
    *lsn = le32(pvd + 156 + 2);
    *size = le32(pvd + 156 + 10);
}

/* a directory record other than "." and ".." */
static int is_entry(const uint8_t *r)
{
    return r[32] > 0 && !(r[32] == 1 && r[33] <= 1);
}

int iso_find(SectorRead rd, void *src, const char *path, uint32_t *lsn, uint32_t *size)
{
    uint8_t sec[2048];
    uint32_t dir_lsn, dir_size;
    const char *p = path;

    if (rd(src, 16, 1, sec) != 0) {
        return -1;
    }
    root(sec, &dir_lsn, &dir_size);
    while (is_sep(*p)) {
        p++;
    }
    while (*p) {
        const char *end = p;
        uint32_t s;
        int found = 0;

        while (*end && !is_sep(*end)) {
            end++;
        }
        for (s = 0; s < (dir_size + 2047) / 2048 && !found; s++) {
            uint32_t off = 0;

            if (rd(src, dir_lsn + s, 1, sec) != 0) {
                return -1;
            }
            while (off < 2048 && sec[off] != 0) {
                const uint8_t *r = sec + off;

                if (is_entry(r) && part_eq(p, (size_t)(end - p), (const char *)r + 33, r[32])) {
                    dir_lsn = le32(r + 2);
                    dir_size = le32(r + 10);
                    found = 1;
                    break;
                }
                off += r[0];
            }
        }
        if (!found) {
            return -1;
        }
        p = *end ? end + 1 : end;
    }
    *lsn = dir_lsn;
    *size = dir_size;
    return 0;
}

static int walk(SectorRead rd, void *src, uint32_t dir_lsn, uint32_t dir_size, const char *prefix, int depth,
                IsoFile *files, int max, int n)
{
    uint8_t sec[2048];
    uint32_t s;

    if (depth > MAX_DEPTH) {
        return -1;
    }
    for (s = 0; s < (dir_size + 2047) / 2048; s++) {
        uint32_t off = 0;

        if (rd(src, dir_lsn + s, 1, sec) != 0) {
            return -1;
        }
        while (off < 2048 && sec[off] != 0) {
            const uint8_t *r = sec + off;

            if (r[0] < 34 || off + r[0] > 2048) {
                return -1;
            }
            if (is_entry(r)) {
                char path[128];
                size_t plen = strlen(prefix), nlen = stem_len((const char *)r + 33, r[32]);

                if (plen + nlen + 2 > sizeof(path)) {
                    return -1;
                }
                memcpy(path, prefix, plen);
                memcpy(path + plen, r + 33, nlen);
                path[plen + nlen] = 0;
                if (r[25] & 2) {
                    path[plen + nlen] = '/';
                    path[plen + nlen + 1] = 0;
                    n = walk(rd, src, le32(r + 2), le32(r + 10), path, depth + 1, files, max, n);
                    if (n < 0) {
                        return -1;
                    }
                } else {
                    if (n < max) {
                        memcpy(files[n].path, path, plen + nlen + 1);
                        files[n].lsn = le32(r + 2);
                        files[n].size = le32(r + 10);
                    }
                    n++;
                }
            }
            off += r[0];
        }
    }
    return n;
}

int iso_list(SectorRead rd, void *src, IsoFile *files, int max)
{
    uint8_t sec[2048];
    uint32_t lsn, size;

    if (rd(src, 16, 1, sec) != 0) {
        return -1;
    }
    root(sec, &lsn, &size);
    return walk(rd, src, lsn, size, "", 0, files, max, 0);
}
