/* Lockstep Traces: replaying a PS2 recording through the Core (PORT-006).
 *
 * This file defines the format. A trace, recorded by the
 * gg-layers PCSX2 fork's GGTrace from the real game, holds for every field:
 *
 *   - each SDK call the game made that the port answers with a shim, in
 *     order, with the PS2's return value and the bytes it wrote back that
 *     the game reads (the pads, the sound driver's replies, a file's
 *     directory entry). These are the field's input: the pads as the game
 *     read them, and the PS2's timing (a CD read busy for 40 fields, an IOP
 *     reply that comes a field late).
 *   - the State Hash (STATE-HASH.md) of EE RAM and the scratchpad at the end
 *     of the field, with the trace's masks, and the hashes of the pages that
 *     changed.
 *
 * The replay steps the Core one field at a time. Each shim asks
 * trace_answer() for the PS2's answer to its call, so the Core sees exactly
 * what the game saw on the PS2. After each field the Core's own State Hash,
 * with the same masks, must equal the PS2's. The first field where it
 * doesn't, or where the Core's calls stop matching the PS2's, fails the
 * replay, naming the field, the region and the pages that differ.
 */
#ifndef GGX_TRACE_H
#define GGX_TRACE_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "trace_env.h"

#define TRACE_FORMAT_VERSION 1u
#define TRACE_PAGES (0x02000000u / 4096u + 0x4000u / 4096u)   /* ram, then spr */

typedef struct CoreHost CoreHost;
typedef struct Trace Trace;

typedef struct {
    uint32_t base, size;
} TraceRange;

typedef struct {
    uint32_t format_version, hash_version, release;
    uint8_t elf_sha256[32];
    uint64_t map_hash;
    char recorder[65], settings[65];
    const char *note;
    uint32_t nmasks;
    const TraceRange *masks;
} TraceHeader;

/* Loads a trace. NULL with a message in err (cap bytes) if it can't. */
Trace *trace_open(const char *path, char *err, size_t cap);
void trace_free(Trace *t);
const TraceHeader *trace_header(const Trace *t);
/* Fields the trace records, the boot (field 0) included. */
uint32_t trace_fields(const Trace *t);

/* One recorded call, for tools and tests: its SDK function (trace_env.h),
   how many identical calls it stands for, the PS2's return value, and its
   outputs (nout x: u8 length, then the bytes). */
typedef struct {
    uint8_t env;
    uint8_t nout;
    uint32_t ret;
    uint32_t rep;
    const uint8_t *outs;
} TraceCall;

/* The calls of one field, in order: up to cap into out, and the count. Not
   during a replay. */
uint32_t trace_field_calls(Trace *t, uint32_t field, TraceCall *out, uint32_t cap);
/* The PS2 vblanks the field took (capped at 255). */
uint32_t trace_field_vblanks(const Trace *t, uint32_t field);

/* Replay. trace_begin attaches the trace to the host before core_host_boot;
   trace_field_begin(n) loads field n's calls before the Core runs it (n = 0
   before the boot), and trace_field_end checks the field once it has run:
   0 if the Core matched the PS2, nonzero if it did not. The failure is
   described by trace_report. */
void trace_begin(Trace *t, CoreHost *h);
void trace_field_begin(Trace *t, uint32_t field);
int trace_field_end(Trace *t);
/* A failure's description: the field, what differed, and the pages. With
   dump_dir, also writes the Core's masked bytes of each differing page
   there, as <dump_dir>/f<field>-<address>.bin. With symbols (a port symbol
   map, common/core/symbols/<kr|jp>.txt), names what is in each page. With
   ps2_dump (the recorder's GG_TRACE_DUMP file for that field), lists the
   differing bytes too. */
void trace_report(Trace *t, FILE *out, const char *dump_dir, const char *symbols, const char *ps2_dump);

/* The survey, for finding what differs: after trace_keep_going, a State
   Hash mismatch no longer ends the replay. trace_resume continues after
   one (a call that differs still ends it: there is no PS2 answer to give),
   and trace_survey lists every page that differed, with the first and last
   field and how often. */
void trace_keep_going(Trace *t);
int trace_resume(Trace *t);
void trace_survey(Trace *t, FILE *out, const char *symbols);

/* The shims' side: the PS2's answer to this call. Returns the PS2's return
   value and writes the PS2's bytes through the call's pointer arguments
   (args: the call's arguments in order, a0-a3 then t0-t3), or, if the
   Core's call is not the one the PS2 made next, records the divergence and
   returns the shim's own value. */
uint32_t trace_answer(CoreHost *h, int env, uint32_t value, const uint32_t *args, int nargs);

/* A shim's return statement: the shim's own value, or during a replay the
   PS2's. TRACE_RETURN(h, sceGsSyncV, parity, mode); */
#define TRACE_RETURN(h, name, value, ...)                                                                     \
    do {                                                                                                      \
        const uint32_t trace_args_[] = {0, ##__VA_ARGS__};                                                    \
        uint32_t trace_v_ = (value);                                                                          \
        return (h)->replay ? trace_answer((h), TRACE_ENV_##name, trace_v_, trace_args_ + 1,                  \
                                          (int)(sizeof(trace_args_) / sizeof(trace_args_[0])) - 1)           \
                           : trace_v_;                                                                        \
    } while (0)

/* The same for a shim with no return value: the call still has to be the
   one the PS2 made next. */
#define TRACE_CALL(h, name, ...)                                                                              \
    do {                                                                                                      \
        const uint32_t trace_args_[] = {0, ##__VA_ARGS__};                                                    \
        if ((h)->replay) {                                                                                    \
            (void)trace_answer((h), TRACE_ENV_##name, 0, trace_args_ + 1,                                     \
                               (int)(sizeof(trace_args_) / sizeof(trace_args_[0])) - 1);                      \
        }                                                                                                     \
    } while (0)

#endif
