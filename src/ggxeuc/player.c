#include "generated/player.h"

static const short idtb[] = { 1, 2, 3 };

void Score_AddByCombo(int side, int dam, int combo, unsigned int flag);

void HitDispRequest(CHARACTER_WORK *offset, TACTHEADER *ahp)
{
    if (offset->ply->HitCount && (offset->actst & 0x20)) {
        offset->ply->HitCount++;
    } else {
        offset->ply->HitCount = 1;
    }
    Score_AddByCombo(offset->padid == 0, ahp->damage, offset->ply->HitCount, ahp->flag);
    offset->ply->reqhit = 1;
}

void HitDisp(CHARACTER_WORK *offset)
{
    int l;
    int ld;
    int ll;

    if (offset->ply->reqhit) {
        offset->ply->hitDispTime = 120;
        offset->ply->reqhit = 0;
        offset->ply->hitDispIn = 0;
    } else if (offset->ply->hitDispTime) {
        offset->ply->hitDispTime--;
        offset->ply->hitDispIn = offset->ply->hitDispIn + 0.0625;
        if (offset->ply->hitDispIn >= 1.0) {
            offset->ply->hitDispIn = 1.0f;
        }
    }
    l = offset->HitPoint;
    ld = offset->ply->lifelengthdamage;
    ll = offset->ply->lifelengthdamagelock;
    if ((offset->actst & 0x220) && (~offset->actst & 0x10000)) {
        if (ld > ll) {
            ld--;
        }
        if (ld < ll) {
            ld = ll;
        }
    } else {
        ll = l;
        if (ld > ll) {
            ld--;
        }
    }
    if (ld < l) {
        ll = l;
        ld = l;
    }
    offset->ply->lifelength = l;
    offset->ply->lifelengthdamage = ld;
    offset->ply->lifelengthdamagelock = ll;
    if (offset->ply->Tension < offset->ply->TensionBarLength) {
        offset->ply->TensionBarLength -= 100;
        if (offset->ply->Tension > offset->ply->TensionBarLength) {
            offset->ply->TensionBarLength = offset->ply->Tension;
        }
    }
    if (offset->ply->Tension > offset->ply->TensionBarLength) {
        offset->ply->TensionBarLength += 100;
        if (offset->ply->Tension < offset->ply->TensionBarLength) {
            offset->ply->TensionBarLength = offset->ply->Tension;
        }
    }
}

int BackStepCheck(CHARACTER_WORK *offset);
int BackWalkCheck(CHARACTER_WORK *offset);
int CmnAttackCheck(CHARACTER_WORK *offset, unsigned int orgflag);
int CmnDustAttackCheck(CHARACTER_WORK *offset, unsigned int orgflag);
int DashCheck(CHARACTER_WORK *offset);
int FaintCheck(CHARACTER_WORK *offset);
int ForwardWalkCheck(CHARACTER_WORK *offset);
int GuardCheck(CHARACTER_WORK *offset, int flag);
int GuardSpecialAttackCheck(CHARACTER_WORK *offset);
int HomingJumpCancelCheck(CHARACTER_WORK *offset);
int JumpCheck(CHARACTER_WORK *offset);
int NoCancelAttackCheck(CHARACTER_WORK *offset);
int PlayerCommandThrowCheck(CHARACTER_WORK *offset);
int PlayerThrowCheck(CHARACTER_WORK *offset);
int ROMANCHANSELCheck(CHARACTER_WORK *offset);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
int SitBackWalkCheck(CHARACTER_WORK *offset);
int SitCheck(CHARACTER_WORK *offset);
int SitForwardWalkCheck(CHARACTER_WORK *offset);
int SkyDashCheck(CHARACTER_WORK *offset);
int SkyJumpCheck(CHARACTER_WORK *offset);
int SpecialAttackCheck(CHARACTER_WORK *offset);
int StandCheck(CHARACTER_WORK *offset);
int TurnCheck(CHARACTER_WORK *offset);
int UkemiCheck(CHARACTER_WORK *offset);
void clrl_commanddat(int side);
extern int BattleStatus;
extern int AfterBattleCounter;
extern GAME_INFOMATION g_info;
extern story_t story;

void PlayerActionBreak(CHARACTER_WORK *offset)
{
    if (offset->HitStopTime == 0 && offset->actnonext != -1) {
        if (SetActNo(offset, offset->actnonext)) {
            goto end;
        }
    }
    if (offset->HitStopTime == 0 && offset->LandBreakActno != -1 && (~offset->actst & 0x10)) {
        if (offset->ply->LandInterrupt) {
            ((void (*)())offset->ply->LandInterrupt)(offset);
        }
        if (SetActNo(offset, offset->LandBreakActno)) {
            goto end;
        }
    }
    if (BattleStatus != 2 && (BattleStatus != 3 || !AfterBattleCounter)) {
        offset->CommandFlag &= ~0x800;
        offset->attackst &= ~0x40000;
        offset->CommandFlag &= ~0x400;
        offset->attackst &= ~0x20;
        offset->CommandFlag |= 0xC056;
        offset->CommandFlag &= ~0x200;
        offset->attackst &= ~0x10;
    }
    if ((~offset->CommandFlag & 0x40) && (~offset->actst & 0x20) && FaintCheck(offset)) {
        offset->actst &= ~0x8000;
        goto end;
    }
    switch (offset->idno) {
    case 1:
        if (offset->ply->PersonalWork[0].w[0] == 1 && !(offset->CommandFlag & 0x40) && !(offset->actst & 0x30)) {
            offset->ply->PersonalWork[0].w[0] = 0;
            offset->SpriteFlag &= ~0x4000;
            SetActNo(offset, 0xAF);
            goto end;
        }
        break;
    case 16:
        break;
    case 17:
        if (offset->ply->PersonalWork[1].ub[0]) {
            offset->ply->PersonalWork[1].ub[0]--;
        }
        break;
    }
    if (offset->ply->ExCommandFunc && !(offset->actst & 0x4000)) {
        if (((int (*)())offset->ply->ExCommandFunc)(offset)) {
            goto end;
        }
    }
    if (offset->actst & 0x4000) {
        goto end;
    }
    if (offset->actst & 0x200) {
        GuardSpecialAttackCheck(offset);
        goto end;
    }
    if ((offset->CommandFlag & 0x800) && UkemiCheck(offset)) {
        goto end;
    }
    if (offset->actst & 0x20) {
        goto end;
    }
    if (offset->actst & 0x200) {
        goto end;
    }
    if (offset->actst & 0x80000) {
        goto end;
    }
    if (offset->ply->KeyHook) {
        if (!((int (*)())offset->ply->KeyHook)(offset)) {
            goto end;
        }
        offset->ply->KeyHook = 0;
    }
    if (offset->ply->TShand) {
        offset->CommandFlag |= 0x40;
        offset->CommandFlag &= ~0x1000;
        offset->attackst &= ~0x30;
        offset->ply->TShand = 0;
    }
    if ((offset->CommandFlag & 0x400) && SkyDashCheck(offset)) {
        goto end;
    }
    if (offset->attackst & 0x1000) {
        if (ROMANCHANSELCheck(offset)) {
            clrl_commanddat(offset->padid);
            goto end;
        }
    }
    if ((offset->attackst & 0x100) && HomingJumpCancelCheck(offset)) {
        goto end;
    }
    if (offset->ply->sousaitime > 0) {
        if (CmnDustAttackCheck(offset, -1)) goto end;
        if (SpecialAttackCheck(offset)) goto end;
        if (CmnAttackCheck(offset, -1)) goto end;
    }
    if ((offset->attackst & 0x400000) && (~offset->attackst & 0x2000)) {
        if (NoCancelAttackCheck(offset)) goto end;
        if (SpecialAttackCheck(offset)) goto end;
    }
    if ((offset->attackst & 0x200) && (~offset->attackst & 0x2000)) {
        if (NoCancelAttackCheck(offset)) goto end;
        if (CmnDustAttackCheck(offset, -1)) goto end;
        if (GuardCheck(offset, 1)) goto end;
        if (SpecialAttackCheck(offset)) goto end;
    }
    if (offset->attackst & 0x20) {
        if (HomingJumpCancelCheck(offset)) goto end;
        if (SpecialAttackCheck(offset)) goto end;
    }
    if ((offset->attackst & 0x1010) == 0x1010) {
        if (CmnDustAttackCheck(offset, offset->ply->chainflag)) goto end;
        if (CmnAttackCheck(offset, offset->ply->chainflag)) goto end;
    }
    if (offset->ply->rendaflag) {
        if (CmnDustAttackCheck(offset, offset->ply->rendaflag)) goto end;
        if (CmnAttackCheck(offset, offset->ply->rendaflag)) goto end;
    }
    if (!(offset->CommandFlag & 0x40)) {
        if (PlayerCommandThrowCheck(offset)) goto end;
        if (NoCancelAttackCheck(offset)) goto end;
        if (CmnDustAttackCheck(offset, -1)) goto end;
        if (GuardCheck(offset, 1)) goto end;
        if (SpecialAttackCheck(offset)) goto end;
        if (PlayerThrowCheck(offset)) goto end;
        if (CmnAttackCheck(offset, -1)) goto end;
        offset->actst &= ~0x8000;
    } else if ((offset->CommandFlag & 0x1000) && SpecialAttackCheck(offset)) {
        goto end;
    }
    if ((offset->attackst & 0x41000) == 0x41000) {
        if (!(offset->actst & 0x10)) {
            if (JumpCheck(offset)) goto end;
        } else {
            if (SkyJumpCheck(offset)) goto end;
        }
    } else if (!(offset->CommandFlag & 0x10)) {
        if (HomingJumpCancelCheck(offset)) goto end;
        offset->ply->SkyDashCount = 0;
        offset->ply->JumpCount = 0;
        if (JumpCheck(offset)) goto end;
    } else if (offset->CommandFlag & 0x20) {
        if (SkyJumpCheck(offset)) goto end;
    }
    if (!(offset->CommandFlag & 1) && StandCheck(offset)) goto turn;
    if (!(offset->CommandFlag & 2)) {
        if (DashCheck(offset)) goto turn;
        if (ForwardWalkCheck(offset)) goto turn;
    }
    if (!(offset->CommandFlag & 0x4000) && SitForwardWalkCheck(offset)) goto turn;
    if (!(offset->CommandFlag & 4)) {
        if (BackStepCheck(offset)) goto end;
        if (GuardCheck(offset, 0)) goto end;
        if (BackWalkCheck(offset)) goto turn;
    }
    if (!(offset->CommandFlag & 0x8000) && SitBackWalkCheck(offset)) goto turn;
    if ((offset->CommandFlag & 0x200) && BackStepCheck(offset)) goto end;
    if (!(offset->CommandFlag & 8)) {
        if (GuardCheck(offset, 0)) goto end;
        SitCheck(offset);
    }
turn:
    if ((g_info.mode & 0x2000) && story.is_enter) goto end;
    if (TurnCheck(offset)) goto end;
    if (!GuardCheck(offset, 0)) return;
end:
    if (!(offset->actst & 0x20)) {
        offset->actst &= ~0x8000;
    }
}

CHARACTER_WORK *EffectCreate_ActNo(CHARACTER_WORK *poffset, unsigned short idno, CREATE_ARG *arg, unsigned short actno);
void GuardPointExecute(CHARACTER_WORK *offset);
void HamonCreate(CHARACTER_WORK *offset);
void HomingExecute(CHARACTER_WORK *offset);
int IsLeft(CHARACTER_WORK *offset);
CHARACTER_WORK *ObjectCreateA(CHARACTER_WORK *poffset, unsigned short idno, CREATE_ARG *arg);
int Random(void);
CHARACTER_WORK *SearchEffectWorkActno(CHARACTER_WORK *off, unsigned short idno, int actno);
CHARACTER_WORK *SearchObjectWork(CHARACTER_WORK *off, unsigned short idno);
void SetChainFlag(CHARACTER_WORK *offset);
void SetGuardSt(CHARACTER_WORK *offset, int pgflag);
void SetShakeoffX(CHARACTER_WORK *offset);
int g_roundmax(int val, int max);
int g_roundmin(int val, int min);
int CmnGuardAct(CHARACTER_WORK *offset, TACTHEADER *header, CHARACTER_WORK *aoffset, int cnt);
extern void (*SLAct_jumpTb[])();
extern void (*KYAct_jumpTb[])();
extern void (*MYAct_jumpTb[])();
extern void (*MLAct_jumpTb[])();
extern void (*AXAct_jumpTb[])();
extern void (*POAct_jumpTb[])();
extern void (*CHAct_jumpTb[])();
extern void (*ZTAct_jumpTb[])();
extern void (*BKAct_jumpTb[])();
extern void (*FAAct_jumpTb[])();
extern void (*TSAct_jumpTb[])();
extern void (*JMAct_jumpTb[])();
extern void (*ANAct_jumpTb[])();
extern void (*JYAct_jumpTb[])();
extern void (*VEAct_jumpTb[])();
extern void (*DZAct_jumpTb[])();
extern void (*KRAct_jumpTb[])();
extern void (*JSAct_jumpTb[])();
extern int BattleStatus;
extern u_int TotalCounter;
extern GAME_INFOMATION g_info;
extern SURVIVAL_INFOMATION *sv_info;

void PlayerMain(CHARACTER_WORK *offset)
{
    void *ActionFuncTbTb[19] = {
        0, SLAct_jumpTb, KYAct_jumpTb, MYAct_jumpTb, MLAct_jumpTb, AXAct_jumpTb, POAct_jumpTb,
        CHAct_jumpTb, ZTAct_jumpTb, BKAct_jumpTb, FAAct_jumpTb, TSAct_jumpTb, JMAct_jumpTb,
        ANAct_jumpTb, JYAct_jumpTb, VEAct_jumpTb, DZAct_jumpTb, KRAct_jumpTb, JSAct_jumpTb,
    };
    void (**FuncTb)();
    int change;

    FuncTb = ActionFuncTbTb[offset->idno];
    SetChainFlag(offset);
    FuncTb[offset->actno](offset);
    HomingExecute(offset);
    SetGuardSt(offset, 1);
    GuardPointExecute(offset);
    if (IsLeft(offset)) {
        if (offset->ply->JustGuardTime < 0) {
            offset->ply->JustGuardIgnoreTime = 0;
        } else if (offset->ply->JustGuardTime > 0) {
            if (offset->ply->JustGuardTime < 20) {
                offset->ply->JustGuardTime++;
            }
        } else if (offset->ply->JustGuardIgnoreTime == 0) {
            offset->ply->JustGuardTime++;
            offset->ply->JustGuardIgnoreTime = 30;
        }
    } else {
        offset->ply->JustGuardTime = 0;
    }
    if (offset->ply->JustGuardIgnoreTime > 0) {
        offset->ply->JustGuardIgnoreTime--;
    }
    if (offset->ply->JGWhiteTime <= 0) {
        offset->SpriteFlag &= ~0x800000;
    } else {
        offset->ply->JGWhiteTime--;
    }
    if ((offset->actst & 0x80000) && offset->HitStopTime == 0) {
        offset->actst &= ~0x80000;
    }
    if (offset->HitStopTime == 0) {
        offset->ActTimer++;
        if (offset->ply->TensionPenaltyTime) {
            offset->ply->TensionPenaltyTime--;
        }
        if (offset->ply->TensionMovePenaltyTime) {
            offset->ply->TensionMovePenaltyTime--;
        }
    }
    if (offset->attackst & 0x100000) {
        offset->ply->GuardFunc = CmnGuardAct;
        offset->attackst &= ~0x200;
        offset->GuardSt = 0x201;
    }
    if (BattleStatus != 2) {
        offset->ply->AttackDown = 0;
        offset->ply->DefenceDown = 0;
        offset->ply->JumpSeal = 0;
        offset->ply->ArtsSeal = 0;
        offset->ply->TShand = 0;
        offset->ply->Poison = 0;
        offset->ply->MistTime = 0;
    }
    if (offset->HitStopTime == 0) {
        if (!(offset->actst & 0x4000)) {
            if (offset->ply->DownTimer > 0) {
                offset->ply->DownTimer--;
            }
            if (offset->ply->DownTimer == 0) {
                offset->CommandFlag |= 0x800;
            } else {
                offset->CommandFlag &= ~0x800;
            }
        }
        if (offset->ply->DustTime) {
            offset->ply->DustTime--;
        }
    }
    if (TotalCounter & 1) {
        change = 0;
        if (offset->ply->AttackDown) {
            offset->ply->AttackDown--;
            change = 1;
            offset->SpriteFlag |= 0x80000;
        }
        if (offset->ply->DefenceDown) {
            offset->ply->DefenceDown--;
            change = 1;
            offset->SpriteFlag |= 0x80000;
        }
        if (offset->ply->JumpSeal) {
            offset->ply->JumpSeal--;
            change = 1;
            offset->SpriteFlag |= 0x80000;
        }
        if (offset->ply->ArtsSeal) {
            offset->ply->ArtsSeal--;
            change = 1;
            offset->SpriteFlag |= 0x80000;
        }
        if (!change) {
            offset->SpriteFlag &= ~0x80000;
        }
        change = 0;
        if (offset->ply->Poison) {
            offset->ply->Poison--;
            change = 1;
            offset->SpriteFlag |= 0x100000;
        }
        if (!change) {
            offset->SpriteFlag &= ~0x100000;
        }
        if (offset->ply->MistTime) {
            offset->ply->MistTime--;
            if (TotalCounter & 2) {
                CREATE_ARG c_arg;
                c_arg.flag = 2;
                c_arg.x = 0;
                c_arg.y = -(Random() & 0xFF) * 100;
                EffectCreate_ActNo(offset, 0x3F, &c_arg, 12);
            }
        }
    }
    if (offset->SpriteFlag & 0x200) {
        if (offset->ply->whiteouttime) {
            offset->ply->whiteouttime--;
            if (offset->ply->whiteouttime == 0) {
                offset->SpriteFlag &= ~0x200;
            }
        }
    } else {
        offset->ply->whiteouttime = 0;
    }
    if (offset->actst & 0x20) {
        offset->ply->notThrowTime = 7;
    }
    if (offset->actst & 0x200) {
        offset->ply->notThrowTime = 6;
    }
    if (offset->actst & 0x8000) {
        offset->ply->notThrowTime = 10;
    }
    if (offset->HitStopTime == 0 && offset->ply->notThrowTime > 0) {
        offset->ply->notThrowTime--;
    }
    if (offset->actst & 0x20) {
        if (~offset->ply->DownFlag & 0x400) {
            offset->ply->DamageTime = g_roundmax(offset->ply->DamageTime + 1, 960);
        }
    } else {
        offset->ply->DamageTime = 0;
        offset->ply->DownFlag &= ~0x400;
    }
    if (offset->ply->MutekiTime) {
        if (offset->HitStopTime == 0) {
            offset->ply->MutekiTime--;
        }
        offset->actst |= 0x20000;
    } else {
        offset->actst &= ~0x20000;
    }
    if (offset->actst & 0x8000) {
        offset->actst |= 0x80;
    }
    if (offset->ply->sousaitime > 0) {
        offset->ply->sousaitime--;
    }
    if (offset->ply->BackStepInvalid) {
        offset->ply->BackStepInvalid--;
    }
    if (offset->ply->ShakeTime) {
        SetShakeoffX(offset);
        offset->ply->ShakeTime--;
    }
    if (offset->ply->Poison && !(TotalCounter & 0x1F) && offset->HitPoint) {
        offset->HitPoint -= 4;
        offset->HitPoint = g_roundmin(offset->HitPoint, 1);
    }
    switch (offset->idno) {
    case 1:
        if (offset->HitPoint == 0) {
            offset->ply->PersonalWork[0].w[0] = 0;
        }
        if (offset->ply->PersonalWork[0].w[0] >= 2) {
            if (!(g_info.mode & 0x200) || offset->padid == sv_info->ply || !sv_info->sv_ext.battle) {
                offset->ply->PersonalWork[0].w[0]--;
            }
            offset->SpriteFlag |= 0x4000;
        } else if (offset->ply->PersonalWork[0].w[0] == 1) {
            offset->SpriteFlag |= 0x4000;
        } else {
            offset->SpriteFlag &= ~0x4000;
        }
        break;
    case 16:
        if ((~offset->ply->ObjFlag & 1) && !(offset->CommandFlag & 0x40) && !(offset->actst & 0x30)) {
            EffectCreate_ActNo(offset, 0x43, 0, 0x12);
        }
        break;
    case 7:
        if (offset->ply->PersonalWork[1].w[0] > 0) {
            offset->ply->PersonalWork[1].w[0]--;
            offset->SpriteFlag |= 0x10000;
        } else {
            offset->SpriteFlag &= ~0x10000;
        }
        break;
    case 11:
        if (!(offset->actst & 0x4020) && !SearchObjectWork(offset, 0x52)) {
            CREATE_ARG c_arg;
            c_arg.flag = 5;
            c_arg.y = -50000;
            c_arg.actno = 0;
            if (offset->dirflag) {
                c_arg.x = -40000;
            } else {
                c_arg.x = 40000;
            }
            ObjectCreateA(offset, 0x52, &c_arg);
        }
        break;
    case 8:
        if (!SearchObjectWork(offset, 0x23)) {
            if (offset->ply->PersonalWork[0].ub[1]) {
                offset->ply->PersonalWork[1].w[0] = g_roundmax(offset->ply->PersonalWork[1].w[0] + 4, 600);
            } else {
                offset->ply->PersonalWork[1].w[0] = g_roundmax(offset->ply->PersonalWork[1].w[0] + 2, 600);
            }
            if (offset->ply->PersonalWork[1].w[0] == 600) {
                offset->ply->PersonalWork[0].ub[1] = 1;
            }
        }
        break;
    case 12:
        if (offset->ply->PersonalWork[1].ub[3]) {
            offset->ply->PersonalWork[1].ub[3]--;
        }
        break;
    case 14:
        if (!SearchEffectWorkActno(offset, 0x55, 3)) {
            EffectCreate_ActNo(offset, 0x55, 0, 3);
        }
        break;
    }
    offset->ply->ObjFlag = 0;
    HamonCreate(offset);
}

void InertiaExe(CHARACTER_WORK *offset);
void JumpControlMain(CHARACTER_WORK *offset);

void PlayerMove(CHARACTER_WORK *offset)
{
    if (!(offset->ply->JumpFlag & 2)) {
        offset->posx += offset->xspeed;
    }
    InertiaExe(offset);
    if (offset->actst & 0x200000) {
        JumpControlMain(offset);
    }
}

extern CHARACTER_WORK *PlayerWork;

void CheckHitBack(CHARACTER_WORK *offset)
{
    if (offset->HitStopTime == 0 && (offset->actst & 0x220) && (offset->actst & 0x800)) {
        if (offset->actst & 2) {
            PlayerWork[1].RHitBackx = offset->HitBackx;
        } else {
            PlayerWork[0].RHitBackx = offset->HitBackx;
        }
    }
}

void PlayerMove2(CHARACTER_WORK *offset)
{
    offset->posx -= offset->RHitBackx;
    offset->RHitBackx = 0;
    if (abs(offset->AdBackx) > 50) {
        offset->posx -= offset->AdBackx;
        offset->AdBackx -= offset->AdBackx / 6;
    } else {
        offset->AdBackx = 0;
    }
}

extern CHARACTER_WORK *EffectWork;
extern int DebugFlag;

void PlayerInfoDisp(void)
{
    CHARACTER_WORK *offset;
    int i;
    int cnt;

    if (DebugFlag & 0x10) {
        for (i = 0; i < 2; i++) {
        }
        for (i = 0; i < 48; i++) {
        }
        cnt = 0;
        for (i = 0; i < 64; i++) {
            offset = &EffectWork[i];
            if (offset->idno < 0x80) {
                if (cnt++ >= 7) {
                    break;
                }
            }
        }
    }
    if (DebugFlag & 0x80) {
        cnt = 0;
        for (i = 0; i < 64; i++) {
            offset = &EffectWork[i];
            if (offset->idno >= 0x80) {
                if (cnt++ >= 9) {
                    break;
                }
            }
        }
    }
}

char *GetCommonPaletteAddr(CHARACTER_WORK *offset, int no);
int privatese_request(CHARACTER_WORK *offset, int no);
int sound_request_main(int no, int type);
void voice_off(CHARACTER_WORK *offset);
int voice_request(CHARACTER_WORK *offset, int no);
extern GAME_INFOMATION g_info;
extern PAD pads[2];
extern CHARACTER_WORK *PlayerWork;

void PlayerSetting(void)
{
    int i;
    static int menu[2] = { 0, 0 };
    static int sec[2] = { 0, 0 };
    static int voice[2] = { 0, 0 };
    static int se[2] = { 0, 0 };
    CHARACTER_WORK *offset;

    for (i = 0; i < 2; i++) {
        offset = &PlayerWork[i];
        if (pads[i].trg & 0x74) {
            switch (menu[i]) {
            case 0:
                offset->ply->CPU = !offset->ply->CPU;
                break;
            case 1:
                g_info.PlayerPalette[i] = (unsigned char)(g_info.PlayerPalette[i] + 1) % 6;
                offset->CommonPalette = GetCommonPaletteAddr(offset, g_info.PlayerPalette[i]);
                offset->SpriteFlag |= 0x400;
                break;
            case 4:
                sound_request_main(sec[i], 0);
                break;
            case 6:
                if (pads[i].trg & 0x54) {
                    voice_off(offset);
                }
                voice_request(offset, voice[i]);
                break;
            case 7:
                privatese_request(offset, se[i]);
                break;
            }
        }
        if (pads[i].trg & 0x8000) {
            switch (menu[i]) {
            case 4:
                sec[i]--;
                break;
            case 6:
                voice[i]--;
                break;
            case 7:
                se[i]--;
                break;
            }
        }
        if (pads[i].trg & 0x2000) {
            switch (menu[i]) {
            case 4:
                sec[i]++;
                break;
            case 6:
                voice[i]++;
                break;
            case 7:
                se[i]++;
                break;
            }
        }
        if (pads[i].trg & 0x1000) {
            menu[i]--;
        }
        if (pads[i].trg & 0x4000) {
            menu[i]++;
        }
        menu[i] &= 7;
    }
}

float nlSqrt(float src);

void PlayerSpeedCalc(CHARACTER_WORK *offset)
{
    float x;
    float y;

    x = offset->xspeed;
    if (~offset->ply->inertiaflag & 2) {
        x += offset->ply->inertia;
    }
    if (offset->actst & 0x10) {
        x = x * 1.5;
        if (offset->yspeed > 0) {
            y = offset->yspeed / 4;
        } else {
            y = offset->yspeed / 2;
        }
        y = y * y;
    } else {
        y = 0;
    }
    x = x * x;
    offset->ply->SpeedSv = nlSqrt(x + y);
}

void Char2ScreenCollision(CHARACTER_WORK *offset);
void Char2WorldCollision(CHARACTER_WORK *offset);
void CharPosCollisionExecute(void);
void ExeAnime(CHARACTER_WORK *offset);
void ExePosByEnemy(CHARACTER_WORK *offset);
void PlayerColliCheck(CHARACTER_WORK *offset);
void PlayerCommandKeyExecute(CHARACTER_WORK *offset);
int PlayerCommandThrowFlagSet(void);
int PlayerThrowFlagSet(void);
void SetLR(CHARACTER_WORK *offset);
void TensionAddExecute(CHARACTER_WORK *offset);
void TensionExecute(CHARACTER_WORK *offset);
void TouchTimeExecute(void);
int get_player(void);
void replay_playback(void);
void tranning_plykey_save(int side);
extern PAD GameKey[2];
extern GAME_INFOMATION g_info;
extern PAD pads[2];
extern SYS_COND sc;
extern CHARACTER_WORK *PlayerWork;
extern TRAINING_SETTING *tr_info;
extern CHARANGE_CTRL *gl_info;
extern u_int TotalCounter;

void PlayerExecute(void)
{
    CHARACTER_WORK *offset;
    int i;
    int firstside;
    int side;

    if (PlayerWork[0].DamColliFlag) {
        firstside = 0;
    } else if (PlayerWork[1].DamColliFlag) {
        firstside = 1;
    } else if ((PlayerWork[0].actst & 0x20) && !(PlayerWork[1].actst & 0x20)) {
        firstside = 1;
    } else if ((PlayerWork[1].actst & 0x20) && !(PlayerWork[0].actst & 0x20)) {
        firstside = 0;
    } else {
        firstside = TotalCounter & 1;
    }
    if ((g_info.mode & 0x100) && !tr_info->tene.mode && tr_info->praccnt == -1) {
        tranning_plykey_save(tr_info->ply);
    }
    for (i = 0, side = firstside; i < 2; i++, side = !side) {
        if (((side == 0 && g_info.mode == 0x102) || (side != 0 && g_info.mode == 0x101)) && !tr_info->tene.mode) {
            ioexe_deku(side);
        } else if ((g_info.mode & 0x8000) && gl_info->ply != side && gl_info->deku) {
            ioexe_deku(side);
        } else if ((g_info.mode & 0x4000) && sc.replay == 2 && side == get_player()) {
            pads[i].old = 0;
            pads[i].data = 0;
            pads[i].trg = 0;
            pads[i].offtrg = 0;
        } else if ((g_info.mode & 0x4000) && side != get_player()) {
            pads[side].old = 0;
            GameKey[side].old = 0;
            pads[side].data = 0;
            GameKey[side].data = 0;
            pads[side].trg = 0;
            GameKey[side].trg = 0;
            pads[side].offtrg = 0;
            GameKey[side].offtrg = 0;
            pads[side].oldtrg = 0;
            GameKey[side].oldtrg = 0;
            SetLR(&PlayerWork[side]);
            PlayerCommandKeyExecute(&PlayerWork[side]);
        } else {
            if ((g_info.mode & 0x100) && tr_info->tene.memflg == 2) {
                pads[tr_info->ply].old = 0;
                pads[tr_info->ply].data = 0;
                pads[tr_info->ply].trg = 0;
                pads[tr_info->ply].offtrg = 0;
            }
            ioexe_play(side);
        }
    }
    if ((g_info.mode & 0x4000) && sc.replay == 2) {
        replay_playback();
        ioexe_play(get_player());
    }
    PlayerThrowFlagSet();
    PlayerCommandThrowFlagSet();
    for (i = 0, side = firstside; i < 2; i++, side = !side) {
        offset = &PlayerWork[side];
        if (offset->actst & 1) {
            PlayerColliCheck(offset);
            PlayerActionBreak(offset);
        }
    }
    for (i = 0, side = firstside; i < 2; i++, side = !side) {
        offset = &PlayerWork[side];
        if (offset->actst & 1) {
            PlayerMain(offset);
        }
    }
    for (i = 0, side = firstside; i < 2; i++, side = !side) {
        offset = &PlayerWork[side];
        if (offset->actst & 1) {
            ExeAnime(offset);
            if (offset->ply->pactst & 0x400) {
                offset->ply->pactst &= ~0x400;
                ExeAnime(offset);
                ExeAnime(offset);
            }
        }
    }
    for (i = 0, side = firstside; i < 2; i++, side = !side) {
        offset = &PlayerWork[side];
        if (offset->actst & 1) {
            if (offset->HitStopTime == 0) {
                PlayerMove(offset);
            }
            Char2WorldCollision(offset);
            Char2ScreenCollision(offset);
            CheckHitBack(offset);
            TensionAddExecute(offset);
            TensionExecute(offset);
        }
    }
    for (i = 0, side = firstside; i < 2; i++, side = !side) {
        offset = &PlayerWork[side];
        if (offset->actst & 1) {
            ExePosByEnemy(offset);
            if (offset->HitStopTime == 0) {
                PlayerMove2(offset);
            }
        }
    }
    CharPosCollisionExecute();
    for (i = 0; i < 2; i++) {
        offset = &PlayerWork[i];
        if (offset->actst & 1) {
            Char2WorldCollision(offset);
            Char2ScreenCollision(offset);
            HitDisp(offset);
        }
    }
    CharPosCollisionExecute();
    TouchTimeExecute();
}

void CellMain(CHARACTER_WORK *offset);
CHARACTER_WORK *CharacterWork_Initial(unsigned int InitId, unsigned short idno);
void ExeAnime(CHARACTER_WORK *offset);
char *GetCommonPaletteAddr(CHARACTER_WORK *offset, int no);
unsigned int *GetInstTbAddr(short side, unsigned short idno);
void ISTSetDualLoad(int flag);
int PreEnterActionSet(CHARACTER_WORK *offset);
void ScreenExecute(void);
void SetAnimeNo(CHARACTER_WORK *offset, int no);
void SetForceStopTime(short t1p, short t2p);
void debugf(char *format, ...);
extern short UseCharacter1p2p[];
extern GAME_INFOMATION g_info;
extern CHARACTER_WORK *PlayerWork;
extern PLAYER_ENTRY Player_Entry[2];

void PlayerInit(void)
{
    CHARACTER_WORK *offset;

    CharacterWork_Initial(0x11, UseCharacter1p2p[0]);
    offset = PlayerWork;
    debugf("Player 1 Init ID %d\n", offset->idno);
    offset->actnonext = -1;
    offset->CommonPalette = GetCommonPaletteAddr(offset, g_info.PlayerPalette[0]);
    offset->InstTb = GetInstTbAddr(0, offset->idno);
    offset->ply = &Player_Entry[0];
    offset->ply->Enemy = &PlayerWork[1];
    SetAnimeNo(offset, 0);
    offset->InstSt |= 0x20;
    ExeAnime(offset);
    ISTSetDualLoad(1);
    CellMain(offset);
    ISTSetDualLoad(0);
    offset->padid = 0;

    CharacterWork_Initial(0x12, UseCharacter1p2p[1]);
    offset = &PlayerWork[1];
    debugf("Player 2 Init ID %d\n", offset->idno);
    offset->actnonext = -1;
    offset->CommonPalette = GetCommonPaletteAddr(offset, g_info.PlayerPalette[1]);
    offset->InstTb = GetInstTbAddr(1, offset->idno);
    offset->ply = &Player_Entry[1];
    offset->ply->Enemy = &PlayerWork[0];
    SetAnimeNo(offset, 0);
    offset->InstSt |= 0x20;
    ExeAnime(offset);
    ISTSetDualLoad(1);
    CellMain(offset);
    ISTSetDualLoad(0);
    offset->padid = 1;

    PreEnterActionSet(&PlayerWork[0]);
    PreEnterActionSet(&PlayerWork[1]);
    ScreenExecute();
    SetForceStopTime(-1, -1);
}

int CPUExecute(CHARACTER_WORK *offset);
void PlayerCommandKeyExecute(CHARACTER_WORK *offset);
void PlayerSpeedCalc(CHARACTER_WORK *offset);
void SetLR(CHARACTER_WORK *offset);
int change_keyLR(int key);
action_t *get_act(void);
void sv_timehosei(void);
void training_execute_prac(void);
extern PAD GameKey[2];
extern GAME_INFOMATION g_info;
extern PAD pads[2];
extern story_t story;
extern CHARACTER_WORK *PlayerWork;
extern TRAINING_SETTING *tr_info;
extern SURVIVAL_INFOMATION *sv_info;
extern int BattleStatus;
extern int AfterBattleCounter;

extern GAME_INFOMATION g_info;
extern USER_SETTING userinfo;
extern CHARANGE_CTRL *gl_info;
extern SURVIVAL_INFOMATION *sv_info;
extern TRAINING_SETTING *tr_info;
extern int PLAYER_HITPOINTMAX;
extern CHARACTER_WORK *PlayerWork;
extern int debdat[8];
extern short int PDFaintPoint[];
int get_player(void);
void ScreenClear(void);

void PlayerParaInit(void)
{
    int i;
    int j;
    CHARACTER_WORK *offset;
    GAME_INFOMATION *p;
    int id;

    p = &g_info;
    p->BattleControlFlag &= ~8;
    for (i = 0; i < 2; i++) {
        offset = &PlayerWork[i];
        debdat[i] = (int)offset;
        debdat[i + 4] = PDFaintPoint[i];
        offset->ColliCnt = 0;
        offset->SpriteFlag &= ~0x40000;
        offset->CellNo = 0xB;
        offset->HitColliFlag = 0;
        offset->DamColliFlag = 0;
        offset->HitStopTime = 0;
        offset->xspeed = 0;
        offset->yspeed = 0;
        offset->RHitBackx = 0;
        if (i == 0) {
            offset->dirflag = 1;
            offset->actst = (offset->actst & 1) | 0xA;
            offset->priority += 0.0;
            offset->posx = -0x3840;
            offset->posy = 0;
            offset->padid = 0;
        } else {
            offset->dirflag = 0;
            offset->actst = (offset->actst & 1) | 0xC;
            offset->priority += 0.5;
            offset->posx = 0x3840;
            offset->posy = 0;
            offset->padid = 1;
        }
        if ((g_info.mode & 0x200) && sv_info->ply == i && sv_info->win) {
            offset->HitPoint = sv_info->hitpoint;
        } else if (g_info.mode & 0x800) {
            offset->HitPoint = g_info.vital[i] * PLAYER_HITPOINTMAX / 8;
        } else if (g_info.mode & 0x8000) {
            offset->HitPoint = gl_info->vital[i];
        } else {
            offset->HitPoint = PLAYER_HITPOINTMAX;
        }
        offset->ply->Tension = 0;
        offset->ply->TensionBarLength = 0;
        offset->ply->TensionMaxTime = 0;
        offset->ply->TensionBalance = 0;
        offset->ply->TensionSideTime = 0;
        offset->ply->TensionNoTouchTime = 0;
        offset->ply->NegativeVal = 0;
        offset->ply->offx = 0;
        offset->ply->ShakeTime = 0;
        offset->ply->reqhit = 0;
        offset->ply->HitCount = 0;
        offset->ply->inertia = 0;
        offset->ply->lifelength = PLAYER_HITPOINTMAX;
        offset->ply->notThrowTime = 0;
        if ((g_info.mode & 0x200) && sv_info->ply == i) {
            offset->ply->lifelengthdamage = sv_info->hitpoint;
        } else if (g_info.mode & 0x800) {
            offset->ply->lifelengthdamage = g_info.vital[i] * PLAYER_HITPOINTMAX / 8;
        } else if (g_info.mode & 0x8000) {
            offset->ply->lifelengthdamage = gl_info->vital[i];
        } else {
            offset->ply->lifelengthdamage = PLAYER_HITPOINTMAX;
        }
        offset->ply->Enemy = i ? &PlayerWork[0] : &PlayerWork[1];
        id = i;
        if (g_info.mode & 0x100) {
            id = (tr_info->ply == i) ? i : (i == 0);
        }
        offset->ply->TensionFlag = 0;
        offset->ply->TensionPenaltyTime = 0;
        offset->ply->PunchKey = userinfo.key[id].punch;
        offset->ply->KickKey = userinfo.key[id].kick;
        offset->ply->SlashKey = userinfo.key[id].slash;
        offset->ply->HighSlashKey = userinfo.key[id].hslash;
        offset->ply->Fuckin = userinfo.key[id].fuckin;
        offset->ply->MemoryKey = userinfo.key[id].memory;
        offset->ply->PandK = userinfo.key[id].pandk;
        offset->ply->SandHS = userinfo.key[id].sandhs;
        offset->ply->PKS = userinfo.key[id].pks;
        offset->ply->PKSHS = userinfo.key[id].pkshs;
        offset->ply->TensionMovePenaltyTime = 0;
        offset->ply->GuardPoint = 0;
        offset->ply->inertiaflag = 0;
        offset->ply->FaintPoint = 0;
        offset->ply->FaintMax = PDFaintPoint[i];
        offset->ply->FaintTime = 0;
        offset->ply->ExCommandFunc = 0;
        offset->ply->CPU = p->isCPU[i];
        offset->ply->sousaitime = 0;
        offset->ply->PosByEnemyAddr = 0;
        offset->ply->pactst = 0;
        offset->ply->KeyHook = 0;
        offset->ply->AttackDown = 0;
        offset->ply->DefenceDown = 0;
        offset->ply->JumpSeal = 0;
        offset->ply->ArtsSeal = 0;
        offset->ply->TShand = 0;
        offset->ply->Poison = 0;
        offset->ply->MistTime = 0;
        offset->ply->homingtime = 0;
        for (j = 0; j < 4; j++) {
            offset->ply->PersonalWork[j].l = 0;
        }
        offset->ply->OverKillFlag = 0;
        offset->ply->ObjFlag = 0;
    }
    ScreenClear();
    if (g_info.mode & 0x4000) {
        PlayerWork[get_player()].ply->Tension = 10000;
    }
}


void ioexe_play(int side)
{
    CHARACTER_WORK *offset;
    int key;
    int tmp;
    int i;
    action_t *act;

    offset = &PlayerWork[side];
    if (!(offset->actst & 1)) {
        return;
    }
    PlayerSpeedCalc(offset);
    if ((!(g_info.mode & 0x2000) || !story.is_enter) && !CPUExecute(offset)) {
        if ((g_info.mode & 0x100) && tr_info->praccnt != -1 && side == tr_info->ply) {
            training_execute_prac();
        }
        SetLR(offset);
        g_info.BattleTime[offset->padid]++;
        if (g_info.mode & 0x200) {
            if (BattleStatus == 2) {
                sv_info->sv_ext.survival_totaltime++;
            } else {
                sv_timehosei();
            }
        }
    }
    if ((g_info.mode & 0x2000) && story.is_enter) {
        key = 0;
        tmp = 0;
        i = (side != story.self_side);
        act = get_act();
        if (act[story.actind[i]].kind == 2 || act[story.actind[i]].kind == 3) {
            if (act[story.actind[i]].kind == 3 || story.actcnt[i] == 0) {
                key = change_keyLR(act[story.actind[i]].No);
            }
            story.actcnt[i]++;
            if (story.actcnt[i] >= act[story.actind[i]].frame) {
                story.actind[i] += 2;
                story.actcnt[i] = 0;
            }
        }
        offset->dirflag = story.direction[i];
        if (key & 0x20) {
            tmp = offset->ply->HighSlashKey;
        }
        if (key & 0x40) {
            tmp |= offset->ply->KickKey;
        }
        if (key & 0x80) {
            tmp |= offset->ply->PunchKey;
        }
        if (key & 0x10) {
            tmp |= offset->ply->SlashKey;
        }
        key &= ~0xF0;
        key |= tmp;
        pads[side].old = story.key_old[side];
        pads[side].data = key;
        story.key_old[side] = key;
        pads[side].trg = ~pads[side].old & key;
        pads[side].offtrg = pads[side].old & ~key;
        SetLR(offset);
    }
    PlayerCommandKeyExecute(offset);
    if ((BattleStatus == 3 && !AfterBattleCounter) || BattleStatus == 4 || BattleStatus == 1) {
        GameKey[side].trg = 0;
        GameKey[side].data = 0;
    }
}

int CPUExecute(CHARACTER_WORK *offset);
unsigned int GetOtherAttackFlag(CHARACTER_WORK *offset);
void PlayerCommandKeyExecute(CHARACTER_WORK *offset);
void PlayerSpeedCalc(CHARACTER_WORK *offset);
int Random(void);
void SetLR(CHARACTER_WORK *offset);
int get_dekustat(CHARACTER_WORK *offset);
void tranning_execute_execute(int side);
void tranning_keyset_deku(int side);
void tranning_memorize_execute(int side);
extern PAD GameKey[2];
extern GAME_INFOMATION g_info;
extern PAD pads[2];
extern CHARACTER_WORK *PlayerWork;
extern TRAINING_SETTING *tr_info;
extern CHARANGE_CTRL *gl_info;
extern int BattleStatus;

void ioexe_deku(int side)
{
    CHARACTER_WORK *offset;
    int key;
    unsigned int flag;

    key = 0;
    offset = &PlayerWork[side];
    if (g_info.mode & 0x100) {
        if (tr_info->tene.memflg == 0 || tr_info->tene.memflg == 1) {
            key = 0x4000;
            if (tr_info->tene.type == 0) {
                key = 0;
            }
            if (tr_info->tene.type == 2 && !(offset->actst & 0x10)) {
                key = 0x1000;
            }
            if (tr_info->tene.guard && offset->ply->hitDispTime) {
                if (offset->dirflag == 0) {
                    key |= 0x2000;
                } else if (offset->dirflag == 1) {
                    key |= 0x8000;
                }
                flag = GetOtherAttackFlag(offset);
                if (tr_info->tene.type == 0 && (flag & 3) == 2) {
                    key |= 0x4000;
                }
                if (tr_info->tene.type == 2 && (flag & 3) == 2) {
                    key |= 0x4000;
                    key &= ~0x1000;
                }
                if (tr_info->tene.type == 1 && (flag & 3) == 1) {
                    key &= ~0x4000;
                }
                if ((offset->actst & 0x30) == 0x30) {
                    key &= ~0xA000;
                }
            }
            if (offset->actst & 0x10) {
                key &= ~0x1000;
            }
            if (tr_info->tene.memflg == 0 && tr_info->tene.type == 3) {
                key &= 0xA000;
                tr_info->attackcnt++;
                if ((unsigned short)offset->actnonexttemp == 0 || (unsigned short)offset->actnonexttemp == 0x3D
                    || (unsigned short)offset->actnonexttemp == 0x11 || (unsigned short)offset->actnonexttemp == 0x12
                    || ((unsigned short)offset->actnonexttemp == 0x52 && offset->idno == 13)
                    || ((unsigned short)offset->actnonexttemp == 0x91 && offset->idno == 11)) {
                    if (tr_info->attackcnt >= 301) {
                        key = offset->ply->HighSlashKey;
                        tr_info->attackcnt = 0;
                    }
                } else {
                    tr_info->attackcnt = 0;
                }
            }
            if (offset->actst & 0x10) {
                key &= ~0x1000;
            }
            if ((offset->CommandFlag & 0x800) || offset->ply->DownTimer == 1) {
                if (!get_dekustat(offset)) {
                    int dat[3] = { 0, 0x2000, 0x8000 };
                    switch (tr_info->tene.ukemi) {
                    case 1:
                        if (offset->dirflag == 1) {
                            key |= 0x2000;
                        } else {
                            key |= 0x8000;
                        }
                        break;
                    case 3:
                        if (offset->dirflag == 1) {
                            key |= 0x8000;
                        } else {
                            key |= 0x2000;
                        }
                        break;
                    case 4:
                        key |= dat[Random() % 3];
                        break;
                    }
                    if (tr_info->tene.ukemi) {
                        key |= offset->ply->PunchKey | offset->ply->KickKey;
                    }
                }
                if ((g_info.mode & 0x8000) && gl_info->deku && offset->padid != gl_info->ply) {
                    int dat[3] = { 0, 0x2000, 0x8000 };
                    key |= dat[0];
                    key |= offset->ply->PunchKey | offset->ply->KickKey;
                }
            }
            pads[side].old = pads[side].data;
            pads[side].data = key;
            pads[side].trg = ~pads[side].old & key;
            pads[side].offtrg = pads[side].old & ~key;
            SetLR(offset);
            PlayerCommandKeyExecute(offset);
        } else if (tr_info->tene.memflg == 2) {
            if (offset->actst & 1) {
                PlayerSpeedCalc(offset);
                if (!CPUExecute(offset)) {
                    tranning_keyset_deku(side);
                    tranning_memorize_execute(side);
                    SetLR(offset);
                    g_info.BattleTime[side]++;
                }
                PlayerCommandKeyExecute(offset);
            }
        } else if (tr_info->tene.memflg == 3) {
            if (offset->actst & 1) {
                PlayerSpeedCalc(offset);
                if (!CPUExecute(offset)) {
                    tranning_execute_execute(side);
                    SetLR(offset);
                    g_info.BattleTime[side]++;
                }
                PlayerCommandKeyExecute(offset);
            }
        }
        if (BattleStatus != 2) {
            pads[side].old = 0;
            GameKey[side].old = 0;
            pads[side].data = 0;
            GameKey[side].data = 0;
            pads[side].trg = 0;
            GameKey[side].trg = 0;
            pads[side].offtrg = 0;
            GameKey[side].offtrg = 0;
            PlayerCommandKeyExecute(offset);
        }
    } else if (g_info.mode & 0x8000) {
        if (BattleStatus != 2) {
            pads[side].old = 0;
            GameKey[side].old = 0;
            pads[side].data = 0;
            GameKey[side].data = 0;
            pads[side].trg = 0;
            GameKey[side].trg = 0;
            pads[side].offtrg = 0;
            GameKey[side].offtrg = 0;
            PlayerCommandKeyExecute(offset);
        } else if (gl_info->deku) {
            if (((offset->CommandFlag & 0x800) || offset->ply->DownTimer == 1) && offset->padid != gl_info->ply) {
                int dat[3] = { 0, 0x2000, 0x8000 };
                key |= dat[0];
                key |= offset->ply->PunchKey | offset->ply->KickKey;
            }
            pads[side].old = pads[side].data;
            pads[side].data = key;
            pads[side].trg = ~pads[side].old & key;
            pads[side].offtrg = pads[side].old & ~key;
            SetLR(offset);
            PlayerCommandKeyExecute(offset);
        }
    }
}
