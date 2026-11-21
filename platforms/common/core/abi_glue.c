/* Twins for game calls whose declared signature differs from the callee's
 * definition (see port/game_prelude.h). WebAssembly would trap on the
 * mismatched call; on the EE these calls just ignored v0 or the extra
 * argument registers. Each twin has the caller's declared signature.
 */
struct CHARACTER_WORK;

int sound_request(int no);
int voice_request(struct CHARACTER_WORK *offset, int no);
int privatese_request(int no, int ch);
void *Get_GCLExRand(struct CHARACTER_WORK *offset);
int PXF_SendPalette(int id, int pal);
void MemoryInit(void);
int sceGsSyncPath(int mode, unsigned short timeout);
int sceSifInitRpc(unsigned int mode);
void *EffectCreate(struct CHARACTER_WORK *poffset, int idno, void *arg);
int EndtoAction(struct CHARACTER_WORK *offset, int no);
int EndtoStand(struct CHARACTER_WORK *offset);
void *SearchCharacterExecute(struct CHARACTER_WORK *offset, int idno, int a, int b);
int TimetoAction(struct CHARACTER_WORK *offset, int time, int no);
int FTO_Exe(int a, int b);
int IsGRAY(struct CHARACTER_WORK *offset);

/* act.c declares both as void. */
void ggx_port_sound_request_void(int no)
{
    sound_request(no);
}

void ggx_port_voice_request_void(struct CHARACTER_WORK *offset, int no)
{
    voice_request(offset, no);
}

void ggx_port_privatese_request_void(int no, int ch)
{
    privatese_request(no, ch);
}

/* act.c passes a second argument that gcl.c's definition never reads. */
void *ggx_port_Get_GCLExRand_2(struct CHARACTER_WORK *offset, int no)
{
    (void)no;
    return Get_GCLExRand(offset);
}

/* g_grasys.c declares it as void. */
void ggx_port_PXF_SendPalette_void(int id, int pal)
{
    PXF_SendPalette(id, pal);
}

/* main.c passes the memory card directory name; memory.c's MemoryInit takes none. */
void ggx_port_MemoryInit_1(char *dir)
{
    (void)dir;
    MemoryInit();
}

/* packet.c declares it as void. */
void ggx_port_sceGsSyncPath_void(int mode, unsigned short timeout)
{
    sceGsSyncPath(mode, timeout);
}

/* throw.c passes a fourth argument EffectCreate never reads, and declares the
   other four as void. */
void *ggx_port_EffectCreate_4(struct CHARACTER_WORK *poffset, int idno, void *arg, int unused)
{
    (void)unused;
    return EffectCreate(poffset, idno, arg);
}

void ggx_port_EndtoAction_void(struct CHARACTER_WORK *offset, int no)
{
    EndtoAction(offset, no);
}

void ggx_port_EndtoStand_void(struct CHARACTER_WORK *offset)
{
    EndtoStand(offset);
}

void ggx_port_SearchCharacterExecute_void(struct CHARACTER_WORK *offset, int idno, int a, int b)
{
    SearchCharacterExecute(offset, idno, a, b);
}

void ggx_port_TimetoAction_void(struct CHARACTER_WORK *offset, int time, int no)
{
    TimetoAction(offset, time, no);
}

/* winner.c declares it as void. */
void ggx_port_FTO_Exe_void(int a, int b)
{
    FTO_Exe(a, b);
}

/* dz_act.c declares IsGRAY(void) but calls it with offset in a0. */
int ggx_port_IsGRAY(struct CHARACTER_WORK *offset)
{
    return IsGRAY(offset);
}

/* soundnew.c declares it as void. */
void ggx_port_sceSifInitRpc_void(unsigned int mode)
{
    sceSifInitRpc(mode);
}

/* memacc.c, stgsel.c and tse_act.c declare debugf with one fixed parameter.
   debugf itself (main.c) is variadic and does nothing. */
void ggx_port_debugf1(char *format)
{
    (void)format;
}
