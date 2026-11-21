/* Deterministic button mashing for test runs: both pads' input for a field
 * is a function of the seed and the field number alone, so a run and its
 * replay from a snapshot see the same input. Each pad holds a random set of
 * buttons (never SELECT, L3 or R3) for six fields at a time. */
#ifndef GGX_MASH_H
#define GGX_MASH_H

#include <stdint.h>

#include "core_host.h"

static inline uint64_t mash_mix(uint64_t x)
{
    x += 0x9e3779b97f4a7c15ull;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
    return x ^ (x >> 31);
}

static inline void mash_input(uint64_t seed, uint32_t field, PadInput pads[2])
{
    uint64_t r = mash_mix(seed ^ mash_mix(field / 6));
    int i;

    for (i = 0; i < 2; i++) {
        pads[i].buttons = (uint16_t)(r >> (16 * i)) & 0xf8ffu;
        pads[i].analog[0] = pads[i].analog[1] = pads[i].analog[2] = pads[i].analog[3] = 0x80;
    }
}

#endif
