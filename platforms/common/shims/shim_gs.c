/* libgraph, libdma, libdev and kernel shims.
 *
 * Nothing is drawn in PORT-002. The GS and DMA calls are logged (they are
 * what PORT-008 and PORT-009 consume), and the libgraph functions that only
 * fill a structure in EE memory do so as the SDK does, so the game's packets
 * hold the same register values they would on the PS2. sceGsSyncV is the
 * Core's vblank: each call is one field, and it returns that field's parity.
 *
 * The game also writes a few GS privileged and timer registers directly
 * (graph.c, meter.c). Those addresses are plain memory in the Core's
 * linear memory, so the writes land there and are not logged.
 */
#include <string.h>

#include "core_host.h"
#include "trace.h"

/* sceGsGParam (graph001.o's .data): inter +0, omode +2, ffmd +4, version +6,
   intcUsed +8, handler +0xc */
#define GP_INTER 0
#define GP_OMODE 2
#define GP_FFMD 4
#define GP_VERSION 6
/* GS CSR revision and id as PCSX2 reports them */
#define GS_REVISION 0x1b

static int16_t gp16(CoreHost *h, uint32_t off)
{
    return (int16_t)ee_r16(h, ggx_sym_gsGParam + off);
}

static void gp_w16(CoreHost *h, uint32_t off, int v)
{
    uint16_t x = (uint16_t)v;
    memcpy(ee_ptr(h, ggx_sym_gsGParam + off, 2), &x, 2);
}

void w2c_env_sceGsResetGraph(struct w2c_env *env, u32 mode, u32 inter, u32 omode, u32 ffmd)
{
    CoreHost *h = env->host;

    if ((int16_t)mode == 0 || (int16_t)mode == 5) {
        gp_w16(h, GP_INTER, (int16_t)inter);
        gp_w16(h, GP_OMODE, (int16_t)omode);
        gp_w16(h, GP_VERSION, GS_REVISION);
        gp_w16(h, GP_FFMD, (int16_t)ffmd != 0);
        if ((int16_t)mode == 0) {
            ee_w32(h, ggx_sym_gsGParam + 8, 0);
            ee_w32(h, ggx_sym_gsGParam + 12, 0);
        }
    }
    shim_log(h, "sceGsResetGraph(%d, %d, %d, %d)", (int16_t)mode, (int16_t)inter, (int16_t)omode, (int16_t)ffmd);
    TRACE_CALL(env->host, sceGsResetGraph, mode, inter, omode, ffmd);
}

/* SDK 2.3's sceGsSetDefDispEnv (src/sdk/sce/graph_setdefdispenv.c, JP).
   KR links SDK 2.5's, which also applies the kernel's display offsets on
   newer consoles; its source is not matched yet, so both builds use this. */
void w2c_env_sceGsSetDefDispEnv(struct w2c_env *env, u32 disp, u32 psm_, u32 w_, u32 h_, u32 dx_, u32 dy_)
{
    CoreHost *h = env->host;
    int psm = (int16_t)psm_, w = (int16_t)w_, hh = (int16_t)h_, dx = (int16_t)dx_, dy = (int16_t)dy_;
    int inter = gp16(h, GP_INTER), omode = gp16(h, GP_OMODE), ffmd = gp16(h, GP_FFMD);
    uint64_t display = ee_r32(h, disp + 0x18) | (uint64_t)ee_r32(h, disp + 0x1c) << 32;
    char a[16];

    ee_w64(h, disp + 0x00, 0x66);
    ee_w64(h, disp + 0x08, inter ? (ffmd ? 3 : 1) : 2);
    ee_w64(h, disp + 0x10, (uint64_t)(psm & 0xf) << 15 | (uint64_t)(((w + 0x3f) >> 6) & 0x3f) << 9);
    if (omode == 2 || omode == 3) {
        int q = w ? (w + 0x9ff) / w : 0;
        int xoff = omode == 2 ? 0x27c : 0x290;
        int yoff = inter == 1 ? (omode == 2 ? 0x32 : 0x48) : (omode == 2 ? 0x19 : 0x24);
        int lines = inter == 1 && ffmd ? hh * 2 - 1 : hh - 1;

        display = (uint64_t)(q - 1) << 23 | (uint64_t)(q * w - 1) << 32 |
                  (((uint64_t)(dx * q) + xoff) & 0xfff) | (uint64_t)lines << 44 |
                  (uint64_t)((dy + yoff) & 0xfff) << 12;
    }
    ee_w64(h, disp + 0x18, display);
    ee_w64(h, disp + 0x20, 0);
    shim_log(h, "sceGsSetDefDispEnv(%s, %d, %d, %d, %d, %d)", log_addr(disp, a), psm, w, hh, dx, dy);
    TRACE_CALL(env->host, sceGsSetDefDispEnv, disp, psm_, w_, h_, dx_, dy_);
}

static int zbufaddr(CoreHost *h, int psm, int width, int height)
{
    int bw = (width + 0x3f) / 0x40;
    int bh = psm & 2 ? (height + 0x3f) / 0x40 : (height + 0x1f) / 0x20;

    return gp16(h, GP_INTER) == 1 && gp16(h, GP_FFMD) == 0 ? bw * bh : bw * bh * 2;
}

/* graph006.o (src/sdk/sce/graph_setdefdrawenv.c) */
void w2c_env_sceGsSetDefDrawEnv(struct w2c_env *env, u32 de, u32 psm_, u32 w_, u32 h_, u32 ztst_, u32 zpsm_)
{
    CoreHost *h = env->host;
    int psm = (int16_t)psm_, w = (int16_t)w_, hh = (int16_t)h_, ztst = (int16_t)ztst_, zpsm = (int16_t)zpsm_;
    uint64_t zb = (uint64_t)(int64_t)(int16_t)zbufaddr(h, psm, w, hh) | (uint64_t)(zpsm & 0xf) << 24;
    uint64_t v;
    char a[16];

    ee_w64(h, de + 0x08, 0x4c);
    ee_w64(h, de + 0x00, (uint64_t)(((w + 0x3f) >> 6) & 0x3f) << 16 | (uint64_t)(psm & 0xf) << 24);
    ee_w64(h, de + 0x18, 0x4e);
    ee_w64(h, de + 0x10, ztst == 0 ? zb | (uint64_t)1 << 32 : zb);
    ee_w64(h, de + 0x28, 0x18);
    ee_w64(h, de + 0x20, (uint64_t)(0x800 - (int16_t)(w >> 1)) << 4 | (uint64_t)(0x800 - (int16_t)(hh >> 1)) << 36);
    ee_w64(h, de + 0x38, 0x40);
    ee_w64(h, de + 0x30, (uint64_t)(w - 1) << 16 | (uint64_t)(hh - 1) << 48);
    ee_w64(h, de + 0x48, 0x1a);
    memcpy(&v, ee_ptr(h, de + 0x40, 8), 8);
    ee_w64(h, de + 0x40, v | 1);
    ee_w64(h, de + 0x58, 0x46);
    memcpy(&v, ee_ptr(h, de + 0x50, 8), 8);
    ee_w64(h, de + 0x50, v | 1);
    ee_w64(h, de + 0x68, 0x45);
    memcpy(&v, ee_ptr(h, de + 0x60, 8), 8);
    ee_w64(h, de + 0x60, psm & 2 ? v | 1 : v & ~(uint64_t)1);
    ee_w64(h, de + 0x78, 0x47);
    ee_w64(h, de + 0x70, ztst ? (uint64_t)(ztst & 3) << 17 | 0x10000 : 0x30000);
    shim_log(h, "sceGsSetDefDrawEnv(%s, %d, %d, %d, %d, %d)", log_addr(de, a), psm, w, hh, ztst, zpsm);
    TRACE_CALL(env->host, sceGsSetDefDrawEnv, de, psm_, w_, h_, ztst_, zpsm_);
}

/* graph016.o (src/sdk/sce/graph_setdefstoreimage.c) */
void w2c_env_sceGsSetDefStoreImage(struct w2c_env *env, u32 di, u32 sbp, u32 sbw, u32 spsm, u32 ssax, u32 ssay,
                                   u32 rrw, u32 rrh)
{
    CoreHost *h = env->host;
    char a[16];

    ee_w32(h, di + 0x0, 0);
    ee_w32(h, di + 0x4, 0x06008000);
    ee_w32(h, di + 0x8, 0x13000000);
    ee_w32(h, di + 0xc, 0x50000006);
    /* GIFtag: nloop 5, eop, nreg 1, regs A+D */
    ee_w64(h, di + 0x10, 5 | 1u << 15 | (uint64_t)1 << 60);
    ee_w64(h, di + 0x18, 0xe);
    ee_w64(h, di + 0x20, (uint64_t)(int16_t)sbp | (uint64_t)(int16_t)sbw << 16 | (uint64_t)(int16_t)spsm << 24);
    ee_w64(h, di + 0x28, 0x50);
    ee_w64(h, di + 0x30, (uint64_t)(int16_t)ssax | (uint64_t)(int16_t)ssay << 16);
    ee_w64(h, di + 0x38, 0x51);
    ee_w64(h, di + 0x40, (uint64_t)(int16_t)rrw | (uint64_t)(int16_t)rrh << 32);
    ee_w64(h, di + 0x48, 0x52);
    ee_w64(h, di + 0x50, 0);
    ee_w64(h, di + 0x58, 0x61);
    ee_w64(h, di + 0x60, 1);
    ee_w64(h, di + 0x68, 0x53);
    shim_log(h, "sceGsSetDefStoreImage(%s, %d, %d, %d, %d, %d, %d, %d)", log_addr(di, a), (int16_t)sbp,
             (int16_t)sbw, (int16_t)spsm, (int16_t)ssax, (int16_t)ssay, (int16_t)rrw, (int16_t)rrh);
    TRACE_CALL(env->host, sceGsSetDefStoreImage, di, sbp, sbw, spsm, ssax, ssay, rrw, rrh);
}

void w2c_env_sceGsPutDispEnv(struct w2c_env *env, u32 disp)
{
    CoreHost *h = env->host;
    uint64_t r[5];
    char a[16];

    memcpy(r, ee_ptr(h, disp, sizeof(r)), sizeof(r));
    shim_log(h, "sceGsPutDispEnv(%s) pmode=0x%llx smode2=0x%llx dispfb=0x%llx display=0x%llx bgcolor=0x%llx",
             log_addr(disp, a), (unsigned long long)r[0], (unsigned long long)r[1], (unsigned long long)r[2],
             (unsigned long long)r[3], (unsigned long long)r[4]);
    TRACE_CALL(env->host, sceGsPutDispEnv, disp);
}

void w2c_env_sceGsResetPath(struct w2c_env *env)
{
    shim_log(env->host, "sceGsResetPath()");
    TRACE_CALL(env->host, sceGsResetPath);
}

u32 w2c_env_sceGsSyncPath(struct w2c_env *env, u32 mode, u32 timeout)
{
    shim_log(env->host, "sceGsSyncPath(%d, %u) = 0", (int)mode, timeout & 0xffff);
    TRACE_RETURN(env->host, sceGsSyncPath, 0, mode, timeout);
}

u32 w2c_env_sceGsSyncV(struct w2c_env *env, u32 mode)
{
    CoreHost *h = env->host;
    uint32_t parity = ++h->vsyncs & 1;

    shim_log(h, "sceGsSyncV(%d) = %u", (int)mode, parity);
    TRACE_RETURN(env->host, sceGsSyncV, parity, mode);
}

u32 w2c_env_sceGsExecStoreImage(struct w2c_env *env, u32 sp, u32 dst)
{
    char a[16], b[16];

    shim_log(env->host, "sceGsExecStoreImage(%s, %s) = 0", log_addr(sp, a), log_addr(dst, b));
    TRACE_RETURN(env->host, sceGsExecStoreImage, 0, sp, dst);
}

/* libdma: channel register blocks at their EE addresses */
static const uint32_t DMA_CHAN[10] = { 0x10008000, 0x10009000, 0x1000a000, 0x1000b000, 0x1000b400,
                                       0x1000c000, 0x1000c400, 0x1000c800, 0x1000d000, 0x1000d400 };
static const char *const DMA_NAME[10] = { "VIF0", "VIF1", "GIF", "fromIPU", "toIPU",
                                          "SIF0", "SIF1", "SIF2", "fromSPR", "toSPR" };

static const char *chan_name(uint32_t addr)
{
    int i;

    for (i = 0; i < 10; i++) {
        if (DMA_CHAN[i] == addr) {
            return DMA_NAME[i];
        }
    }
    return "?";
}

void w2c_env_sceDmaReset(struct w2c_env *env, u32 mode)
{
    shim_log(env->host, "sceDmaReset(%d)", (int)mode);
    TRACE_CALL(env->host, sceDmaReset, mode);
}

u32 w2c_env_sceDmaGetChan(struct w2c_env *env, u32 id)
{
    uint32_t addr = id < 10 ? DMA_CHAN[id] : 0;

    shim_log(env->host, "sceDmaGetChan(%u) = 0x%08x", id, addr);
    TRACE_RETURN(env->host, sceDmaGetChan, addr, id);
}

void w2c_env_sceDmaSend(struct w2c_env *env, u32 chan, u32 tag)
{
    char a[16];

    /* bit 31 of the address selects the scratchpad */
    shim_log(env->host, "sceDmaSend(%s, %s%s)", chan_name(chan), tag & 0x80000000u ? "spr:" : "",
             log_addr(tag & 0x7fffffffu, a));
    if (chan == DMA_CHAN[2] && env->host->gif_send) {
        env->host->gif_send(env->host->gif_ctx, tag);
    }
    TRACE_CALL(env->host, sceDmaSend, chan, tag);
}

u32 w2c_env_sceDmaSync(struct w2c_env *env, u32 chan, u32 mode, u32 timeout)
{
    shim_log(env->host, "sceDmaSync(%s, %d, %d) = 0", chan_name(chan), (int)mode, (int)timeout);
    TRACE_RETURN(env->host, sceDmaSync, 0, chan, mode, timeout);
}

u32 w2c_env_sceDmaGetEnv(struct w2c_env *env, u32 de)
{
    char a[16];

    shim_log(env->host, "sceDmaGetEnv(%s) = 0", log_addr(de, a));
    TRACE_RETURN(env->host, sceDmaGetEnv, 0, de);
}

u32 w2c_env_sceDmaPutEnv(struct w2c_env *env, u32 de)
{
    char a[16];

    shim_log(env->host, "sceDmaPutEnv(%s) = 0", log_addr(de, a));
    TRACE_RETURN(env->host, sceDmaPutEnv, 0, de);
}

/* libdev */
void w2c_env_sceDevVif0Reset(struct w2c_env *env)
{
    shim_log(env->host, "sceDevVif0Reset()");
    TRACE_CALL(env->host, sceDevVif0Reset);
}

void w2c_env_sceDevVu0Reset(struct w2c_env *env)
{
    shim_log(env->host, "sceDevVu0Reset()");
    TRACE_CALL(env->host, sceDevVu0Reset);
}

void w2c_env_sceDevGifPutImtMode(struct w2c_env *env, u32 mode)
{
    shim_log(env->host, "sceDevGifPutImtMode(%d)", (int)mode);
    TRACE_CALL(env->host, sceDevGifPutImtMode, mode);
}

/* kernel */
void w2c_env_FlushCache(struct w2c_env *env, u32 op)
{
    shim_log(env->host, "FlushCache(%d)", (int)op);
    TRACE_CALL(env->host, FlushCache, op);
}
