#include "core_host.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "fsys.h"
#include "mc_card.h"
#include "sha256.h"
#include "wasm-rt-exceptions.h"
#include "wasm-rt-impl.h"

#if defined(VERSION_KR)
#define RELEASE "KR"
#define BOOT_ELF "SLPM_645.25"
static const char ELF_SHA256[] = "ef3278dcf50feee1da1ac423042db6a3eeeaf845a3644a1d625bb97d0024755f";
#elif defined(VERSION_JP)
#define RELEASE "JP"
#define BOOT_ELF "SLPS_201.37"
static const char ELF_SHA256[] = "8bd352acb95a533abd22f11c7f9da4a3157dc8836fc7d91ae8340bde92d8a411";
#else
#error "build with VERSION_KR or VERSION_JP"
#endif

static uint8_t *ee_at(CoreHost *h, uint32_t addr, uint32_t len)
{
    uint64_t end = (uint64_t)addr + len;

    if ((end <= EE_RAM_SIZE) || (addr >= EE_SPR_BASE && end <= EE_SPR_BASE + EE_SPR_SIZE) ||
        (addr >= CORE_OWN_BASE && end <= h->own_end)) {
        return h->mem + addr;
    }
    fprintf(stderr, "ggx: field %u: EE access out of range: 0x%08x+%u\n", h->field, addr, len);
    if (h->log) {
        fprintf(h->log, "f%06u FATAL EE access out of range: 0x%08x+%u\n", h->field, addr, len);
        fflush(h->log);
    }
    exit(3);
}

uint8_t *ee_ptr(CoreHost *h, uint32_t addr, uint32_t len)
{
    uint8_t *p = ee_at(h, addr, len);

    core_state_touch(h, addr, len);
    return p;
}

const uint8_t *ee_rptr(CoreHost *h, uint32_t addr, uint32_t len)
{
    return ee_at(h, addr, len);
}

/* The Core and every supported host are little-endian, like the EE. */
uint32_t ee_r32(CoreHost *h, uint32_t addr)
{
    uint32_t v;
    memcpy(&v, ee_at(h, addr, 4), 4);
    return v;
}

uint16_t ee_r16(CoreHost *h, uint32_t addr)
{
    uint16_t v;
    memcpy(&v, ee_at(h, addr, 2), 2);
    return v;
}

void ee_w32(CoreHost *h, uint32_t addr, uint32_t v)
{
    memcpy(ee_ptr(h, addr, 4), &v, 4);
}

void ee_w64(CoreHost *h, uint32_t addr, uint64_t v)
{
    memcpy(ee_ptr(h, addr, 8), &v, 8);
}

const char *ee_str(CoreHost *h, uint32_t addr, char *buf, size_t cap)
{
    size_t i;

    for (i = 0; i + 1 < cap; i++) {
        buf[i] = (char)*ee_at(h, addr + (uint32_t)i, 1);
        if (!buf[i]) {
            return buf;
        }
    }
    buf[i] = 0;
    return buf;
}

void shim_log_flush(CoreHost *h)
{
    if (h->log && h->log_repeats) {
        fprintf(h->log, "  x%u\n", h->log_repeats + 1);
    }
    h->log_repeats = 0;
}

void shim_log(CoreHost *h, const char *fmt, ...)
{
    char line[sizeof(h->log_last)];
    va_list ap;

    if (!h->log) {
        return;
    }
    va_start(ap, fmt);
    vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    if (!strcmp(line, h->log_last)) {
        h->log_repeats++;
        return;
    }
    shim_log_flush(h);
    fprintf(h->log, "f%06u %s\n", h->field, line);
    memcpy(h->log_last, line, sizeof(line));
}

const char *log_addr(uint32_t addr, char buf[16])
{
    if (addr >= CORE_OWN_BASE) {
        return "core";
    }
    snprintf(buf, 16, "0x%08x", addr);
    return buf;
}

const char *log_str(CoreHost *h, uint32_t addr, char *buf, size_t cap)
{
    char raw[256];
    size_t i, o = 0;

    if (addr == 0) {
        return "NULL";
    }
    ee_str(h, addr, raw, sizeof(raw));
    buf[o++] = '"';
    for (i = 0; raw[i] && o + 6 < cap; i++) {
        unsigned char c = (unsigned char)raw[i];
        if (c >= 0x20 && c < 0x7f && c != '"' && c != '\\') {
            buf[o++] = (char)c;
        } else {
            o += (size_t)snprintf(buf + o, cap - o, "\\x%02x", c);
        }
    }
    buf[o++] = '"';
    buf[o] = 0;
    return buf;
}

static int read_file(const char *path, uint8_t **data, size_t *size)
{
    FILE *f = fs_open(path, "rb");
    long n;

    if (!f) {
        return -1;
    }
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    *data = malloc((size_t)n);
    if (!*data || fread(*data, 1, (size_t)n, f) != (size_t)n) {
        fclose(f);
        free(*data);
        return -1;
    }
    fclose(f);
    *size = (size_t)n;
    return 0;
}

/* The boot ELF named by SYSTEM.CNF ("BOOT2 = cdrom0:\SLPM_645.25;1"). */
static int read_boot_elf(Disc *d, uint8_t **data, size_t *size)
{
    uint32_t lsn, n;
    char cnf[2049];
    char *p, *q;

    if (disc_find(d, "SYSTEM.CNF", &lsn, &n) != 0 || disc_read(d, lsn, 1, cnf) != 0) {
        return -1;
    }
    cnf[n < 2048 ? n : 2048] = 0;
    p = strstr(cnf, "cdrom0:");
    if (!p) {
        return -1;
    }
    p += 7;
    for (q = p; *q && *q != '\r' && *q != '\n'; q++) {
    }
    *q = 0;
    if (disc_find(d, p, &lsn, &n) != 0) {
        return -1;
    }
    *data = malloc(((size_t)n + 2047) & ~(size_t)2047);
    if (!*data || disc_read(d, lsn, (n + 2047) / 2048, *data) != 0) {
        free(*data);
        return -1;
    }
    *size = n;
    return 0;
}

static uint32_t rd32(const uint8_t *p)
{
    return p[0] | (p[1] << 8) | (p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* Load every PT_LOAD segment at its EE address and clear its bss, as the
   PS2 loader and crt0 do. */
static int load_elf(CoreHost *h, const uint8_t *elf, size_t size)
{
    uint8_t digest[32];
    char hex[65];
    uint32_t phoff, i;
    int k;

    sha256(elf, size, digest);
    memcpy(h->elf_sha256, digest, 32);
    for (k = 0; k < 32; k++) {
        snprintf(hex + 2 * k, 3, "%02x", digest[k]);
    }
    if (strcmp(hex, ELF_SHA256) != 0) {
        fprintf(stderr, "ggx: the disc's boot ELF is not %s (SHA-256 %s). This build of the port "
                        "needs the %s disc.\n", BOOT_ELF, hex,
#if defined(VERSION_KR)
                "Korean SLPM-645.25"
#else
                "Japanese SLPS-201.37"
#endif
        );
        return -1;
    }
    if (size < 52 || memcmp(elf, "\x7f" "ELF\x01\x01", 6) != 0) {
        return -1;
    }
    phoff = rd32(elf + 28);
    for (i = 0; i < (uint32_t)(elf[44] | elf[45] << 8); i++) {
        const uint8_t *ph = elf + phoff + 32 * i;
        uint32_t off = rd32(ph + 4), vaddr = rd32(ph + 8), filesz = rd32(ph + 16), memsz = rd32(ph + 20);

        if (rd32(ph) != 1) {
            continue;
        }
        if ((uint64_t)off + filesz > size || filesz > memsz) {
            return -1;
        }
        memcpy(ee_ptr(h, vaddr, memsz), elf + off, filesz);
        memset(h->mem + vaddr + filesz, 0, memsz - filesz);
    }
    return 0;
}

/* The extracted assets, or with --disc the image, which must be this
   build's release. */
static int open_disc(Disc *d, const CoreHostConfig *cfg)
{
    const Release *want = release_by_id(RELEASE), *found;
    char why[1024];

    if (!cfg->disc_path) {
        return asset_store_open(&d->store, cfg->data_dir, want, stderr);
    }
    if (disc_image_open(&d->image, cfg->disc_path, why, sizeof(why)) != 0) {
        fprintf(stderr, "ggx: %s\n", why);
        return -1;
    }
    found = assets_identify(d->image, cfg->disc_path, why, sizeof(why));
    if (!found) {
        fprintf(stderr, "ggx: %s", why);
        return -1;
    }
    if (found != want) {
        fprintf(stderr, "ggx: %s is %s (%s), but this build of the port is for %s (%s): build it with "
                        "-DGGX_VERSION=%s for this disc\n",
                cfg->disc_path, found->title, found->serial, want->title, want->serial, found->id);
        return -1;
    }
    return 0;
}

int core_card_kind(const char *name)
{
    return !strcmp(name, "store") ? CORE_CARD_STORE : !strcmp(name, "empty") ? CORE_CARD_EMPTY
                                                     : !strcmp(name, "none") ? CORE_CARD_NONE : -1;
}

/* Hosts open in the process: the wasm runtime (its signal handler and
   alternate stack) is set up for the first and torn down after the last,
   so a rollback test can run several Cores side by side (PORT-013). */
static int rt_users;

int core_host_open(CoreHost *h, const CoreHostConfig *cfg)
{
    uint8_t *elf = NULL;
    size_t elf_size = 0;
    int rc, i;

    memset(h, 0, sizeof(*h));
    h->log = cfg->log;
    h->env.host = h;
    h->next_module_id = 1;
    for (i = 0; i < 2; i++) {
        /* a slot without a card still gets an empty one, for a snapshot that has a card there */
        h->cards[i] = cfg->cards[i] != CORE_CARD_NONE;
        h->card[i] = mc_card_open(cfg->cards[i] == CORE_CARD_STORE ? cfg->save_dir : NULL,
#ifdef VERSION_KR
                                  SAVE_RELEASE_KR,
#else
                                  SAVE_RELEASE_JP,
#endif
                                  stderr);
        if (!h->card[i]) {
            return -1;
        }
        mc_card_set_extensions(h->card[i], cfg->extensions, NULL, cfg->n_extensions);
    }
    if (open_disc(&h->disc, cfg) != 0) {
        return -1;
    }
    rc = cfg->elf_path ? read_file(cfg->elf_path, &elf, &elf_size) : read_boot_elf(&h->disc, &elf, &elf_size);
    if (rc != 0) {
        fprintf(stderr, "ggx: could not read the boot ELF\n");
        return -1;
    }
    if (!rt_users++) {
        wasm_rt_init();
    }
    h->rt = 1;
    wasm2c_ggx_instantiate(&h->inst, &h->env);
    h->mem = h->inst.w2c_memory.data;
    h->mem_size = h->inst.w2c_memory.size;
    if (core_state_init(h) != 0) {
        free(elf);
        return -1;
    }
    rc = load_elf(h, elf, elf_size);
    free(elf);
    if (rc == 0) {
        shim_log(h, "boot %s", BOOT_ELF);
    }
    return rc;
}

void core_host_close(CoreHost *h)
{
    shim_log_flush(h);
    core_state_free(h);
    if (h->rt) {
        wasm2c_ggx_free(&h->inst);
        if (!--rt_users) {
            wasm_rt_free();
        }
        h->rt = 0;
    }
    disc_close(&h->disc);
    mc_card_close(h->card[0]);
    mc_card_close(h->card[1]);
}

static uint32_t job_mode(CoreHost *h)
{
    return ee_r32(h, ggx_sym_JobMode);
}

static void log_job(CoreHost *h, uint32_t before)
{
    uint32_t now = job_mode(h);

    if (now != before) {
        shim_log(h, "job %u -> %u %s", before, now, ggx_func_name(ee_r32(h, ggx_sym_jobfunc + 4 * now)));
    }
}

/* the host whose Core is running, for ggx_core_bad_indirect */
static CoreHost *running;

/* Called by the Core (tools/w2c_patch.py) just before a call_indirect trap. */
void ggx_core_bad_indirect(u32 index, int missing)
{
    if (running) {
        shim_log(running, "TRAP call_indirect to 0x%08x %s: %s", index, ggx_func_name(index),
                 missing ? "no function there" : "the caller's signature differs");
    }
}

static int run(CoreHost *h, void (*entry)(w2c_ggx *), const char *what)
{
    wasm_rt_trap_t trap;
    uint32_t before = job_mode(h);

    running = h;
    trap = (wasm_rt_trap_t)wasm_rt_impl_try();
    if (trap != WASM_RT_TRAP_NONE) {
        core_state_collect(h);
        shim_log(h, "TRAP in %s: %s", what, wasm_rt_strerror(trap));
        fprintf(stderr, "ggx: field %u: the Core trapped in %s: %s\n", h->field, what, wasm_rt_strerror(trap));
        return -1;
    }
    entry(&h->inst);
    /* the pages the Core wrote belong to this host's state */
    core_state_collect(h);
    log_job(h, before);
    return 0;
}

int core_host_boot(CoreHost *h)
{
    return run(h, w2c_ggx_GameBoot, "GameBoot");
}

int core_host_field(CoreHost *h)
{
    int rc;

    h->pad_acted[0] = h->pad_acted[1] = 0;
    rc = run(h, w2c_ggx_GameField, "GameField");

    if (rc == 0) {
        h->field++;
    }
    return rc;
}
