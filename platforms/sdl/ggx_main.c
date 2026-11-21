/* ggx: the Core with the renderer on SDL_GPU (PORT-008, PORT-009).
 *
 *   ggx [--data-dir <dir> | --disc <image>] [--elf <boot elf>] [--fields N] [--until <job>]
 *       [--log <file> | --no-log] [--card1 store|empty|none] [--card2 empty|none]
 *       [--save-dir <dir>] [--extension <id>]...
 *       [--scale N] [--merge on|off] [--offscreen] [--no-render] [--no-focus] [--render-all]
 *       [--gpu vulkan|metal|direct3d12]
 *       [--dump <field>[,<field>...] --dump-dir <dir>] [--ps2-state <dir>] [--stats]
 *       [--hash-log <file>] [--mash SEED] [--trace <file.ggxt>]
 *       [--text en|off] [--text-dir <dir>] [--script <file>]
 *       [--config <file>] [--fullscreen | --windowed] [--scaling fit|integer]
 *       [--present vsync|mailbox|immediate] [--audio on|off] [--snd-log <file.ggxs>] [--audio-out <file.wav>]
 *       [--gs-dump <file.gs> --gs-dump-template <file> --gs-dump-fields A,B --dump-dir <dir>]
 *       [--record <file.ggxreplay>] [--replay-dir <dir>]
 *   ggx --play <file.ggxreplay> [--from start|fight] [the display and sound options]
 *   ggx [--first-run <image>] ...
 *   ggx --replay <file> --dump-dir <dir> [--scale N] [--merge on|off] [--gpu ...] [--stats]
 *
 * The first run (PORT-015, firstrun.h): with no extraction of this build's
 * release in the user data directory, ggx asks for the player's disc image
 * on a screen of its own, extracts it with a progress bar, refuses a wrong
 * or modified image naming the supported releases, and starts the game (or
 * hands over to the other release's build beside it, ggx-kr or ggx-jp).
 * --first-run <image> runs that screen as if the image had been chosen.
 *
 * It boots and steps the Core exactly as ggx-headless does, and writes the
 * same log; the renderer only reads what the game sends the GS (the GIF DMA
 * sends, through CoreHost.gif_send), so the log and the State Hash are the
 * same with rendering on, off (--no-render) or headless. --hash-log and
 * --mash are ggx-headless's (the State Hash after the boot and every field;
 * deterministic button mashing on both pads, mash.h).
 *
 * With a window (PORT-010), the Core runs at the NTSC field rate on a
 * fixed-step accumulator, and each frame is presented as its field ends; the
 * display runs free at its own rate and holds the newest frame between
 * presents, repeating it when no field was due (ADR-013). Nothing is
 * interpolated. --present picks how frames reach the screen: vsync (the
 * default) waits for the display's next vblank, mailbox replaces a waiting
 * frame, immediate doesn't wait, which on a VRR display paces the display
 * to the Core. The picture is 4:3, scaled to fit or by
 * whole multiples of the PS2's 448 lines (--scaling, F6), windowed or
 * fullscreen (--fullscreen, F11, Alt+Enter, or a double-click in the
 * middle of the picture). The input layer (input.h)
 * reads keyboards, gamepads and arcade sticks into both pads, just before
 * each field; F3 and F4 remap players 1 and 2, F5 swaps their controllers,
 * F7 and F8 switch their pads between digital and analog. The settings are
 * kept in ggx.cfg in the user data directory (config.h; --config names
 * another file). A developer build (-DGGX_DEV_BUILD=ON) adds Tab, which runs
 * the Core as fast as it goes. --no-focus shows the window without
 * activating it, for test runs beside other work. --frame-times writes
 * a CSV line per field: its job, the vblanks it took, and where its wall
 * time went (waiting for the clock, the Core with the Draw Layer's decode,
 * the sound, the renderer's submit, the present), in microseconds.
 *
 * Memory card slot 1 holds the native save store (PORT-012): saves/ in the
 * user data directory, beside ggx.cfg (--save-dir names another), so what
 * the game saves is there the next time; ggx-save imports a save from a
 * PS2 memory card image and exports one back. --card1 empty|none,
 * --card2 and --extension are ggx-headless's.
 *
 * Button prompts (PORT-017, ADR-019, prompts/pr.h): where the game draws
 * its Circle, X, Square and Triangle (KEY CONFIG, the SOUND TEST and
 * gallery help lines), each player sees the letter on the button bound to
 * it on the device they used last (sdl/input.h), in the game's own font;
 * PlayStation pads keep the symbols. --prompts forces a family for the run
 * (playstation, xbox, steam, nintendo, keyboard, other, or auto), over the
 * config's "prompts", and works offscreen too.
 *
 * X confirms in menus (PORT-019): "confirm = x" (the default) or circle in
 * the config, per player, or --confirm for the run; with --confirm, a
 * --script's buttons go through it too, as a player's would. F9, or Select
 * held a second, opens the port's settings panel over the game (--settings
 * at start).
 *
 * Unlocks (PORT-020, common/host/unlock.h): "unlock = all" (or items:
 * characters, colours, gallery, challenges) in the config, or --unlock for
 * the run, raises them at the title, after the memory card's load; the
 * settings panel's Unlocks do the same at once; and GOHAN on OPTIONS >
 * PASSWORD unlocks everything. Off by default.
 *
 * --display (or display = in ggx.cfg) opens the window on a display, by its
 * number from 1 or a part of its name; --display list lists them. The Core
 * is paced by the 59.94 Hz clock; with --pacing display (pacing = display),
 * where the display refreshes at a whole multiple of the field rate and its
 * vblanks can be followed (Windows), by the display's vblanks instead
 * (pace.h), falling back to the clock if they don't make sense. The startup
 * lines say which; --vblank-log writes every vblank wake-up and every paced
 * field's decision, and --vblank-source tries one source only (kernel or
 * dxgi), or makes them return at once (nowait), to see that dropped. For
 * measuring: --frame-tag writes each frame's
 * field into the window's corner (tools/latency/cadence.swift reads it),
 * --fake-vblank paces to a made-up display at a rate, --pace-lead-ms fixes
 * the lead, and --frames-in-flight sets SDL's frames in flight (1).
 *
 * --script plays a GGScript input script (common/input/ggscript.h) on both
 * pads instead of the controllers, as ggx-headless does; its "shot" steps
 * write the next frame to --dump-dir as <name>.png. In a window the
 * controllers take over when it ends without "quit". --until A,B,C stops
 * once each job has run in turn, and --forbid fails the run if a listed job
 * runs (ggx-headless).
 *
 * --offscreen opens no window and runs the fields as fast as it can
 * (--render-all draws every frame as a window would, and reports the rate);
 * --dump writes the frames that finished on the listed fields to
 * --dump-dir as PNG (640x448 times --scale). --ps2-state loads a PCSX2
 * state after boot: ee.bin (EE RAM), spr.bin (the scratchpad) and vram.bin
 * (GS local memory), extracted from a PCSX2 save state, so a frame
 * PCSX2 drew can be drawn again here and compared.
 *
 * --replay draws a PCSX2 GS dump's GIF stream and privileged registers
 * (converted to the port's format) with no Core at all, and writes the
 * frame shown at each vsync as <dir>/v<vsync>.png: the renderer alone
 * against PCSX2's frames. --gs-dump is the other way round: the GIF stream
 * of fields A to B-1, and the display registers the game wrote, are
 * written as a PCSX2 GS dump (pcsx2-gsrunner draws it), and the renderer's
 * frames as <dir>/v<vsync>.png, so PCSX2's GS and this renderer draw the
 * same frames of the port's own Core.
 *
 * --merge (on by default) merges the display circuits as the game sets them
 * (DRAW-INVENTORY.md, GS Path); off shows the frame as drawn. --scale sets
 * the internal resolution.
 *
 * --text en draws the English text layer (PORT-014, text/LANG-FORMAT.md)
 * in place of the game's Japanese or Korean text pictures; the original
 * text is the default (--text off), and F2 switches between them in a
 * window. The translation files and fonts are read from --text-dir, by
 * default the text/ folder beside ggx. Only the renderer's textures change.
 *
 * --trace drives the Core from a Lockstep Trace (PORT-006) as ggx-replay
 * does, the PS2's answers standing in for the SDK and the pads, and checks
 * the State Hash after every field, with the renderer (and the text layer)
 * drawing: so a trace can be watched, dumped and checked with English on
 * and off. A mismatch is reported and the run goes on.
 *
 * Sound (PORT-011): in a window the game's sound plays (audio_out.h), the
 * sound driver fed by the command stream the SIF shim hands it
 * (audio/audio_host.h); --audio off (or "audio = off" in ggx.cfg) plays
 * none. --snd-log records the stream and --audio-out writes the sound to a
 * WAV file, in a window or offscreen. None of it touches the Core: the log
 * and the State Hash are the same with sound on or off.
 *
 * Replays (PORT-013, common/host/replays.h): every field runs through a
 * session (session.h), which records it. In a window, with "replays = on"
 * (the default), each fight that ends is written as a replay into replays/
 * in the user data directory (or --replay-dir), named by the date, the time
 * and the release: the run from the boot to that fight's end, so it holds
 * every fight before it too. --record writes the whole run as one replay
 * when ggx quits (in a window or offscreen). --play plays a replay: its
 * cards, unlocks and extensions, every field's input; the controllers take
 * no part (close the window to stop). In a window it runs fast to the
 * replay's first fight (--from start plays it all from the boot) and then
 * at the field rate, with picture and sound, and stops at its end. Every
 * State Hash the replay recorded is checked on the way; ggx says if one
 * differs, and why the replay can't play if it can't (another release,
 * Core build or the extensions). ggx-replays lists them (list, info).
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "audio_host.h"
#include "audio_out.h"
#include "config.h"
#include "core_host.h"
#include "core_state.h"
#include "dl_gpu.h"
#include "dl_stream.h"
#include "firstrun.h"
#include "fsys.h"
#include "ggscript.h"
#include "input.h"
#include "mash.h"
#include "pace.h"
#include "png_write.h"
#include "replays.h"
#include "session.h"
#include "confirm.h"
#include "menu.h"
#include "pr.h"
#include "save_file.h"
#include "settings.h"
#include "unlock.h"
#include "tl_host.h"
#include "trace.h"
#include "vblank.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#if defined(VERSION_KR)
#define RELEASE "KR"
#define RELEASE_ID TL_KR
#define TRACE_RELEASE 1u
#else
#define RELEASE "JP"
#define RELEASE_ID TL_JP
#define TRACE_RELEASE 2u
#endif

#define FIELD_NS 16683350ull   /* 1 / 59.94 Hz */
#define GS_PRIV_BASE 0x12000000u   /* the GS privileged registers in Core memory */
#define MAX_DUMPS 256

/* --gs-dump: the port's own GIF stream written as a PCSX2 GS dump, which
   pcsx2-gsrunner then draws, so PCSX2's GS renders exactly what the port's
   renderer does. The template (refs.py gsdump-template) is a PCSX2 dump's
   header, GS state and privileged registers, uncompressed; the state's GS
   memory is replaced by the renderer's when the recording starts, and each
   vsync carries the display registers GS_runtime_preset wrote. */
typedef struct {
    FILE *f;
    uint32_t vsyncs;
    uint8_t regs[8192];      /* the privileged registers, as GSPrivRegSet */
} GsDump;

typedef struct {
    CoreHost *h;
    DlStream *s;
    GsDump *dump;
} Hook;

static uint32_t rd32le(const uint8_t *p)
{
    return p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static int gsdump_start(GsDump *d, const char *path, const char *tmpl, const GsMem *mem)
{
    FILE *t = fs_open(tmpl, "rb");
    int64_t n = t ? fs_size(t) : -1;
    uint8_t *buf = n > 16 ? malloc((size_t)n) : NULL;
    uint64_t off = 0;
    int rc = -1;

    if (buf && fs_seek(t, 0) == 0 && fread(buf, 1, (size_t)n, t) == (size_t)n && rd32le(buf) == 0xFFFFFFFFu) {
        /* GSState::Freeze (version 9) ends with VRAM, the four GIF paths
           (16 + 4 bytes each) and Q; 8 KB of privileged registers follow */
        off = 8ull + rd32le(buf + 4) + rd32le(buf + 12) - 84 - GS_MEM_SIZE;
        if (rd32le(buf + 8) == 9 && off + GS_MEM_SIZE + 84 + 8192 == (uint64_t)n) {
            memcpy(buf + off, mem->m, GS_MEM_SIZE);
            memcpy(d->regs, buf + n - 8192, 8192);
            d->f = fs_open(path, "wb");
            rc = d->f && fwrite(buf, 1, (size_t)n, d->f) == (size_t)n ? 0 : -1;
        }
    }
    if (t) {
        fclose(t);
    }
    free(buf);
    if (rc != 0) {
        fprintf(stderr, "ggx: cannot start the GS dump %s from %s\n", path, tmpl);
    }
    d->vsyncs = 0;
    return rc;
}

static void gsdump_tap(void *ctx, const uint8_t *q, uint32_t n)
{
    GsDump *d = ctx;
    uint8_t h[6] = { 0, 3, (uint8_t)(n << 4), (uint8_t)(n >> 4), (uint8_t)(n >> 12), (uint8_t)(n >> 20) };

    fwrite(h, 1, 6, d->f);
    fwrite(q, 16, n, d->f);
}

/* A vsync, as PCSX2 records one: the privileged registers (CSR's FIELD bit
   for the field that starts), then the field. The fields alternate. The
   registers the game writes (PMODE, DISPFB1/2, DISPLAY1/2: graph.c
   GS_runtime_preset) are the Core's; the rest stay the template's, since
   the SDK calls that set them on the PS2 are logging shims here. */
static void gsdump_vsync(GsDump *d, const uint8_t *priv)
{
    uint8_t field = (uint8_t)(d->vsyncs & 1), v[2] = { 1, 0 }, r = 3;

    memcpy(d->regs, priv, 8);
    memcpy(d->regs + 0x70, priv + 0x70, 0x40);

    d->regs[0x1001] = (uint8_t)((d->regs[0x1001] & ~0x20) | field << 5);
    v[1] = field;
    fwrite(&r, 1, 1, d->f);
    fwrite(d->regs, 1, sizeof(d->regs), d->f);
    fwrite(v, 1, 2, d->f);
    d->vsyncs++;
}

/* Which of the game's four packet lists a send is (packet.c: handle_list
   holds each list's two pages). */
static int packet_list(CoreHost *h, uint32_t tag)
{
    uint32_t a = tag & 0x7FFFFFFFu;
    int i;

    for (i = 0; i < 4; i++) {
        uint32_t p = ee_r32(h, ggx_sym_handle_list + 4 * (uint32_t)i);

        if (p && p + 8 <= EE_RAM_SIZE && (ee_r32(h, p) == a || ee_r32(h, p + 4) == a)) {
            return i;
        }
    }
    return DL_LIST_OTHER;
}

static void on_gif(void *ctx, uint32_t tag)
{
    Hook *k = ctx;
    const DlFrame *f = dl_stream_frame(k->s);
    uint32_t before = f ? f->serial : 0;
    DlMem m;

    m.ram = k->h->mem;
    m.spr = k->h->mem + EE_SPR_BASE;
    m.priv = k->h->mem + GS_PRIV_BASE;
    dl_stream_send(k->s, &m, tag, packet_list(k->h, tag));
    f = dl_stream_frame(k->s);
    if (k->dump->f && f && f->serial != before) {
        /* the GraphConv blit: the display shows this frame from the vsync */
        gsdump_vsync(k->dump, m.priv);
    }
}

/* The frame done at each vsync of a converted GS dump (refs.py write_replay),
   as PNG. Version 2 has the privileged registers before each vsync; with
   version 1's none, the display is GS_runtime_preset's with no offset. */
static int replay(const char *path, DlStream *s, DlGpu *gpu, const char *dir, uint8_t *pixels)
{
    FILE *f = fs_open(path, "rb");
    uint8_t hdr[8], rec[8], *data = NULL;
    uint32_t vsync = 0, cap = 0;
    int rc = 0;

    if (!f || fread(hdr, 1, 8, f) != 8 || memcmp(hdr, "GGXR", 4) != 0 || (hdr[4] != 1 && hdr[4] != 2) ||
        hdr[5] || hdr[6] || hdr[7] ||
        fread(dl_stream_gs_mem(s)->m, 1, GS_MEM_SIZE, f) != GS_MEM_SIZE) {
        fprintf(stderr, "ggx: %s is not a replay file\n", path);
        if (f) {
            fclose(f);
        }
        return 1;
    }
    while (rc == 0 && fread(rec, 1, 8, f) == 8) {
        uint32_t size = rec[4] | rec[5] << 8 | rec[6] << 16 | (uint32_t)rec[7] << 24;
        const DlFrame *fr;

        if (rec[0] == 1 || rec[0] == 3) {
            if (size > cap) {
                cap = size;
                data = realloc(data, cap);
                if (!data) {
                    rc = 1;
                    break;
                }
            }
            if (fread(data, 1, size, f) != size) {
                rc = 1;
                break;
            }
            if (rec[0] == 3 && size == 8192) {
                dl_stream_priv(s, data);
            } else if (rec[0] == 1 && rec[1] == 3) {
                dl_stream_gif(s, data, size / 16, DL_LIST_REPLAY);
            }
            continue;
        }
        vsync++;
        fr = dl_stream_frame(s);
        if (fr) {
            char name[32], out[1024];
            uint32_t ow, oh;

            dl_gpu_output(gpu, &ow, &oh);
            if (dl_gpu_render_read(gpu, fr, dl_stream_textures(s), pixels) != 0) {
                fprintf(stderr, "ggx: vsync %u: the frame did not render completely\n", vsync);
            }
            snprintf(name, sizeof(name), "v%06u.png", vsync);
            if (fs_join(out, sizeof(out), dir, "/", name) != 0 || png_write_rgba(out, pixels, ow, oh) != 0) {
                fprintf(stderr, "ggx: cannot write %s\n", name);
                rc = 1;
            }
        }
    }
    free(data);
    fclose(f);
    return rc;
}

static void usage(void)
{
    fprintf(stderr, "usage: ggx [--data-dir <dir> | --disc <image>] [--first-run <image>] [--elf <boot elf>] [--fields N] [--until <job>]\n"
                    "           [--log <file> | --no-log] [--card1 store|empty|none] [--card2 empty|none] [--save-dir <dir>] "
                    "[--extension <id>]...\n"
                    "           [--scale N] [--merge on|off] [--offscreen] [--no-render] [--no-focus] [--render-all] "
                    "[--gpu vulkan|metal|direct3d12]\n"
                    "           [--dump <field>[,<field>...] --dump-dir <dir>] [--ps2-state <dir>] [--stats]\n"
                    "           [--frame-times <file.csv>] [--display N|<name>|list] [--pacing display|clock]\n"
                    "           [--frame-tag] [--fake-vblank <hz>] [--pace-lead-ms N] [--frames-in-flight N]\n"
                    "           [--vblank-log <file.csv>] [--vblank-source auto|kernel|dxgi|nowait]\n"
                    "           [--hash-log <file>] [--mash SEED] [--trace <file.ggxt>] [--text en|off] "
                    "[--text-dir <dir>] [--audio on|off] [--snd-log <file.ggxs>] [--audio-out <file.wav>]\n"
                    "           [--script <file>] [--until <job>[,<job>...]] [--forbid <job>[,<job>...]] [--config <file>] "
                    "[--fullscreen | --windowed]\n"
                    "           [--scaling fit|integer] [--present vsync|mailbox|immediate] [--prompts <family>|auto]\n"
                    "           [--confirm x|circle] [--settings] [--unlock all|none|<item>,...]\n"
                    "           [--gs-dump <file.gs> --gs-dump-template <file> --gs-dump-fields A,B --dump-dir <dir>]\n"
                    "           [--record <file.ggxreplay>] [--replay-dir <dir>]\n"
                    "       ggx --play <file.ggxreplay> [--from start|fight] [the display and sound options]\n"
                    "       ggx --replay <file> --dump-dir <dir> [--scale N] [--merge on|off] [--gpu <driver>] "
                    "[--stats]\n");
    exit(64);
}

static uint64_t fnv1a(const uint8_t *p, size_t n, uint64_t h)
{
    size_t i;

    for (i = 0; i < n; i++) {
        h = (h ^ p[i]) * 0x100000001b3ull;
    }
    return h;
}

/* A --hash-log line, as ggx-headless writes it. */
static void log_hashes(CoreHost *h, FILE *f)
{
    static const uint32_t ee[] = { CORE_REGION_RAM, CORE_REGION_SPR };

    fprintf(f, "f%06u %016llx ram+spr %016llx\n", h->field, (unsigned long long)core_hash(h),
            (unsigned long long)core_hash_regions(h, ee, 2));
}

static int read_exact(const char *dir, const char *name, uint8_t *dst, size_t n)
{
    char path[1024];
    FILE *f;
    size_t got;

    if (fs_join(path, sizeof(path), dir, "/", name) != 0 || !(f = fs_open(path, "rb"))) {
        fprintf(stderr, "ggx: cannot read %s/%s\n", dir, name);
        return -1;
    }
    got = fread(dst, 1, n, f);
    fclose(f);
    if (got != n) {
        fprintf(stderr, "ggx: %s/%s is not %zu bytes\n", dir, name, n);
        return -1;
    }
    return 0;
}

/* A PCSX2 state's EE RAM and scratchpad into the Core (through ee_ptr, so
   PORT-004's page marks see it), and its GS memory into the renderer. */
static int load_ps2_state(CoreHost *h, DlStream *s, const char *dir)
{
    if (read_exact(dir, "ee.bin", ee_ptr(h, 0, EE_RAM_SIZE), EE_RAM_SIZE) != 0 ||
        read_exact(dir, "spr.bin", ee_ptr(h, EE_SPR_BASE, EE_SPR_SIZE), EE_SPR_SIZE) != 0) {
        return -1;
    }
    if (s) {
        return read_exact(dir, "vram.bin", dl_stream_gs_mem(s)->m, GS_MEM_SIZE);
    }
    return 0;
}

/* --frame-tag (for measuring the display, tools/latency/cadence.swift): the
   field of the frame shown, written into the window's top left corner as 16
   squares of 16 pixels, white for 1, between a white square and a black one,
   so a capture of the window says which field each refresh showed. */
static SDL_GPUTexture *tag_tex;   /* 2x1: white, black */

static int tag_init(SDL_GPUDevice *dev)
{
    SDL_GPUTextureCreateInfo ci;
    SDL_GPUTransferBufferCreateInfo tb;
    SDL_GPUTransferBuffer *buf;
    SDL_GPUCommandBuffer *cmd;
    SDL_GPUCopyPass *cp;
    SDL_GPUTextureTransferInfo src;
    SDL_GPUTextureRegion dst;
    uint8_t *map;
    static const uint8_t px[8] = { 255, 255, 255, 255, 0, 0, 0, 255 };

    memset(&ci, 0, sizeof(ci));
    ci.type = SDL_GPU_TEXTURETYPE_2D, ci.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER, ci.width = 2, ci.height = 1, ci.layer_count_or_depth = 1;
    ci.num_levels = 1;
    memset(&tb, 0, sizeof(tb));
    tb.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, tb.size = sizeof(px);
    if (!(tag_tex = SDL_CreateGPUTexture(dev, &ci)) || !(buf = SDL_CreateGPUTransferBuffer(dev, &tb))) {
        return -1;
    }
    map = SDL_MapGPUTransferBuffer(dev, buf, false);
    if (!map || !(cmd = SDL_AcquireGPUCommandBuffer(dev))) {
        return -1;
    }
    memcpy(map, px, sizeof(px));
    SDL_UnmapGPUTransferBuffer(dev, buf);
    memset(&src, 0, sizeof(src));
    src.transfer_buffer = buf;
    memset(&dst, 0, sizeof(dst));
    dst.texture = tag_tex, dst.w = 2, dst.h = 1, dst.d = 1;
    cp = SDL_BeginGPUCopyPass(cmd);
    SDL_UploadToGPUTexture(cp, &src, &dst, false);
    SDL_EndGPUCopyPass(cp);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(dev, buf);
    return 0;
}

static void tag_draw(SDL_GPUCommandBuffer *cmd, SDL_GPUTexture *sc, uint32_t tag)
{
    int i;

    for (i = 0; i < 18; i++) {
        int one = i == 0 ? 1 : i == 17 ? 0 : (int)((tag >> (15 - (i - 1))) & 1);
        SDL_GPUBlitInfo bi;

        memset(&bi, 0, sizeof(bi));
        bi.source.texture = tag_tex, bi.source.x = one ? 0 : 1, bi.source.w = 1, bi.source.h = 1;
        bi.destination.texture = sc, bi.destination.x = (Uint32)i * 16, bi.destination.w = 16;
        bi.destination.h = 16;
        bi.load_op = SDL_GPU_LOADOP_LOAD;
        bi.filter = SDL_GPU_FILTER_NEAREST;
        SDL_BlitGPUTexture(cmd, &bi);
    }
}

/* The output at 4:3, centred: as large as fits ("fit"), or the largest whole
   multiple of the PS2's 448 lines that fits ("integer"; fit if none does). */
/* The game image's size in a sw x sh window (pixels): 4:3, as large as
   fits, an integer multiple of 448 lines if asked and it fits. Centred. */
static void image_size(Uint32 sw, Uint32 sh, int integer, Uint32 *ow, Uint32 *oh)
{
    Uint32 w = sw, h = sw * 3 / 4, k = sh / DL_FB_H;

    if (h > sh) {
        h = sh, w = sh * 4 / 3;
    }
    if (integer && k >= 1) {
        while (k > 1 && DL_FB_H * k * 4 / 3 > sw) {
            k--;
        }
        if (DL_FB_H * k * 4 / 3 <= sw) {
            h = DL_FB_H * k, w = h * 4 / 3;
        }
    }
    *ow = w, *oh = h;
}

/* With fence, the fence of the GPU work that puts the frame in the swapchain. */
static void present(SDL_GPUDevice *dev, SDL_Window *win, DlGpu *gpu, int have_frame, int integer, int64_t tag,
                    SDL_GPUFence **fence)
{
    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(dev);
    SDL_GPUTexture *sc;
    Uint32 sw, sh, ow, oh;

    if (!cmd) {
        return;
    }
    if (SDL_WaitAndAcquireGPUSwapchainTexture(cmd, win, &sc, &sw, &sh) && sc) {
        SDL_GPUBlitInfo bi;
        SDL_GPUTexture *out = dl_gpu_output(gpu, &ow, &oh);
        Uint32 w, h;

        image_size(sw, sh, integer, &w, &h);
        memset(&bi, 0, sizeof(bi));
        bi.load_op = SDL_GPU_LOADOP_CLEAR;
        bi.clear_color.a = 1.0f;
        bi.filter = SDL_GPU_FILTER_NEAREST;
        if (have_frame) {
            bi.source.texture = out, bi.source.w = ow, bi.source.h = oh;
            bi.destination.texture = sc;
            bi.destination.x = (sw - w) / 2, bi.destination.y = (sh - h) / 2;
            bi.destination.w = w, bi.destination.h = h;
            SDL_BlitGPUTexture(cmd, &bi);
        }
        if (tag >= 0 && tag_tex) {
            tag_draw(cmd, sc, (uint32_t)tag);
        }
    }
    if (fence) {
        *fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);
    } else {
        SDL_SubmitGPUCommandBuffer(cmd);
    }
}

/* --present: how frames reach the screen. vsync waits for the display;
   mailbox replaces a frame still waiting; immediate doesn't wait, which on
   a VRR display is a refresh at the Core's own 59.94 Hz. */
static SDL_GPUPresentMode present_mode(const char *name)
{
    return !strcmp(name, "immediate") ? SDL_GPU_PRESENTMODE_IMMEDIATE
           : !strcmp(name, "mailbox") ? SDL_GPU_PRESENTMODE_MAILBOX
                                      : SDL_GPU_PRESENTMODE_VSYNC;
}

/* The next job of an --until list ("A,B,C") into want; 0 when none is left. */
static int until_next(const char **list, char *want, size_t cap)
{
    const char *p = *list, *comma;
    size_t n;

    want[0] = 0;
    if (!p || !*p) {
        return 0;
    }
    comma = strchr(p, ',');
    n = comma ? (size_t)(comma - p) : strlen(p);
    if (n >= cap) {
        n = cap - 1;
    }
    memcpy(want, p, n);
    want[n] = 0;
    *list = comma ? comma + 1 : p + strlen(p);
    return 1;
}

/* The English text layer, bound to the host's disc. NULL if it can't be
   loaded (the reason on stderr). */
static TextLayer *open_text(CoreHost *h, const char *dir)
{
    char base[1024], lang[1100], fonts[1100], err[4096];
    TextLayer *t;
    int n;

    if (!dir) {
        const char *b = SDL_GetBasePath();

        snprintf(base, sizeof(base), "%stext", b ? b : "");
        dir = base;
    }
    if (fs_join(lang, sizeof(lang), dir, "/", "en/main.lang") != 0 ||
        fs_join(fonts, sizeof(fonts), dir, "/", "fonts") != 0) {
        return NULL;
    }
    t = tl_open(RELEASE_ID, lang, fonts, err, sizeof(err));
    if (!t) {
        fprintf(stderr, "ggx: the English text: %s\n", err);
        return NULL;
    }
    n = tl_bind_host(t, h, err, sizeof(err));
    if (n < 0) {
        fprintf(stderr, "ggx: the English text: %s\n", err);
        tl_free(t);
        return NULL;
    }
    if (err[0]) {
        fprintf(stderr, "ggx: the English text: some pictures are not on this disc:\n%s", err);
    }
    fprintf(stderr, "ggx: English text for %d pictures\n", n);
    return t;
}

/* What the window does between fields (PORT-010): its settings, the input
   layer, and the hotkeys. */
/* The renderer's substitutions (DlSubst): the button prompts redraw the
   HUD font's symbols, the English text the text pictures; no upload is
   both. */
typedef struct {
    Prompts *prompts;
    TextLayer *text;
    int text_on;
} Subst;

static const uint8_t *subst_both(void *ctx, uint64_t hash, int psm, uint32_t w, uint32_t h, const uint8_t *texels,
                                 uint32_t variant)
{
    Subst *b = ctx;
    const uint8_t *r = b->prompts ? pr_subst(b->prompts, hash, psm, w, h, texels, variant) : NULL;

    if (!r && b->text && b->text_on) {
        r = tl_subst(b->text, hash, psm, w, h, texels, variant);
    }
    return r;
}

static void set_subst(DlStream *s, Subst *b)
{
    int any = b->prompts || (b->text && b->text_on);

    dl_stream_set_subst(s, any ? subst_both : NULL, any ? b : NULL);
    dl_stream_set_variant(s, b->prompts ? pr_variant : NULL, b->prompts);
}

/* The button prompts (PORT-017), from the HUD font on the host's disc.
   NULL (and the PS2's symbols) if the font isn't there. */
static Prompts *open_prompts(CoreHost *h)
{
    TlRawPicture digits, symbols;
    Prompts *p = NULL;
    char err[256];

    if (tl_host_picture(h, PR_FONT_FILE, PR_FONT_DIGITS, &digits) == 0 &&
        tl_host_picture(h, PR_FONT_FILE, PR_FONT_SYMBOLS, &symbols) == 0) {
        if (!(p = pr_open(&digits, &symbols, err, sizeof(err)))) {
            fprintf(stderr, "ggx: button prompts: %s\n", err);
        }
    } else {
        fprintf(stderr, "ggx: button prompts: the HUD font isn't on this disc\n");
    }
    free(digits.texels);
    free(symbols.texels);
    return p;
}

/* The game's KEY CONFIG (userinfo.key[pad], USER_KEYSET: PUNCH, KICK,
   SLASH, HSLASH first) for the input layer's macros. Read straight from
   the Core's memory, read only, as the text layer reads seekpos. */
static void game_keys(CoreHost *h, Input *in)
{
    int p;

    for (p = 0; p < 2; p++) {
        const uint8_t *k = h->mem + ggx_sym_userinfo + 44 * p;
        uint32_t w[4];
        MacroKeys m;
        int i;

        for (i = 0; i < 4; i++) {
            w[i] = k[4 * i] | (uint32_t)k[4 * i + 1] << 8 | (uint32_t)k[4 * i + 2] << 16 | (uint32_t)k[4 * i + 3] << 24;
        }
        m.punch = w[0], m.kick = w[1], m.slash = w[2], m.hslash = w[3];
        input_set_game_keys(in, p, &m);
    }
}

/* The settings panel's overlay, when what it shows changes. */
static void show_settings(Settings *s, DlGpu *gpu)
{
    int changed;
    const uint8_t *o = settings_overlay(s, &changed);

    if (changed) {
        dl_gpu_set_overlay(gpu, o);
    }
}

/* Each player's letters from the input layer, and who the shared screens
   follow. */
static void update_prompts(Prompts *p, const Input *in)
{
    char l[PR_NBUTTONS];
    int i;

    if (!p || !in) {
        return;
    }
    for (i = 0; i < 2; i++) {
        char h[PR_NBUTTONS];

        input_prompt_letters(in, i, l);
        pr_set(p, i, l);
        /* the help lines, where X confirms: the X button in Circle's place, and the other way round */
        memcpy(h, l, sizeof(h));
        if (input_confirm(in, i) == CONFIRM_X) {
            h[PR_CIRCLE] = l[PR_CROSS] ? l[PR_CROSS] : PR_SYMBOL(PR_CROSS);
            h[PR_CROSS] = l[PR_CIRCLE] ? l[PR_CIRCLE] : PR_SYMBOL(PR_CIRCLE);
        }
        pr_set_help(p, i, h);
    }
    pr_set_shared(p, input_last_player(in));
}

typedef struct {
    SDL_Window *win;
    Config *cfg;
    Input *input;
    DlStream *stream;
    TextLayer *text;
    int text_on;
    Subst *sub;
    Settings *settings;
    int fullscreen, integer;
    int fast;              /* developer builds: Tab runs the Core unthrottled */
    int quit;
    int vsync;             /* frames wait for the display's vblank */
    int pacing;            /* pace the Core to the display where it can (pacing = display) */
    SDL_DisplayID display; /* the display the window is on, and its refresh rate */
    float hz;
    VBlank *vb;
    Pace pace;
    double fake_hz;        /* --fake-vblank: a made-up display, for testing */
    SDL_GPUDevice *dev;
    SDL_GPUPresentMode want_mode, mode;   /* the present mode asked for, and the swapchain's */
    FILE *vlog;            /* --vblank-log */
} Window;

/* The first run's screens (firstrun.h): a 640x448 picture, scaled as the
   game's is. */
static SDL_GPUTexture *fr_tex;

static void first_run_show(void *ctx, const uint8_t *rgba)
{
    Window *w = ctx;
    SDL_GPUTransferBufferCreateInfo tb;
    SDL_GPUTransferBuffer *buf;
    SDL_GPUCommandBuffer *cmd;
    SDL_GPUTexture *sc;
    Uint32 sw, sh;
    uint8_t *map;

    if (!fr_tex) {
        SDL_GPUTextureCreateInfo ci;

        memset(&ci, 0, sizeof(ci));
        ci.type = SDL_GPU_TEXTURETYPE_2D, ci.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER, ci.width = FR_W, ci.height = FR_H, ci.layer_count_or_depth = 1;
        ci.num_levels = 1;
        if (!(fr_tex = SDL_CreateGPUTexture(w->dev, &ci))) {
            return;
        }
    }
    memset(&tb, 0, sizeof(tb));
    tb.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, tb.size = FR_W * FR_H * 4;
    if (!(buf = SDL_CreateGPUTransferBuffer(w->dev, &tb))) {
        return;
    }
    if ((map = SDL_MapGPUTransferBuffer(w->dev, buf, false)) != NULL) {
        memcpy(map, rgba, (size_t)FR_W * FR_H * 4);
        SDL_UnmapGPUTransferBuffer(w->dev, buf);
    }
    if (map && (cmd = SDL_AcquireGPUCommandBuffer(w->dev)) != NULL) {
        SDL_GPUTextureTransferInfo src;
        SDL_GPUTextureRegion dst;
        SDL_GPUCopyPass *cp;

        memset(&src, 0, sizeof(src));
        src.transfer_buffer = buf;
        memset(&dst, 0, sizeof(dst));
        dst.texture = fr_tex, dst.w = FR_W, dst.h = FR_H, dst.d = 1;
        cp = SDL_BeginGPUCopyPass(cmd);
        SDL_UploadToGPUTexture(cp, &src, &dst, false);
        SDL_EndGPUCopyPass(cp);
        if (SDL_WaitAndAcquireGPUSwapchainTexture(cmd, w->win, &sc, &sw, &sh) && sc) {
            SDL_GPUBlitInfo bi;
            Uint32 iw, ih;

            image_size(sw, sh, w->integer, &iw, &ih);
            memset(&bi, 0, sizeof(bi));
            bi.load_op = SDL_GPU_LOADOP_CLEAR;
            bi.clear_color.a = 1.0f;
            bi.filter = SDL_GPU_FILTER_NEAREST;
            bi.source.texture = fr_tex, bi.source.w = FR_W, bi.source.h = FR_H;
            bi.destination.texture = sc;
            bi.destination.x = (sw - iw) / 2, bi.destination.y = (sh - ih) / 2;
            bi.destination.w = iw, bi.destination.h = ih;
            SDL_BlitGPUTexture(cmd, &bi);
        }
        SDL_SubmitGPUCommandBuffer(cmd);
    }
    SDL_ReleaseGPUTransferBuffer(w->dev, buf);
}

static void window_title(Window *w)
{
    const char *prompt = input_remap_prompt(w->input);

    SDL_SetWindowTitle(w->win, prompt ? prompt : w->fast ? "Guilty Gear X Plus (fast)" : "Guilty Gear X Plus");
}

static void save_settings(Window *w)
{
    config_set(w->cfg, "window", w->fullscreen ? "fullscreen" : "windowed");
    config_set(w->cfg, "scaling", w->integer ? "integer" : "fit");
    if (config_save(w->cfg) != 0) {
        fprintf(stderr, "ggx: cannot save %s\n", config_path(w->cfg));
    }
}

/* Fullscreen or back to the window as it was (SDL keeps its size and
   place), saved as "window =". */
static void set_fullscreen(Window *w, int on)
{
    w->fullscreen = on;
    SDL_SetWindowFullscreen(w->win, on);
    save_settings(w);
}

/* A point (window coordinates) in the middle half of the game image, where
   a double-click toggles fullscreen: clicks near the edges, the window's
   own controls or the bars beside the picture don't. */
static int in_image_middle(Window *w, float x, float y)
{
    int pw, ph;
    Uint32 iw, ih;
    float d = SDL_GetWindowPixelDensity(w->win), cx, cy;

    if (!SDL_GetWindowSizeInPixels(w->win, &pw, &ph) || d <= 0) {
        return 0;
    }
    image_size((Uint32)pw, (Uint32)ph, w->integer, &iw, &ih);
    cx = x * d - pw / 2.0f, cy = y * d - ph / 2.0f;
    return cx >= -(float)iw / 4 && cx <= (float)iw / 4 && cy >= -(float)ih / 4 && cy <= (float)ih / 4;
}

/* The display's number, from 1, as SDL lists them (display = N). */
static int display_number(SDL_DisplayID id)
{
    int n = 0, i, k = 0;
    SDL_DisplayID *ids = SDL_GetDisplays(&n);

    for (i = 0; ids && i < n; i++) {
        if (ids[i] == id) {
            k = i + 1;
        }
    }
    SDL_free(ids);
    return k;
}

/* The display a "display" setting names: its number, or a part of its name
   (any case); 0 and the list of displays if none matches. */
static SDL_DisplayID display_named(const char *want)
{
    int n = 0, i;
    SDL_DisplayID *ids = SDL_GetDisplays(&n), id = 0;
    char *end;
    long k = strtol(want, &end, 10);

    for (i = 0; ids && i < n && !id; i++) {
        const char *name = SDL_GetDisplayName(ids[i]);

        if (*want && ((*end == 0 && k == i + 1) || (*end && name && SDL_strcasestr(name, want)))) {
            id = ids[i];
        }
    }
    if (!id) {
        if (*want) {
            fprintf(stderr, "ggx: no display is \"%s\"; the displays are:\n", want);
        } else {
            fprintf(stderr, "ggx: the displays are:\n");
        }
        for (i = 0; ids && i < n; i++) {
            const SDL_DisplayMode *m = SDL_GetCurrentDisplayMode(ids[i]);

            fprintf(stderr, "ggx:   %d  %s, %dx%d at %.2f Hz\n", i + 1, SDL_GetDisplayName(ids[i]),
                    m ? m->w : 0, m ? m->h : 0, m ? m->refresh_rate : 0.0f);
        }
    }
    SDL_free(ids);
    return id;
}

/* Paced, mailbox where the swapchain has it: a frame is shown at the next
   vblank and one still waiting is replaced, so the queue never holds more
   than one. With vsync a frame missing its vblank stays queued, and a frame a
   vblank keeps it there: every frame a refresh or two late from then on
   (36 ms on one test display). By the clock, the mode asked for. */
static void swapchain_mode(Window *w)
{
    SDL_GPUPresentMode mode = w->pace.k && w->want_mode == SDL_GPU_PRESENTMODE_VSYNC &&
                                      SDL_WindowSupportsGPUPresentMode(w->dev, w->win, SDL_GPU_PRESENTMODE_MAILBOX)
                                  ? SDL_GPU_PRESENTMODE_MAILBOX
                                  : w->want_mode;

    if (mode != w->mode && SDL_SetGPUSwapchainParameters(w->dev, w->win, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, mode)) {
        w->mode = mode;
    }
}

/* The display the window is on, when the window opens and whenever it moves
   to another display or the display's mode changes: its refresh rate, and
   how the Core is paced on it (pace.h). A fullscreen window moved to another
   display (dragged, or Win+Shift+arrow) goes fullscreen again there, so it
   covers that display exactly: a window that doesn't is composed by the
   system with the rest of the desktop, a refresh later, where a covering one
   is flipped straight to the display. */
static void display_update(Window *w)
{
    SDL_DisplayID id = SDL_GetDisplayForWindow(w->win);
    const SDL_DisplayMode *m = SDL_GetCurrentDisplayMode(id);
    const char *name = SDL_GetDisplayName(id), *why = NULL;
    float hz = m ? m->refresh_rate : 0.0f, k = hz / 59.94f;

    if (!id || (id == w->display && hz == w->hz)) {
        return;
    }
    if (w->display && id != w->display && w->fullscreen) {
        w->display = id;   /* the events this raises find nothing to do */
        SDL_SetWindowFullscreen(w->win, false);
        SDL_SetWindowPosition(w->win, SDL_WINDOWPOS_CENTERED_DISPLAY(id), SDL_WINDOWPOS_CENTERED_DISPLAY(id));
        SDL_SetWindowFullscreen(w->win, true);
        SDL_SyncWindow(w->win);
        fprintf(stderr, "ggx: fullscreen again on the display the window moved to\n");
    }
    w->display = id;
    w->hz = hz;
    vblank_close(w->vb);
    w->vb = NULL;
    if (w->fake_hz > 0) {
        hz = (float)w->fake_hz, k = hz / 59.94f;
        fprintf(stderr, "ggx: --fake-vblank: a made-up display at %.2f Hz, not the real one\n", hz);
    }
    pace_init(&w->pace, hz, SDL_GetPerformanceFrequency(), w->pacing && w->vsync);
    if (w->pace.k && !(w->vb = w->fake_hz > 0 ? vblank_fake(w->fake_hz) : vblank_open(w->win, hz, w->vlog))) {
        pace_init(&w->pace, hz, SDL_GetPerformanceFrequency(), 0);
        why = "this system gives no vblanks to follow";
    }
    swapchain_mode(w);
    fprintf(stderr, "ggx: on display %d (%s), %.2f Hz: %.2f refreshes a field\n", display_number(id),
            name ? name : "?", hz, k);
    if (w->pace.k) {
        fprintf(stderr, "ggx: paced by the display: a field every %d refresh%s, presented %s\n", w->pace.k,
                w->pace.k > 1 ? "es" : "",
                w->mode == SDL_GPU_PRESENTMODE_MAILBOX ? "mailbox (none queued behind another)" : "with vsync");
        return;
    }
    why = why                ? why
          : !w->vsync        ? "the present mode doesn't wait for vblanks"
          : !w->pacing       ? "the default (pacing = display follows the display's vblanks, on Windows)"
          : hz <= 0.0f       ? "the display's rate is unknown"
                             : "the rate isn't a whole multiple of the field rate";
    fprintf(stderr, "ggx: paced by the 59.94 Hz clock: %s\n", why);
    if (w->vsync && hz > 0.0f && fabsf(k - floorf(k + 0.5f)) > 0.02f) {
        fprintf(stderr, "ggx: frames can't all stay on screen equally long at this rate, so smooth scrolling "
                        "judders: 60 or 120 Hz does better, or --present immediate on a VRR display\n");
    }
}

static void on_event(Window *w, const SDL_Event *e)
{
    SDL_Scancode k;

    if (e->type == SDL_EVENT_QUIT) {
        w->quit = 1;
        return;
    }
    if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        /* the mouse is never game input: only this */
        if (e->button.button == SDL_BUTTON_LEFT && e->button.clicks == 2 && in_image_middle(w, e->button.x, e->button.y)) {
            set_fullscreen(w, !w->fullscreen);
        }
        return;
    }
    if (e->type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN || e->type == SDL_EVENT_WINDOW_LEAVE_FULLSCREEN) {
        /* the system's own fullscreen control too (macOS's green button) */
        int on = e->type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN;

        if (on != w->fullscreen) {
            w->fullscreen = on;
            save_settings(w);
        }
        return;
    }
    if (e->type == SDL_EVENT_WINDOW_DISPLAY_CHANGED || e->type == SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED) {
        display_update(w);
        return;
    }
    if (input_event(w->input, e)) {
        window_title(w);
        return;
    }
    if (e->type == SDL_EVENT_KEY_DOWN && !e->key.repeat && w->settings &&
        (e->key.scancode == SDL_SCANCODE_F9 || (e->key.scancode == SDL_SCANCODE_ESCAPE && settings_is_open(w->settings)))) {
        /* the port's settings panel (settings.h) */
        if (settings_is_open(w->settings)) {
            settings_close(w->settings);
        } else if (e->key.scancode == SDL_SCANCODE_F9) {
            settings_open(w->settings);
        }
        return;
    }
    if (e->type != SDL_EVENT_KEY_DOWN || e->key.repeat) {
        return;
    }
    k = e->key.scancode;
    if (k == SDL_SCANCODE_F11 || (k == SDL_SCANCODE_RETURN && (e->key.mod & SDL_KMOD_ALT))) {
        set_fullscreen(w, !w->fullscreen);
    } else if (k == SDL_SCANCODE_F6) {
        w->integer = !w->integer;
        fprintf(stderr, "ggx: %s scaling\n", w->integer ? "integer" : "fit");
        save_settings(w);
    } else if (k == SDL_SCANCODE_F2 && w->text) {
        /* the English text layer on or off: only the renderer's textures change */
        w->text_on = !w->text_on;
        w->sub->text_on = w->text_on;
        set_subst(w->stream, w->sub);
        fprintf(stderr, "ggx: %s text\n", w->text_on ? "English" : "original");
    } else if (k == SDL_SCANCODE_F3 || k == SDL_SCANCODE_F4) {
        input_remap_start(w->input, k == SDL_SCANCODE_F4);
        window_title(w);
    } else if (k == SDL_SCANCODE_F5) {
        input_swap_players(w->input);
    } else if (k == SDL_SCANCODE_F7 || k == SDL_SCANCODE_F8) {
        input_toggle_analog(w->input, k == SDL_SCANCODE_F8);
        if (config_save(w->cfg) != 0) {
            fprintf(stderr, "ggx: cannot save %s\n", config_path(w->cfg));
        }
#ifdef GGX_DEV_BUILD
    } else if (k == SDL_SCANCODE_TAB) {
        w->fast = !w->fast;
        window_title(w);
#endif
    }
}

/* After a field: the sound driver renders its 800.8 samples (a field is a
   59.94 Hz tick), and in a window they go to the speakers. */
static void play_sound(AudioHost *a, AudioOut *o, uint32_t field)
{
    const int16_t *pcm;
    uint32_t n;

    audio_host_field(a, field, 1);
    pcm = audio_host_take(a, &n);
    audio_out_put(o, pcm, n);
}

int main(int argc, char **argv)
{
    CoreHostConfig cfg;
    static CoreHost host;
    static uint32_t dumps[MAX_DUMPS];
    char pref[1024], cfg_default[1100], want[64], save_dir[1100];
    const char *extensions[CORE_MAX_EXTENSIONS];
    int n_ext = 0;
    const char *until = NULL, *log_path = NULL, *dump_dir = NULL, *ps2_state = NULL, *gpu_name = NULL;
    const char *replay_path = NULL, *hash_path = NULL, *gsdump_path = NULL, *gsdump_tmpl = NULL;
    const char *text_dir = NULL, *trace_path = NULL, *config_file = NULL, *script_path = NULL;
    const char *window_opt = NULL, *scaling_opt = NULL, *present_opt = NULL, *forbid = NULL, *audio_opt = NULL;
    AudioHostConfig acfg = { 0 };
    AudioHost *audio = NULL;
    AudioOut *speakers = NULL;
    int text_on = 0, text_asked = 0;
    TextLayer *text = NULL;
    Trace *trace = NULL;
    GgScript *script = NULL;
    int script_on = 0;
    uint32_t trace_n = 0, trace_bad = 0;
    unsigned long gsdump_from = 0, gsdump_to = 0;
    static GsDump gsd;
    unsigned long fields = 0;
    uint64_t seed = 0;
    int mash = 0;
    FILE *hash_log = NULL, *times = NULL;
    const char *times_path = NULL;
    uint64_t t_wait = 0, t0 = 0, t_core = 0, t_sound = 0, t_render = 0, t_present = 0;
    uint32_t vs0 = 0;
    uint64_t tex0 = 0, texels0 = 0, batches0 = 0, over0 = 0, pc0 = 0;
    int i, ndumps = 0, rc = 0, reached = 0, was = 0, no_log = 0, offscreen = 0, render = 1, stats = 0;
    int no_focus = 0, render_all = 0, vsync = 1, frame_tag = 0, in_flight = 1;
    const char *display_opt = NULL, *pacing_opt = NULL, *first_run_img = NULL;
    int first_run = 0;
    double lead_ms = 0;
    int64_t shown_field = -1;
    uint64_t field_pc = 0, vb_last = 0, vb_period = 0;
    int paced = 0, unshown = 0, just_shown = 0;
    DlGpuOptions gopt = { 1, 1 };
    SDL_GPUDevice *dev = NULL;
    DlStream *stream = NULL;
    DlGpu *gpu = NULL;
    Hook hook;
    Window w;
    static Subst sub;
    const char *prompts_opt = NULL;
    int confirm_opt = -1, settings_open_opt = 0;
    const char *unlock_opt = NULL;
    Session sess;
    const char *record_path = NULL, *play_path = NULL, *from_opt = NULL, *replay_dir = NULL;
    Replay *rec = NULL, *play = NULL;
    int auto_replays = 0, play_bad = 0;
    uint32_t play_skip = 0;
    int unlocks_asked = 0;
    ConfirmLatch script_latch[2];
    uint32_t drawn = 0;
    uint64_t hash, last = 0, start = 0;
    int64_t acc = 0;
    uint32_t late = 0;
    uint8_t *pixels = NULL;

    memset(&cfg, 0, sizeof(cfg));
    memset(&w, 0, sizeof(w));
    cfg.cards[0] = CORE_CARD_STORE;
    for (i = 1; i < argc; i++) {
        const char *a = argv[i];

        if (!strcmp(a, "--no-log")) {
            no_log = 1;
        } else if (!strcmp(a, "--offscreen")) {
            offscreen = 1;
        } else if (!strcmp(a, "--no-render")) {
            render = 0;
        } else if (!strcmp(a, "--render-all")) {
            render_all = 1;
        } else if (!strcmp(a, "--settings")) {
            settings_open_opt = 1;
        } else if (!strcmp(a, "--no-focus")) {
            no_focus = 1;
        } else if (!strcmp(a, "--frame-tag")) {
            frame_tag = 1;
        } else if (!strcmp(a, "--stats")) {
            stats = 1;
        } else if (!strcmp(a, "--fullscreen") || !strcmp(a, "--windowed")) {
            window_opt = a + 2;
        } else if (i + 1 >= argc) {
            usage();
        } else if (!strcmp(a, "--frame-times")) {
            times_path = argv[++i];
        } else if (!strcmp(a, "--data-dir")) {
            cfg.data_dir = argv[++i];
        } else if (!strcmp(a, "--disc")) {
            cfg.disc_path = argv[++i];
        } else if (!strcmp(a, "--first-run")) {
            first_run_img = argv[++i];
        } else if (!strcmp(a, "--elf")) {
            cfg.elf_path = argv[++i];
        } else if (!strcmp(a, "--fields")) {
            fields = strtoul(argv[++i], NULL, 10);
        } else if (!strcmp(a, "--until")) {
            until = argv[++i];
        } else if (!strcmp(a, "--log")) {
            log_path = argv[++i];
        } else if (!strcmp(a, "--card1") || !strcmp(a, "--card2")) {
            int slot = a[6] - '1';
            const char *v = argv[++i];

            if (core_card_kind(v) < 0 || (slot == 1 && core_card_kind(v) == CORE_CARD_STORE)) {
                usage();
            }
            cfg.cards[slot] = core_card_kind(v);
        } else if (!strcmp(a, "--save-dir")) {
            cfg.save_dir = argv[++i];
        } else if (!strcmp(a, "--extension")) {
            if (n_ext == CORE_MAX_EXTENSIONS) {
                usage();
            }
            extensions[n_ext++] = argv[++i];
        } else if (!strcmp(a, "--scale")) {
            gopt.scale = atoi(argv[++i]);
            if (gopt.scale < 1 || gopt.scale > 8) {
                usage();
            }
        } else if (!strcmp(a, "--merge")) {
            const char *v = argv[++i];

            if (strcmp(v, "on") && strcmp(v, "off")) {
                usage();
            }
            gopt.display_merge = !strcmp(v, "on");
        } else if (!strcmp(a, "--gpu")) {
            gpu_name = argv[++i];
        } else if (!strcmp(a, "--dump")) {
            char *p = argv[++i];

            while (*p && ndumps < MAX_DUMPS) {
                dumps[ndumps++] = (uint32_t)strtoul(p, &p, 10);
                if (*p == ',') {
                    p++;
                }
            }
        } else if (!strcmp(a, "--dump-dir")) {
            dump_dir = argv[++i];
        } else if (!strcmp(a, "--ps2-state")) {
            ps2_state = argv[++i];
        } else if (!strcmp(a, "--gs-dump")) {
            gsdump_path = argv[++i];
        } else if (!strcmp(a, "--gs-dump-template")) {
            gsdump_tmpl = argv[++i];
        } else if (!strcmp(a, "--gs-dump-fields")) {
            char *p = argv[++i];

            gsdump_from = strtoul(p, &p, 10);
            gsdump_to = *p == ',' ? strtoul(p + 1, NULL, 10) : gsdump_from + 1;
        } else if (!strcmp(a, "--hash-log")) {
            hash_path = argv[++i];
        } else if (!strcmp(a, "--mash")) {
            mash = 1;
            seed = strtoull(argv[++i], NULL, 0);
        } else if (!strcmp(a, "--script")) {
            script_path = argv[++i];
        } else if (!strcmp(a, "--forbid")) {
            forbid = argv[++i];
        } else if (!strcmp(a, "--text")) {
            const char *v = argv[++i];

            if (strcmp(v, "en") && strcmp(v, "off")) {
                usage();
            }
            text_on = !strcmp(v, "en");
            text_asked = 1;
        } else if (!strcmp(a, "--text-dir")) {
            text_dir = argv[++i];
            text_asked = 1;
        } else if (!strcmp(a, "--trace")) {
            trace_path = argv[++i];
        } else if (!strcmp(a, "--replay")) {
            replay_path = argv[++i];
            offscreen = 1;
        } else if (!strcmp(a, "--record")) {
            record_path = argv[++i];
        } else if (!strcmp(a, "--replay-dir")) {
            replay_dir = argv[++i];
        } else if (!strcmp(a, "--play")) {
            play_path = argv[++i];
        } else if (!strcmp(a, "--from")) {
            from_opt = argv[++i];
            if (strcmp(from_opt, "start") && strcmp(from_opt, "fight")) {
                usage();
            }
        } else if (!strcmp(a, "--unlock")) {
            unlock_opt = argv[++i];
            if (unlock_parse(unlock_opt) < 0) {
                usage();
            }
        } else if (!strcmp(a, "--confirm")) {
            confirm_opt = confirm_parse(argv[++i]);
            if (confirm_opt < 0) {
                usage();
            }
        } else if (!strcmp(a, "--prompts")) {
            prompts_opt = argv[++i];
            if (strcmp(prompts_opt, "auto") && input_family_parse(prompts_opt) < 0) {
                usage();
            }
        } else if (!strcmp(a, "--config")) {
            config_file = argv[++i];
        } else if (!strcmp(a, "--scaling")) {
            scaling_opt = argv[++i];
            if (strcmp(scaling_opt, "fit") && strcmp(scaling_opt, "integer")) {
                usage();
            }
        } else if (!strcmp(a, "--audio")) {
            audio_opt = argv[++i];
            if (strcmp(audio_opt, "on") && strcmp(audio_opt, "off")) {
                usage();
            }
        } else if (!strcmp(a, "--snd-log")) {
            acfg.snd_log = argv[++i];
        } else if (!strcmp(a, "--audio-out")) {
            acfg.wav = argv[++i];
        } else if (!strcmp(a, "--display")) {
            display_opt = argv[++i];
        } else if (!strcmp(a, "--pacing")) {
            pacing_opt = argv[++i];
            if (strcmp(pacing_opt, "display") && strcmp(pacing_opt, "clock")) {
                usage();
            }
        } else if (!strcmp(a, "--vblank-log")) {
            if (!(w.vlog = fs_open(argv[++i], "wb"))) {
                fprintf(stderr, "ggx: cannot write %s\n", argv[i]);
                return 1;
            }
            fprintf(w.vlog, "# ggx --vblank-log: the performance counter at %llu ticks a second\n"
                            "# wake,<counter>: a WaitForVBlank return\n"
                            "# field,<field>,<now>,<grid vblank>,<grid period>,<target>,<start at>,<lead>,<late>,<paced k>\n"
                            "# frame,<field>,<target>,<its GPU work done>,<held before presenting>\n",
                    (unsigned long long)SDL_GetPerformanceFrequency());
        } else if (!strcmp(a, "--vblank-source")) {
            vblank_prefer(argv[++i]);
        } else if (!strcmp(a, "--fake-vblank")) {
            w.fake_hz = atof(argv[++i]);
        } else if (!strcmp(a, "--pace-lead-ms")) {
            lead_ms = atof(argv[++i]);
        } else if (!strcmp(a, "--frames-in-flight")) {
            in_flight = atoi(argv[++i]);
            if (in_flight < 1 || in_flight > 3) {
                usage();
            }
        } else if (!strcmp(a, "--present")) {
            present_opt = argv[++i];
            if (strcmp(present_opt, "vsync") && strcmp(present_opt, "mailbox") && strcmp(present_opt, "immediate")) {
                usage();
            }
        } else {
            usage();
        }
    }
    if ((no_log && log_path) || (cfg.disc_path && cfg.data_dir) || (ndumps && !dump_dir) || (ndumps && !render) ||
        (replay_path && (!dump_dir || !render)) ||
        (gsdump_path && (!gsdump_tmpl || !dump_dir || !render || gsdump_to <= gsdump_from)) ||
        (text_on && !render) || ((trace_path != NULL) + mash + (script_path != NULL) + (play_path != NULL) > 1) ||
        (record_path && (trace_path || play_path)) || (from_opt && !play_path) ||
        (play_path && (n_ext || unlock_opt || cfg.cards[0] != CORE_CARD_STORE || cfg.cards[1] != CORE_CARD_NONE)) ||
        (first_run_img && (cfg.disc_path || replay_path || trace_path || !render))) {
        usage();
    }
    if (play_path) {
        char why[512];
        static const char *ids[RP_MAX_EXTENSIONS];

        if (!(play = rp_open(play_path, why, sizeof(why)))) {
            fprintf(stderr, "ggx: %s can't be played: %s\n", play_path, why);
            return 6;
        }
        /* the replay's cards and extensions: nothing reaches the save store */
        rp_host_config(play, cfg.cards, ids, &n_ext);
        for (i = 0; i < n_ext; i++) {
            extensions[i] = ids[i];
        }
        if (!fields || fields > rp_fields(play)) {
            fields = rp_fields(play);
        }
    }
    if (offscreen && !fields && !until && !trace_path && !script_path && !play_path) {
        fields = 3600;
    }
    if (!cfg.disc_path && !cfg.data_dir) {
        if (fs_pref_dir(pref, sizeof(pref)) != 0) {
            fprintf(stderr, "ggx: no user data directory: set HOME, or pass --data-dir\n");
            return 1;
        }
        cfg.data_dir = pref;
    }
    if (!cfg.disc_path && !replay_path && !trace_path) {
        /* the first run (firstrun.h): needed when this build's release isn't extracted; or the other release's
           is, and its build is beside this one, which plays it */
        char other[1100];

        other[0] = 0;
        first_run = first_run_img != NULL || (render && !play_path && fr_needed(cfg.data_dir, other, sizeof(other)));
        if (!first_run && other[0]) {
            int code = 1;

            fprintf(stderr, "ggx: the other release's disc is extracted: handing over to %s\n", other);
            if (fr_handoff(other, argc, argv, &code) == 0) {
                return code;
            }
            return 1;
        }
    }
    if (!cfg.save_dir) {
        if (save_store_dir(save_dir, sizeof(save_dir), cfg.data_dir) != 0) {
            fprintf(stderr, "ggx: no user data directory for saves: set HOME, or pass --save-dir\n");
            return 1;
        }
        cfg.save_dir = save_dir;
    }
    cfg.extensions = extensions;
    cfg.n_extensions = n_ext;
    if (!config_file) {
        /* the settings live beside the extracted assets, in the user data directory */
        char dir[1024];

        if (fs_pref_dir(dir, sizeof(dir)) != 0 || fs_join(cfg_default, sizeof(cfg_default), dir, "ggx.cfg", NULL) != 0) {
            snprintf(cfg_default, sizeof(cfg_default), "ggx.cfg");
        }
        config_file = cfg_default;
    }
    w.cfg = config_load(config_file);
    if (!w.cfg) {
        return 1;
    }
    if (script_path) {
        char err[512];

        if (!(script = ggs_load(script_path, err, sizeof(err)))) {
            fprintf(stderr, "ggx: %s\n", err);
            return 1;
        }
        script_on = 1;
    }
#ifdef _WIN32
    _setmode(_fileno(stdout), _O_BINARY);
#endif
    if (times_path) {
        if (!(times = fs_open(times_path, "wb"))) {
            fprintf(stderr, "ggx: cannot write %s\n", times_path);
            return 1;
        }
        fprintf(times, "field,job,vblanks,start_us,wait_us,core_us,sound_us,render_us,present_us,tex_new,tex_texels,batches,gs_blend,start_pc,paced,frame,behind\n");
    }
    if (hash_path && !(hash_log = fs_open(hash_path, "wb"))) {
        fprintf(stderr, "ggx: cannot write %s\n", hash_path);
        return 1;
    }
    if (!no_log) {
        cfg.log = log_path ? fs_open(log_path, "wb") : stdout;
        if (!cfg.log) {
            fprintf(stderr, "ggx: cannot write %s\n", log_path);
            return 1;
        }
    }

    if (render) {
        SDL_GPUShaderFormat formats =
            SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_DXIL;

        if (no_focus) {
            /* a test run beside someone's work: show the window, don't activate it */
            SDL_SetHint(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, "0");
        }
        if (!SDL_Init(SDL_INIT_VIDEO | (offscreen ? 0 : SDL_INIT_GAMEPAD))) {
            fprintf(stderr, "ggx: SDL: %s\n", SDL_GetError());
            if (!offscreen) {
                return 1;
            }
        } else {
            dev = SDL_CreateGPUDevice(formats, false, gpu_name);
        }
        if (!dev && offscreen) {
            /* no display at all (a CI runner): SDL's offscreen video
               driver, which Vulkan can still use; Metal needs Cocoa, which
               works without a window server session too */
            SDL_Quit();
            SDL_SetHintWithPriority(SDL_HINT_VIDEO_DRIVER, "offscreen", SDL_HINT_OVERRIDE);
            if (SDL_Init(SDL_INIT_VIDEO)) {
                dev = SDL_CreateGPUDevice(formats, false, gpu_name);
            }
        }
        if (!dev) {
            fprintf(stderr, "ggx: no GPU device: %s\n", SDL_GetError());
            return 1;
        }
        if (!offscreen) {
            const char *pm = present_opt ? present_opt : config_get_or(w.cfg, "present", "vsync");
            SDL_GPUPresentMode mode = present_mode(pm);

            w.fullscreen = !strcmp(window_opt ? window_opt : config_get_or(w.cfg, "window",
                                   input_on_steam_deck() ? "fullscreen" : "windowed"), "fullscreen");
            w.integer = !strcmp(scaling_opt ? scaling_opt : config_get_or(w.cfg, "scaling", "fit"), "integer");
            if (display_opt && !strcmp(display_opt, "list")) {
                display_named("");   /* matches none: lists them */
                return 0;
            }
            {
                /* on the display the settings name (display = N or a part of its name), else the system's choice */
                const char *want_display = display_opt ? display_opt : config_get(w.cfg, "display");
                SDL_DisplayID on = want_display && *want_display ? display_named(want_display) : 0;
                SDL_PropertiesID props = SDL_CreateProperties();

                SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Guilty Gear X Plus");
                SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 1280);
                SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 960);
                SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
                SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
                SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, w.fullscreen);
                if (on) {
                    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED_DISPLAY(on));
                    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED_DISPLAY(on));
                }
                w.win = SDL_CreateWindowWithProperties(props);
                SDL_DestroyProperties(props);
            }
            if (!w.win || !SDL_ClaimWindowForGPUDevice(dev, w.win)) {
                fprintf(stderr, "ggx: window: %s\n", SDL_GetError());
                return 1;
            }
            if (mode != SDL_GPU_PRESENTMODE_VSYNC) {
                if (SDL_WindowSupportsGPUPresentMode(dev, w.win, mode) &&
                    SDL_SetGPUSwapchainParameters(dev, w.win, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, mode)) {
                    vsync = 0;
                } else {
                    fprintf(stderr, "ggx: present mode %s isn't available here: vsync\n", pm);
                }
            }
            /* one frame queued at most: the frame shown is the newest the Core finished */
            SDL_SetGPUAllowedFramesInFlight(dev, (Uint32)in_flight);
            w.vsync = vsync;
            w.dev = dev;
            w.want_mode = w.mode = vsync ? SDL_GPU_PRESENTMODE_VSYNC : mode;
            /* the clock by default: pacing by the display is opt-in until it
               holds the field rate on every display tested */
            w.pacing = !strcmp(pacing_opt ? pacing_opt : config_get_or(w.cfg, "pacing", "clock"), "display");
            w.pace.fixed_lead = (uint64_t)(lead_ms / 1000.0 * (double)SDL_GetPerformanceFrequency());
            if (frame_tag && tag_init(dev) != 0) {
                fprintf(stderr, "ggx: --frame-tag: %s\n", SDL_GetError());
                return 1;
            }
            w.input = input_new(w.cfg);
            if (!w.input) {
                return 1;
            }
        }
        stream = dl_stream_new();
        gpu = stream ? dl_gpu_new(dev, &gopt) : NULL;
        if (!gpu) {
            fprintf(stderr, "ggx: cannot start the renderer\n");
            return 1;
        }
        fprintf(stderr, "ggx: rendering with %s at %dx%d (SDL %d.%d.%d, video %s)\n", SDL_GetGPUDeviceDriver(dev),
                DL_FB_W * gopt.scale, DL_FB_H * gopt.scale, SDL_VERSIONNUM_MAJOR(SDL_GetVersion()),
                SDL_VERSIONNUM_MINOR(SDL_GetVersion()), SDL_VERSIONNUM_MICRO(SDL_GetVersion()),
                SDL_GetCurrentVideoDriver());
        if (w.win) {
            fprintf(stderr, "ggx: %s, %s scaling, present %s; settings in %s\n", w.fullscreen ? "fullscreen" : "windowed",
                    w.integer ? "integer" : "fit", vsync ? "vsync" : (present_opt ? present_opt : config_get(w.cfg, "present")),
                    config_path(w.cfg));
            display_update(&w);
        }
        pixels = malloc((size_t)DL_FB_W * DL_FB_H * 4 * (size_t)(gopt.scale * gopt.scale));
        if (!pixels) {
            return 1;
        }
    }
    if (replay_path) {
        rc = replay(replay_path, stream, gpu, dump_dir, pixels);
        if (stats) {
            dl_stream_print_stats(stream, stderr);
        }
        dl_gpu_free(gpu);
        dl_stream_free(stream);
        free(pixels);
        SDL_DestroyGPUDevice(dev);
        SDL_Quit();
        return rc;
    }
    if (first_run) {
        FirstRun fr;
        char fonts[1100];
        const char *b = SDL_GetBasePath();
        int r;

        memset(&fr, 0, sizeof(fr));
        if (text_dir) {
            snprintf(fonts, sizeof(fonts), "%s/fonts", text_dir);
        } else {
            snprintf(fonts, sizeof(fonts), "%stext/fonts", b ? b : "");
        }
        fr.win = w.win;
        fr.data_dir = cfg.data_dir;
        fr.fonts_dir = fonts;
        fr.image = first_run_img;
        fr.dump_dir = dump_dir;
        fr.show = w.win ? first_run_show : NULL;
        fr.show_ctx = &w;
        r = fr_run(&fr);
        if (r == FR_HANDOFF) {
            int code = 1;

            if (fr_tex) {
                SDL_ReleaseGPUTexture(dev, fr_tex);
            }
            dl_gpu_free(gpu);
            dl_stream_free(stream);
            if (w.win) {
                SDL_ReleaseWindowFromGPUDevice(dev, w.win);
                SDL_DestroyWindow(w.win);
            }
            SDL_DestroyGPUDevice(dev);
            SDL_Quit();
            fprintf(stderr, "ggx: handing over to %s\n", fr.other);
            return fr_handoff(fr.other, argc, argv, &code) == 0 ? code : 1;
        }
        if (fr_tex) {
            SDL_ReleaseGPUTexture(dev, fr_tex);
            fr_tex = NULL;
        }
        if (r == FR_QUIT) {
            return 0;
        }
        if (r == FR_REFUSED) {
            return 7;
        }
    }
    if (core_host_open(&host, &cfg) != 0) {
        return 1;
    }
    session_init(&sess, &host, 0);
    if (play) {
        RpBuild here;
        char why[512];

        rp_build_of(&host, cfg.extensions, cfg.n_extensions, &here);
        if (rp_compatible(play, &here, why, sizeof(why)) != 0) {
            fprintf(stderr, "ggx: %s can't be played on this build: %s\n", play_path, why);
            return 6;
        }
        if (rp_start(play, &sess) != 0) {
            fprintf(stderr, "ggx: %s: the memory card it records doesn't fit on a card\n", play_path);
            return 6;
        }
    }
    if (w.win && !strcmp(audio_opt ? audio_opt : config_get_or(w.cfg, "audio", "on"), "on")) {
        speakers = audio_out_open();
    }
    acfg.play = speakers != NULL;
    if ((acfg.play || acfg.snd_log || acfg.wav) && !(audio = audio_host_open(&host, &acfg))) {
        return 1;
    }
    if (trace_path) {
        char terr[512];
        const TraceHeader *th;

        if (!(trace = trace_open(trace_path, terr, sizeof(terr)))) {
            fprintf(stderr, "ggx: %s\n", terr);
            return 1;
        }
        th = trace_header(trace);
        if (th->release != TRACE_RELEASE || memcmp(host.elf_sha256, th->elf_sha256, 32) != 0) {
            fprintf(stderr, "ggx: %s is not a trace of this build's disc (%s)\n", trace_path, RELEASE);
            return 1;
        }
        if (!fields || fields > trace_fields(trace) - 1) {
            fields = trace_fields(trace) - 1;
        }
        trace_begin(trace, &host);
        trace_keep_going(trace);
    }
    if (stream) {
        /* bound before the boot: the file table is the boot ELF's, already
           loaded; nothing here touches Core memory */
        text = open_text(&host, text_dir);
        if (!text && (text_on || text_asked)) {
            return 1;
        }
        sub.text = text;
        sub.text_on = text_on;
        sub.prompts = open_prompts(&host);
        set_subst(stream, &sub);
    }
    w.sub = &sub;
    if (stream && !w.input && prompts_opt) {
        /* no window: the prompts still follow --prompts (a forced family's letters) */
        w.input = input_new(w.cfg);
        if (!w.input) {
            return 1;
        }
    }
    if (w.input && prompts_opt) {
        input_force_family(w.input, input_family_parse(prompts_opt));
    }
    confirm_reset(&script_latch[0]);
    confirm_reset(&script_latch[1]);
    if (w.input && confirm_opt >= 0) {
        input_set_confirm(w.input, 0, confirm_opt, 0);
        input_set_confirm(w.input, 1, confirm_opt, 0);
    }
    update_prompts(sub.prompts, w.input);
    if (w.input && stream) {
        w.settings = settings_new(w.input, w.cfg, sub.prompts, &host);
        if (w.settings && settings_open_opt) {
            settings_open(w.settings);
        }
    }
    {
        /* unlock = all (or items) in the config, or --unlock: at the title, after the memory card's load */
        const char *v = unlock_opt ? unlock_opt : config_get_or(w.cfg, "unlock", "none");
        int m = unlock_parse(v);

        if (m < 0) {
            fprintf(stderr, "ggx: unlock = %s: not all, none, or characters, colours, gallery, challenges; none\n", v);
            m = 0;
        }
        if (!trace_path && !play) {
            sess.unlock_pending = m;
            unlocks_asked = m;
        }
    }
    if (!trace_path && !play) {
        /* replays (PORT-013): the run from the boot, with the cards as they are now */
        auto_replays = w.win && !strcmp(config_get_or(w.cfg, "replays", "on"), "on");
        if ((record_path || auto_replays) && !(rec = rp_record(&sess, unlocks_asked, cfg.extensions, cfg.n_extensions))) {
            fprintf(stderr, "ggx: cannot record a replay\n");
            return 1;
        }
    }
    if (play) {
        uint32_t f, k;
        int j;

        /* in a window, fast to the first fight */
        for (j = 0; w.win && (!from_opt || !strcmp(from_opt, "fight")) && j < rp_marks(play); j++) {
            rp_mark_at(play, j, &f, &k);
            if (k == RP_MARK_FIGHT) {
                play_skip = f;
                break;
            }
        }
        fprintf(stderr, "ggx: playing %s: %u fields%s\n", play_path, rp_fields(play),
                play_skip ? ", fast to its first fight" : "");
    }
    w.stream = stream;
    w.text = text;
    w.text_on = text_on;
    for (i = 0; i < 2; i++) {
        pad_clear(&host.pads[i]);
    }
    if (stream) {
        hook.h = &host;
        hook.s = stream;
        hook.dump = &gsd;
        host.gif_send = on_gif;
        host.gif_ctx = &hook;
    }
    if (trace) {
        trace_field_begin(trace, trace_n);
    }
    if (core_host_boot(&host) != 0) {
        rc = 2;
    }
    play_sound(audio, speakers, 0);
    if (trace && rc == 0 && trace_field_end(trace) != 0) {
        trace_bad++;
        if (trace_resume(trace) != 0) {
            rc = 1;
        }
    }
    if (rc == 0 && ps2_state && load_ps2_state(&host, stream, ps2_state) != 0) {
        rc = 1;
    }
    if (hash_log) {
        log_hashes(&host, hash_log);
    }
    want[0] = 0;
    until_next(&until, want, sizeof(want));
    last = start = SDL_GetTicksNS();
    acc = FIELD_NS;   /* the first field at once */
    while (rc == 0 && !w.quit && (!fields || host.field < fields)) {
        const DlFrame *f;

        if (w.win) {
            /* The clock (ADR-013): where the display refreshes at a whole
               multiple of the field rate and its vblanks can be followed,
               the Core steps once every k vblanks, each field starting a
               measured lead before the vblank its frame is for (pace.h).
               Otherwise it steps at 59.94 Hz from an accumulator of real
               time, and the loop sleeps until the next field is due. Input
               is read just before each field, and the frame is presented as
               soon as the field has run. The display holds the newest frame
               between presents (a repeat), so nothing waits in the swapchain
               behind frames the Core didn't make. */
            SDL_Event e;
            uint64_t now;

            while (SDL_PollEvent(&e)) {
                on_event(&w, &e);
            }
            if (w.quit) {
                break;
            }
            now = SDL_GetTicksNS();
            acc += (int64_t)(now - last);
            last = now;
            if (acc > (int64_t)(6 * FIELD_NS)) {
                acc = FIELD_NS;   /* a stall (a window drag, a breakpoint): don't race to catch up */
            }
            if (input_remap_prompt(w.input)) {
                acc = 0;          /* the Core holds while a player remaps */
                present(dev, w.win, gpu, drawn != 0, w.integer, frame_tag ? shown_field : -1, NULL);
                SDL_DelayPrecise(FIELD_NS);
                continue;
            }
            if (w.settings && settings_is_open(w.settings)) {
                /* the Core holds while the settings panel is open: its frame again, under the panel */
                PadInput held[2];
                const DlFrame *last = dl_stream_frame(stream);

                input_set_menu(w.input, 0, 0);
                input_set_menu(w.input, 1, 0);
                input_pads(w.input, held);
                settings_update(w.settings, held);
                {
                    /* at the title or in OPTIONS: at once, so the panel shows it (session_unlock) */
                    int asked = settings_take_unlocks(w.settings);

                    if (asked && !play) {
                        session_unlock(&sess, asked, "the settings");
                    }
                }
                show_settings(w.settings, gpu);
                update_prompts(sub.prompts, w.input);
                if (last) {
                    dl_gpu_render_submit(gpu, last, dl_stream_textures(stream));
                }
                acc = 0;
                present(dev, w.win, gpu, last != NULL, w.integer, -1, NULL);
                SDL_DelayPrecise(FIELD_NS);
                continue;
            }
            field_pc = SDL_GetPerformanceCounter();
            /* the display's vblanks, if they are being followed and are
               fresh (none for a while: a display asleep, a window hidden) */
            paced = w.pace.k && !w.fast && vblank_get(w.vb, &vb_last, &vb_period) &&
                    field_pc - vb_last < 4 * vb_period;
            if (w.pace.k) {
                /* a source that doesn't wait for vblanks (vblank_why), or
                   vblanks that make no sense (pace_sane): the clock */
                const char *why = vblank_why(w.vb);

                if (!why && paced) {
                    why = pace_sane(&w.pace, vb_period);
                }
                if (why) {
                    fprintf(stderr, "ggx: pacing by the display stopped: %s; the 59.94 Hz clock from here\n", why);
                    pace_init(&w.pace, w.hz, SDL_GetPerformanceFrequency(), 0);
                    vblank_close(w.vb);
                    w.vb = NULL;
                    swapchain_mode(&w);
                    continue;
                }
            }
            unshown = paced && just_shown && acc >= (int64_t)FIELD_NS;
            just_shown = 0;
            if (unshown) {
                /* A field behind 59.94 Hz: frames that missed their vblanks
                   have each cost a refresh, more than the display's own rate
                   gives back (at 60.00 Hz, one every 17.5 s). This field runs
                   now, in the time left after the frame just shown, and isn't
                   shown itself, as the clock does when it falls behind, so
                   the game keeps time and the sound stays fed. Only here:
                   while waiting for the next field acc counts that field too. */
                acc -= FIELD_NS;
                w.pace.unshown++;
                paced = 0;
            } else if (paced) {
                uint64_t at = pace_start(&w.pace, field_pc, vb_last, vb_period);

                if (field_pc < at) {
                    uint64_t d0 = SDL_GetTicksNS();

                    SDL_DelayPrecise((at - field_pc) * 1000000000ull / SDL_GetPerformanceFrequency());
                    t_wait += SDL_GetTicksNS() - d0;
                    continue;
                }
                /* against the 59.94 Hz clock, up to a field ahead of it: the
                   display's rate, a little faster, must not build up credit
                   that would hide a run of misses */
                acc -= FIELD_NS;
                acc = acc < -(int64_t)FIELD_NS ? -(int64_t)FIELD_NS : acc;
            } else {
                w.pace.target = 0;
                acc = acc < 0 ? 0 : acc;
                if (w.fast) {
                    acc = FIELD_NS;
                }
                if (acc < (int64_t)FIELD_NS) {
                    uint64_t d0 = SDL_GetTicksNS();

                    SDL_DelayPrecise((uint64_t)((int64_t)FIELD_NS - acc));
                    t_wait += SDL_GetTicksNS() - d0;
                    continue;
                }
                acc -= FIELD_NS;
                if (acc >= (int64_t)FIELD_NS) {
                    late++;       /* a whole field behind the clock */
                }
            }
            if (w.vlog) {
                fprintf(w.vlog, "field,%u,%llu,%llu,%llu,%llu,%llu,%llu,%d,%d\n", host.field,
                        (unsigned long long)field_pc, (unsigned long long)(paced ? vb_last : 0),
                        (unsigned long long)(paced ? vb_period : 0), (unsigned long long)(paced ? w.pace.target : 0),
                        (unsigned long long)(paced ? w.pace.target - w.pace.lead : 0),
                        (unsigned long long)(paced ? w.pace.lead : 0),
                        paced && field_pc + w.pace.lead / 2 > w.pace.target, paced ? w.pace.k : 0);
            }
            if (!script_on) {
                int p;

                game_keys(&host, w.input);
                for (p = 0; p < 2; p++) {
                    input_set_menu(w.input, p, menu_confirms(&host, p));
                }
                input_pads(w.input, host.pads);
                if (w.settings && settings_watch(w.settings, host.pads)) {
                    /* Select held a second opened the settings: this field waits for them */
                    continue;
                }
            }
        }
        if (mash) {
            mash_input(seed, host.field, host.pads);
        }
        if (script_on) {
            int st = ggs_play_to(script, (uint64_t)host.vsyncs + 1, host.pads);

            if (st == GGS_QUIT) {
                shim_log(&host, "script quit at frame %llu", (unsigned long long)ggs_frames(script));
                break;
            }
            if (confirm_opt >= 0) {
                /* --confirm: the script's buttons are a player's, so X confirms in menus as for the pads */
                int p;

                for (p = 0; p < 2; p++) {
                    host.pads[p].buttons = confirm_apply(&script_latch[p], host.pads[p].buttons,
                                                         confirm_opt == CONFIRM_X && menu_confirms(&host, p));
                }
            }
            if (st == GGS_DONE && w.win) {
                /* the script brought the game here: the controllers have it now */
                fprintf(stderr, "ggx: the script ended at field %u; the controllers play on\n", host.field);
                script_on = 0;
            }
        }
        if (gsdump_path && host.field == gsdump_from) {
            if (gsdump_start(&gsd, gsdump_path, gsdump_tmpl, dl_stream_gs_mem(stream)) != 0) {
                rc = 1;
                break;
            }
            dl_stream_set_tap(stream, gsdump_tap, &gsd);
        }
        if (trace) {
            trace_field_begin(trace, ++trace_n);
        }
        t0 = SDL_GetTicksNS();
        pc0 = SDL_GetPerformanceCounter();
        vs0 = host.vsyncs;
        if (trace) {
            if (core_host_field(&host) != 0) {
                rc = 2;
                break;
            }
        } else {
            /* the session (session.h): the unlocks asked for in the settings or the config, made where the game
               reads them afresh, GOHAN on the password screen; recorded for a replay, or a replay's input */
            SessionInput in;

            memset(&in, 0, sizeof(in));
            in.pads[0] = host.pads[0];
            in.pads[1] = host.pads[1];
            if (w.settings && !play) {
                in.unlock = (uint8_t)settings_take_unlocks(w.settings);
            }
            if (play) {
                const SessionInput *p = rp_input(play, host.field);
                char why[256];

                if (!p) {
                    break;
                }
                in = *p;
                if (host.field % RP_CHECK_EVERY == 0 &&
                    rp_check(play, host.field, core_hash(&host), why, sizeof(why)) != 0 && !play_bad++) {
                    fprintf(stderr, "ggx: %s doesn't play as recorded: %s\n", play_path, why);
                }
            }
            if (session_field(&sess, &in) != 0) {
                rc = 2;
                break;
            }
            if (play && w.win) {
                w.fast = host.field < play_skip;
            }
            if (rp_watch(rec, &host) == RP_MARK_FIGHT_END && auto_replays) {
                char dir[1100], path[1300], why[512], stamp[32];
                time_t now = time(NULL);
                struct tm *tm = localtime(&now);

                strftime(stamp, sizeof(stamp), "%Y%m%d-%H%M%S", tm);
                int ok = replay_dir ? snprintf(dir, sizeof(dir), "%s", replay_dir) < (int)sizeof(dir)
                                    : rp_dir(dir, sizeof(dir), cfg.data_dir) == 0;

                if (ok && fs_mkdirs(dir) == 0 &&
                    snprintf(path, sizeof(path), "%s/%s-%s.ggxreplay", dir, stamp, RELEASE) < (int)sizeof(path) &&
                    rp_write(rec, path, why, sizeof(why)) == 0) {
                    fprintf(stderr, "ggx: the fight's replay: %s\n", path);
                } else {
                    fprintf(stderr, "ggx: could not write the fight's replay\n");
                }
            }
        }
        t_core = SDL_GetTicksNS();
        /* fast to a replay's first fight: the driver hears it all, the speakers don't */
        play_sound(audio, w.fast && play ? NULL : speakers, host.field - 1);
        t_sound = SDL_GetTicksNS();
        if (trace && trace_field_end(trace) != 0) {
            if (!trace_bad++) {
                fprintf(stderr, "ggx: %s: the State Hash differs from the PS2's from field %u\n", trace_path, trace_n);
            }
            if (trace_resume(trace) != 0) {
                rc = 1;
                break;
            }
        }
        if (hash_log) {
            log_hashes(&host, hash_log);
        }
        if (forbid) {
            const char *name = ggx_func_name(ee_r32(&host, ggx_sym_jobfunc + 4 * ee_r32(&host, ggx_sym_JobMode)));
            const char *p = strstr(forbid, name);
            size_t n = strlen(name);

            if (p && (p == forbid || p[-1] == ',') && (p[n] == ',' || !p[n])) {
                shim_log(&host, "forbidden job %s", name);
                fprintf(stderr, "ggx: field %u: %s ran, which --forbid lists\n", host.field, name);
                rc = 5;
                break;
            }
        }
        if (want[0]) {
            uint32_t job = ee_r32(&host, ggx_sym_JobMode);
            int is = !strcmp(ggx_func_name(ee_r32(&host, ggx_sym_jobfunc + 4 * job)), want);

            /* it was the job when this field began and still is */
            if (is && was) {
                shim_log(&host, "reached %s", want);
                fprintf(stderr, "ggx: field %u: reached %s\n", host.field, want);
                was = 0;
                if (!until_next(&until, want, sizeof(want))) {
                    reached = 1;
                    break;
                }
            } else {
                was = is;
            }
        }
        f = stream ? dl_stream_frame(stream) : NULL;
        if (f && f->serial != drawn) {
            int want_png = gsd.f != NULL, k;
            const char *shot = script_on ? ggs_shots(script) : "";

            for (k = 0; k < ndumps; k++) {
                want_png |= dumps[k] == host.field;
            }
            want_png |= *shot && dump_dir;
            if (want_png) {
                char name[160], path[1024];
                uint32_t ow, oh;

                dl_gpu_output(gpu, &ow, &oh);
                if (dl_gpu_render_read(gpu, f, dl_stream_textures(stream), pixels) != 0) {
                    fprintf(stderr, "ggx: field %u: the frame did not render completely\n", host.field);
                }
                if (gsd.f) {
                    /* named by the dump's vsync that shows it */
                    snprintf(name, sizeof(name), "v%06u.png", gsd.vsyncs);
                } else if (*shot) {
                    /* a script's shot: the frame finished on the field after it */
                    snprintf(name, sizeof(name), "%s.png", shot);
                } else {
                    snprintf(name, sizeof(name), "%s_f%06u.png", RELEASE, host.field);
                }
                if (fs_join(path, sizeof(path), dump_dir, "/", name) != 0 || png_write_rgba(path, pixels, ow, oh) != 0) {
                    fprintf(stderr, "ggx: cannot write %s\n", name);
                    rc = 1;
                }
            } else if (w.win || render_all) {
                dl_gpu_render_submit(gpu, f, dl_stream_textures(stream));
            } else {
                /* offscreen with nothing to dump: keep the texture cache trim */
                dl_tex_evict(dl_stream_textures(stream), f->serial, 120);
            }
            drawn = f->serial;
            shown_field = (int64_t)host.field - 1;
        }
        if (gsd.f && host.field >= gsdump_to) {
            dl_stream_set_tap(stream, NULL, NULL);
            if (fclose(gsd.f) != 0) {
                rc = 1;
            }
            gsd.f = NULL;
            fprintf(stderr, "ggx: wrote %s: fields %lu-%lu, %u frames\n", gsdump_path, gsdump_from, gsdump_to - 1,
                    gsd.vsyncs);
        }
        update_prompts(sub.prompts, w.input);
        if (w.settings) {
            show_settings(w.settings, gpu);
        }
        t_render = SDL_GetTicksNS();
        if (w.win) {
            input_rumble(w.input, (const uint8_t(*)[2])host.actuators);
            if ((paced || acc < (int64_t)FIELD_NS) && !w.fast && !unshown) {
                /* in time: show it. A field or more behind the clock, run
                   the next field first (presenting can wait for the display) */
                uint64_t hold = 0;
                SDL_GPUFence *fence = NULL;

                if (paced) {
                    /* a frame done early waits for the vblank before its own (pace.h) */
                    uint64_t pc = SDL_GetPerformanceCounter(), at = pace_present_at(&w.pace, vb_period);

                    if (pc < at) {
                        hold = at - pc;
                        SDL_DelayPrecise(hold * 1000000000ull / SDL_GetPerformanceFrequency());
                    }
                }
                present(dev, w.win, gpu, drawn != 0, w.integer, frame_tag ? shown_field : -1, paced ? &fence : NULL);
                if (paced) {
                    /* the frame's GPU work: when it ended says whether the
                       frame made its vblank (pace.h, pace_done) */
                    uint64_t done, target = w.pace.target;

                    if (fence) {
                        SDL_WaitForGPUFences(dev, true, &fence, 1);
                        SDL_ReleaseGPUFence(dev, fence);
                    }
                    done = SDL_GetPerformanceCounter();
                    pace_done(&w.pace, field_pc, done, hold, vb_period);
                    just_shown = 1;
                    if (w.vlog) {
                        fprintf(w.vlog, "frame,%u,%llu,%llu,%llu\n", host.field - 1, (unsigned long long)target,
                                (unsigned long long)done, (unsigned long long)hold);
                    }
                }
            } else if (w.fast && host.field % 8 == 0) {
                present(dev, w.win, gpu, drawn != 0, w.integer, frame_tag ? shown_field : -1, NULL);
            }
        }
        if (times) {
            const DlStats *ds = stream ? dl_stream_stats(stream) : NULL;

            t_present = SDL_GetTicksNS();
            fprintf(times, "%u,%s,%u,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%d,%d,%u\n", host.field - 1,
                    ggx_func_name(ee_r32(&host, ggx_sym_jobfunc + 4 * ee_r32(&host, ggx_sym_JobMode))),
                    host.vsyncs - vs0, (unsigned long long)((t0 - start) / 1000), (unsigned long long)(t_wait / 1000),
                    (unsigned long long)((t_core - t0) / 1000), (unsigned long long)((t_sound - t_core) / 1000),
                    (unsigned long long)((t_render - t_sound) / 1000),
                    (unsigned long long)((t_present - t_render) / 1000),
                    (unsigned long long)(ds ? ds->tex_new - tex0 : 0),
                    (unsigned long long)(ds ? ds->tex_new_texels - texels0 : 0),
                    (unsigned long long)(ds ? ds->batches - batches0 : 0),
                    (unsigned long long)(ds ? ds->alpha_over_one - over0 : 0), (unsigned long long)pc0,
                    paced ? w.pace.k : 0, shown_field == (int64_t)host.field - 1, w.pace.behind);
            if (ds) {
                tex0 = ds->tex_new, texels0 = ds->tex_new_texels, batches0 = ds->batches, over0 = ds->alpha_over_one;
            }
            t_wait = 0;
        }
    }
    if (play && rc == 0) {
        char why[256];

        if (host.field == rp_fields(play) && rp_check(play, host.field, core_hash(&host), why, sizeof(why)) != 0) {
            fprintf(stderr, "ggx: %s doesn't play as recorded: %s\n", play_path, why);
            play_bad++;
        }
        if (play_bad) {
            rc = 1;
        } else {
            fprintf(stderr, "ggx: %s: %u of its %u fields played as recorded, every State Hash checked agrees\n",
                    play_path, host.field, rp_fields(play));
        }
    }
    if (record_path && rc == 0) {
        char why[512];

        if (rp_write(rec, record_path, why, sizeof(why)) != 0) {
            fprintf(stderr, "ggx: cannot write the replay: %s\n", why);
            rc = 1;
        } else {
            fprintf(stderr, "ggx: wrote %s: %u fields from the boot\n", record_path, host.field);
        }
    }
    rp_free(rec);
    rp_free(play);
    hash = fnv1a(ee_ptr(&host, 0, EE_RAM_SIZE), EE_RAM_SIZE, 0xcbf29ce484222325ull);
    hash = fnv1a(ee_ptr(&host, EE_SPR_BASE, EE_SPR_SIZE), EE_SPR_SIZE, hash);
    shim_log(&host, "end fields=%u vsyncs=%u ram_fnv1a64=%016llx", host.field, host.vsyncs, (unsigned long long)hash);
    if (w.win || render_all) {
        double secs = (double)(SDL_GetTicksNS() - start) / 1e9;

        fprintf(stderr, "ggx: %u fields in %.2f s (%.2f fields/s)", host.field, secs, secs > 0 ? host.field / secs : 0);
        if (w.win) {
            fprintf(stderr, ", %u started a field or more late", late + w.pace.late);
            if (w.pace.behind) {
                fprintf(stderr, ", %u frames missed their vblank", w.pace.behind);
            }
            if (w.pace.unshown) {
                fprintf(stderr, ", %u fields run unshown to keep time", w.pace.unshown);
            }
        }
        fprintf(stderr, "\n");
    }
    if (times) {
        fclose(times);
    }
    if (stats && stream) {
        dl_stream_print_stats(stream, stderr);
    }
    if (trace) {
        fprintf(stderr, "ggx: %s: %u fields, %u with a State Hash other than the PS2's (English text %s)\n", trace_path,
                trace_n + 1, trace_bad, w.text_on || text_on ? "on" : "off");
        trace_free(trace);
    }
    ggs_free(script);
    tl_free(text);
    if (audio_host_close(audio) != 0) {
        fprintf(stderr, "ggx: writing the sound failed\n");
        rc = rc ? rc : 1;
    }
    audio_out_report(speakers);
    audio_out_close(speakers);
    pr_free(sub.prompts);
    core_host_close(&host);
    if (log_path) {
        fclose(cfg.log);
    }
    if (hash_log) {
        fclose(hash_log);
    }
    dl_gpu_free(gpu);
    dl_stream_free(stream);
    free(pixels);
    settings_free(w.settings);
    input_free(w.input);
    config_free(w.cfg);
    if (w.win) {
        SDL_ReleaseWindowFromGPUDevice(dev, w.win);
        vblank_close(w.vb);
        SDL_DestroyWindow(w.win);
        if (w.vlog) {
            fflush(w.vlog);   /* not closed: the vblank thread may still write a line as it stops */
        }
    }
    if (dev) {
        SDL_DestroyGPUDevice(dev);
    }
    if (render) {
        SDL_Quit();
    }
    if (rc == 0 && want[0] && !reached) {
        fprintf(stderr, "ggx: %s not reached\n", want);
        rc = 3;
    }
    return rc;
}
