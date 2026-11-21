/* The Core host: one Simulation Core instance and what the PS2 SDK shims share.
 *
 * The Core is the game C built for wasm32 and turned into C by wasm2c
 * (platforms/README.md). Its linear memory is laid out as the EE sees it:
 * EE RAM at 0 (the player's ELF loaded at its own addresses), the
 * scratchpad at 0x70000000, and the module's own data and stack above that.
 * Every pointer the game hands a shim is an EE address into that memory.
 */
#ifndef GGX_CORE_HOST_H
#define GGX_CORE_HOST_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "ggx_core.h"
#include "core_state.h"
#include "disc.h"
#include "pad.h"

#define EE_RAM_SIZE 0x02000000u
#define EE_SPR_BASE 0x70000000u
#define EE_SPR_SIZE 0x00004000u
/* where wasm-ld puts the Core's own data and stack (--global-base) */
#define CORE_OWN_BASE 0x70010000u

typedef struct CoreHost CoreHost;

/* wasm2c passes this to every import; the shims reach the host through it. */
struct w2c_env {
    CoreHost *host;
};


typedef struct {
    int pending;          /* a command is waiting for sceMcSync */
    int func;             /* its sceMcFunc number */
    int result;
} McState;

struct CoreHost {
    w2c_ggx inst;
    struct w2c_env env;
    uint8_t *mem;         /* the Core's linear memory */
    uint64_t mem_size;
    uint32_t own_end;     /* the end of the Core's own area: its stack's top, rounded up to a page */
    CoreState *state;     /* regions, page tracking, State Hash (core_state.h) */
    uint8_t elf_sha256[32];

    uint32_t field;       /* fields completed by GameField */
    uint32_t vsyncs;      /* sceGsSyncV calls: the Core's vblank count */
    FILE *log;
    char log_last[512];   /* the previous line, and how often it repeated */
    uint32_t log_repeats;

    Disc disc;
    PadInput pads[2];     /* the field's input, both ports (common/input/pad.h) */
    uint8_t pad_acted[2];     /* scePadSetActDirect called for the port in the field running; reset each
                                 field, so not state */
    uint8_t actuators[2][2];  /* the motors each port last asked for (scePadSetActDirect): small, large;
                                 for the host's rumble, not Core state */
    McState mc;
    int cards[2];         /* memory card slots: 1 holds a card */
    struct McCard *card[2];   /* what each slot's card holds (common/saves/mc_card.h); not Core state */
    uint32_t next_module_id;
    int rt;               /* the Core is instantiated (core_host_close frees it) */
    struct Trace *replay; /* a Lockstep Trace being replayed (common/trace/trace.h): the shims answer from it */

    /* Optional: told of every GIF DMA send (the chain's EE address). The
       renderer (PORT-008) reads the chain and never writes Core memory;
       the headless host leaves it NULL. */
    void (*gif_send)(void *ctx, uint32_t tag);
    void *gif_ctx;

    /* Optional (PORT-011): told of every call to the sound driver's RPC
       server (SNDRV.IRX), before its reply is written: the field running,
       the command with its 0x80 flag, and the bytes the game sent. The
       audio layer (audio/) is fed this way and never writes Core memory. */
    void (*snd_rpc)(void *ctx, uint32_t field, uint32_t cmd, const uint8_t *data, uint32_t size);
    /* and of every disc read the game makes itself (sceCdRead), which moves
       the drive the sound driver streams the BGM from */
    void (*snd_cd_read)(void *ctx, uint32_t field, uint32_t lsn, uint32_t sectors);
    void *snd_ctx;
};

typedef struct {
    const char *data_dir;   /* the user data directory holding the extracted assets */
    const char *disc_path;  /* instead, read straight from the player's image (development) */
    const char *elf_path;   /* optional: the boot ELF outside the disc */
    FILE *log;
    int cards[2];           /* CORE_CARD_* */
    const char *save_dir;   /* the native save store, for CORE_CARD_STORE */
    const char *const *extensions;  /* extensions or mods active, recorded in what the game saves */
    int n_extensions;
} CoreHostConfig;

/* what a memory card slot holds */
enum { CORE_CARD_NONE = 0, CORE_CARD_EMPTY = 1, CORE_CARD_STORE = 2 };
#define CORE_MAX_EXTENSIONS 16

/* --card1/--card2: "store", "empty" or "none" as CORE_CARD_*, or -1. */
int core_card_kind(const char *name);

int core_host_open(CoreHost *h, const CoreHostConfig *cfg);
void core_host_close(CoreHost *h);
/* 0 on success; nonzero if the Core trapped (the log says where). */
int core_host_boot(CoreHost *h);
int core_host_field(CoreHost *h);

/* EE memory. Every access is bounds-checked against EE RAM, the scratchpad
   and the Core's own area; anything else is a host bug or a bad pointer
   from the game, and stops the run. ee_ptr is for memory the caller may
   write, so it marks those pages changed (core_state.h); ee_rptr, the ee_r
   and ee_str readers do not. */
uint8_t *ee_ptr(CoreHost *h, uint32_t addr, uint32_t len);
const uint8_t *ee_rptr(CoreHost *h, uint32_t addr, uint32_t len);
uint32_t ee_r32(CoreHost *h, uint32_t addr);
void ee_w32(CoreHost *h, uint32_t addr, uint32_t v);
void ee_w64(CoreHost *h, uint32_t addr, uint64_t v);
uint16_t ee_r16(CoreHost *h, uint32_t addr);
/* A NUL-terminated string from EE memory, truncated to cap-1 bytes. */
const char *ee_str(CoreHost *h, uint32_t addr, char *buf, size_t cap);

/* The shim call log: one line per call, the same on every OS. A run of
   identical lines is written once, then "  x<count>". */
void shim_log(CoreHost *h, const char *fmt, ...)
#if defined(__GNUC__) || defined(__clang__)
    __attribute__((format(printf, 2, 3)))
#endif
    ;
/* An address for the log. EE addresses print as hex; an address in the
   Core's own area (its stack or a string literal) depends on the compiler
   that built the Core, so it prints as "core". */
const char *log_addr(uint32_t addr, char buf[16]);
void shim_log_flush(CoreHost *h);
/* A string argument, quoted, with non-ASCII bytes escaped. */
const char *log_str(CoreHost *h, uint32_t addr, char *buf, size_t cap);

/* PS2 names for addresses, from the build's symbol map (generated). */
const char *ggx_func_name(uint32_t addr);
extern const uint32_t ggx_sym_JobMode, ggx_sym_jobfunc, ggx_sym_gsGParam, ggx_sym_handle_list, ggx_sym_seekpos,
    ggx_sym_pad_read_buf, ggx_sym_mcPwd, ggx_sym_userinfo, ggx_sym_Pause, ggx_sym_sc, ggx_sym_opt, ggx_sym_tr_info,
    ggx_sym_g_info, ggx_sym_story, ggx_sym_LimitStatus;
/* Function statics from the statics map (common/core/symbols/statics_*.txt). */
extern const uint32_t ggx_static_Pause_Execute_pos, ggx_static_Opt_KeyConfig_pos, ggx_static_Opt_KeyConfig_btn,
    ggx_static_Opt_Password_passbuf, ggx_static_Opt_Password_passidx, ggx_static_Opt_Password_passmode;

#endif
