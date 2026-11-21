/* Lockstep Trace reader and replay (trace.h). */
#include "trace.h"

#include <stdlib.h>
#include <string.h>

#include "core_host.h"
#include "core_state.h"
#define XXH_INLINE_ALL
#include "xxhash.h"
#include "zstd.h"

#define RAM_PAGES (EE_RAM_SIZE / 4096u)
#define PAGE 4096u

static const char *const ENV_NAMES[TRACE_ENV_MAX] = {
#define TRACE_ENV_NAME(id, name) [id] = #name,
    TRACE_ENV_LIST(TRACE_ENV_NAME)
#undef TRACE_ENV_NAME
};

static const char *env_name(unsigned id)
{
    return id < TRACE_ENV_MAX && ENV_NAMES[id] ? ENV_NAMES[id] : "?";
}

typedef TraceCall Call;   /* outs: nout x (u8 length, bytes) */

struct Trace {
    uint8_t *data;
    size_t size;
    TraceHeader hdr;
    TraceRange *masks;
    char *note;
    uint8_t nspec[256];
    uint8_t spec[256][8];      /* per output, the argument it is written through */
    uint8_t spec_src[256][8];  /* nonzero: that argument of this env's last call */
    uint32_t last_args[256][8];
    size_t *field_off;
    uint32_t nfields;

    /* replay */
    CoreHost *h;
    uint32_t *page_mask_first; /* per page, its first entry in page_masks; TRACE_PAGES + 1 entries */
    TraceRange *page_masks;    /* page-relative */
    uint32_t ps2_pages[TRACE_PAGES];   /* the low 32 bits of each page hash, as the trace stores them */
    uint64_t core_pages[TRACE_PAGES];
    uint32_t zero_page;
    uint32_t field;
    Call *calls;
    uint32_t ncalls, calls_cap;
    uint32_t ci, crep, call_no;
    TraceRange *transient;
    uint32_t ntransient, transient_cap;
    TraceRange *prev;          /* the previous field's transient masks */
    uint32_t nprev, prev_cap;
    int hashed;                /* core_pages holds every page */
    uint64_t state, ram, spr;
    uint64_t core_state, core_ram, core_spr;
    int failed;
    char why[1024];
    /* the survey (trace_keep_going) */
    int keep_going;
    uint32_t mismatched_fields;
    uint32_t *diff_first, *diff_last, *diff_count;
};

/* ------------------------------------------------------------------ reading */

typedef struct {
    const uint8_t *p, *end;
    int bad;
} Rd;

static uint8_t rd8(Rd *r)
{
    if (r->p >= r->end) {
        r->bad = 1;
        return 0;
    }
    return *r->p++;
}

static uint32_t rd32(Rd *r)
{
    uint32_t v = 0;
    int i;

    for (i = 0; i < 4; i++) {
        v |= (uint32_t)rd8(r) << (8 * i);
    }
    return v;
}

static uint64_t rd64(Rd *r)
{
    uint64_t lo = rd32(r);
    return lo | ((uint64_t)rd32(r) << 32);
}

static uint64_t rdvar(Rd *r)
{
    uint64_t v = 0;
    int shift = 0;
    uint8_t b;

    do {
        b = rd8(r);
        if (shift < 64) {
            v |= (uint64_t)(b & 0x7f) << shift;
        }
        shift += 7;
    } while ((b & 0x80) && !r->bad);
    return v;
}

static const uint8_t *rdskip(Rd *r, size_t n)
{
    const uint8_t *p = r->p;

    if ((size_t)(r->end - r->p) < n) {
        r->bad = 1;
        r->p = r->end;
        return p;
    }
    r->p += n;
    return p;
}

/* Reads a field record at r. With t's replay buffers given, keeps its calls
   and transient masks and applies its page hashes to ps2_pages. */
static int read_field(Trace *t, Rd *r, int keep)
{
    uint32_t n, i, k, p;
    int64_t prev = -1;

    if (rd8(r) != 'F') {
        return -1;
    }
    if (keep) {
        t->field = (uint32_t)rdvar(r);
    } else {
        (void)rdvar(r);
    }
    (void)rd8(r);                         /* PS2 vblanks */
    n = (uint32_t)rdvar(r);
    if (keep) {
        if (n > t->calls_cap) {
            t->calls_cap = n + 64;
            t->calls = realloc(t->calls, t->calls_cap * sizeof(Call));
        }
        t->ncalls = n;
    }
    for (i = 0; i < n && !r->bad; i++) {
        Call c;
        uint8_t id = rd8(r);

        c.env = id & 0x7f;
        c.rep = (id & 0x80) ? (uint32_t)rdvar(r) : 1;
        c.ret = (uint32_t)rdvar(r);
        c.nout = rd8(r);
        c.outs = r->p;
        for (k = 0; k < c.nout; k++) {
            rdskip(r, rd8(r));
        }
        if (keep) {
            t->calls[i] = c;
        }
    }
    /* transient masks: count ranges of size bytes, stride apart; kept as plain ranges */
    n = (uint32_t)rdvar(r);
    if (keep) {
        t->ntransient = 0;
    }
    for (i = 0; i < n && !r->bad; i++) {
        uint32_t base = (uint32_t)rdvar(r), size = (uint32_t)rdvar(r), stride = (uint32_t)rdvar(r);
        uint32_t count = (uint32_t)rdvar(r), k;

        if (!keep || count > 0x10000) {
            continue;
        }
        if (t->ntransient + count > t->transient_cap) {
            t->transient_cap = t->ntransient + count + 8;
            t->transient = realloc(t->transient, t->transient_cap * sizeof(TraceRange));
        }
        for (k = 0; k < count; k++) {
            t->transient[t->ntransient].base = base + k * stride;
            t->transient[t->ntransient].size = size;
            t->ntransient++;
        }
    }
    {
        uint64_t s = rd64(r), ram = rd64(r), spr = rd64(r);
        if (keep) {
            t->state = s;
            t->ram = ram;
            t->spr = spr;
        }
    }
    n = (uint32_t)rdvar(r);
    for (i = 0; i < n && !r->bad; i++) {
        uint32_t hsh;

        p = (uint32_t)(prev + 1 + (int64_t)rdvar(r));
        hsh = rd32(r);
        if (p >= TRACE_PAGES) {
            r->bad = 1;
            break;
        }
        if (keep) {
            t->ps2_pages[p] = hsh;
        }
        prev = p;
    }
    return r->bad ? -1 : 0;
}

/* A trace file may be zstd-compressed whole (the corpus is: .ggxt.zst). Replaces t->data with the
   decompressed bytes; 0 on success. */
static int unzstd(Trace *t)
{
    ZSTD_DStream *z = ZSTD_createDStream();
    ZSTD_inBuffer in = { t->data, t->size, 0 };
    size_t cap = t->size * 4 + (1u << 20), ret = 1;
    uint8_t *out = malloc(cap);
    ZSTD_outBuffer o = { out, cap, 0 };

    ZSTD_initDStream(z);
    while (out && in.pos < in.size && ret != 0) {
        if (o.pos == o.size) {
            o.size *= 2;
            out = realloc(out, o.size);
            o.dst = out;
        }
        ret = ZSTD_decompressStream(z, &o, &in);
        if (ZSTD_isError(ret)) {
            break;
        }
    }
    ZSTD_freeDStream(z);
    if (!out || ret != 0) {
        free(out);
        return -1;
    }
    free(t->data);
    t->data = out;
    t->size = o.pos;
    return 0;
}

Trace *trace_open(const char *path, char *err, size_t cap)
{
    FILE *f = fopen(path, "rb");
    Trace *t;
    Rd r;
    long size;
    uint32_t i, n, header_size;

    if (!f) {
        snprintf(err, cap, "cannot read %s", path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    t = calloc(1, sizeof(*t));
    t->data = malloc((size_t)size + 1);
    t->size = (size_t)size;
    if (size < 8 || fread(t->data, 1, (size_t)size, f) != (size_t)size) {
        size = 0;
    }
    fclose(f);
    if (size >= 4 && t->data[0] == 0x28 && t->data[1] == 0xb5 && t->data[2] == 0x2f && t->data[3] == 0xfd &&
        unzstd(t) != 0) {
        snprintf(err, cap, "%s: damaged zstd stream", path);
        trace_free(t);
        return NULL;
    }
    if (t->size < 208 || memcmp(t->data, "GGXTRACE", 8) != 0) {
        snprintf(err, cap, "%s is not a Lockstep Trace", path);
        trace_free(t);
        return NULL;
    }
    r.p = t->data + 8;
    r.end = t->data + t->size;
    r.bad = 0;
    t->hdr.format_version = rd32(&r);
    t->hdr.hash_version = rd32(&r);
    t->hdr.release = rd32(&r);
    header_size = rd32(&r);
    if (t->hdr.format_version != TRACE_FORMAT_VERSION || t->hdr.hash_version != 1) {
        snprintf(err, cap, "%s is trace format %u, State Hash %u; this build reads format %u, State Hash 1", path,
                 t->hdr.format_version, t->hdr.hash_version, TRACE_FORMAT_VERSION);
        trace_free(t);
        return NULL;
    }
    memcpy(t->hdr.elf_sha256, rdskip(&r, 32), 32);
    t->hdr.map_hash = rd64(&r);
    t->hdr.nmasks = rd32(&r);
    (void)rd32(&r);   /* flags */
    memcpy(t->hdr.recorder, rdskip(&r, 64), 64);
    memcpy(t->hdr.settings, rdskip(&r, 64), 64);
    t->masks = calloc(t->hdr.nmasks + 1, sizeof(TraceRange));
    for (i = 0; i < t->hdr.nmasks && !r.bad; i++) {
        t->masks[i].base = rd32(&r);
        t->masks[i].size = rd32(&r);
    }
    t->hdr.masks = t->masks;
    n = rd32(&r);
    for (i = 0; i < n && !r.bad; i++) {
        uint8_t id = rd8(&r), k, m = rd8(&r);
        t->nspec[id] = m < 8 ? m : 8;
        for (k = 0; k < m; k++) {
            uint8_t a = rd8(&r), src = (a & 0x80) ? rd8(&r) : 0;
            if (k < 8) {
                t->spec[id][k] = a & 7;
                t->spec_src[id][k] = src;
            }
        }
    }
    n = rd32(&r);
    t->note = calloc(1, (size_t)n + 1);
    memcpy(t->note, rdskip(&r, n), r.bad ? 0 : n);
    t->hdr.note = t->note;
    if (r.bad || header_size > t->size) {
        snprintf(err, cap, "%s: damaged header", path);
        trace_free(t);
        return NULL;
    }
    /* index the field records */
    r.p = t->data + header_size;
    {
        uint32_t cap_f = 1024;
        t->field_off = malloc(cap_f * sizeof(size_t));
        while (r.p < r.end && *r.p == 'F') {
            if (t->nfields == cap_f) {
                cap_f *= 2;
                t->field_off = realloc(t->field_off, cap_f * sizeof(size_t));
            }
            t->field_off[t->nfields] = (size_t)(r.p - t->data);
            if (read_field(t, &r, 0) != 0) {
                break;
            }
            t->nfields++;
        }
    }
    if (r.p >= r.end || *r.p != 'E') {
        snprintf(err, cap, "%s: no end record after %u fields (the recording was cut short)", path, t->nfields);
        trace_free(t);
        return NULL;
    }
    return t;
}

void trace_free(Trace *t)
{
    if (!t) {
        return;
    }
    free(t->data);
    free(t->masks);
    free(t->note);
    free(t->field_off);
    free(t->calls);
    free(t->transient);
    free(t->prev);
    free(t->page_masks);
    free(t->page_mask_first);
    free(t->diff_first);
    free(t->diff_last);
    free(t->diff_count);
    free(t);
}

const TraceHeader *trace_header(const Trace *t)
{
    return &t->hdr;
}

uint32_t trace_fields(const Trace *t)
{
    return t->nfields;
}

/* ------------------------------------------------------------------ hashing, as the recorder does */

static uint32_t page_base(uint32_t p)
{
    return p < RAM_PAGES ? p * PAGE : EE_SPR_BASE + (p - RAM_PAGES) * PAGE;
}

static void put32(uint8_t *b, uint32_t v)
{
    b[0] = (uint8_t)v;
    b[1] = (uint8_t)(v >> 8);
    b[2] = (uint8_t)(v >> 16);
    b[3] = (uint8_t)(v >> 24);
}

static void put64(uint8_t *b, uint64_t v)
{
    put32(b, (uint32_t)v);
    put32(b + 4, (uint32_t)(v >> 32));
}

/* page-relative overlap of r with page p */
static int overlap(const TraceRange *r, uint32_t p, TraceRange *out)
{
    uint64_t b = page_base(p), lo = r->base > b ? r->base : b;
    uint64_t hi = (uint64_t)r->base + r->size, pe = b + PAGE;

    if (hi > pe) {
        hi = pe;
    }
    if (lo >= hi) {
        return 0;
    }
    out->base = (uint32_t)(lo - b);
    out->size = (uint32_t)(hi - lo);
    return 1;
}

static void masked_page(Trace *t, uint32_t p, uint8_t *buf)
{
    uint32_t i;
    TraceRange o;

    memcpy(buf, t->h->mem + page_base(p), PAGE);
    for (i = t->page_mask_first[p]; i < t->page_mask_first[p + 1]; i++) {
        memset(buf + t->page_masks[i].base, 0, t->page_masks[i].size);
    }
    for (i = 0; i < t->ntransient; i++) {
        if (overlap(&t->transient[i], p, &o)) {
            memset(buf + o.base, 0, o.size);
        }
    }
}

static int page_has_transient(const Trace *t, uint32_t p)
{
    uint32_t i;
    TraceRange o;

    for (i = 0; i < t->ntransient; i++) {
        if (overlap(&t->transient[i], p, &o)) {
            return 1;
        }
    }
    return 0;
}

static uint64_t region_hash(uint32_t id, uint32_t base, uint32_t size, const uint64_t *pages, uint32_t n)
{
    XXH3_state_t x;
    uint8_t head[12];

    put32(head, id);
    put32(head + 4, base);
    put32(head + 8, size);
    XXH3_64bits_reset(&x);
    XXH3_64bits_update(&x, head, sizeof(head));
    XXH3_64bits_update(&x, pages, (size_t)n * 8);
    return XXH3_64bits_digest(&x);
}

static void hash_page(Trace *t, uint32_t p)
{
    static uint8_t buf[PAGE];

    if (t->page_mask_first[p] == t->page_mask_first[p + 1] && !page_has_transient(t, p)) {
        t->core_pages[p] = XXH3_64bits(t->h->mem + page_base(p), PAGE);
    } else {
        masked_page(t, p, buf);
        t->core_pages[p] = XXH3_64bits(buf, PAGE);
    }
}

/* The Core's page hashes, as the recorder computes the PS2's. After the
   first field only the pages the Core changed are rehashed (core_state's
   page epochs), and those a transient mask covers now or did last field. */
static void core_hashes(Trace *t)
{
    uint8_t b[40];
    uint32_t p, i, epoch;
    const uint32_t *ram = core_state_page_epochs(t->h, 1, &epoch);
    const uint32_t *spr = core_state_page_epochs(t->h, 2, &epoch);
    TraceRange o;

    for (p = 0; p < TRACE_PAGES; p++) {
        if (!t->hashed || (p < RAM_PAGES ? ram[p] : spr[p - RAM_PAGES]) == epoch) {
            hash_page(t, p);
        }
    }
    if (t->hashed) {
        for (i = 0; i < t->nprev; i++) {
            for (p = t->prev[i].base >> 12; p < TRACE_PAGES; p++) {
                if (!overlap(&t->prev[i], p, &o)) {
                    break;
                }
                hash_page(t, p);
            }
        }
        for (i = 0; i < t->ntransient; i++) {
            for (p = t->transient[i].base >> 12; p < TRACE_PAGES; p++) {
                if (!overlap(&t->transient[i], p, &o)) {
                    break;
                }
                hash_page(t, p);
            }
        }
    }
    t->hashed = 1;
    core_state_next_epoch(t->h);
    if (t->ntransient > t->prev_cap) {
        t->prev_cap = t->ntransient;
        t->prev = realloc(t->prev, t->prev_cap * sizeof(TraceRange));
    }
    if (t->ntransient) {
        memcpy(t->prev, t->transient, t->ntransient * sizeof(TraceRange));
    }
    t->nprev = t->ntransient;
    t->core_ram = region_hash(1, 0, EE_RAM_SIZE, t->core_pages, RAM_PAGES);
    t->core_spr = region_hash(2, EE_SPR_BASE, EE_SPR_SIZE, t->core_pages + RAM_PAGES, TRACE_PAGES - RAM_PAGES);
    memcpy(b, "GGXH", 4);
    put32(b + 4, 1);
    put32(b + 8, 2);
    put32(b + 12, 1);
    put64(b + 16, t->core_ram);
    put32(b + 24, 2);
    put64(b + 28, t->core_spr);
    t->core_state = XXH3_64bits(b, 36);
}

/* ------------------------------------------------------------------ replay */

void trace_begin(Trace *t, CoreHost *h)
{
    static const uint8_t zero[PAGE];
    uint32_t p, i, n = 0, cap = 64;
    TraceRange o;

    t->h = h;
    h->replay = t;
    t->zero_page = (uint32_t)XXH3_64bits(zero, PAGE);
    for (p = 0; p < TRACE_PAGES; p++) {
        t->ps2_pages[p] = t->zero_page;
    }
    t->page_mask_first = calloc(TRACE_PAGES + 1, sizeof(uint32_t));
    t->page_masks = malloc(cap * sizeof(TraceRange));
    for (p = 0; p < TRACE_PAGES; p++) {
        t->page_mask_first[p] = n;
        for (i = 0; i < t->hdr.nmasks; i++) {
            if (overlap(&t->masks[i], p, &o)) {
                if (n == cap) {
                    cap *= 2;
                    t->page_masks = realloc(t->page_masks, cap * sizeof(TraceRange));
                }
                t->page_masks[n++] = o;
            }
        }
    }
    t->page_mask_first[TRACE_PAGES] = n;
    t->field = 0;
    t->failed = 0;
}

static void diverge(Trace *t, const char *fmt, const char *a, const char *b)
{
    if (t->failed) {
        return;
    }
    t->failed = 1;
    snprintf(t->why, sizeof(t->why), fmt, a, b);
}

void trace_field_begin(Trace *t, uint32_t field)
{
    Rd r;

    if (field >= t->nfields) {
        return;
    }
    r.p = t->data + t->field_off[field];
    r.end = t->data + t->size;
    r.bad = 0;
    read_field(t, &r, 1);
    t->ci = t->crep = t->call_no = 0;
}

uint32_t trace_field_calls(Trace *t, uint32_t field, TraceCall *out, uint32_t cap)
{
    uint32_t i;

    if (field >= t->nfields) {
        return 0;
    }
    trace_field_begin(t, field);
    for (i = 0; i < t->ncalls && i < cap; i++) {
        out[i] = t->calls[i];
    }
    return t->ncalls;
}

uint32_t trace_field_vblanks(const Trace *t, uint32_t field)
{
    Rd r;

    if (field >= t->nfields) {
        return 0;
    }
    r.p = t->data + t->field_off[field];
    r.end = t->data + t->size;
    r.bad = 0;
    (void)rd8(&r);      /* 'F' */
    (void)rdvar(&r);    /* the field number */
    return rd8(&r);
}

static int ee_range_ok(const CoreHost *h, uint32_t addr, uint32_t len)
{
    uint64_t end = (uint64_t)addr + len;

    return end <= EE_RAM_SIZE || (addr >= EE_SPR_BASE && end <= EE_SPR_BASE + EE_SPR_SIZE) ||
           (addr >= CORE_OWN_BASE && end <= h->own_end);
}

uint32_t trace_answer(CoreHost *h, int env, uint32_t value, const uint32_t *args, int nargs)
{
    Trace *t = h->replay;
    const Call *c;
    const uint8_t *o;
    uint32_t k;
    char at[64];

    if (t->failed) {
        return value;
    }
    t->call_no++;
    snprintf(at, sizeof(at), "%u", t->call_no);
    if (t->ci >= t->ncalls) {
        diverge(t, "call %s: the port called %s; the PS2 made no more calls in this field", at, env_name((unsigned)env));
        return value;
    }
    c = &t->calls[t->ci];
    if (c->env != env) {
        char what[128];
        snprintf(what, sizeof(what), "%s, where the PS2 called %s", env_name((unsigned)env), env_name(c->env));
        diverge(t, "call %s: the port called %s", at, what);
        return value;
    }
    for (k = 0; k < (uint32_t)nargs && k < 8; k++) {
        t->last_args[env][k] = args[k];
    }
    o = c->outs;
    for (k = 0; k < c->nout; k++) {
        uint8_t len = *o++;
        uint8_t arg = k < t->nspec[env] ? t->spec[env][k] : 0xff;
        uint8_t src = k < t->nspec[env] ? t->spec_src[env][k] : 0;

        if (len && (src || arg < nargs)) {
            uint32_t addr = src ? t->last_args[src][arg] : args[arg];
            if (ee_range_ok(h, addr, len)) {
                memcpy(ee_ptr(h, addr, len), o, len);
            } else {
                char what[96];
                snprintf(what, sizeof(what), "%s's output to 0x%08x", env_name((unsigned)env), addr);
                diverge(t, "call %s: %s is outside the Core's memory", at, what);
            }
        }
        o += len;
    }
    if (++t->crep >= c->rep) {
        t->ci++;
        t->crep = 0;
    }
    return c->ret;
}

int trace_field_end(Trace *t)
{
    if (!t->failed && t->ci < t->ncalls) {
        char at[64];
        snprintf(at, sizeof(at), "%u", t->call_no + 1);
        diverge(t, "call %s: the PS2 called %s; the port made no more calls in this field", at,
                env_name(t->calls[t->ci].env));
    }
    if (t->failed) {
        return 1;
    }
    core_hashes(t);
    if (t->core_state != t->state && t->keep_going) {
        uint32_t p;
        for (p = 0; p < TRACE_PAGES; p++) {
            if ((uint32_t)t->core_pages[p] != t->ps2_pages[p]) {
                if (!t->diff_count[p]++) {
                    t->diff_first[p] = t->field;
                }
                t->diff_last[p] = t->field;
            }
        }
        t->mismatched_fields++;
    }
    if (t->core_state != t->state) {
        t->failed = 2;
        snprintf(t->why, sizeof(t->why), "State Hash %016llx, the PS2's %016llx (%s%s%s)",
                 (unsigned long long)t->core_state, (unsigned long long)t->state, t->core_ram != t->ram ? "ram" : "",
                 t->core_ram != t->ram && t->core_spr != t->spr ? ", " : "", t->core_spr != t->spr ? "spr" : "");
        return 1;
    }
    return 0;
}

void trace_keep_going(Trace *t)
{
    t->keep_going = 1;
    t->diff_first = calloc(TRACE_PAGES, sizeof(uint32_t));
    t->diff_last = calloc(TRACE_PAGES, sizeof(uint32_t));
    t->diff_count = calloc(TRACE_PAGES, sizeof(uint32_t));
}

int trace_resume(Trace *t)
{
    if (t->failed != 2 || !t->keep_going) {
        return -1;
    }
    t->failed = 0;
    return 0;
}

/* ------------------------------------------------------------------ the report */

typedef struct {
    uint32_t *addr;
    char **name;
    uint32_t n;
} Syms;

static void syms_load(Syms *s, const char *path)
{
    FILE *f = path ? fopen(path, "r") : NULL;
    char line[512], name[256], addr[32], kind[8], bind[8];
    uint32_t cap = 0;

    memset(s, 0, sizeof(*s));
    if (!f) {
        return;
    }
    while (fgets(line, sizeof(line), f)) {
        size_t l;
        if (line[0] == '#' || sscanf(line, "%255s %31s %7s %7s", name, addr, kind, bind) != 4 || addr[0] == '-') {
            continue;
        }
        l = strlen(name);
        if (l > 6 && (!strcmp(name + l - 6, "_START") || !strcmp(name + l - 4, "_END") ||
                      !strcmp(name + l - 5, "_SIZE") || !strcmp(name + l - 5, "_VRAM"))) {
            continue;
        }
        if (s->n == cap) {
            cap = cap ? cap * 2 : 4096;
            s->addr = realloc(s->addr, cap * sizeof(uint32_t));
            s->name = realloc(s->name, cap * sizeof(char *));
        }
        s->addr[s->n] = (uint32_t)strtoul(addr, NULL, 16);
        s->name[s->n] = malloc(l + 1);
        memcpy(s->name[s->n], name, l + 1);
        s->n++;
    }
    fclose(f);
    /* the maps are sorted by address */
}

static const char *syms_at(const Syms *s, uint32_t a, char *buf, size_t cap)
{
    uint32_t lo = 0, hi = s->n;

    if (s->n && a >= EE_SPR_BASE) {     /* the symbol maps name no scratchpad addresses */
        snprintf(buf, cap, "spr+0x%x", a - EE_SPR_BASE);
        return buf;
    }
    if (!s->n) {
        snprintf(buf, cap, "%08x", a);
        return buf;
    }
    while (lo < hi) {
        uint32_t mid = (lo + hi) / 2;
        if (s->addr[mid] <= a) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    if (lo == 0) {
        snprintf(buf, cap, "%08x", a);
    } else {
        snprintf(buf, cap, "%s+0x%x", s->name[lo - 1], a - s->addr[lo - 1]);
    }
    return buf;
}

static void syms_free(Syms *s)
{
    uint32_t i;

    for (i = 0; i < s->n; i++) {
        free(s->name[i]);
    }
    free(s->name);
    free(s->addr);
}

void trace_report(Trace *t, FILE *out, const char *dump_dir, const char *symbols, const char *ps2_dump)
{
    static uint8_t core[PAGE], ps2[PAGE];
    FILE *pd = ps2_dump ? fopen(ps2_dump, "rb") : NULL;
    Syms s;
    uint32_t p, i, shown = 0, ndiff = 0;
    char nb[300];

    fprintf(out, "field %u: %s\n", t->field, t->why);
    if (t->failed == 1) {
        fprintf(out, "  the PS2's calls in field %u:\n", t->field);
        for (i = 0; i < t->ncalls; i++) {
            fprintf(out, "    %s%u %s = %d%s", i == t->ci ? "> " : "  ", i + 1, env_name(t->calls[i].env),
                    (int)t->calls[i].ret, t->calls[i].rep > 1 ? "" : "\n");
            if (t->calls[i].rep > 1) {
                fprintf(out, " x%u\n", t->calls[i].rep);
            }
        }
        return;
    }
    if (ps2_dump && !pd) {
        fprintf(out, "  (cannot read %s)\n", ps2_dump);
    }
    syms_load(&s, symbols);
    for (p = 0; p < TRACE_PAGES; p++) {
        ndiff += (uint32_t)t->core_pages[p] != t->ps2_pages[p];
    }
    fprintf(out, "  %u pages differ:\n", ndiff);
    for (p = 0; p < TRACE_PAGES; p++) {
        uint32_t b = page_base(p);

        if ((uint32_t)t->core_pages[p] == t->ps2_pages[p]) {
            continue;
        }
        fprintf(out, "  %s %08x  %s  port %08x  PS2 %08x\n", p < RAM_PAGES ? "ram" : "spr", b,
                syms_at(&s, b, nb, sizeof(nb)), (uint32_t)t->core_pages[p], t->ps2_pages[p]);
        masked_page(t, p, core);
        if (dump_dir) {
            char path[1024];
            FILE *f;
            snprintf(path, sizeof(path), "%s/f%u-%08x.bin", dump_dir, t->field, b);
            if ((f = fopen(path, "wb"))) {
                fwrite(core, 1, PAGE, f);
                fclose(f);
            }
        }
        if (pd && fseek(pd, (long)p * (long)PAGE, SEEK_SET) == 0 && fread(ps2, 1, PAGE, pd) == PAGE) {
            uint32_t o = 0, runs = 0;
            while (o < PAGE && runs < 8) {
                uint32_t e;
                if (core[o] == ps2[o]) {
                    o++;
                    continue;
                }
                for (e = o; e < PAGE && (core[e] != ps2[e] || (e + 1 < PAGE && core[e + 1] != ps2[e + 1])); e++) {
                }
                fprintf(out, "      %08x +%u  %s  port", b + o, e - o, syms_at(&s, b + o, nb, sizeof(nb)));
                for (i = o; i < e && i < o + 16; i++) {
                    fprintf(out, " %02x", core[i]);
                }
                fprintf(out, "%s  PS2", e - o > 16 ? " ..." : "");
                for (i = o; i < e && i < o + 16; i++) {
                    fprintf(out, " %02x", ps2[i]);
                }
                fprintf(out, "%s\n", e - o > 16 ? " ..." : "");
                o = e;
                runs++;
            }
        }
        if (++shown == 64 && ndiff > 64) {
            fprintf(out, "  ... and %u more\n", ndiff - 64);
            break;
        }
    }
    if (pd) {
        fclose(pd);
    }
    syms_free(&s);
}

void trace_survey(Trace *t, FILE *out, const char *symbols)
{
    Syms s;
    uint32_t p, n = 0;
    char nb[300];

    if (!t->keep_going) {
        return;
    }
    syms_load(&s, symbols);
    for (p = 0; p < TRACE_PAGES; p++) {
        n += t->diff_count[p] != 0;
    }
    fprintf(out, "survey: %u of %u fields mismatched; %u pages differed at some field:\n", t->mismatched_fields,
            t->field + 1, n);
    for (p = 0; p < TRACE_PAGES; p++) {
        if (t->diff_count[p]) {
            fprintf(out, "  %08x  %-40s  fields %u-%u, %u times\n", page_base(p), syms_at(&s, page_base(p), nb, sizeof(nb)),
                    t->diff_first[p], t->diff_last[p], t->diff_count[p]);
        }
    }
    syms_free(&s);
}
