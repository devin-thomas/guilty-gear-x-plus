#include "generated/playdat.h"

static const short int idtb[] = { 1, 2, 3 };

extern USER_SETTING userinfo;
extern GAME_INFOMATION g_info;
extern short int UseCharacter1p2p[];

void set_PlayDat(int idno, short int *p)
{
    PLAYER_EXT_INFO_V3 *info;
    int error;
    int valtb[2];

    info = (PLAYER_EXT_INFO_V3 *)p;
    if (info->fid != 0xE5) {
        return;
    }
    if (info->ver == info->fid) {
        info->ver = 0;
    }
    valtb[0] = idno;
    valtb[1] = info->ver;
    error = 0;
    if (info->ver <= 3) {
        error = 1;
    }
    ForwardWalk_XSpeedByIdno[idno] = info->fwlk;
    BackWalk_XSpeedByIdno[idno] = info->bwlk;
    Dash_XSpeedByIdno[idno] = info->fdhx;
    BackStep_XSpeedByIdno[idno] = info->bdhx;
    if (userinfo.gguse[idno] & 1) {
        ForwardWalk_XSpeedByIdno[idno] = info->fwlk * 12 / 10;
        BackWalk_XSpeedByIdno[idno] = info->bwlk * 12 / 10;
        Dash_XSpeedByIdno[idno] = info->fdhx * 12 / 10;
        BackStep_XSpeedByIdno[idno] = info->bdhx * 12 / 10;
    }
    BackStep_YSpeedByIdno[idno] = info->bdhy;
    BackStep_GravityByIdno[idno] = info->bdhg;
    FrontJump_XSpeedByIdno[idno] = info->fjpx;
    BackJump_XSpeedByIdno[idno] = info->bjpx;
    Jump_YSpeedByIdno[idno] = info->jmpy;
    Jump_GravityByIdno[idno] = info->jmpg;
    FrontHighJump_XSpeedByIdno[idno] = info->fhjx;
    BackHighJump_XSpeedByIdno[idno] = info->bhjx;
    HighJump_YSpeedByIdno[idno] = info->hjpy;
    HighJump_GravityByIdno[idno] = info->hjpg;
    Dash_AccelByIdno[idno] = info->fdha;
    if (userinfo.gguse[idno] & 1) {
        Dash_AccelByIdno[idno] = info->fdha * 12 / 10;
    }
    Dash_ResistByIdno[idno] = info->fdhr;
    HomingJump_YMAX[idno] = info->hjym;
    HomingJump_XMAX[idno] = info->hjxm;
    HomingJump_XResist[idno] = info->hjxr;
    HomingTarget_YOffset[idno] = info->htyo;
    SkyDash_Height[idno] = info->skdh;
    if (userinfo.gguse[idno] & 1) {
        SkyDash_Height[idno] = 10;
    }
    PDSkyDashF_Time[idno] = info->fsdt;
    PDSkyDashB_Time[idno] = info->bsdt;
    PDFaintPoint[idno] = info->fntp;
    PDDefPoint[idno] = info->defp;
    if (userinfo.gguse[idno] & 1) {
        PDDefPoint[idno] += 10;
    }
    PDKonjo[idno] = info->konp;
    PDKaishin[idno] = info->crtp;
    PDDef_Gravity[idno] = info->defg;
    PDSkyDashCount[idno] = info->sdct;
    ChainTableByIdno[idno] = info->ChainTable;
    PDJumpCount[idno] = info->sjct;
    PDSkyDashF_AttackTime[idno] = info->fsda;
    if (userinfo.gguse[idno] & 1) {
        PDSkyDashF_AttackTime[idno] = 1;
    }
    PDSkyDashB_AttackTime[idno] = info->bsda;
    PDTensionAdd_FWalk[idno] = info->tnfw;
    PDTensionAdd_FJump[idno] = info->tnfj;
    PDTensionAdd_FDash[idno] = info->tnfd;
    PDTensionAdd_FSDash[idno] = info->tnfs;
    PDGCGauge_DefPoint[idno] = info->gcdf;
    PDGCGauge_Recovery[idno] = info->gcrc;
    PDEneAttackRate[idno] = 0;
    PDEneTensionRate[idno] = 0;
    if (g_info.isCPU[idno]) {
        if (g_info.BattleCnt[idno == 0] == 9) {
            PDEneAttackRate[idno] = 6;
            PDEneTensionRate[idno] = 16;
            PDDefPoint[idno] -= 50;
            PDFaintPoint[idno] = 90;
            PDGCGauge_Recovery[idno] = 2;
        } else if (g_info.BattleCnt[idno == 0] == 8) {
            PDEneAttackRate[idno] = 5;
            PDEneTensionRate[idno] = 12;
            PDDefPoint[idno] -= 40;
            PDGCGauge_Recovery[idno] = 2;
        } else if (g_info.BattleCnt[idno == 0] == 7) {
            PDEneAttackRate[idno] = 4;
            PDEneTensionRate[idno] = 8;
            PDDefPoint[idno] -= 30;
        }
    } else if (UseCharacter1p2p[idno] == 11 || UseCharacter1p2p[idno] == 16) {
        PDEneTensionRate[idno] = -2;
    }
    if (error) {
        ForwardWalk_XSpeedByIdno[idno] = 0;
        BackWalk_XSpeedByIdno[idno] = 0;
        PDDef_Gravity[idno] = 600;
    }
}


int *SkipInstExtInfo(short unsigned int *p)
{
    if (((unsigned char *)p)[0] == 0xE5) {
        if (((unsigned char *)p)[1] < 3 || ((unsigned char *)p)[1] > 0x80) {
            return (int *)(p + 0x40);
        }
        return (int *)(p + 0x80);
    }
    return (int *)p;
}
