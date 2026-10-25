#include "generated/vibrate.h"

extern GAME_INFOMATION g_info;
extern TRAINING_SETTING *tr_info;
extern USER_SETTING userinfo;
void PadSetActuator(int pad, int sel, int time);

void lets_purupuru(CHARACTER_WORK *offset, int damage) {
    if (((g_info.mode >> offset->padid) & 1) || ((g_info.mode & 0x100) && tr_info->tene.mode == 2)) {
        if (userinfo.key[offset->padid].vibrate) {
            if (damage >= 50) {
                PadSetActuator(offset->padid, 1, 120);
            } else {
                PadSetActuator(offset->padid, 1, 10);
            }
        }
    }
}
