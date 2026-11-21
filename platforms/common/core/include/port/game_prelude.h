/* Force-included before every game TU in the port build (-include), with
 * GGX_TU_<name> defined for the TU being compiled. The matching build never
 * sees this file.
 *
 * Some game TUs call functions they never declare. EE-GCC 2.9 accepts that,
 * and on MIPS an implicit `int f()` call works whatever f returns. WebAssembly
 * checks every call's signature against the callee, so a void or variadic
 * callee reached through an implicit declaration links to a trap instead.
 *
 * Each block below gives one TU the declarations it is missing, with the
 * callee's real return type (the STABS signatures in include/generated). A
 * parameter list is left unprototyped, `T f();`, where the TU's own headers
 * name a parameter type differently from STABS.
 */
#ifndef GGX_PORT_GAME_PRELUDE_H
#define GGX_PORT_GAME_PRELUDE_H

/* TensionDraw's FAKEMATCH pins its locals to MIPS registers with asm labels
   (`register int k1 asm("$4")`, see FAKEMATCHES.md). They name nothing on
   wasm32; without them the locals are ordinary. Its empty __asm__ fences are
   a different keyword and stay. */
#ifdef GGX_TU_cockpit
#define asm(reg)
#endif

/* PacketDmaTagRef falls off its end without a return; nothing uses its value. */
#ifdef GGX_TU_packet
#pragma clang diagnostic ignored "-Wreturn-type"
#endif

#if defined(GGX_TU_g_strip) || defined(GGX_TU_mainctrl) || defined(GGX_TU_objwork) || \
    defined(GGX_TU_sound) || defined(GGX_TU_soundnew)
void debugf(char *format, ...);
#endif

/* These TUs declare debugf with one fixed parameter, but it is variadic
   (main.c defines it). Their calls go to a one-argument twin in the port Core. */
#if defined(GGX_TU_memacc) || defined(GGX_TU_stgsel) || defined(GGX_TU_tse_act)
#define debugf ggx_port_debugf1
#endif

/* Callers that declare a callee with another return type than its definition.
   They reach it through a twin in platforms/common/core/abi_glue.c that has the
   declared type and calls the real function. */
#ifdef GGX_TU_act
#define sound_request ggx_port_sound_request_void
#define voice_request ggx_port_voice_request_void
#define privatese_request ggx_port_privatese_request_void
#define Get_GCLExRand ggx_port_Get_GCLExRand_2
#endif
#ifdef GGX_TU_g_grasys
#define PXF_SendPalette ggx_port_PXF_SendPalette_void
#endif
#ifdef GGX_TU_main
#define MemoryInit ggx_port_MemoryInit_1
#endif
#ifdef GGX_TU_packet
#define sceGsSyncPath ggx_port_sceGsSyncPath_void
#endif
#ifdef GGX_TU_soundnew
#define sceSifInitRpc ggx_port_sceSifInitRpc_void
#endif
#ifdef GGX_TU_throw
#define EffectCreate ggx_port_EffectCreate_4
#define EndtoAction ggx_port_EndtoAction_void
#define EndtoStand ggx_port_EndtoStand_void
#define SearchCharacterExecute ggx_port_SearchCharacterExecute_void
#define TimetoAction ggx_port_TimetoAction_void
#endif
#ifdef GGX_TU_winner
#define FTO_Exe ggx_port_FTO_Exe_void
#endif

/* Splat names for functions that have a real name elsewhere (same address). */
#ifdef GGX_TU_cpu
#define func_001B1A18 get_cpu_level
#endif
#ifdef GGX_TU_file
#define func_001BBF50 sceCdSearchFile
#endif

#if defined(GGX_TU_ch_act) || defined(GGX_TU_player) || defined(GGX_TU_screen) || \
    defined(GGX_TU_vee_act) || defined(GGX_TU_zt_act)
int abs(int);
#endif

#ifdef GGX_TU_act
void ExCollisionClear();
void SetDefaultHitVoice();
void SetDefaultTension();
/* af_RENEWAL_COLLISION calls it with no arguments; offset is still in a0. */
void RenewalCollision();
#define RenewalCollision() RenewalCollision(offset)
#endif

#if defined(GGX_TU_actsub) || defined(GGX_TU_nameentr)
int g_roundmin(int val, int min);
#endif

#ifdef GGX_TU_cp_act
void SetActNoNext();
int SetChangeFrameAnime();
#endif

#ifdef GGX_TU_file
void *mmalloc(int size, char *name);
void *dmalloc(int size, char *name);
#endif

#ifdef GGX_TU_g_strip
void PXF_GetSize(int id, int *x, int *y);
void nlUnitMatrix(void);
void nlRotateAZ(unsigned short r);
void nlCalcPoint();
void nlsDispStripTex();
#endif

#ifdef GGX_TU_graph
void FlushCache(int operation);
#endif

#ifdef GGX_TU_select
void SetFadeSpeed(int frame);
void Rq_Fadeout(void);
int IsFadeEnd(void);
#endif

#ifdef GGX_TU_zte_act
void *EffectCreate_ActNo();
#endif

#ifdef GGX_TU_system
void g_strip_init(void);
void Screen_MemKeep(void);
void PolyEffInit(void);
#endif

#endif
