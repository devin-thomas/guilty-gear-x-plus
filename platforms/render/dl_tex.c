/* The renderer's texture cache (dl_tex.h). */
#include "dl_tex.h"

#include <stdlib.h>
#include <string.h>

#include "dl_frame.h"

uint64_t dl_hash64(const void *data, size_t n, uint64_t seed)
{
    const uint8_t *p = data;
    uint64_t h = seed ^ (n * 0x9E3779B97F4A7C15ull), w;

    for (; n >= 8; n -= 8, p += 8) {
        memcpy(&w, p, 8);
        h = (h ^ (w * 0xBF58476D1CE4E5B9ull)) * 0x94D049BB133111EBull;
        h ^= h >> 31;
    }
    if (n) {
        w = 0;
        memcpy(&w, p, n);
        h = (h ^ (w * 0xBF58476D1CE4E5B9ull)) * 0x94D049BB133111EBull;
    }
    h ^= h >> 29;
    h *= 0xBF58476D1CE4E5B9ull;
    return h ^ (h >> 32);
}

void dl_tex_init(DlTexCache *c)
{
    memset(c, 0, sizeof(*c));
}

void dl_tex_free(DlTexCache *c)
{
    uint32_t i;

    for (i = 0; i < c->n; i++) {
        free(c->e[i].pixels);
    }
    free(c->e);
    free(c->map);
    free(c->dead);
    memset(c, 0, sizeof(*c));
}

static void map_put(DlTexCache *c, uint64_t key, int32_t idx)
{
    uint32_t m = c->map_cap - 1, i = (uint32_t)key & m;

    while (c->map[i] >= 0) {
        i = (i + 1) & m;
    }
    c->map[i] = idx;
}

static void map_rebuild(DlTexCache *c, uint32_t cap)
{
    uint32_t i;

    free(c->map);
    c->map_cap = cap;
    c->map = malloc(sizeof(int32_t) * cap);
    if (!c->map) {
        abort();
    }
    for (i = 0; i < cap; i++) {
        c->map[i] = -1;
    }
    for (i = 0; i < c->n; i++) {
        if (c->e[i].live) {
            map_put(c, c->e[i].key, (int32_t)i);
        }
    }
}

int32_t dl_tex_find(DlTexCache *c, uint64_t key)
{
    uint32_t m, i;

    if (!c->map) {
        return -1;
    }
    m = c->map_cap - 1;
    for (i = (uint32_t)key & m; c->map[i] >= 0; i = (i + 1) & m) {
        if (c->e[c->map[i]].key == key) {
            return c->map[i];
        }
    }
    return -1;
}

int32_t dl_tex_add(DlTexCache *c, uint64_t key, uint16_t w, uint16_t h, uint8_t kind)
{
    uint32_t i, live = 0;
    DlTex *t;

    for (i = 0; i < c->n && c->e[i].live; i++) {
    }
    if (i == c->n) {
        if (c->n == c->cap) {
            c->cap = c->cap ? c->cap * 2 : 256;
            c->e = realloc(c->e, sizeof(DlTex) * c->cap);
            if (!c->e) {
                abort();
            }
        }
        c->n++;
    }
    t = &c->e[i];
    memset(t, 0, sizeof(*t));
    t->key = key;
    t->w = w;
    t->h = h;
    t->kind = kind;
    t->live = 1;
    t->pixels = malloc((size_t)w * h * (kind == DL_TEX_RGBA ? 4 : 1));
    if (!t->pixels) {
        abort();
    }
    for (live = 0, i = 0; i < c->n; i++) {
        live += c->e[i].live;
    }
    if (!c->map || live * 2 > c->map_cap) {
        map_rebuild(c, c->map_cap ? c->map_cap * 2 : 1024);
    } else {
        map_put(c, key, (int32_t)(t - c->e));
    }
    return (int32_t)(t - c->e);
}

void dl_tex_evict(DlTexCache *c, uint32_t serial, uint32_t age)
{
    uint32_t i, dropped = 0;

    for (i = 0; i < c->n; i++) {
        DlTex *t = &c->e[i];

        /* signed: a texture the frame being built uses is newer than serial */
        if (!t->live || (int32_t)(serial - t->last_used) <= (int32_t)age) {
            continue;
        }
        if (t->gpu) {
            if (c->ndead == c->cap_dead) {
                c->cap_dead = c->cap_dead ? c->cap_dead * 2 : 64;
                c->dead = realloc(c->dead, sizeof(void *) * c->cap_dead);
                if (!c->dead) {
                    abort();
                }
            }
            c->dead[c->ndead++] = t->gpu;
        }
        free(t->pixels);
        memset(t, 0, sizeof(*t));
        dropped++;
    }
    if (dropped) {
        map_rebuild(c, c->map_cap);
    }
}
