#include "generated/object.h"

extern int StageNo;
extern void (*ANBF_Act_jumpTb[])();
extern void (*ANNG_Act_jumpTb[])();
extern void (*ANUP_Act_jumpTb[])();
extern void (*AXCH_Act_jumpTb[])();
extern void (*AXVQ_Act_jumpTb[])();
extern void (*BKSA_Act_jumpTb[])();
extern void (*BKTA_Act_jumpTb[])();
extern void (*CHBB_Act_jumpTb[])();
extern void (*CHCB_Act_jumpTb[])();
extern void (*CHLF_Act_jumpTb[])();
extern void (*CHTU_Act_jumpTb[])();
extern void (*DZ16_Act_jumpTb[])();
extern void (*DZBT_Act_jumpTb[])();
extern void (*DZSW_Act_jumpTb[])();
extern void (*EFDef_ActRandPos_jumpTb[])();
extern void (*EFDef_Act_jumpTb[])();
extern void (*EFH17_Act_jumpTb[])();
extern void (*EFH20_Act_jumpTb[])();
extern void (*EFK25_Act_jumpTb[])();
extern void (*EFRY_Act_jumpTb[])();
extern void (*FALH_Act_jumpTb[])();
extern void (*FAMN_Act_jumpTb[])();
extern void (*FANG_Act_jumpTb[])();
extern void (*JMDS_Act_jumpTb[])();
extern void (*JMFK_Act_jumpTb[])();
extern void (*JMGD_Act_jumpTb[])();
extern void (*JMHS_Act_jumpTb[])();
extern void (*JMKR_Act_jumpTb[])();
extern void (*JSEF_Act_jumpTb[])();
extern void (*JSEX_Act_jumpTb[])();
extern void (*JYCO_Act_jumpTb[])();
extern void (*JYEF_Act_jumpTb[])();
extern void (*JYKL_Act_jumpTb[])();
extern void (*KAKUSEI_Act_jumpTb[])();
extern void (*KREF_Act_jumpTb[])();
extern void (*KYSE_Act_jumpTb[])();
extern void (*Kunsyou_Act_jumpTb[])();
extern void (*MLKR_Act_jumpTb[])();
extern void (*MLRG_Act_jumpTb[])();
extern void (*MLRS_Act_jumpTb[])();
extern void (*MYEE_Act_jumpTb[])();
extern void (*MYFU_Act_jumpTb[])();
extern void (*MYIB_Act_jumpTb[])();
extern void (*MYIG_Act_jumpTb[])();
extern void (*MYIK_Act_jumpTb[])();
extern void (*MYKU_Act_jumpTb[])();
extern void (*PLY01_Act_jumpTb[])();
extern void (*PLYHIBANACnt_Act_jumpTb[])();
extern void (*PLYLIGHTNING_Act_jumpTb[])();
extern void (*PLYPG2_Act_jumpTb[])();
extern void (*PLYPG_Act_jumpTb[])();
extern void (*POBM_Act_jumpTb[])();
extern void (*POGB_Act_jumpTb[])();
extern void (*SAXObj2_Act_jumpTb[])();
extern void (*SAXObj_Act_jumpTb[])();
extern void (*SBKObj_Act_jumpTb[])();
extern void (*SDZObj2_Act_jumpTb[])();
extern void (*SDZObj3_Act_jumpTb[])();
extern void (*SDZObj_Act_jumpTb[])();
extern void (*SJMObj_Act_jumpTb[])();
extern void (*SKRObj2_Act_jumpTb[])();
extern void (*SKRObj3_Act_jumpTb[])();
extern void (*SKRObj_Act_jumpTb[])();
extern void (*SLSW_Act_jumpTb[])();
extern void (*SLTR_Act_jumpTb[])();
extern void (*SMLHato_Act_jumpTb[])();
extern void (*SMLObj2_Act_jumpTb[])();
extern void (*SMYObj_Act_jumpTb[])();
extern void (*SPOObj2_Act_jumpTb[])();
extern void (*SPOObj_Act_jumpTb[])();
extern void (*SSLObj2_Act_jumpTb[])();
extern void (*SSLObj3_Act_jumpTb[])();
extern void (*SSLObj4_Act_jumpTb[])();
extern void (*SSLObj_Act_jumpTb[])();
extern void (*STSObj2_Act_jumpTb[])();
extern void (*STSObj_Act_jumpTb[])();
extern void (*SVEObj_Act_jumpTb[])();
extern void (*TSEX_Act_jumpTb[])();
extern void (*TSKK_Act_jumpTb[])();
extern void (*TSKR_Act_jumpTb[])();
extern void (*TSSL_Act_jumpTb[])();
extern void (*VEBB_Act_jumpTb[])();
extern void (*VEBL_Act_jumpTb[])();
extern void (*VEKT_Act_jumpTb[])();
extern void (*VEMC_Act_jumpTb[])();
extern void (*VETB_Act_jumpTb[])();
extern void (*ZTBD_Act_jumpTb[])();
extern void (*ZTDR_Act_jumpTb[])();
extern void (*ZTED_Act_jumpTb[])();

void ExcuteActJumpTbByIdno(CHARACTER_WORK *offset)
{
	switch (offset->idno) {
	case 32:
		DZSW_Act_jumpTb[offset->actno](offset);
		break;
	case 67:
		DZBT_Act_jumpTb[offset->actno](offset);
		break;
	case 36:
		DZ16_Act_jumpTb[offset->actno](offset);
		break;
	case 33:
		MLRG_Act_jumpTb[offset->actno](offset);
		break;
	case 52:
		MLKR_Act_jumpTb[offset->actno](offset);
		break;
	case 34:
		VEBL_Act_jumpTb[offset->actno](offset);
		break;
	case 49:
		VETB_Act_jumpTb[offset->actno](offset);
		break;
	case 53:
		VEKT_Act_jumpTb[offset->actno](offset);
		break;
	case 54:
		VEBB_Act_jumpTb[offset->actno](offset);
		break;
	case 50:
		VEMC_Act_jumpTb[offset->actno](offset);
		break;
	case 46:
		MLRS_Act_jumpTb[offset->actno](offset);
		break;
	case 40:
		CHCB_Act_jumpTb[offset->actno](offset);
		break;
	case 59:
		CHBB_Act_jumpTb[offset->actno](offset);
		break;
	case 69:
		CHTU_Act_jumpTb[offset->actno](offset);
		break;
	case 43:
		FANG_Act_jumpTb[offset->actno](offset);
		break;
	case 44:
		SLTR_Act_jumpTb[offset->actno](offset);
		break;
	case 66:
		SLSW_Act_jumpTb[offset->actno](offset);
		break;
	case 41:
	case 86:
		KYSE_Act_jumpTb[offset->actno](offset);
		break;
	case 57:
		BKTA_Act_jumpTb[offset->actno](offset);
		break;
	case 58:
		BKSA_Act_jumpTb[offset->actno](offset);
		break;
	case 62:
		MYEE_Act_jumpTb[offset->actno](offset);
		break;
	case 70:
		MYKU_Act_jumpTb[offset->actno](offset);
		break;
	case 68:
		MYIK_Act_jumpTb[offset->actno](offset);
		break;
	case 72:
		MYFU_Act_jumpTb[offset->actno](offset);
		break;
	case 55:
		MYIG_Act_jumpTb[offset->actno](offset);
		break;
	case 75:
		MYIB_Act_jumpTb[offset->actno](offset);
		break;
	case 35:
		ZTED_Act_jumpTb[offset->actno](offset);
		break;
	case 65:
		ZTDR_Act_jumpTb[offset->actno](offset);
		break;
	case 37:
		JMHS_Act_jumpTb[offset->actno](offset);
		break;
	case 47:
		JMGD_Act_jumpTb[offset->actno](offset);
		break;
	case 61:
		POBM_Act_jumpTb[offset->actno](offset);
		break;
	case 63:
		JYEF_Act_jumpTb[offset->actno](offset);
		break;
	case 85:
		JYCO_Act_jumpTb[offset->actno](offset);
		break;
	case 38:
		JMFK_Act_jumpTb[offset->actno](offset);
		break;
	case 39:
		JMKR_Act_jumpTb[offset->actno](offset);
		break;
	case 42:
		EFDef_Act_jumpTb[offset->actno](offset);
		break;
	case 45:
		EFDef_Act_jumpTb[offset->actno](offset);
		break;
	case 56:
		EFDef_Act_jumpTb[offset->actno](offset);
		break;
	case 48:
		JMDS_Act_jumpTb[offset->actno](offset);
		break;
	case 51:
		ZTBD_Act_jumpTb[offset->actno](offset);
		break;
	case 60:
		CHLF_Act_jumpTb[offset->actno](offset);
		break;
	case 64:
		FALH_Act_jumpTb[offset->actno](offset);
		break;
	case 74:
		FAMN_Act_jumpTb[offset->actno](offset);
		break;
	case 71:
		JYKL_Act_jumpTb[offset->actno](offset);
		break;
	case 73:
		POGB_Act_jumpTb[offset->actno](offset);
		break;
	case 76:
		ANNG_Act_jumpTb[offset->actno](offset);
		break;
	case 78:
		ANBF_Act_jumpTb[offset->actno](offset);
		break;
	case 83:
		ANUP_Act_jumpTb[offset->actno](offset);
		break;
	case 79:
		TSSL_Act_jumpTb[offset->actno](offset);
		break;
	case 80:
		TSEX_Act_jumpTb[offset->actno](offset);
		break;
	case 82:
		TSKR_Act_jumpTb[offset->actno](offset);
		break;
	case 84:
		TSKK_Act_jumpTb[offset->actno](offset);
		break;
	case 77:
		AXCH_Act_jumpTb[offset->actno](offset);
		break;
	case 81:
		AXVQ_Act_jumpTb[offset->actno](offset);
		break;
	case 87:
		KREF_Act_jumpTb[offset->actno](offset);
		break;
	case 89:
		JSEF_Act_jumpTb[offset->actno](offset);
		break;
	case 96:
		JSEX_Act_jumpTb[offset->actno](offset);
		break;
	case 97:
		EFDef_Act_jumpTb[offset->actno](offset);
		break;
	case 98:
		Kunsyou_Act_jumpTb[offset->actno](offset);
		break;
	case 99:
		EFRY_Act_jumpTb[offset->actno](offset);
		break;
	case 105:
		EFDef_Act_jumpTb[offset->actno](offset);
		break;
	case 100:
		EFDef_Act_jumpTb[offset->actno](offset);
		break;
	case 104:
		EFDef_Act_jumpTb[offset->actno](offset);
		break;
	case 102:
		EFDef_Act_jumpTb[offset->actno](offset);
		break;
	case 109:
		EFDef_Act_jumpTb[offset->actno](offset);
		break;
	case 111:
		EFH17_Act_jumpTb[offset->actno](offset);
		break;
	case 108:
		EFDef_Act_jumpTb[offset->actno](offset);
		break;
	case 106:
		EFDef_ActRandPos_jumpTb[offset->actno](offset);
		break;
	case 107:
		EFDef_ActRandPos_jumpTb[offset->actno](offset);
		break;
	case 103:
		EFH20_Act_jumpTb[offset->actno](offset);
		break;
	case 110:
		EFK25_Act_jumpTb[offset->actno](offset);
		break;
	case 112:
		PLY01_Act_jumpTb[offset->actno](offset);
		break;
	case 114:
		PLYHIBANACnt_Act_jumpTb[offset->actno](offset);
		break;
	case 115:
		PLYPG_Act_jumpTb[offset->actno](offset);
		break;
	case 116:
		PLYPG2_Act_jumpTb[offset->actno](offset);
		break;
	case 117:
		PLYLIGHTNING_Act_jumpTb[offset->actno](offset);
		break;
	case 119:
		KAKUSEI_Act_jumpTb[offset->actno](offset);
		break;
	case 128:
		switch (StageNo) {
		case 1:
			SAXObj_Act_jumpTb[offset->actno](offset);
			break;
		case 2:
			SBKObj_Act_jumpTb[offset->actno](offset);
			break;
		case 3:
			SMLHato_Act_jumpTb[offset->actno](offset);
			break;
		case 4:
			SJMObj_Act_jumpTb[offset->actno](offset);
			break;
		case 5:
			SMYObj_Act_jumpTb[offset->actno](offset);
			break;
		case 6:
			SPOObj_Act_jumpTb[offset->actno](offset);
			break;
		case 7:
			SVEObj_Act_jumpTb[offset->actno](offset);
			break;
		case 9:
			STSObj_Act_jumpTb[offset->actno](offset);
			break;
		case 10:
			SDZObj_Act_jumpTb[offset->actno](offset);
			break;
		case 8:
			SSLObj_Act_jumpTb[offset->actno](offset);
			break;
		case 11:
			SKRObj_Act_jumpTb[offset->actno](offset);
			break;
		}
		break;
	case 129:
		switch (StageNo) {
		case 1:
			SAXObj2_Act_jumpTb[offset->actno](offset);
			break;
		case 10:
			SDZObj2_Act_jumpTb[offset->actno](offset);
			break;
		case 9:
			STSObj2_Act_jumpTb[offset->actno](offset);
			break;
		case 8:
			SSLObj2_Act_jumpTb[offset->actno](offset);
			break;
		case 3:
			SMLObj2_Act_jumpTb[offset->actno](offset);
			break;
		case 6:
			SPOObj2_Act_jumpTb[offset->actno](offset);
			break;
		case 11:
			SKRObj2_Act_jumpTb[offset->actno](offset);
			break;
		}
		break;
	case 130:
		switch (StageNo) {
		case 10:
			SDZObj3_Act_jumpTb[offset->actno](offset);
			break;
		case 8:
			SSLObj3_Act_jumpTb[offset->actno](offset);
			break;
		case 11:
			SKRObj3_Act_jumpTb[offset->actno](offset);
			break;
		}
		break;
	case 131:
		if (StageNo == 8) {
			SSLObj4_Act_jumpTb[offset->actno](offset);
		}
		break;
	}
}

int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void ZTED_CommandCheck(CHARACTER_WORK *offset);

void ObjectMain(CHARACTER_WORK *offset)
{
    if (offset->HitStopTime == 0 && offset->actnonext != -1) {
        SetActNo(offset, offset->actnonext);
    }
    if (offset->HitStopTime == 0 && offset->LandBreakActno != -1 && (~offset->actst & 0x10)) {
        SetActNo(offset, offset->LandBreakActno);
    }
    if (offset->idno == 0x23) {
        ZTED_CommandCheck(offset);
    }
    ExcuteActJumpTbByIdno(offset);
    if ((offset->actst & 0x80000) && offset->HitStopTime == 0) {
        offset->actst &= ~0x80000;
    }
    if (offset->HitStopTime == 0) {
        offset->ActTimer++;
    }
}

void CHCB_ColliCheck(CHARACTER_WORK *offset);
void DZBT_ColliCheck(CHARACTER_WORK *offset);
void DZSW_ColliCheck(CHARACTER_WORK *offset);
void FANG_ColliCheck(CHARACTER_WORK *offset);
void JMHS_ColliCheck(CHARACTER_WORK *offset);
void JSEX_ColliCheck(CHARACTER_WORK *offset);
void JYCO_ColliCheck(CHARACTER_WORK *offset);
void JYEF_ColliCheck(CHARACTER_WORK *offset);
void KREF_ColliCheck(CHARACTER_WORK *offset);
void KYSE_ColliCheck(CHARACTER_WORK *offset);
void MLRG_ColliCheck(CHARACTER_WORK *offset);
void POBM_ColliCheck(CHARACTER_WORK *offset);
void SLTR_ColliCheck(CHARACTER_WORK *offset);
void SetHitStop(CHARACTER_WORK *offset, unsigned int flag, short unsigned int lvflag, int type);
void VEBL_ColliCheck(CHARACTER_WORK *offset);
void VEMC_ColliCheck(CHARACTER_WORK *offset);
void VETB_ColliCheck(CHARACTER_WORK *offset);
void ZTED_ColliCheck(CHARACTER_WORK *offset);

void ObjectColliCheck(CHARACTER_WORK *offset)
{
    if (offset->HitColliFlag) {
        offset->ActHitCount++;
        offset->ColliFlag = offset->HitColliFlag;
        offset->actst |= 0x40;
        offset->attackst |= 0x1000;
        if (offset->HitColliFlag == -1) {
            offset->attackst |= 0x80000;
        }
        if (offset->ActHeader.flag2 & 1) {
            SetHitStop(offset, offset->ActHeader.flag, offset->ActHeader.lvflag, 0);
        }
    }
    if (offset->DamColliFlag) {
        offset->actst |= 0x40;
        offset->attackst |= 0x801000;
        offset->ColliFlag = offset->DamColliFlag;
    }
    switch (offset->idno) {
    case 32:
    	DZSW_ColliCheck(offset);
    	break;
    case 67:
    	DZBT_ColliCheck(offset);
    	break;
    case 33:
    	MLRG_ColliCheck(offset);
    	break;
    case 34:
    	VEBL_ColliCheck(offset);
    	break;
    case 50:
    	VEMC_ColliCheck(offset);
    	break;
    case 35:
    	ZTED_ColliCheck(offset);
    	break;
    case 37:
    	JMHS_ColliCheck(offset);
    	break;
    case 40:
    	CHCB_ColliCheck(offset);
    	break;
    case 43:
    	FANG_ColliCheck(offset);
    	break;
    case 41:
    case 86:
    	KYSE_ColliCheck(offset);
    	break;
    case 44:
    	SLTR_ColliCheck(offset);
    	break;
    case 49:
    	VETB_ColliCheck(offset);
    	break;
    case 63:
    	JYEF_ColliCheck(offset);
    	break;
    case 61:
    	POBM_ColliCheck(offset);
    	break;
    case 85:
    	JYCO_ColliCheck(offset);
    	break;
    case 87:
    	KREF_ColliCheck(offset);
    	break;
    case 96:
    	JSEX_ColliCheck(offset);
    	break;
    }
    offset->HitColliFlag = 0;
    offset->DamColliFlag = 0;
}

void JumpControlMain(CHARACTER_WORK *offset);

void ObjectMove(CHARACTER_WORK *offset)
{
    offset->posx += offset->xspeed;
    if (offset->actst & 0x200000) {
        JumpControlMain(offset);
    }
}

CHARACTER_WORK *ObjectCreate(CHARACTER_WORK *poffset, short unsigned int idno)
{
    return ObjectCreateA(poffset, idno, 0);
}

CHARACTER_WORK *CharacterWork_Initial(unsigned int InitId, short unsigned int idno);
unsigned int *GetInstTbAddr(short int side, short unsigned int idno);
void ParentFlagSet(CHARACTER_WORK *poffset, CHARACTER_WORK *offset, TINITINSTANCE *ins);
CHARACTER_WORK *SearchObjectWork(CHARACTER_WORK *off, short unsigned int idno);
int SetActNo(CHARACTER_WORK *offset, unsigned int no);
void SetAnimeNo(CHARACTER_WORK *offset, int no);

CHARACTER_WORK *ObjectCreateA(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg)
{
    CHARACTER_WORK *offset;
    int flag;

    flag = 0;
    if (arg) {
        flag = arg->flag;
    }
    if (flag & 8) {
        offset = SearchObjectWork(poffset, idno);
        if (offset) {
            return offset;
        }
    }
    offset = CharacterWork_Initial(0x20, idno);
    if (offset == 0) {
        return 0;
    }
    offset->parentWork = poffset;
    offset->actst |= 8;
    if (poffset) {
        if (flag & 0x80) {
            offset->dirflag = !poffset->dirflag;
        } else {
            offset->dirflag = poffset->dirflag;
        }
    } else {
        offset->dirflag = 0;
    }
    if (flag & 1) {
        offset->posx = arg->x;
        offset->posy = arg->y;
    } else if (flag & 2) {
        if (offset->dirflag == 1) {
            offset->posx = poffset->posx + arg->x;
        } else {
            offset->posx = poffset->posx - arg->x;
        }
        offset->posy = poffset->posy + arg->y;
    } else {
        offset->posx = poffset->posx;
        offset->posy = poffset->posy;
    }
    if (flag & 4) {
        SetActNo(offset, arg->actno);
    }
    if (flag & 0x40) {
        offset->HitBackx = arg->tx;
        offset->RHitBackx = arg->ty;
    }
    if (flag & 0x20) {
        offset->Localid = arg->Localid;
    }
    if (flag & 0x100) {
        TINITINSTANCE ins;
        ins.Kind = 0x20;
        ins.Flag = 0;
        ParentFlagSet(poffset, offset, &ins);
    }
    if (flag & 0x200) {
        TINITINSTANCE ins;
        ins.Kind = 0x40;
        ins.Flag = 0;
        ParentFlagSet(poffset, offset, &ins);
    }
    if (flag & 0x400) {
        TINITINSTANCE ins;
        ins.Kind = 2;
        ins.Flag = 0;
        ParentFlagSet(poffset, offset, &ins);
    }
    if (flag & 0x1000) {
        offset->angle = arg->angle;
    }
    offset->priority += -1.1;
    offset->InstTb = GetInstTbAddr(offset->padid, offset->idno);
    SetAnimeNo(offset, 0);
    offset->InstSt |= 0x20;
    offset->padid = poffset->padid;
    offset->actst |= poffset->actst & 6;
    if (flag & 0x800) {
        offset->parentWork = poffset->parentWork;
    }
    return offset;
}

CHARACTER_WORK *ObjectCreate_ActNo(CHARACTER_WORK *poffset, short unsigned int idno, CREATE_ARG *arg, short unsigned int actno)
{
    CREATE_ARG c_arg;

    if (arg == 0) {
        c_arg.flag = 0;
        arg = &c_arg;
    }
    arg->flag |= 4;
    arg->actno = actno;
    return ObjectCreateA(poffset, idno, arg);
}


void ObjectKill(CHARACTER_WORK *offset);
extern CHARACTER_WORK *ObjectWork;

void object_killall(void)
{
    CHARACTER_WORK *offset;
    int i;

    offset = ObjectWork;
    for (i = 0; i < 48; i++, offset++) {
        if ((offset->idno < 0x80 || offset->idno > 0x83) && (offset->actst & 1)) {
            ObjectKill(offset);
        }
    }
}

void ObjectKill(CHARACTER_WORK *offset);
extern CHARACTER_WORK *ObjectWork;
void ParentExe(CHARACTER_WORK *offset);
void ParentExe_Position(CHARACTER_WORK *offset);
void ExeAnime(CHARACTER_WORK *offset);
void Char2ScreenCollision(CHARACTER_WORK *offset);
void Char2WorldCollision(CHARACTER_WORK *offset);

void ObjectExecute(void)
{
    CHARACTER_WORK *offset;
    int i;

retry:
    for (i = 0; i < 48; i++) {
        offset = &ObjectWork[i];
        if (offset == 0) {
            continue;
        }
        if (!(offset->actst & 1) || !(~offset->actst & 0x40000000)) {
            continue;
        }
        ParentExe(offset);
        ObjectColliCheck(offset);
        if (offset->HitStopTime == 0) {
            ObjectMove(offset);
        }
        ObjectMain(offset);
        ParentExe_Position(offset);
        offset->actst |= 0x40000000;
        if (offset->actst & 0x80000000) {
            ObjectKill(offset);
        } else {
            ExeAnime(offset);
            if (offset->actst & 0x1000000) {
                Char2ScreenCollision(offset);
                Char2WorldCollision(offset);
            }
        }
    }
    for (i = 0; i < 48; i++) {
        offset = &ObjectWork[i];
        if (offset == 0) {
            continue;
        }
        if ((offset->actst & 1) && (~offset->actst & 0x40000000)) {
            goto retry;
        }
    }
    for (i = 0; i < 48; i++) {
        offset = &ObjectWork[i];
        if (offset == 0) {
            continue;
        }
        if (offset->actst & 1) {
            offset->actst &= ~0x40000000;
        }
    }
}
