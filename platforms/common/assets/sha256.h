#ifndef GGX_SHA256_H
#define GGX_SHA256_H

#include <stddef.h>
#include <stdint.h>

void sha256(const void *data, size_t len, uint8_t out[32]);

/* The same, a piece at a time. */
typedef struct {
    uint32_t h[8];
    uint8_t buf[64];
    size_t fill;
    uint64_t len;
} Sha256;

void sha256_init(Sha256 *s);
void sha256_update(Sha256 *s, const void *data, size_t len);
void sha256_final(Sha256 *s, uint8_t out[32]);
/* out gets the 64 lowercase hex digits and a NUL. */
void sha256_hex(const uint8_t digest[32], char out[65]);

#endif
