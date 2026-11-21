/* Where the game is asking to confirm or cancel (menu.h). */
#include "menu.h"

/* job.c's jobfunc table: the fights and the character select */
enum { JOB_SELECT_FIRST = 12, JOB_SELECT_LAST = 15, JOB_GAME_FIRST = 30, JOB_GAME_LAST = 33, JOB_OPTION_MAIN = 48 };

/* GAME_INFOMATION.mode, SYS_COND.grpvar[0], TRAINING_SETTING.cnfside (type-inventory.csv) */
#define G_INFO_MODE 120
#define SC_GRPVAR0 20
#define TR_CNFSIDE 4
#define STORY_IS_ENTER 12
#define MODE_TRAINING 0x100
#define MODE_STORY 0x2000
#define MODE_GALLERY 0x8000
#define OPT_KEY_CONFIG 0x20
#define EE_RAM_END 0x2000000u

static uint8_t r8(CoreHost *h, uint32_t addr)
{
    return (uint8_t)(ee_r32(h, addr & ~3u) >> (8 * (addr & 3)));
}

/* KEY CONFIG is up: its DEFAULT and EXIT rows confirm, the rest take any button. */
static int key_config(CoreHost *h, int pad)
{
    return ee_r32(h, ggx_static_Opt_KeyConfig_pos + 4u * (uint32_t)(pad & 1)) >= 11;
}

int menu_confirms(CoreHost *h, int pad)
{
    uint32_t job = ee_r32(h, ggx_sym_JobMode);

    if (job >= JOB_SELECT_FIRST && job <= JOB_SELECT_LAST) {
        return 0;
    }
    if (job >= JOB_GAME_FIRST && job <= JOB_GAME_LAST) {
        uint32_t mode, pos, tr;
        int kc;

        mode = ee_r32(h, ggx_sym_g_info + G_INFO_MODE);
        if (ee_r32(h, ggx_sym_Pause) == 0) {
            /* a story's talk before the fight (the fighters are held; WAIT skips on Circle), or the fight */
            return (mode & MODE_STORY) && ee_r32(h, ggx_sym_story + STORY_IS_ENTER) != 0;
        }
        if (r8(h, ggx_sym_sc + SC_GRPVAR0) == 0) {
            return 1;              /* the pause menu itself */
        }
        /* a pause menu's sub-screen: KEY CONFIG, or the command list */
        pos = ee_r32(h, ggx_static_Pause_Execute_pos);
        if (mode & MODE_TRAINING) {
            tr = ee_r32(h, ggx_sym_tr_info);
            kc = tr && tr < EE_RAM_END && r8(h, tr + TR_CNFSIDE) == 0 && pos == 5;
        } else {
            kc = pos == ((mode & MODE_GALLERY) ? 3u : 1u);
        }
        return kc ? key_config(h, pad) : 1;
    }
    if (job == JOB_OPTION_MAIN) {
        uint32_t thd = ee_r32(h, ggx_sym_opt);

        if (thd != 0xFFFFFFFFu && (thd & OPT_KEY_CONFIG)) {
            return key_config(h, pad);
        }
    }
    return 1;
}
