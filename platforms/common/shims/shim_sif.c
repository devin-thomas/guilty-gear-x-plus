/* SIF and IOP shims: the IOP side of the game, which the port does not run.
 *
 * The IOP reboots and loads its modules instantly. The one RPC server the
 * game binds is SNDRV.IRX, the sound driver (0x1010101). The calls
 * themselves, with their command numbers and the bytes the game sent, are
 * the sound command stream: snd_rpc hands each to the audio layer (audio/,
 * PORT-011), which plays it. Every call completes at once and the reply
 * buffer reads as zeros: no BGM playing (call 0x1F) and every sound bank
 * loaded (0x20), whether sound is on or off, so the Core computes the same
 * either way (audio/README.md, The replies stay zeros).
 */
#include <string.h>

#include "core_host.h"
#include "trace.h"

/* sceSifClientData: rpcd (16 bytes), command +0x10, buff +0x14, cbuff +0x18,
   func +0x1c, para +0x20, serve +0x24 */
#define CLIENT_SERVE 0x24
/* What a bound client's serve points at. On the PS2 it is the server's
   record in IOP memory, which the EE never dereferences; any nonzero value
   tells SnDrvInit the bind is done. */
#define SNDRV_SERVE 0x0001f000u

u32 w2c_env_sceSifInitRpc(struct w2c_env *env, u32 mode)
{
    shim_log(env->host, "sceSifInitRpc(%u)", mode);
    TRACE_RETURN(env->host, sceSifInitRpc, 0, mode);
}

u32 w2c_env_sceSifRebootIop(struct w2c_env *env, u32 img)
{
    char s[300];

    shim_log(env->host, "sceSifRebootIop(%s) = 1", log_str(env->host, img, s, sizeof(s)));
    TRACE_RETURN(env->host, sceSifRebootIop, 1, img);
}

u32 w2c_env_sceSifSyncIop(struct w2c_env *env)
{
    shim_log(env->host, "sceSifSyncIop() = 1");
    TRACE_RETURN(env->host, sceSifSyncIop, 1);
}

u32 w2c_env_sceFsReset(struct w2c_env *env)
{
    shim_log(env->host, "sceFsReset() = 0");
    TRACE_RETURN(env->host, sceFsReset, 0);
}

u32 w2c_env_sceSifLoadModule(struct w2c_env *env, u32 name, u32 arglen, u32 args)
{
    CoreHost *h = env->host;
    char s[300], a[16];
    uint32_t id = h->next_module_id++;

    shim_log(h, "sceSifLoadModule(%s, %u, %s) = %u", log_str(h, name, s, sizeof(s)), arglen, log_addr(args, a), id);
    TRACE_RETURN(env->host, sceSifLoadModule, id, name, arglen, args);
}

u32 w2c_env_sceSifBindRpc(struct w2c_env *env, u32 cd, u32 request, u32 mode)
{
    CoreHost *h = env->host;
    char a[16];

    ee_w32(h, cd + CLIENT_SERVE, SNDRV_SERVE);
    shim_log(h, "sceSifBindRpc(%s, 0x%x, %u) = 0", log_addr(cd, a), request, mode);
    TRACE_RETURN(env->host, sceSifBindRpc, 0, cd, request, mode);
}

u32 w2c_env_sceSifCallRpc(struct w2c_env *env, u32 cd, u32 rpc_number, u32 mode, u32 send, u32 ssize,
                          u32 receive, u32 rsize, u32 end_func, u32 end_param)
{
    CoreHost *h = env->host;
    char a[16], b[16], c[16];

    if (h->snd_rpc) {
        h->snd_rpc(h->snd_ctx, h->field, rpc_number, ssize ? ee_rptr(h, send, ssize) : NULL, ssize);
    }
    if (rsize) {
        memset(ee_ptr(h, receive, rsize), 0, rsize);
    }
    shim_log(h, "sceSifCallRpc(%s, 0x%02x, %u, %s, %u, %s, %u, 0x%x, 0x%x) = 0", log_addr(cd, a), rpc_number, mode,
             log_addr(send, b), ssize, log_addr(receive, c), rsize, end_func, end_param);
    TRACE_RETURN(env->host, sceSifCallRpc, 0, cd, rpc_number, mode, send, ssize, receive, rsize, end_func, end_param);
}

u32 w2c_env_sceSifCheckStatRpc(struct w2c_env *env, u32 cd)
{
    char a[16];

    shim_log(env->host, "sceSifCheckStatRpc(%s) = 0", log_addr(cd, a));
    TRACE_RETURN(env->host, sceSifCheckStatRpc, 0, cd);
}

/* IOP file I/O: only graph.c's screenshot dump to host0: uses it. */
u32 w2c_env_sceOpen(struct w2c_env *env, u32 name, u32 flags, u32 va)
{
    char s[300];

    (void)va;
    shim_log(env->host, "sceOpen(%s, 0x%x) = -1", log_str(env->host, name, s, sizeof(s)), flags);
    TRACE_RETURN(env->host, sceOpen, (u32)-1, name, flags, va);
}

u32 w2c_env_sceWrite(struct w2c_env *env, u32 fd, u32 buf, u32 n)
{
    char a[16];

    shim_log(env->host, "sceWrite(%d, %s, %u) = -1", (int)fd, log_addr(buf, a), n);
    TRACE_RETURN(env->host, sceWrite, (u32)-1, fd, buf, n);
}

u32 w2c_env_sceClose(struct w2c_env *env, u32 fd)
{
    shim_log(env->host, "sceClose(%d) = -1", (int)fd);
    TRACE_RETURN(env->host, sceClose, (u32)-1, fd);
}
