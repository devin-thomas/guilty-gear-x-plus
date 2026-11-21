/* libpad shims: both ports hold a DualShock 2 whose state is the host's
 * input for the current field (CoreHost.pads), encoded as libpad delivers
 * it (common/input/pad.h): digital (0x41) unless the host puts a port in
 * analog mode. The headless host leaves both at "nothing pressed". The
 * first read and the actuator alignment requests take as long as on the
 * PS2 (pad.h, PadRequest), by the Core's vblank count (the field running is
 * CoreHost.field + 1, as field counts the fields completed). The actuators
 * are kept for the host's rumble and logged.
 *
 * A request outlives the field, so it is Core state. It is kept where
 * libpad keeps its own: in the port's DMA buffer (pad_read_buf, the area
 * scePadPortOpen names), its last eight bytes, which libpad's EE side would
 * read and the game never does. So snapshots carry it, and the Lockstep
 * Traces, which mask that buffer because the IOP rewrites it every vblank,
 * are unaffected.
 */
#include <string.h>

#include "core_host.h"
#include "trace.h"

#define PAD_BUF 256   /* pad_read_buf[2][16] of 128-bit words */

static PadRequest request(CoreHost *h, u32 port)
{
    PadRequest r;

    r.call = ee_r32(h, ggx_sym_pad_read_buf + (port & 1) * PAD_BUF + PAD_BUF - 8);
    r.done = ee_r32(h, ggx_sym_pad_read_buf + (port & 1) * PAD_BUF + PAD_BUF - 4);
    return r;
}

u32 w2c_env_scePadInit(struct w2c_env *env, u32 mode)
{
    shim_log(env->host, "scePadInit(%u) = 1", mode);
    TRACE_RETURN(env->host, scePadInit, 1, mode);
}

u32 w2c_env_scePadPortOpen(struct w2c_env *env, u32 port, u32 slot, u32 addr)
{
    char a[16];

    shim_log(env->host, "scePadPortOpen(%u, %u, %s) = 1", port, slot, log_addr(addr, a));
    TRACE_RETURN(env->host, scePadPortOpen, 1, port, slot, addr);
}

u32 w2c_env_scePadGetReqState(struct w2c_env *env, u32 port, u32 slot)
{
    PadRequest r = request(env->host, port);
    u32 v = pad_req_state(&r, env->host->vsyncs);

    shim_log(env->host, "scePadGetReqState(%u, %u) = %u", port, slot, v);
    TRACE_RETURN(env->host, scePadGetReqState, v, port, slot);
}

u32 w2c_env_scePadRead(struct w2c_env *env, u32 port, u32 slot, u32 rdata)
{
    CoreHost *h = env->host;
    const PadInput *in = &h->pads[port & 1];
    PadRequest r = request(h, port);
    u32 v = 0;

    if (pad_read_ok(&r, h->field + 1, h->vsyncs)) {
        v = pad_encode(in, ee_ptr(h, rdata, 32));
        shim_log(h, "scePadRead(%u, %u) = %u buttons=0x%04x%s", port, slot, v, in->buttons,
                 in->mode == PAD_MODE_ANALOG ? " analog" : "");
    } else {
        /* libpad is not ready (the first read, or a request in its last vblanks): the buffer is left as it was */
        shim_log(h, "scePadRead(%u, %u) = 0", port, slot);
    }
    TRACE_RETURN(h, scePadRead, v, port, slot, rdata);
}

u32 w2c_env_scePadSetActAlign(struct w2c_env *env, u32 port, u32 slot, u32 data)
{
    CoreHost *h = env->host;
    const uint8_t *p = ee_ptr(h, data, 6);
    PadRequest r;

    pad_request(&r, h->vsyncs, h->pad_acted[port & 1]);
    ee_w32(h, ggx_sym_pad_read_buf + (port & 1) * PAD_BUF + PAD_BUF - 8, r.call);
    ee_w32(h, ggx_sym_pad_read_buf + (port & 1) * PAD_BUF + PAD_BUF - 4, r.done);
    shim_log(env->host, "scePadSetActAlign(%u, %u, %02x %02x %02x %02x %02x %02x) = 1", port, slot, p[0], p[1], p[2],
             p[3], p[4], p[5]);
    TRACE_RETURN(env->host, scePadSetActAlign, 1, port, slot, data);
}

u32 w2c_env_scePadSetActDirect(struct w2c_env *env, u32 port, u32 slot, u32 data)
{
    CoreHost *h = env->host;
    const uint8_t *p = ee_ptr(h, data, 2);
    PadRequest r = request(h, port);
    u32 v = pad_act_direct(&r, h->vsyncs);

    h->actuators[port & 1][0] = p[0];
    h->actuators[port & 1][1] = p[1];
    h->pad_acted[port & 1] = 1;
    shim_log(h, "scePadSetActDirect(%u, %u, %02x %02x) = %u", port, slot, p[0], p[1], v);
    TRACE_RETURN(h, scePadSetActDirect, v, port, slot, data);
}
