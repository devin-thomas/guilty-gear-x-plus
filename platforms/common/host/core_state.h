/* Core state: its regions, the State Hash, and save/load (PORT-004).
 *
 * Every byte of mutable Core state is in one of seven regions (SPEC.md 4,
 * ADR-014). Five are EE address ranges in the Core's linear memory, one is
 * the Core module's own data and stack, and one holds the few values the
 * host and shims keep for the Core. This file defines the regions, the
 * State Hash and the snapshot format; tools/state_hash.py
 * is a second implementation of both.
 *
 * Snapshots are copied a page at a time, and only the pages that differ:
 * every store the Core makes marks its 4 KiB page (tools/w2c_patch.py), and
 * a snapshot remembers when it was last made equal to the Core. The State
 * Hash keeps a hash per page, so it too only rehashes what changed.
 */
#ifndef GGX_CORE_STATE_H
#define GGX_CORE_STATE_H

#include <stddef.h>
#include <stdint.h>

typedef struct CoreHost CoreHost;
typedef struct CoreState CoreState;
typedef struct CoreSnapshot CoreSnapshot;

/* State Hash and snapshot format versions (STATE-HASH.md) */
#define CORE_STATE_HASH_VERSION 1u
#define CORE_STATE_FORMAT_VERSION 1u

/* Region ids, in State Hash order. */
enum {
    CORE_REGION_RAM = 1,    /* EE RAM 0x00000000-0x02000000 */
    CORE_REGION_SPR,        /* scratchpad 0x70000000-0x70004000 */
    CORE_REGION_EEIO,       /* EE registers 0x10000000-0x10010000 */
    CORE_REGION_VUMEM,      /* VU0/VU1 memory 0x11000000-0x11010000 */
    CORE_REGION_GSPRIV,     /* GS privileged registers 0x12000000-0x12002000 */
    CORE_REGION_OWN,        /* the Core module's own data and stack, from 0x70010000 */
    CORE_REGION_HOST,       /* host and shim state for the Core, 64 bytes */
    CORE_REGION_COUNT = 7
};

typedef struct {
    uint32_t id;
    const char *name;
    uint32_t base;          /* EE address; 0 for the host region */
    uint32_t size;
} CoreRegion;

/* Called by core_host_open once the Core is instantiated. */
int core_state_init(CoreHost *h);
void core_state_free(CoreHost *h);
/* Picks up the pages the Core wrote since the last call (core_host.c calls
   it after every run of the Core). */
void core_state_collect(CoreHost *h);
/* Marks EE memory the host or a shim is about to write. */
void core_state_touch(CoreHost *h, uint32_t addr, uint32_t len);

/* Each page of region id's last-changed epoch, and the epoch now (in
   *epoch); core_state_next_epoch starts a new one. A page whose epoch is the
   current one changed since the last core_state_next_epoch. PORT-006's
   replay uses them to rehash only what a field changed; snapshots are not
   affected. */
const uint32_t *core_state_page_epochs(const CoreHost *h, uint32_t id, uint32_t *epoch);
uint32_t core_state_next_epoch(CoreHost *h);

/* The bytes of Core memory the last core_save or core_load copied: a
   save's or load's cost, the same on every machine (PORT-013). */
size_t core_state_last_copy(const CoreHost *h);

/* The Core build id (STATE-HASH.md section 1): what a snapshot or a replay
   records of the Core that made it. */
uint64_t core_build_id(const CoreHost *h);

/* The regions, in id order. */
const CoreRegion *core_regions(const CoreHost *h);

/* The State Hash of every region. */
uint64_t core_hash(CoreHost *h);
/* The State Hash of the listed regions (ids, any order; hashed in id order).
   PORT-006 compares the Core with PCSX2 over CORE_REGION_RAM and _SPR. */
uint64_t core_hash_regions(CoreHost *h, const uint32_t *ids, int n);
/* One region's hash (STATE-HASH.md: region hash). */
uint64_t core_region_hash(CoreHost *h, uint32_t id);

/* Snapshots. A snapshot is a buffer of core_state_size() bytes in the
   snapshot format, plus bookkeeping that never leaves the process. Saving
   into or loading from a snapshot this host saved before copies only the
   pages that changed in between; any other snapshot is copied whole. */
CoreSnapshot *core_snapshot_new(const CoreHost *h);
void core_snapshot_free(CoreSnapshot *s);
size_t core_state_size(const CoreHost *h);
/* 0 on success. */
int core_save(CoreHost *h, CoreSnapshot *s);
/* 0 on success. Otherwise one of these, with the Core untouched: */
enum {
    CORE_LOAD_FORMAT = -1,    /* not a snapshot, or another format or State Hash version */
    CORE_LOAD_RELEASE = -2,   /* for the other release or another boot ELF */
    CORE_LOAD_BUILD = -3,     /* from another Core build (its own area differs) */
    CORE_LOAD_DAMAGED = -4    /* its pages do not hash to the State Hash it records */
};
int core_load(CoreHost *h, CoreSnapshot *s);
const char *core_load_error(int rc);
/* The snapshot's bytes, to write to a file or send, and how many. */
const uint8_t *core_snapshot_data(const CoreSnapshot *s);
size_t core_snapshot_size(const CoreSnapshot *s);
/* Replaces the snapshot's bytes with ones read from a file or received.
   0 if they have the size and header this host expects. */
int core_snapshot_set_data(CoreSnapshot *s, const void *data, size_t size);
/* The State Hash recorded when the snapshot was saved. */
uint64_t core_snapshot_hash(const CoreSnapshot *s);

/* State scans (the poison/scan test). In a GGX_STATE_SCAN build the Core
   also marks every page it reads. Prints each page outside the regions the
   Core wrote (and read, in a scan build) since the last call, and returns
   how many there were. */
int core_state_scan(CoreHost *h, const char *when);
/* Fills the memory around and between the regions with a poison pattern:
   the 16 MiB on either side of each, and all of each smaller gap. A Core
   that read any of it would compute something else; one that wrote any of
   it fails core_state_poison_check, which returns the bytes changed. */
void core_state_poison(CoreHost *h);
uint64_t core_state_poison_check(CoreHost *h);

/* ggx_core_dirty marks every page the Core stores to; ggx_core_read, in a
   GGX_STATE_SCAN build, every page it loads from. One byte per 4 KiB page
   of the 32-bit address space. */
#define CORE_PAGE_SHIFT 12
#define CORE_PAGE_SIZE 4096u
extern uint8_t ggx_core_dirty[1u << 20];
extern uint8_t ggx_core_read[1u << 20];

#endif
