# Fake matches and stand-ins

Every place in `src/` where the C is not what the original source most likely said, but compiles to the original bytes. Each one is tagged in the source with the tag in the first column, so `grep -rn "FAKEMATCH\|_STANDIN\|STATIC_EXTERN\|SCHED_BARRIER" src/` finds them all. Last checked 2026-10-25, against the 3,615/3,615 build.

Keep this file in step with the source: add a row when a tag goes in, and delete it when natural C replaces the tag.

## Tags

| Tag | Meaning | Strength |
| --- | --- | --- |
| `FAKEMATCH` | Forces the compiler with constructs the original almost certainly didn't use (local register variables, empty asm, code-free statements kept only for their effect on the schedule). | Strongest. The function's C is a stand-in, not a decompilation. |
| `SCHED_BARRIER` | An empty volatile asm that fences the scheduler and emits nothing. | Strong; same kind as FAKEMATCH, used before that tag existed. |
| `REGALLOC_STANDIN` | Natural-looking C (an extra local, a parameter pun) that steers register allocation or spill order, with no evidence in the debug data. | Medium. |
| `INLINE_STANDIN` | The original inlined a function here; the stand-in calls or repeats it without `inline`. | Medium. |
| `TYPE_STANDIN` | A type that differs from the generated header (for example, an alignment that STABS doesn't record). | Low. Usually a header gap, not a code question. |
| `STATIC_EXTERN` | A function-local or file `static` declared `extern` at its fixed address, because a real `static` would be placed elsewhere. | Low. Data placement only. |

## FAKEMATCH and SCHED_BARRIER

| Function | File | Tag | What is forced | Natural form |
| --- | --- | --- | --- | --- |
| `TensionDraw` | `src/ggxeuc/cockpit.c` | `FAKEMATCH` (6 lines) | Three register pins: 193 in a0, 10000 in a2, the lh/mult value in a1. An empty-asm pair keeps v1 busy without code, and a dead `if (side) b = 1.0f;` fences sched1 before `offl`. | Open. 16 words off: `nonmatching/TensionDraw.c`. See [TensionDraw](#tensiondraw) below. |
| `DZBTAct_SHW1_HomingLaser` | `src/ggxeuc/dze_act.c` (line 1274) | `SCHED_BARRIER` | `__asm__ volatile ("");` keeps the `last` load out of the GetEnemyPlayer sched block, so `li rst,1` lands in the jal delay slot. | Open. |

## REGALLOC_STANDIN

| Function | File | Stand-in |
| --- | --- | --- |
| `grp_except_store_image` | `src/ggxeuc/graph.c` | `int fmt = *(int *)&source_format;` Taking the parameter's address renumbers its pseudo, so gcse's PRE creates the format<<16 register after x<<16 and reload gives it the higher spill slot. |
| `QuickConti_Execute` | `src/ggxeuc/winner.c` (line 1296) | Block-local `int a = alpha;`. There is no such local in the KR stabs; it ties alpha and alpha+0x20 to one pseudo (v1). |

## INLINE_STANDIN

| File | Where | Stand-in |
| --- | --- | --- |
| `src/ggxeuc/sprite.c` | line 916 | The original inlines XYConvP here (KR lines 189-191); XYConvP is defined without `inline`. |
| `src/ggxeuc/sprite.c` | line 950 | The original inlines XYConv here (KR lines 180-182). |
| `src/ggxeuc/sprite.c` | line 1114 | The original inlines XYConv here (KR lines 180-182). |

## TYPE_STANDIN

| File | Where | Stand-in |
| --- | --- | --- |
| `src/ggxeuc/bgsub.c` | line 725 (`MayCloud_Execute`) | `NLVERTEX03A`, NLVERTEX03 with 16-byte alignment. STABS does not record alignment, so `generated/types/NLVERTEX03.h` lacks it. Fixing the header may let the stand-in go. |

## STATIC_EXTERN

131 declarations in 30 files. The names below are the C names; the source comment on each line gives the address and, where known, the original static's name.

| File | Count | Declarations (line) |
| --- | ---: | --- |
| `src/ggxeuc/bg.c` | 5 | `LayerHeaderp` (203), `LayerAnimeTb` (221), `bgset_datap` (534), `BGBeingCount` (776, 1028) |
| `src/ggxeuc/fight.c` | 29 | `IntIrqMain_pal1`, `_sprtid1`, `_sprtid2`, `_sprtid3`, `_a`, `_fightctrl_time` (1687-1692); `TimeOverMain_sprtid`, `_pal`, `_y1`, `_y2`, `_fightctrl_time` (1812-1816); `BeforeFight_Rock_sprtid1`, `_sprtid2`, `_pal`, `_v`, `_x1` (1902-1906); `ScoreMain_time`, `_hp`, `_tp`, `_pp`, `_t`, `_h`, `_p`, `_wait`, `_total` (2109-2117); `DrawDispMain_sprtid`, `_pal`, `_v`, `_x1` (2223-2226) |
| `src/ggxeuc/finish.c` | 1 | `KakuseiFinish_wave` (281) |
| `src/ggxeuc/game.c` | 1 | `GameMain_SlowCount` (1127) |
| `src/ggxeuc/poly_eff.c` | 1 | `hibana_vertexdata` (23) |
| `src/ggxeuc/sprite.c` | 2 | `nlnDispNum` (151), `wrp` (152) |
| `src/ggxeuc/story.c` | 2 | `fade_picture_scene_width` (40), `fade_picture_scene_height` (41) |
| `src/ggxeuc/winner.c` | 13 | `quick_restart_mode` (50); `winner_cont_fade`, `_msk`, `_fms` (264-266); `winner_go_fms`, `_over_x`, `_over_t` (331-333); `winner_skip_y1`, `_y2`, `_fade`, `_target`, `_fms`, `_off_x` (1123-1128) |
| `src/sdk/newlib/locale.c` | 1 | the C locale's conventions table (10) |
| `src/sdk/sce/alarm.c` | 3 | `alarm_handler` (51), `alarm_entry` (53), `alarm_syscalls` (55) |
| `src/sdk/sce/deci2.c` | 1 | `deci2Buffer` (8) |
| `src/sdk/sce/eecdvd.c` | 15 | `cd_cbfunc` (104), `poff_cbarg` (108), `cb_thread_word` (110), `cd_thread_id` (112), `cd_thread_stat` (114), `cb_thread_param` (116), `poff_cd` (118), `poff_sdata` (120), `search_req` (122), `search_rdata` (124), `search_cd` (126), `init_cd` (128), `diskready_cd` (130), `init_sdata` (132), `diskready_sdata` (133) |
| `src/sdk/sce/eeloadfile.c` | 3 | `lf_buf` (17), `lf_cd` (19), `lf_iopVersion` (21) |
| `src/sdk/sce/exit.c` | 1 | `exit_SysEntry` (11) |
| `src/sdk/sce/filestub.c` | 9 | `rcv_adr` (26), `fsIoctlArg` (28), `fsRecvBuf` (93), `fsRcvPkt` (95), `fsIobTab` (97), `fsClient` (99), `fsVersion` (101), `fsCmdBuf` (103), `fsPoffHandler` (283) |
| `src/sdk/sce/glue.c` | 1 | `heap_ptr` (19) |
| `src/sdk/sce/graph_execstoreimage.c` | 1 | the `{0x06000000, 0, 0, 0}` table (18) |
| `src/sdk/sce/graph_resetgraph.c` | 1 | the `"PsIIlibgraph2300"` (JP) / `"PsIIlibgraph2500"` (KR) string (10) |
| `src/sdk/sce/graph_resetpath.c` | 1 | the STCYCL/STMASK/... VIF packet (9) |
| `src/sdk/sce/initsys.c` | 2 | `_SyscallEntry` (25), `initsys_SysEntry` (26) |
| `src/sdk/sce/iopreset.c` | 1 | `resetData` (12) |
| `src/sdk/sce/kprintf.c` | 1 | `deci2_line` (23) |
| `src/sdk/sce/libmc.c` | 10 | `mcClient` (50), `mcInfoType` (52), `mcInfoFree` (54), `mcInfoFormat` (56), `mcAux` (58), `mcCmd` (60), `mcName` (62), `mcRecv` (64), `mcPwd` (66), `mcRdata` (68) |
| `src/sdk/sce/libosd.c` | 1 | `osdsrc` (11) |
| `src/sdk/sce/libpad.c` | 5 | `padClient` (53), `padSlot` (55), `padCmd` (58 SDK 2.5, 60), `padRpcBuf` (63) |
| `src/sdk/sce/sifcmd.c` | 7 | `cmdSendBuf` (65), `cmdAckBuf` (67), `cmdInitPkt` (69), `cmdDmacId` (71), `cmdData` (73), `sysCmdTable` (75), `cmdSreg` (77) |
| `src/sdk/sce/sifrpc.c` | 4 | `rpc_pkt_table` (76), `rpc_rdata_table` (78), `rpc_client_table` (80), `rpc_data` (82) |
| `src/sdk/sce/thread.c` | 3 | `kernEventStack` (32), `kernEventSema` (34), `kernEventRing` (36) |
| `src/sdk/sce/tlbfunc.c` | 2 | `tlbsrc` (19), `tlbfunc_SysEntry` (20) |
| `src/sdk/sce/tty.c` | 4 | `tty_queue` (33), `tty_rec` (35), `tty_sbuf` (37), `tty_rbuf` (39) |

## Not stand-ins

- The `__asm__ __volatile__` blocks in `src/ggxeuc/nllib.c` (VU0 macro code) and the `src/sdk/gcc/` and `src/sdk/sce/` asm are the original's own inline assembly, not tags.
- Untagged renames that only avoid symbol collisions, such as bgsub's `bgsub_idtb`, change names, not code.

## TensionDraw

`TensionDraw` (`src/ggxeuc/cockpit.c`) draws the Tension gauge. It is the only function whose C is a stand-in. Its natural C, `nonmatching/TensionDraw.c`, behaves the same and compiles to the same instructions except in one block: the code right after the `GetTensionVal` call. There, the registers and the position of a `lui`/`lbu` pair differ, which leaves it 16 words off. Many attempts found no natural source form that gives the original's allocation, so the matched version adds six tagged lines to that natural C and changes nothing else:

```c
    int val = GetTensionVal(offset);
    register int k1 asm("$4") = 0xC1, k2 asm("$6") = 10000, m0 asm("$3");   /* FAKEMATCH */
    register int tb asm("$5") = offset->ply->TensionBarLength * k1;         /* FAKEMATCH */
    int ten = tb / k2;                                                      /* FAKEMATCH */
    ...
    __asm__("" : "=r"(m0));                                                 /* FAKEMATCH */
    __asm__("" : "+r"(a) : "r"(m0));                                        /* FAKEMATCH */
    if (side) b = 1.0f;                                                     /* FAKEMATCH */
    offl = userinfo.offs_cockpit_lower;
```

They replace one natural line, `int ten = offset->ply->TensionBarLength * 0xC1 / 10000;`. The program's behaviour is unchanged: both `asm` statements are empty, and `b` is assigned on every path before it is read.

What each piece does, with EE-GCC 2.9-ee-991111 at `-O2 -G8`:
- **`k1`, `k2` and `tb` (local register variables)** put 193 in a0, 10000 in a2 and the `lh`/`mult` product in a1, which are the original's registers. Hard registers skip local allocation, which also keeps a0, a1 and a2 out of reach of the block's other values.
- **`m0` and the two empty `asm` statements** keep v1 occupied between them without emitting code, so nothing else in the block is allocated to v1. The original's allocation needs v1 busy there, yet no instruction of the original sets it: whatever held it left no trace.
- **`if (side) b = 1.0f;`** ends the first basic block before `offl`, so the first scheduling pass can't hoist the `userinfo` load or the `lbu` above the division. The dead store and the empty branch are deleted before the second scheduling pass, which then keeps the original order. The condition must not read `val`: that keeps `val` live in v0 and moves `mflo` to v1.

All six are needed. Removing any one of them gives 7 to 13 words of difference.

**To replace it,** a natural form has to get the same three effects without hard registers or `asm`:
1. 193, 10000 and the product in a0, a2 and a1;
2. a v1 occupant that leaves no code;
3. a fence for the first scheduling pass before `offl` that keeps `val` out of v0 at the division.

When one matches on both discs with every other `cockpit.c` function still matching, restore the natural `ten` line, delete the six tagged lines, and remove this section and its table row.
