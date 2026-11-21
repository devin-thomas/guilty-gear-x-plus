/* A minimal PNG writer for frame dumps (no zlib needed). */
#ifndef GGX_PNG_WRITE_H
#define GGX_PNG_WRITE_H

#include <stdint.h>

/* Write w x h RGBA pixels (alpha dropped) as an 8-bit RGB PNG. 0 on success. */
int png_write_rgba(const char *path, const uint8_t *rgba, uint32_t w, uint32_t h);

#endif
