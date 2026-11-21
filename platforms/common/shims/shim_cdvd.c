/* eecdvd (libcdvd) shims: the disc, read synchronously from the player's image.
 *
 * Every read completes inside sceCdRead, so sceCdSync always reports the
 * drive idle and there is never a read error. The real drive's timing is
 * not modelled: the game polls sceCdSync from FileReadMain once per field
 * and does nothing else while it waits.
 */
#include <string.h>

#include "core_host.h"
#include "trace.h"

/* sceCdlFILE: lsn +0x0, size +0x4, name[16] +0x8, date[8] +0x18 */

u32 w2c_env_sceCdInit(struct w2c_env *env, u32 mode)
{
    shim_log(env->host, "sceCdInit(%u) = 1", mode);
    TRACE_RETURN(env->host, sceCdInit, 1, mode);
}

u32 w2c_env_sceCdMmode(struct w2c_env *env, u32 media)
{
    shim_log(env->host, "sceCdMmode(%u) = 1", media);
    TRACE_RETURN(env->host, sceCdMmode, 1, media);
}

u32 w2c_env_sceCdDiskReady(struct w2c_env *env, u32 mode)
{
    shim_log(env->host, "sceCdDiskReady(%u) = 2", mode);
    TRACE_RETURN(env->host, sceCdDiskReady, 2, mode);   /* SCECdComplete */
}

u32 w2c_env_sceCdSearchFile(struct w2c_env *env, u32 fp, u32 name)
{
    CoreHost *h = env->host;
    char path[256], s[300], a[16];
    uint32_t lsn, size;
    int found;

    ee_str(h, name, path, sizeof(path));
    found = disc_find(&h->disc, path, &lsn, &size) == 0;
    if (found) {
        const char *base = strrchr(path, '\\');
        const char *src = base ? base + 1 : path;
        size_t len = strlen(src);
        char *n = (char *)ee_ptr(h, fp + 8, 16);

        ee_w32(h, fp, lsn);
        ee_w32(h, fp + 4, size);
        memset(n, 0, 16);
        memcpy(n, src, len < 15 ? len : 15);
        shim_log(h, "sceCdSearchFile(%s, %s) = 1 lsn=%u size=%u", log_addr(fp, a), log_str(h, name, s, sizeof(s)),
                 lsn, size);
    } else {
        shim_log(h, "sceCdSearchFile(%s, %s) = 0", log_addr(fp, a), log_str(h, name, s, sizeof(s)));
    }
    TRACE_RETURN(env->host, sceCdSearchFile, found, fp, name);
}

u32 w2c_env_sceCdRead(struct w2c_env *env, u32 lsn, u32 sectors, u32 buf, u32 mode)
{
    CoreHost *h = env->host;
    char a[16], m[16];
    int ok = disc_read(&h->disc, lsn, sectors, ee_ptr(h, buf, sectors * 2048)) == 0;

    shim_log(h, "sceCdRead(lsn=%u, sectors=%u, %s, %s) = %d", lsn, sectors, log_addr(buf, a), log_addr(mode, m), ok);
    if (h->snd_cd_read) {
        h->snd_cd_read(h->snd_ctx, h->field, lsn, sectors);   /* the sound driver's drive model (PORT-011) */
    }
    TRACE_RETURN(env->host, sceCdRead, ok, lsn, sectors, buf, mode);
}

u32 w2c_env_sceCdSync(struct w2c_env *env, u32 mode)
{
    shim_log(env->host, "sceCdSync(%u) = 0", mode);
    TRACE_RETURN(env->host, sceCdSync, 0, mode);   /* idle */
}

u32 w2c_env_sceCdGetError(struct w2c_env *env)
{
    shim_log(env->host, "sceCdGetError() = 0");
    TRACE_RETURN(env->host, sceCdGetError, 0);
}

u32 w2c_env_sceCdStop(struct w2c_env *env)
{
    shim_log(env->host, "sceCdStop() = 1");
    TRACE_RETURN(env->host, sceCdStop, 1);
}
