/* A frame dump writer: RGB, 8 bits, stored (uncompressed) deflate blocks, so it needs no zlib. The encoder is
 * minimal-png from game-kit; this file keeps the port's own file calls (fs_open takes a UTF-8 path on Windows). */
#include "png_write.h"

#include "fsys.h"
#include "mpng/mpng.h"

int png_write_rgba(const char *path, const uint8_t *rgba, uint32_t w, uint32_t h)
{
    MpngImage img = { rgba, w, h, 0, MPNG_RGBA };
    uint8_t *png = NULL;
    size_t len = 0;
    FILE *f;
    int rc;

    if (mpng_encode(&img, MPNG_RGB, &png, &len) != MPNG_OK) { /* alpha dropped */
        return -1;
    }
    f = fs_open(path, "wb");
    if (!f) {
        mpng_free(png);
        return -1;
    }
    rc = fwrite(png, 1, len, f) == len ? 0 : -1;
    if (fclose(f) != 0) {
        rc = -1;
    }
    mpng_free(png);
    return rc;
}
