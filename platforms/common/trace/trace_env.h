/* The SDK calls a Lockstep Trace records (trace.h).
 *
 * Every call the game makes into a PS2 library that the port replaces with
 * a shim (common/shims/) crosses the boundary between what the Core
 * computes and what the PS2's hardware and IOP answer. The trace records
 * each one in order, with its return value and, for the calls listed in
 * the lockstep map, the bytes the PS2 wrote back that the game reads.
 * Replaying a trace, the shims answer from it instead.
 *
 * The ids are part of the trace format: never renumber, only append.
 * tools/lockstep_map.py reads this list.
 */
#ifndef GGX_TRACE_ENV_H
#define GGX_TRACE_ENV_H

#define TRACE_ENV_LIST(X) \
    X(1, sceGsSyncV) \
    X(2, scePadRead) \
    X(3, scePadGetReqState) \
    X(4, sceSifCheckStatRpc) \
    X(5, sceSifCallRpc) \
    X(6, sceCdSync) \
    X(7, sceCdRead) \
    X(8, sceCdSearchFile) \
    X(9, sceCdGetError) \
    X(10, sceCdDiskReady) \
    X(11, sceCdInit) \
    X(12, sceCdMmode) \
    X(13, sceCdStop) \
    X(14, sceGsResetGraph) \
    X(15, sceGsSetDefDispEnv) \
    X(16, sceGsSetDefDrawEnv) \
    X(17, sceGsSetDefStoreImage) \
    X(18, sceGsPutDispEnv) \
    X(19, sceGsResetPath) \
    X(20, sceGsSyncPath) \
    X(21, sceGsExecStoreImage) \
    X(22, sceDmaReset) \
    X(23, sceDmaGetChan) \
    X(24, sceDmaSend) \
    X(25, sceDmaSync) \
    X(26, sceDmaGetEnv) \
    X(27, sceDmaPutEnv) \
    X(28, sceDevVif0Reset) \
    X(29, sceDevVu0Reset) \
    X(30, sceDevGifPutImtMode) \
    X(31, FlushCache) \
    X(32, sceMcInit) \
    X(33, sceMcGetInfo) \
    X(34, sceMcSync) \
    X(35, sceMcOpen) \
    X(36, sceMcClose) \
    X(37, sceMcRead) \
    X(38, sceMcWrite) \
    X(39, sceMcMkdir) \
    X(40, sceMcChdir) \
    X(41, sceMcGetDir) \
    X(42, sceMcDelete) \
    X(43, sceMcFormat) \
    X(44, scePadInit) \
    X(45, scePadPortOpen) \
    X(46, scePadSetActAlign) \
    X(47, scePadSetActDirect) \
    X(48, sceSifInitRpc) \
    X(49, sceSifRebootIop) \
    X(50, sceSifSyncIop) \
    X(51, sceFsReset) \
    X(52, sceSifLoadModule) \
    X(53, sceSifBindRpc) \
    X(54, sceOpen) \
    X(55, sceWrite) \
    X(56, sceClose)

enum {
#define TRACE_ENV_ENUM(id, name) TRACE_ENV_##name = id,
    TRACE_ENV_LIST(TRACE_ENV_ENUM)
#undef TRACE_ENV_ENUM
    TRACE_ENV_MAX = 57
};

#endif
