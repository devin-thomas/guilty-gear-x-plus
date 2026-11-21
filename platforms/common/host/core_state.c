/* Core state: its regions, the State Hash, and save/load (core_state.h).
 * core_state.h describes the State Hash this implements. */
#include "core_state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_host.h"

#define XXH_INLINE_ALL
#include "xxhash.h"

uint8_t ggx_core_dirty[1u << 20];
uint8_t ggx_core_read[1u << 20];
#ifdef GGX_STATE_SCAN
static CoreHost *scan_host;   /* the host reads are reported for */
#if defined(_WIN32)
#include <windows.h>
#define backtrace(frames, n) (int)RtlCaptureStackBackTrace(0, (n), (frames), NULL)
#elif defined(__GLIBC__) || defined(__APPLE__)
#include <execinfo.h>
#else
#define backtrace(frames, n) 0
#endif
#endif

/* The Core's one mutable wasm global; tools/w2c_patch.py fails the build
   if wasm2c's instance struct ever holds another. */
#define CORE_SP(h) ((h)->inst.w2c_0x5F_stack_pointer0x2E1)

#define HOST_REGION_SIZE 64u
#define MEM_REGIONS (CORE_REGION_COUNT - 1)   /* all but the host region */
#define HEADER_SIZE 4096u
#define ROUND_PAGE(n) (((n) + CORE_PAGE_SIZE - 1) & ~(size_t)(CORE_PAGE_SIZE - 1))

#if defined(VERSION_KR)
#define RELEASE 1u
#else
#define RELEASE 2u
#endif

struct CoreState {
    CoreRegion regions[CORE_REGION_COUNT];
    uint32_t first[CORE_REGION_COUNT];    /* each region's first page in the page arrays */
    uint32_t npages[CORE_REGION_COUNT];
    uint32_t offset[CORE_REGION_COUNT];   /* each region's data in a snapshot */
    uint32_t pages;                       /* every region's pages, the host region's one included */
    uint32_t hash_offset;                 /* the page hash table in a snapshot */
    size_t size;                          /* a snapshot's size */
    uint64_t build_id;
    uint64_t instance;                    /* this host, among all in the process */

    /* A page's epoch is the epoch in which it last changed. A snapshot
       saved or loaded in epoch E is equal to the Core in every page whose
       epoch is below E. Generations start over when the epoch would wrap. */
    uint32_t epoch, generation;
    uint32_t *page_epoch;
    uint64_t *page_hash;
    uint8_t *stale;                       /* page_hash needs recomputing */
    uint8_t host[HOST_REGION_SIZE];
    size_t last_copy;                     /* bytes the last save or load copied */
};

struct CoreSnapshot {
    uint8_t *data;
    size_t size;
    uint64_t owner;                       /* the instance it was last made equal to; 0 if none */
    uint32_t generation, sync_epoch;
};

static void put32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

static uint32_t get32(const uint8_t *p)
{
    return p[0] | (p[1] << 8) | (p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void put64(uint8_t *p, uint64_t v)
{
    put32(p, (uint32_t)v);
    put32(p + 4, (uint32_t)(v >> 32));
}

static uint64_t get64(const uint8_t *p)
{
    return get32(p) | ((uint64_t)get32(p + 4) << 32);
}

/* The host region (STATE-HASH.md): what the host and shims keep for the
   Core, as nine little-endian words and zero padding. */
static void host_pack(CoreHost *h)
{
    uint8_t *p = h->state->host;

    memset(p, 0, HOST_REGION_SIZE);
    put32(p + 0, CORE_SP(h));
    put32(p + 4, h->field);
    put32(p + 8, h->vsyncs);
    put32(p + 12, h->next_module_id);
    put32(p + 16, (uint32_t)h->mc.pending);
    put32(p + 20, (uint32_t)h->mc.func);
    put32(p + 24, (uint32_t)h->mc.result);
    put32(p + 28, (uint32_t)h->cards[0]);
    put32(p + 32, (uint32_t)h->cards[1]);
}

static void host_unpack(CoreHost *h, const uint8_t *p)
{
    CORE_SP(h) = get32(p + 0);
    h->field = get32(p + 4);
    h->vsyncs = get32(p + 8);
    h->next_module_id = get32(p + 12);
    h->mc.pending = (int)get32(p + 16);
    h->mc.func = (int)get32(p + 20);
    h->mc.result = (int)get32(p + 24);
    h->cards[0] = (int)get32(p + 28);
    h->cards[1] = (int)get32(p + 32);
}

/* The length of a region's page i: 4096, or what is left of the region. */
static uint32_t page_len(const CoreRegion *g, uint32_t i)
{
    uint32_t off = i * CORE_PAGE_SIZE;

    return g->size - off < CORE_PAGE_SIZE ? g->size - off : CORE_PAGE_SIZE;
}

/* A page's bytes in the Core (the host region's are its packed copy). */
static const uint8_t *page_data(CoreHost *h, int r, uint32_t i)
{
    uint32_t off = i * CORE_PAGE_SIZE;

    return r == MEM_REGIONS ? h->state->host + off : h->mem + h->state->regions[r].base + off;
}

/* instance numbers: a snapshot never mistakes a later host for its own */
static uint64_t instances;

int core_state_init(CoreHost *h)
{
    static const CoreRegion defs[CORE_REGION_COUNT] = {
        { CORE_REGION_RAM, "ram", 0x00000000u, EE_RAM_SIZE },
        { CORE_REGION_SPR, "spr", EE_SPR_BASE, EE_SPR_SIZE },
        { CORE_REGION_EEIO, "eeio", 0x10000000u, 0x00010000u },
        { CORE_REGION_VUMEM, "vumem", 0x11000000u, 0x00010000u },
        { CORE_REGION_GSPRIV, "gspriv", 0x12000000u, 0x00002000u },
        { CORE_REGION_OWN, "own", CORE_OWN_BASE, 0 },
        { CORE_REGION_HOST, "host", 0, HOST_REGION_SIZE },
    };
    CoreState *s = calloc(1, sizeof(*s));
    XXH3_state_t x;
    uint8_t w[8];
    uint32_t sp0 = CORE_SP(h), off;
    int r;

    if (!s) {
        return -1;
    }
    /* the module's data, then its stack, which starts at the top and grows
       down (CoreToolchain.cmake passes wasm-ld --no-stack-first) */
    if (sp0 <= CORE_OWN_BASE || sp0 > h->mem_size) {
        fprintf(stderr, "ggx: the Core's stack pointer 0x%08x is not above its own data at 0x%08x\n", sp0,
                CORE_OWN_BASE);
        free(s);
        return -1;
    }
    h->own_end = (uint32_t)ROUND_PAGE(sp0);
    memcpy(s->regions, defs, sizeof(defs));
    s->regions[CORE_REGION_OWN - 1].size = h->own_end - CORE_OWN_BASE;
    off = HEADER_SIZE;
    for (r = 0; r < CORE_REGION_COUNT; r++) {
        s->first[r] = s->pages;
        s->npages[r] = (uint32_t)(ROUND_PAGE(s->regions[r].size) / CORE_PAGE_SIZE);
        s->pages += s->npages[r];
    }
    s->hash_offset = off;
    off += (uint32_t)ROUND_PAGE((size_t)s->pages * 8);
    for (r = 0; r < CORE_REGION_COUNT; r++) {
        s->offset[r] = off;
        off += (uint32_t)ROUND_PAGE(s->regions[r].size);
    }
    s->size = off;
    s->page_epoch = calloc(s->pages, sizeof(*s->page_epoch));
    s->page_hash = calloc(s->pages, sizeof(*s->page_hash));
    s->stale = malloc(s->pages);
    if (!s->page_epoch || !s->page_hash || !s->stale) {
        free(s->page_epoch);
        free(s->page_hash);
        free(s->stale);
        free(s);
        return -1;
    }
    memset(s->stale, 1, s->pages);
    s->epoch = 1;
    s->instance = ++instances;

    /* The Core build: its own area as instantiated (string literals, port
       statics, an empty stack) and where its stack starts. Two builds that
       agree on these lay out the own area the same way. */
    XXH3_64bits_reset(&x);
    XXH3_64bits_update(&x, h->mem + CORE_OWN_BASE, h->own_end - CORE_OWN_BASE);
    put32(w, h->own_end);
    put32(w + 4, sp0);
    XXH3_64bits_update(&x, w, 8);
    s->build_id = XXH3_64bits_digest(&x);
    h->state = s;
#ifdef GGX_STATE_SCAN
    scan_host = h;
#endif
    return 0;
}

void core_state_free(CoreHost *h)
{
    CoreState *s = h->state;

    if (s) {
        free(s->page_epoch);
        free(s->page_hash);
        free(s->stale);
        free(s);
        h->state = NULL;
    }
}

size_t core_state_last_copy(const CoreHost *h)
{
    return h->state->last_copy;
}

uint64_t core_build_id(const CoreHost *h)
{
    return h->state->build_id;
}

const CoreRegion *core_regions(const CoreHost *h)
{
    return h->state->regions;
}

static uint32_t next_epoch(CoreState *s);

static void mark(CoreState *s, uint32_t p)
{
    s->page_epoch[p] = s->epoch;
    s->stale[p] = 1;
}

void core_state_collect(CoreHost *h)
{
    CoreState *s = h->state;
    int r;

    for (r = 0; r < MEM_REGIONS; r++) {
        uint8_t *m = ggx_core_dirty + (s->regions[r].base >> CORE_PAGE_SHIFT);
        uint32_t n = s->npages[r], i, j;

        for (i = 0; i < n; i += 8) {
            if (i + 8 <= n) {
                uint64_t w;

                memcpy(&w, m + i, 8);
                if (!w) {
                    continue;
                }
            }
            for (j = i; j < i + 8 && j < n; j++) {
                if (m[j]) {
                    m[j] = 0;
                    mark(s, s->first[r] + j);
                }
            }
        }
    }
}

/* PORT-006's replay rehashes only the pages that changed in a field. */
const uint32_t *core_state_page_epochs(const CoreHost *h, uint32_t id, uint32_t *epoch)
{
    const CoreState *s = h->state;

    *epoch = s->epoch;
    return id >= 1 && id <= CORE_REGION_COUNT ? s->page_epoch + s->first[id - 1] : NULL;
}

uint32_t core_state_next_epoch(CoreHost *h)
{
    return next_epoch(h->state);
}

void core_state_touch(CoreHost *h, uint32_t addr, uint32_t len)
{
    CoreState *s = h->state;
    uint64_t end = (uint64_t)addr + len;
    int r;

    if (!s || !len) {
        return;
    }
    for (r = 0; r < MEM_REGIONS; r++) {
        const CoreRegion *g = &s->regions[r];
        uint64_t lo = addr > g->base ? addr : g->base;
        uint64_t hi = end < (uint64_t)g->base + g->size ? end : (uint64_t)g->base + g->size;
        uint64_t p;

        for (p = (lo - g->base) >> CORE_PAGE_SHIFT; lo < hi && p <= (hi - 1 - g->base) >> CORE_PAGE_SHIFT; p++) {
            mark(s, s->first[r] + (uint32_t)p);
        }
    }
}

/* Brings every stale page hash up to date. */
static void refresh(CoreHost *h)
{
    CoreState *s = h->state;
    int r;

    host_pack(h);
    mark(s, s->first[MEM_REGIONS]);
    for (r = 0; r < CORE_REGION_COUNT; r++) {
        uint32_t i;

        for (i = 0; i < s->npages[r]; i++) {
            uint32_t p = s->first[r] + i;

            if (s->stale[p]) {
                s->page_hash[p] = XXH3_64bits(page_data(h, r, i), page_len(&s->regions[r], i));
                s->stale[p] = 0;
            }
        }
    }
}

/* STATE-HASH.md: a region hash over its page hashes, then the State Hash
   over region hashes. Shared by the Core and by snapshot checks. The page
   hashes are host-endian u64s; every supported host is little-endian. */
static uint64_t region_hash_of(const CoreRegion *g, const uint64_t *page_hashes, uint32_t n)
{
    XXH3_state_t x;
    uint8_t head[12];

    put32(head, g->id);
    put32(head + 4, g->base);
    put32(head + 8, g->size);
    XXH3_64bits_reset(&x);
    XXH3_64bits_update(&x, head, sizeof(head));
    XXH3_64bits_update(&x, page_hashes, (size_t)n * 8);
    return XXH3_64bits_digest(&x);
}

static uint64_t state_hash_of(const CoreState *s, const uint64_t *page_hashes, uint32_t mask)
{
    XXH3_state_t x;
    uint8_t b[12];
    uint32_t n = 0;
    int r;

    for (r = 0; r < CORE_REGION_COUNT; r++) {
        n += (mask >> r) & 1;
    }
    memcpy(b, "GGXH", 4);
    put32(b + 4, CORE_STATE_HASH_VERSION);
    put32(b + 8, n);
    XXH3_64bits_reset(&x);
    XXH3_64bits_update(&x, b, 12);
    for (r = 0; r < CORE_REGION_COUNT; r++) {
        if ((mask >> r) & 1) {
            put32(b, s->regions[r].id);
            put64(b + 4, region_hash_of(&s->regions[r], page_hashes + s->first[r], s->npages[r]));
            XXH3_64bits_update(&x, b, 12);
        }
    }
    return XXH3_64bits_digest(&x);
}

uint64_t core_hash_regions(CoreHost *h, const uint32_t *ids, int n)
{
    uint32_t mask = 0;
    int i;

    for (i = 0; i < n; i++) {
        if (ids[i] >= 1 && ids[i] <= CORE_REGION_COUNT) {
            mask |= 1u << (ids[i] - 1);
        }
    }
    core_state_collect(h);
    refresh(h);
    return state_hash_of(h->state, h->state->page_hash, mask);
}

uint64_t core_hash(CoreHost *h)
{
    core_state_collect(h);
    refresh(h);
    return state_hash_of(h->state, h->state->page_hash, (1u << CORE_REGION_COUNT) - 1);
}

uint64_t core_region_hash(CoreHost *h, uint32_t id)
{
    CoreState *s = h->state;

    if (id < 1 || id > CORE_REGION_COUNT) {
        return 0;
    }
    core_state_collect(h);
    refresh(h);
    return region_hash_of(&s->regions[id - 1], s->page_hash + s->first[id - 1], s->npages[id - 1]);
}

/* ---- snapshots ---- */

size_t core_state_size(const CoreHost *h)
{
    return h->state->size;
}

static void write_header(CoreHost *h, uint8_t *d, uint64_t hash)
{
    CoreState *s = h->state;
    int r;

    memset(d, 0, 80 + 16 * CORE_REGION_COUNT);
    memcpy(d, "GGXSTATE", 8);
    put32(d + 8, CORE_STATE_FORMAT_VERSION);
    put32(d + 12, CORE_STATE_HASH_VERSION);
    put32(d + 16, RELEASE);
    put32(d + 20, CORE_REGION_COUNT);
    put64(d + 24, s->build_id);
    memcpy(d + 32, h->elf_sha256, 32);
    put64(d + 64, hash);
    put32(d + 72, s->pages);
    put32(d + 76, s->hash_offset);
    for (r = 0; r < CORE_REGION_COUNT; r++) {
        uint8_t *t = d + 80 + 16 * r;

        put32(t, s->regions[r].id);
        put32(t + 4, s->regions[r].base);
        put32(t + 8, s->regions[r].size);
        put32(t + 12, s->offset[r]);
    }
}

CoreSnapshot *core_snapshot_new(const CoreHost *h)
{
    CoreSnapshot *s = calloc(1, sizeof(*s));

    if (!s) {
        return NULL;
    }
    s->size = h->state->size;
    s->data = malloc(s->size);
    if (!s->data) {
        free(s);
        return NULL;
    }
    /* touch every page now, so the first save does not wait on the OS */
    memset(s->data, 0, s->size);
    return s;
}

void core_snapshot_free(CoreSnapshot *s)
{
    if (s) {
        free(s->data);
        free(s);
    }
}

const uint8_t *core_snapshot_data(const CoreSnapshot *s)
{
    return s->data;
}

size_t core_snapshot_size(const CoreSnapshot *s)
{
    return s->size;
}

uint64_t core_snapshot_hash(const CoreSnapshot *s)
{
    return get64(s->data + 64);
}

int core_snapshot_set_data(CoreSnapshot *s, const void *data, size_t size)
{
    if (size != s->size || memcmp(data, "GGXSTATE", 8) != 0) {
        return -1;
    }
    memcpy(s->data, data, size);
    s->owner = 0;
    return 0;
}

/* The epoch a snapshot saved or loaded now is equal to the Core as of. */
static uint32_t next_epoch(CoreState *s)
{
    if (s->epoch == UINT32_MAX - 1) {
        /* start over: every snapshot of the old generation copies whole */
        memset(s->page_epoch, 0, (size_t)s->pages * sizeof(*s->page_epoch));
        s->epoch = 0;
        s->generation++;
    }
    return ++s->epoch;
}

int core_save(CoreHost *h, CoreSnapshot *snap)
{
    CoreState *s = h->state;
    uint64_t *table = (uint64_t *)(snap->data + s->hash_offset);
    int full = snap->owner != s->instance || snap->generation != s->generation;
    int r;

    if (snap->size != s->size) {
        return -1;
    }
    core_state_collect(h);
    refresh(h);
    s->last_copy = 0;
    for (r = 0; r < CORE_REGION_COUNT; r++) {
        uint32_t i = 0, j;

        /* each run of changed pages in one copy */
        while (i < s->npages[r]) {
            const uint32_t *e = s->page_epoch + s->first[r];

            if (!full && e[i] < snap->sync_epoch) {
                i++;
                continue;
            }
            for (j = i + 1; j < s->npages[r] && (full || e[j] >= snap->sync_epoch); j++) {
            }
            memcpy(snap->data + s->offset[r] + (size_t)i * CORE_PAGE_SIZE, page_data(h, r, i),
                   (size_t)(j - 1 - i) * CORE_PAGE_SIZE + page_len(&s->regions[r], j - 1));
            s->last_copy += (size_t)(j - 1 - i) * CORE_PAGE_SIZE + page_len(&s->regions[r], j - 1);
            memcpy(table + s->first[r] + i, s->page_hash + s->first[r] + i, (size_t)(j - i) * 8);
            i = j;
        }
    }
    write_header(h, snap->data, state_hash_of(s, s->page_hash, (1u << CORE_REGION_COUNT) - 1));
    snap->owner = s->instance;
    snap->generation = s->generation;
    snap->sync_epoch = next_epoch(s);
    return 0;
}

/* A snapshot this host did not save: its header must describe this Core
   exactly, and its pages must hash to the State Hash it records. Fills
   hashes with its page hashes. */
static int check_foreign(CoreHost *h, const CoreSnapshot *snap, uint64_t *hashes)
{
    CoreState *s = h->state;
    uint8_t want[80 + 16 * CORE_REGION_COUNT];
    int r;

    write_header(h, want, 0);
    if (memcmp(snap->data, want, 16) != 0 || memcmp(snap->data + 20, want + 20, 4) != 0) {
        return CORE_LOAD_FORMAT;
    }
    if (memcmp(snap->data + 16, want + 16, 4) != 0 || memcmp(snap->data + 32, want + 32, 32) != 0) {
        return CORE_LOAD_RELEASE;
    }
    if (memcmp(snap->data + 24, want + 24, 8) != 0 || memcmp(snap->data + 72, want + 72, sizeof(want) - 72) != 0) {
        return CORE_LOAD_BUILD;
    }
    for (r = 0; r < CORE_REGION_COUNT; r++) {
        uint32_t i;

        for (i = 0; i < s->npages[r]; i++) {
            const uint8_t *d = snap->data + s->offset[r] + (size_t)i * CORE_PAGE_SIZE;

            hashes[s->first[r] + i] = XXH3_64bits(d, page_len(&s->regions[r], i));
        }
    }
    if (state_hash_of(s, hashes, (1u << CORE_REGION_COUNT) - 1) != core_snapshot_hash(snap)) {
        return CORE_LOAD_DAMAGED;
    }
    return 0;
}

const char *core_load_error(int rc)
{
    switch (rc) {
    case 0:
        return "loaded";
    case CORE_LOAD_RELEASE:
        return "it is for the other release or another boot ELF";
    case CORE_LOAD_BUILD:
        return "it is from another Core build (a different Core compiler lays out the Core's own area differently)";
    case CORE_LOAD_DAMAGED:
        return "it is damaged: its pages do not hash to the State Hash it records";
    default:
        return "it is not a snapshot of this format and State Hash version";
    }
}

int core_load(CoreHost *h, CoreSnapshot *snap)
{
    CoreState *s = h->state;
    const uint64_t *table = (const uint64_t *)(snap->data + s->hash_offset);
    uint64_t *checked = NULL;
    int full = snap->owner != s->instance || snap->generation != s->generation;
    int r, rc;

    if (snap->size != s->size) {
        return CORE_LOAD_FORMAT;
    }
    if (full) {
        checked = malloc((size_t)s->pages * sizeof(*checked));
        if (!checked) {
            return CORE_LOAD_FORMAT;
        }
        if ((rc = check_foreign(h, snap, checked)) != 0) {
            free(checked);
            return rc;
        }
        /* later loads of this snapshot read its table, so it gets the
           hashes just computed from its pages */
        memcpy(snap->data + s->hash_offset, checked, (size_t)s->pages * sizeof(*checked));
        table = checked;
    }
    core_state_collect(h);
    s->last_copy = 0;
    for (r = 0; r < MEM_REGIONS; r++) {
        const CoreRegion *g = &s->regions[r];
        uint32_t *e = s->page_epoch + s->first[r];
        uint32_t i = 0, j, k;

        while (i < s->npages[r]) {
            if (!full && e[i] < snap->sync_epoch) {
                i++;
                continue;
            }
            for (j = i + 1; j < s->npages[r] && (full || e[j] >= snap->sync_epoch); j++) {
            }
            memcpy(h->mem + g->base + (size_t)i * CORE_PAGE_SIZE, snap->data + s->offset[r] + (size_t)i * CORE_PAGE_SIZE,
                   (size_t)(j - 1 - i) * CORE_PAGE_SIZE + page_len(g, j - 1));
            s->last_copy += (size_t)(j - 1 - i) * CORE_PAGE_SIZE + page_len(g, j - 1);
            memcpy(s->page_hash + s->first[r] + i, table + s->first[r] + i, (size_t)(j - i) * 8);
            memset(s->stale + s->first[r] + i, 0, j - i);
            for (k = i; k < j; k++) {
                e[k] = s->epoch;
            }
            i = j;
        }
    }
    host_unpack(h, snap->data + s->offset[MEM_REGIONS]);
    free(checked);
    snap->owner = s->instance;
    snap->generation = s->generation;
    snap->sync_epoch = next_epoch(s);
    return 0;
}

/* ---- the poison/scan test ---- */

static int in_regions(const CoreState *s, uint32_t page)
{
    int r;

    for (r = 0; r < MEM_REGIONS; r++) {
        uint32_t first = s->regions[r].base >> CORE_PAGE_SHIFT;

        if (page >= first && page < first + s->npages[r]) {
            return 1;
        }
    }
    return 0;
}

static int scan_map(CoreState *s, uint8_t *map, const char *what, const char *when)
{
    uint32_t p, run = 0;
    int found = 0;

    for (p = 0; p <= (1u << 20); p++) {
        int hit = p < (1u << 20) && map[p] && !in_regions(s, p);

        if (p < (1u << 20)) {
            map[p] = 0;
        }
        if (hit) {
            if (!run) {
                run = p + 1;
            }
            found++;
        } else if (run) {
            fprintf(stderr, "ggx: %s: the Core %s 0x%08x-0x%08x, outside every state region\n", when, what,
                    (run - 1) << CORE_PAGE_SHIFT, p << CORE_PAGE_SHIFT);
            run = 0;
        }
    }
    return found;
}

#ifdef GGX_STATE_SCAN
/* The first read of each page (tools/w2c_patch.py). One outside the regions
   is reported as it happens, with the native call stack that made it as
   offsets from w2c_ggx_GameField, which `nm -n` on the program turns into
   Core function names. */
void ggx_core_first_read(u32 addr)
{
    void *frames[8];
    int n, i;

    ggx_core_read[addr >> CORE_PAGE_SHIFT] = 1;
    if (scan_host && !in_regions(scan_host->state, addr >> CORE_PAGE_SHIFT)) {
        fprintf(stderr, "ggx: field %u: the Core read 0x%08x, outside every state region, from", scan_host->field, addr);
        n = backtrace(frames, 8);
        for (i = 1; i < n; i++) {
            fprintf(stderr, " GameField%+lld", (long long)((uintptr_t)frames[i] - (uintptr_t)&w2c_ggx_GameField));
        }
        fprintf(stderr, "\n");
    }
}
#endif

int core_state_scan(CoreHost *h, const char *when)
{
    core_state_collect(h);
    return scan_map(h->state, ggx_core_dirty, "wrote", when) + scan_map(h->state, ggx_core_read, "read", when);
}

#define POISON 0xa5u
#define POISON_SPAN 0x01000000u

/* The poisoned ranges: the gaps between the memory regions (and below the
   first and above the last), each cut to POISON_SPAN next to a region. */
static int poison_ranges(CoreHost *h, uint64_t lo[], uint64_t hi[])
{
    const CoreRegion *g = h->state->regions;
    uint64_t prev = 0;
    int order[MEM_REGIONS], n = 0, i, j;

    for (i = 0; i < MEM_REGIONS; i++) {
        order[i] = i;
    }
    for (i = 1; i < MEM_REGIONS; i++) {
        for (j = i; j > 0 && g[order[j]].base < g[order[j - 1]].base; j--) {
            int t = order[j];
            order[j] = order[j - 1];
            order[j - 1] = t;
        }
    }
    for (i = 0; i <= MEM_REGIONS; i++) {
        uint64_t a = prev, b = i < MEM_REGIONS ? g[order[i]].base : h->mem_size;

        if (i < MEM_REGIONS) {
            prev = (uint64_t)g[order[i]].base + g[order[i]].size;
        }
        if (b <= a) {
            continue;
        }
        if (b - a <= 2 * (uint64_t)POISON_SPAN) {
            lo[n] = a, hi[n++] = b;
        } else {
            if (i > 0) {
                lo[n] = a, hi[n++] = a + POISON_SPAN;
            }
            lo[n] = b - POISON_SPAN, hi[n++] = b;
        }
    }
    return n;
}

void core_state_poison(CoreHost *h)
{
    uint64_t lo[2 * MEM_REGIONS + 2], hi[2 * MEM_REGIONS + 2];
    int i, n = poison_ranges(h, lo, hi);

    for (i = 0; i < n; i++) {
        memset(h->mem + lo[i], POISON, (size_t)(hi[i] - lo[i]));
    }
}

uint64_t core_state_poison_check(CoreHost *h)
{
    uint64_t lo[2 * MEM_REGIONS + 2], hi[2 * MEM_REGIONS + 2], bad = 0, a;
    int i, n = poison_ranges(h, lo, hi);

    for (i = 0; i < n; i++) {
        for (a = lo[i]; a < hi[i]; a++) {
            if (h->mem[a] != POISON) {
                if (!bad) {
                    fprintf(stderr, "ggx: poison changed at 0x%08llx\n", (unsigned long long)a);
                }
                bad++;
            }
        }
    }
    return bad;
}
