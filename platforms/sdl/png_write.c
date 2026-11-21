/* A minimal PNG writer for frame dumps: RGB, 8 bits, stored (uncompressed)
 * deflate blocks, so it needs no zlib. */
#include "png_write.h"

#include <stdlib.h>
#include <string.h>

#include "fsys.h"

static uint32_t crc_table[256];

static void crc_init(void)
{
    uint32_t n, c;
    int k;

    for (n = 0; n < 256; n++) {
        for (c = n, k = 0; k < 8; k++) {
            c = c & 1 ? 0xEDB88320u ^ (c >> 1) : c >> 1;
        }
        crc_table[n] = c;
    }
}

static uint32_t crc(uint32_t c, const uint8_t *p, size_t n)
{
    while (n--) {
        c = crc_table[(c ^ *p++) & 0xFF] ^ (c >> 8);
    }
    return c;
}

static void be32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24), p[1] = (uint8_t)(v >> 16), p[2] = (uint8_t)(v >> 8), p[3] = (uint8_t)v;
}

static int chunk(FILE *f, const char *type, const uint8_t *data, uint32_t n)
{
    uint8_t h[8];
    uint32_t c;

    be32(h, n);
    memcpy(h + 4, type, 4);
    c = crc(0xFFFFFFFFu, h + 4, 4);
    c = crc(c, data, n) ^ 0xFFFFFFFFu;
    if (fwrite(h, 1, 8, f) != 8 || (n && fwrite(data, 1, n, f) != n)) {
        return -1;
    }
    be32(h, c);
    return fwrite(h, 1, 4, f) == 4 ? 0 : -1;
}

int png_write_rgba(const char *path, const uint8_t *rgba, uint32_t w, uint32_t h)
{
    static const uint8_t sig[8] = { 0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n' };
    size_t row = (size_t)w * 3 + 1, raw_n = row * h, blocks = (raw_n + 65534) / 65535;
    size_t z_n = 2 + raw_n + 5 * blocks + 4, i, o = 0;
    uint8_t ihdr[13], *raw = malloc(raw_n), *z = malloc(z_n);
    uint32_t a = 1, b = 0, x, y;
    FILE *f;
    int rc = -1;

    if (!raw || !z) {
        goto done;
    }
    if (!crc_table[1]) {
        crc_init();
    }
    for (y = 0; y < h; y++) {
        raw[y * row] = 0;
        for (x = 0; x < w; x++) {
            memcpy(raw + y * row + 1 + 3 * x, rgba + ((size_t)y * w + x) * 4, 3);
        }
    }
    z[o++] = 0x78;
    z[o++] = 0x01;
    for (i = 0; i < raw_n; i += 65535) {
        uint32_t n = (uint32_t)(raw_n - i < 65535 ? raw_n - i : 65535);

        z[o++] = i + n == raw_n;
        z[o++] = (uint8_t)n, z[o++] = (uint8_t)(n >> 8);
        z[o++] = (uint8_t)~n, z[o++] = (uint8_t)(~n >> 8);
        memcpy(z + o, raw + i, n);
        o += n;
    }
    for (i = 0; i < raw_n; i++) {
        a = (a + raw[i]) % 65521;
        b = (b + a) % 65521;
    }
    be32(z + o, b << 16 | a);
    o += 4;
    be32(ihdr, w);
    be32(ihdr + 4, h);
    ihdr[8] = 8, ihdr[9] = 2, ihdr[10] = 0, ihdr[11] = 0, ihdr[12] = 0;
    f = fs_open(path, "wb");
    if (!f) {
        goto done;
    }
    rc = fwrite(sig, 1, 8, f) == 8 && chunk(f, "IHDR", ihdr, 13) == 0 && chunk(f, "IDAT", z, (uint32_t)o) == 0 &&
                 chunk(f, "IEND", NULL, 0) == 0
             ? 0
             : -1;
    if (fclose(f) != 0) {
        rc = -1;
    }
done:
    free(raw);
    free(z);
    return rc;
}
