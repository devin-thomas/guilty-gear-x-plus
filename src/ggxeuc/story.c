#include "generated/story.h"

int FileReadCheck(void);
void JobNext(void);
extern short int UseCharacter1p2p[];
void BGM_Free(void);
void JobChange(int job);
extern PAD pads[2];
extern USER_SETTING userinfo;
extern SYS_COND sc;
char *FileReadSelect(int fid, int chan);
void g_main_End(void);
void heap_clear(void);
void PXF_SpriteClear(void);
void FadeInit(void);
extern int SubJob;
extern int SubJobCount;
void BGM_Request(int no, int loop);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
int IST_Load(int *texadr, int sprid, int palid);
void LinkAddrTb(u_int *addr);
void gallery_story_set(int no);
void nlFreeCommonTextureAll(void);
void nlFreePaletteAll(void);
void dcfont_init(void);
void CreditFontLoad(int id);
void InitCancelFlag(void);
void nlObjPutSetFadeColor(float r, float g, float b);
extern GAME_INFOMATION g_info;
void set_bgmvol(void);
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);
#define ENEMY flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1]
void EntrySelectCharacter(int playerside, int idno, int palette, int iscpu);
void SetStageNo(int no);
double pow(double x, double y);
extern u_int TotalCounter;
int sound_request(int no);
double sin(double x);
#define PI 3.14159265358979323846
extern int fade_picture_scene_width; /* STATIC_EXTERN 0x5cc364 */
extern int fade_picture_scene_height; /* STATIC_EXTERN 0x5cc368 */

void StgSelFree_story(void)
{
    close_talk_scene();
}

int get_cpu_level(void)
{
    float level;

    level = ENEMY.level;
    switch (g_info.Difficulty) {
    case 0:
        level *= 0.25f;
        break;
    case 1:
        level *= 0.5f;
        break;
    case 2:
        level *= 1.25f;
        break;
    case 3:
        level *= 2.0f;
        break;
    case 4:
        level *= 3.0f;
        break;
    case 5:
        level *= 6.0f;
        break;
    default:
        level = ENEMY.level * (float)pow(2.0, g_info.Difficulty - 2);
        break;
    }
    return level;
}

int set_gallery_flag(int id)
{
    int i;
    int sum;

    sum = 0;
    for (i = 0; i < UseCharacter1p2p[story.self_side] - 1; i++) {
        sum += misc_info[i]->nfile;
    }
    gallery_story_set(sum + (id - misc_info[UseCharacter1p2p[story.self_side] - 1]->nmessage));
    return id;
}

void change_subjob(void (*job)())
{
    if (job == the_end) {
        bg_motion = 400;
    } else if (job == talk_scene) {
        bg_motion = 0xA0;
        black = 0xA;
        which = 0;
        wtime = 0;
        motion3 = 0;
        motion2 = 0;
        motion = 0;
        BGM_Free();
    } else if (job == opening_scene) {
        bg_motion = 0x9E;
        black = 0x1E;
        BGM_Request(0x16, 1);
        set_bgmvol();
    } else if (job == ending_scene) {
        id_offset = 0;
        pic_motion = 0x280;
    } else if (job == picture_scene) {
        wtime = 300;
    } else if (job == game_over) {
        wtime = bg_motion = 300;
    }
    subjob_func = job;
}

int FACE_ID(int pos)
{
    int id = face_id[pos];

    if (pos && UseCharacter1p2p[story.self_side] == id && id == 7) {
        id = 0x14;
    } else if (pos && UseCharacter1p2p[story.self_side] == 1 && (u_char)flow_info[0][story.scene.page - 1][story.scene.line - 1].id == 0x12) {
        id = 0x10;
    } else if (pos && story.gguse[story.self_side ? 0 : 1] && id == 2) {
        id = 0x11;
    } else if (id > 0x10) {
        id++;
    }
    return id;
}

int MSG_XPOS(int i)
{
    int x = 0;

    switch (flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].talk[i].position) {
    case 0:
        x = 0x1A0;
        break;
    case 1:
        x = 0xE0;
        break;
    case 2:
        x = 0x140;
        break;
    case 3:
        x = flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].talk[i].x;
        break;
    }
    return x;
}

int MSG_YPOS(int i)
{
    int y = 0;

    switch (flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].talk[i].position) {
    case 0:
        y = 0x82;
        break;
    case 1:
        y = 0x15E;
        break;
    case 2:
        y = 0xF0;
        break;
    case 3:
        y = flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].talk[i].y;
        break;
    }
    return y;
}

void StoryInit(void)
{
    nlFreeCommonTextureAll();
    nlFreePaletteAll();
    FadeInit();
    dcfont_init();
    CreditFontLoad(0x14);
    InitCancelFlag();
    SubJob = 0;
    SubJobCount = 0;
    change_subjob(story_main_init);
    heap_clear();
    fmessage = FileReadSelect(misc_info[UseCharacter1p2p[story.self_side] - 1]->file_id, 0);
    fface = FileReadSelect(0x24, 1);
    fstgsel = FileReadSelect(0x35, 2);
    JobNext();
}

void StoryLoad(void)
{
    if (FileReadCheck()) {
        JobNext();
    }
}

void StoryMain(void)
{
    subjob_func();
}

void init_storymode(int side)
{
    story.self_side = side;
    story.nlose = 0;
    story.is_win = 1;
    story.is_enter = 0;
    story.actind[0] = 0;
    story.actind[1] = 1;
    story.actcnt[0] = 0;
    story.actcnt[1] = 0;
    story.is_boss[0] = 0;
    story.is_boss[1] = 0;
    story.scene = (scene_t){ 1, 1, 0, 0, 0 };
    story.is_itigeki = 0;
    story.is_towin = 0;
    story.is_zorome = 0;
    story.is_perfect = 0;
}

int init_story_battle(void)
{
    int id;
    action_t *act;

    if (story.is_win) {
        next_scene(1);
    }
    id = ENEMY.id;
    story.gguse[story.self_side] = 0;
    story.gguse[story.self_side ? 0 : 1] = (id >> 8) & 3;
    story.is_boss[0] = 0;
    g_info.PlayerPalette[story.self_side] = 0;
    g_info.PlayerPalette[story.self_side == 0] = 0;
    story.is_boss[1] = 0;
    if (UseCharacter1p2p[story.self_side] == (u_char)ENEMY.id) {
        if (UseCharacter1p2p[story.self_side] == 7) {
            g_info.PlayerPalette[story.self_side == 0] = 8;
        }
        if (UseCharacter1p2p[story.self_side] == 2) {
            g_info.PlayerPalette[story.self_side == 0] = 7;
        }
    }
    if (id & 0x800) {
        g_info.PlayerPalette[story.self_side == 0] = 9;
    }
    EntrySelectCharacter(story.self_side == 0, (u_char)ENEMY.id, g_info.PlayerPalette[story.self_side == 0], 1);
    SetStageNo(ENEMY.stage);
    act = ENEMY.act;
    if (act) {
        if (act[0].kind == 5 && act[0].No >= 0 && act[0].No < 3) {
            g_info.PlayerVoice[story.self_side] = act[0].No;
        }
        if (act[1].kind == 5 && act[1].No >= 0 && act[1].No < 3) {
            g_info.PlayerVoice[story.self_side == 0] = act[1].No;
        }
    }
    face_id[0] = UseCharacter1p2p[story.self_side];
    face_id[1] = (u_char)ENEMY.id;
    return 0;
}

void init_face_motion(int i)
{
    int n;

    for (n = 0; n < 16; n++) {
        face_pt[i][n] = (point_t){ 800, 800, 0.0f, 0.0f };
    }
    xpos[i] = 0x2E4;
    ypos[i] = 0x244;
    zm[i] = 1.0f;
    trnsl[i] = 0.01f;
    alpha[i] = 1.0f;
    fchange[i] = 0;
}

void story_main_init(void)
{
    init_face_motion(0);
    init_face_motion(1);
    old_middle_id = -1;
    middle_id = -1;
    old_lower_id = -1;
    old_upper_id = -1;
    lower_id = -1;
    upper_id = -1;
    id = -1;
    ex_id = -1;
    set_fileid(fface, 0x50);
    set_fileid(fmessage, 0x90);
    motion = motion2 = motion3 = wtime = which = 0;
    if ((u_char)flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].id == 0xFE) {
        fade_init(draw_op_message, 3);
        change_subjob(opening_scene);
    } else if ((u_char)flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].id == 0xFF) {
        pic_id = set_gallery_flag(flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].stage - 1);
        fade_init(draw_ed_message, 0);
        change_subjob(ending_scene);
        BGM_Request(flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].level ? flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].level : 0x1F, 1);
    } else {
        change_subjob(talk_scene);
    }
}

int change_keyLR(int key)
{
    int old = key;

    if (story.self_side) {
        if (key & 0x8000) {
            key &= ~0x8000;
            key |= 0x2000;
        }
        if (old & 0x2000) {
            key &= ~0x2000;
            key |= 0x8000;
        }
    }
    return key;
}

int change_actLR(int i, int act)
{
    if (story.self_side) {
        if (act == 0x11) {
            story.direction[i] = 1;
            act = 0x12;
        } else if (act == 0x12) {
            story.direction[i] = 0;
            act = 0x11;
        } else if (act == 0x13) {
            story.direction[i] = 1;
            act = 0x12;
        } else if (act == 0x14) {
            story.direction[i] = 0;
            act = 0x11;
        }
    } else {
        if (act == 0x11) {
            story.direction[i] = 0;
        } else if (act == 0x12) {
            story.direction[i] = 1;
        } else if (act == 0x13) {
            story.direction[i] = 0;
        } else if (act == 0x14) {
            story.direction[i] = 1;
        }
    }
    return act;
}

int change_dirLR(int dir)
{
    if (story.self_side) {
        if (dir == 0) {
            dir = 1;
        } else if (dir == 1) {
            dir = 0;
        }
    }
    return dir;
}

int change_posLR(int pos)
{
    return story.self_side ? -pos : pos;
}

void next_scene(int is_win)
{
    scene_t scene;

    if (is_win) {
        scene = flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].win_scene;
    } else {
        scene = flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].lose_scene;
    }
    if (is_win) {
        if ((scene.kind == 1 && story.is_itigeki) || (scene.kind == 2 && story.is_towin) || (scene.kind == 3 && story.is_zorome) || (scene.kind == 4 && story.is_perfect)) {
            scene.page = scene._page;
            scene.line = scene._line;
        }
    }
    story.is_itigeki = 0;
    story.is_towin = 0;
    story.is_zorome = 0;
    story.is_perfect = 0;
    if (scene.page == 0 && scene.line == 1) {
        story.scene.line++;
    } else if (scene.page != 0 || scene.line != 2) {
        story.scene = scene;
    }
}

enemy_t *get_enemy(void)
{
    return &flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1];
}

int get_enemy_id(void)
{
    return (u_char)flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].id;
}

action_t *get_act(void)
{
    return flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].act;
}

int is_retry(void)
{
    return flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].retry;
}

int is_end(void)
{
    scene_t win;
    scene_t lose;

    win = flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].win_scene;
    lose = flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].lose_scene;
    return win.page == 0 && win.line == 0 && (u_char)lose.page == 0;
}

int get_end_bgm(void)
{
    return flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].level;
}

void fade_init(void (*func)(), int flag)
{
    if (flag == 1) {
        fadespeed = 8;
        fade_func = fadeout;
        fadecount = 0;
    } else if (flag == 0) {
        fadecount = 0xFF;
        fadespeed = 8;
        fade_func = fadein;
    } else if (flag == 2) {
        fadespeed = 2;
        fadecount = 0;
        fade_func = fadeout;
    } else if (flag == 3) {
        fadecount = 0xFF;
        fadespeed = 2;
        fade_func = fadein;
    }
    draw_func = func;
}

int fadeout(void)
{
    if (fadecount < 0x100) {
        draw_func();
        polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 1.0f, fadecount << 24);
        fadecount += fadespeed;
        return 0;
    }
    return 1;
}

int fadein(void)
{
    draw_func();
    if (fadecount >= 0) {
        polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 1.0f, fadecount << 24);
        fadecount -= fadespeed;
        return 0;
    }
    return 2;
}

void set_fileid(char *file, int base)
{
    int i;

    LinkAddrTb((u_int *)file);
    for (i = 0; ((int *)file)[i] != 0; i++) {
        IST_Load((int *)file + i, base + i, -1);
    }
}

int set_msgid(int i)
{
    int pos = ENEMY.talk[i].position;
    int act = ENEMY.talk[i].actno;

    if (pos == 0) {
        upper_id = i;
        middle_id = -1;
        motion = 100;
        which = 1;
        msg_pos = 0;
        if (act == 4) {
            motion2 = 100;
        }
    } else if (pos == 1) {
        lower_id = i;
        middle_id = -1;
        motion2 = 100;
        which = 2;
        msg_pos = pos;
        if (act == 4) {
            motion = 100;
        }
    } else if (pos == 2) {
        old_middle_id = -1;
        middle_id = i;
        motion3 = 100;
        which = 3;
        msg_pos = pos;
        if (act == 4) {
            motion2 = 100;
            motion = 100;
        }
    }
    only_f = act == 4;
    return -1;
}

void draw_sprite_im(int id, int x, int y, float z, float zm_x, float zm_y, float trnsl, float u0, float u1, float v0, float v1)
{
    NL_SPR_ARG sprite;

    sprite.sprno = id;
    sprite.x = x;
    sprite.y = y;
    sprite.z = z;
    sprite.zm_x = zm_x;
    sprite.zm_y = zm_y;
    sprite.u0 = u0;
    sprite.u1 = u1;
    sprite.v0 = v0;
    sprite.v1 = v1;
    sprite.listType = 1;
    sprite.attr = 10;
    sprite.trnsl = trnsl;
    sprite.ang = 0;
    gSpriteDraw_NaomiLibVersion(&sprite);
}

void draw_sprite(int id, int x, int y, float zm, float trnsl)
{
    draw_sprite_im(id, x, y, 1.0f, zm, zm, trnsl, 0.0f, 1.0f, 0.0f, 1.0f);
}

void draw_sprite_y(int id, int x, int y, float zm, float trnsl)
{
    draw_sprite_im(id, x, y, 1.0f, zm, zm * 448.0f / 480.0f, trnsl, 0.0f, 1.0f, 0.0f, 1.0f);
}

void draw_sprite_z(int id, int x, int y, float z, float zm, float trnsl)
{
    draw_sprite_im(id, x, y, z, zm, zm, trnsl, 0.0f, 1.0f, 0.0f, 1.0f);
}

void draw_sprite_uv(int id, int x, int y, float zm_x, float zm_y, float trnsl, float u0, float u1, float v0, float v1)
{
    draw_sprite_im(id, x, y, 1.0f, zm_x, zm_y, trnsl, u0, u1, v0, v1);
}

void draw_blackbox(int mx, int my)
{
    polydraw_FBox(mx - 230, my - 64, mx + 230, my + 64, 1.0f, 0x8F000000);
}

void draw_blackbox_trnsl(int mx, int my, int trnsl)
{
    polydraw_FBox(mx - 230, my - 64, mx + 230, my + 64, 1.0f, trnsl << 24);
}

void draw_blackbox_move(int mx, int my, float n)
{
    polydraw_FBox(mx - 230.0f, my - n, mx + 230.0f, my + n, 1.0f, 0x8F000000);
}

void draw_talk_scene(void)
{
    draw_face();
    if (upper_id >= 0) {
        if (motion == 0) {
            if (msg_pos == 0 || only_f == 0) {
                draw_sprite(ENEMY.talk[upper_id].file_id + 0x8F, MSG_XPOS(upper_id), MSG_YPOS(upper_id), 1.0f, alpha[0] * 0.5f + 0.25f);
            }
        } else {
            if (motion > 70) {
                if (old_upper_id >= 0) {
                    draw_sprite(ENEMY.talk[old_upper_id].file_id + 0x8F, MSG_XPOS(old_upper_id), MSG_YPOS(old_upper_id), 1.0f, (motion - 70) / 30.0f);
                } else {
                    motion = 70;
                }
            } else if (motion > 30) {
                if (msg_pos == 0 || only_f == 0) {
                    old_upper_id = upper_id;
                } else {
                    old_upper_id = -1;
                    upper_id = -1;
                }
            } else if (msg_pos == 0 || only_f == 0) {
                draw_sprite(ENEMY.talk[upper_id].file_id + 0x8F, MSG_XPOS(upper_id) + motion * motion / 2.45f, MSG_YPOS(upper_id), 1.0f, 0.2f);
                draw_sprite(ENEMY.talk[upper_id].file_id + 0x8F, MSG_XPOS(upper_id) - motion * motion / 2.45f, MSG_YPOS(upper_id), 1.0f, (30 - motion) / 40.0f);
            }
            motion--;
        }
    } else {
        motion = 0;
    }
    if (lower_id >= 0) {
        if (motion2 == 0) {
            if (msg_pos == 1 || only_f == 0) {
                draw_sprite(ENEMY.talk[lower_id].file_id + 0x8F, MSG_XPOS(lower_id), MSG_YPOS(lower_id), 1.0f, alpha[1] * 0.5f + 0.25f);
            }
        } else {
            if (motion2 > 70) {
                if (old_lower_id >= 0) {
                    draw_sprite(ENEMY.talk[old_lower_id].file_id + 0x8F, MSG_XPOS(old_lower_id), MSG_YPOS(old_lower_id), 1.0f, (motion2 - 70) / 30.0f);
                } else {
                    motion2 = 70;
                }
            } else if (motion2 > 30) {
                if (msg_pos == 1 || only_f == 0) {
                    old_lower_id = lower_id;
                } else {
                    old_lower_id = -1;
                    lower_id = -1;
                }
            } else if (msg_pos == 1 || only_f == 0) {
                draw_sprite(ENEMY.talk[lower_id].file_id + 0x8F, MSG_XPOS(lower_id) - motion2 * motion2 / 2.45f, MSG_YPOS(lower_id), 1.0f, 0.2f);
                draw_sprite(ENEMY.talk[lower_id].file_id + 0x8F, MSG_XPOS(lower_id) + motion2 * motion2 / 2.45f, MSG_YPOS(lower_id), 1.0f, (30 - motion2) / 40.0f);
            }
            motion2--;
        }
    } else {
        motion2 = 0;
    }
    if (middle_id >= 0) {
        if (motion3 == 0) {
            draw_sprite_z(ENEMY.talk[middle_id].file_id + 0x8F, MSG_XPOS(middle_id), MSG_YPOS(middle_id), 0.4f, 1.2f, 0.75f);
            old_middle_id = middle_id;
        } else {
            if (motion3 > 70) {
                if (old_middle_id >= 0) {
                    draw_sprite_z(ENEMY.talk[old_middle_id].file_id + 0x8F, MSG_XPOS(old_middle_id), MSG_YPOS(old_middle_id), 0.4f, 1.2f, (motion3 - 70) / 30.0f);
                } else {
                    motion3 = 70;
                }
            } else if (motion3 > 30) {
                old_middle_id = middle_id;
            } else {
                draw_sprite_z(ENEMY.talk[middle_id].file_id + 0x8F, MSG_XPOS(middle_id) - motion3 * motion3 / 2.45f, MSG_YPOS(middle_id), 0.4f, 1.2f, 0.2f);
                draw_sprite_z(ENEMY.talk[middle_id].file_id + 0x8F, MSG_XPOS(middle_id) + motion3 * motion3 / 2.45f, MSG_YPOS(middle_id), 0.4f, 1.2f, (30 - motion3) / 40.0f);
            }
            motion3--;
        }
    } else {
        motion3 = 0;
    }
}

int draw_face(void)
{
    int i;
    int n;
    int tmp;

    StgSelMain_story();
    if (bg_motion) {
        return 0;
    }
    {
        polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 1.0f, (10 - black) * 191 / 10 << 24);
        if (black) {
            black--;
        }
        for (i = 0; i < 2; i++) {
            if (fchange[i]) {
                if (fmotion_kind == 4) {
                    if (xpos[i] > 440) {
                        if (face_pt[i][0].x == face_pt[i][15].x) {
                            face_id[i] = fchange[i];
                            init_face_motion(i);
                            continue;
                        }
                        trnsl[i] = 1.0f;
                    } else {
                        xpos[i] += 18;
                        ypos[i] = xpos[i] * 0.5f;
                        trnsl[i] += 0.0075f;
                    }
                    if (alpha[0] >= 0.999f) {
                        draw_sprite_z(FACE_ID(0) + 0x4F, 100, 130, 0.75f, 1.0f, 1.0f);
                    }
                } else if (fmotion_kind == 3) {
                    if (xpos[i] > 704) {
                        if (face_pt[i][0].x == face_pt[i][15].x) {
                            face_id[i] = fchange[i];
                            init_face_motion(i);
                            continue;
                        }
                        xpos[i] = 0x2E4;
                        ypos[i] = 0;
                        trnsl[i] = 1.0f;
                    } else {
                        xpos[i] += 18;
                        trnsl[i] += 0.0075f;
                        ypos[i] = xpos[i] * 0.42727273f;
                        if (xpos[i] == 0x264) {
                            sound_request(0x19);
                        }
                        if (xpos[i] >= 0x262 && xpos[i] <= 0x28A) {
                            polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 0.75f, 0xFFFFFFFF);
                        }
                    }
                } else if (fmotion_kind == 2) {
                    if (i == 0) {
                        draw_sprite_z(fchange[0] + 0x4F, 100, 130, 0.75f, 1.0f, 1.0f);
                    } else {
                        draw_sprite_z(fchange[i] + 0x66, 0x21C, 0x15E, 0.75f, 1.0f, 1.0f);
                    }
                    if (xpos[i] > 704) {
                        if (face_pt[i][0].x == face_pt[i][15].x) {
                            face_id[i] = fchange[i];
                            for (n = 0; n < 16; n++) {
                                face_pt[i][n] = (point_t){ 0, 0, 1.0f, 1.0f };
                            }
                            xpos[i] = 0;
                            ypos[i] = 0;
                            trnsl[i] = 1.0f;
                            alpha[i] = 1.0f;
                            fchange[i] = 0;
                            continue;
                        }
                        xpos[i] = 0x2E4;
                        ypos[i] = 0;
                        trnsl[i] = 1.0f;
                    } else {
                        xpos[i] += 18;
                        ypos[i] = 0;
                        trnsl[i] += 0.0075f;
                    }
                    for (n = 14; n >= 0; n--) {
                        face_pt[i][n + 1] = face_pt[i][n];
                    }
                    face_pt[i][0] = (point_t){ xpos[i], ypos[i], zm[i], trnsl[i] };
                    for (n = 15; n >= 0; n--) {
                        if (i == 0) {
                            draw_sprite_z(FACE_ID(0) + 0x66, 0x334 - face_pt[i][n].x, face_pt[i][n].y + 130, 1.0f, face_pt[i][n].zm, n ? face_pt[i][n].trnsl : 1.0f);
                        } else {
                            draw_sprite_z(FACE_ID(1) + 0x66, 0x21C - face_pt[i][n].x, 0x15E - face_pt[i][n].y, 1.0f, face_pt[i][n].zm, n ? face_pt[i][n].trnsl : 1.0f);
                        }
                    }
                    continue;
                } else if (fmotion_kind == 0) {
                    if (xpos[i] > 704) {
                        if (face_pt[i][0].x == face_pt[i][15].x) {
                            face_id[i] = fchange[i];
                            init_face_motion(i);
                            continue;
                        }
                        xpos[i] = 0x2E4;
                        ypos[i] = 0;
                        trnsl[i] = 1.0f;
                    } else {
                        xpos[i] += 36;
                        ypos[i] = 0;
                        trnsl[i] += 0.0075f;
                    }
                } else if (fmotion_kind == 1) {
                    if (fmotion == 0) {
                        if (face_pt[i][0].x == face_pt[i][15].x) {
                            face_id[i] = fchange[i];
                            fchange[i] = 0;
                            alpha[i] = 1.0f;
                            if (i == 0) {
                                draw_sprite_z(FACE_ID(0) + 0x4F, 100, 130, 2.0f, 1.0f, 1.0f);
                                draw_sprite_z(FACE_ID(0) + 0x4F, 100, 130, 1.0f, 1.0f, (which & 2) ? ((alpha[0] > 0.1f) ? (alpha[0] -= 0.1f) : 0.0f) : ((alpha[0] < 0.9f) ? (alpha[0] += 0.1f) : 1.0f));
                            } else {
                                draw_sprite_z(FACE_ID(1) + 0x66, 0x21C, 0x15E, 2.0f, 1.0f, 1.0f);
                                draw_sprite_z(FACE_ID(1) + 0x66, 0x21C, 0x15E, 1.0f, 1.0f, (which & 1) ? ((alpha[1] > 0.1f) ? (alpha[1] -= 0.1f) : 0.0f) : ((alpha[1] < 0.9f) ? (alpha[1] += 0.1f) : 1.0f));
                            }
                            continue;
                        }
                        xpos[i] = 0;
                        ypos[i] = 0;
                        trnsl[i] = 0;
                    } else {
                        xpos[i] = sin((180 - fmotion * 4) / 180.0f * PI) * 110.0;
                        ypos[i] = sin((180 - fmotion * 4) / 180.0f * PI) * 60.0;
                        trnsl[i] = fmotion / 45.0f;
                        fmotion--;
                    }
                    for (n = 14; n >= 0; n--) {
                        face_pt[i][n + 1] = face_pt[i][n];
                    }
                    face_pt[i][0] = (point_t){ xpos[i], ypos[i], zm[i], trnsl[i] };
                    tmp = face_id[i];
                    face_id[i] = fchange[i];
                    for (n = 15; n >= 0; n--) {
                        if (i == 0) {
                            draw_sprite_z(FACE_ID(0) + 0x4F, 100 - face_pt[i][n].x, 130 - face_pt[i][n].y, fmotion >= 23 ? 1.0f : 0.75f, face_pt[i][n].zm, n ? (1.0f - face_pt[i][n].trnsl) / 6.0f : 1.0f - face_pt[i][n].trnsl);
                        } else {
                            draw_sprite_z(FACE_ID(1) + 0x66, face_pt[i][n].x + 0x21C, face_pt[i][n].y + 0x15E, fmotion >= 23 ? 1.0f : 0.75f, face_pt[i][n].zm, n ? (1.0f - face_pt[i][n].trnsl) / 6.0f : 1.0f - face_pt[i][n].trnsl);
                        }
                    }
                    face_id[i] = tmp;
                    for (n = 15; n >= 0; n--) {
                        if (i == 0) {
                            draw_sprite_z(FACE_ID(0) + 0x4F, face_pt[i][n].x + 100, face_pt[i][n].y + 130, 1.0f, face_pt[i][n].zm, n ? face_pt[i][n].trnsl / 6.0f : face_pt[i][n].trnsl);
                        } else {
                            draw_sprite_z(FACE_ID(1) + 0x66, 0x21C - face_pt[i][n].x, 0x15E - face_pt[i][n].y, 1.0f, face_pt[i][n].zm, n ? face_pt[i][n].trnsl / 6.0f : face_pt[i][n].trnsl);
                        }
                    }
                    continue;
                }
            } else if (xpos[i] < 36 && ypos[i] < 36) {
                if (face_pt[i][0].x == face_pt[i][15].x && face_pt[i][0].y == face_pt[i][15].y) {
                    if (i == 0) {
                        draw_sprite_z(FACE_ID(0) + 0x4F, 100, 130, 2.0f, 1.0f, 1.0f);
                        draw_sprite_z(FACE_ID(0) + 0x4F, 100, 130, 1.0f, 1.0f, (which & 2) ? ((alpha[0] > 0.1f) ? (alpha[0] -= 0.1f) : 0.0f) : ((alpha[0] < 0.9f) ? (alpha[0] += 0.1f) : 1.0f));
                    } else {
                        draw_sprite_z(FACE_ID(1) + 0x66, 0x21C, 0x15E, 2.0f, 1.0f, 1.0f);
                        draw_sprite_z(FACE_ID(1) + 0x66, 0x21C, 0x15E, 1.0f, 1.0f, (which & 1) ? ((alpha[1] > 0.1f) ? (alpha[1] -= 0.1f) : 0.0f) : ((alpha[1] < 0.9f) ? (alpha[1] += 0.1f) : 1.0f));
                    }
                    continue;
                }
                xpos[i] = 0;
                ypos[i] = 0;
                trnsl[i] = 1.0f;
            } else if (i == 1 && UseCharacter1p2p[story.self_side] == 0x10 && (u_char)ENEMY.id == 3 && FACE_ID(1) == 3) {
                xpos[1] = 0;
                fchange[1] = 0;
                ypos[1] -= 36;
                trnsl[1] += 0.0075f;
            } else {
                xpos[i] -= 36;
                ypos[i] = 0;
                fchange[i] = 0;
                trnsl[i] += 0.0075f;
            }
            for (n = 14; n >= 0; n--) {
                face_pt[i][n + 1] = face_pt[i][n];
            }
            face_pt[i][0] = (point_t){ xpos[i], ypos[i], zm[i], trnsl[i] };
            for (n = 15; n >= 0; n--) {
                if (i == 0) {
                    draw_sprite_z(FACE_ID(0) + 0x4F, face_pt[i][n].x + 100, face_pt[i][n].y + 130, 1.0f, face_pt[i][n].zm, n ? face_pt[i][n].trnsl : 1.0f);
                } else {
                    draw_sprite_z(FACE_ID(1) + 0x66, 0x21C - face_pt[i][n].x, 0x15E - face_pt[i][n].y, 1.0f, face_pt[i][n].zm, n ? face_pt[i][n].trnsl : 1.0f);
                }
            }
        }
    }
    return 0;
}

static u_long next = 1;
int do_rand(u_long *ctx)
{
    return (*ctx = *ctx * 1103515245 + 12345) % ((u_long)0x7FFFFFFF + 1);
}

int my_rand(void)
{
    return do_rand(&next);
}

void draw_pre_op(int fy)
{
    int i;
    int x1;
    int x2;
    float zm;
    float trnsl;

    i = (128 - bg_motion) * 10;
    x1 = op_x - 676 + i;
    x2 = op_x + 1316 - i;
    zm = (x2 - x1) / 640.0f;
    trnsl = bg_motion / 128.0f;
    zm = zm >= 0.0f ? zm : -zm;
    zm = ((zm > 1.0f ? 1.0f : zm) < 0.0f) ? 0.0f : (zm > 1.0f ? 1.0f : zm);
    trnsl = trnsl >= 0.0f ? trnsl : -trnsl;
    if (bg_motion == 0 || bg_motion > 128) {
        return;
    }
    draw_sprite(0x82, 320, 240, 1.0f, 0.5f);
    if (bg_motion >= 28) {
        draw_sprite_im(0x82, 320, 240, 1.0f, zm, 1.0f - (1.0f - trnsl) * 0.125f, trnsl / 3.5f, 0.0f, 1.0f, 0.0f, 1.0f);
        draw_sprite_im(0x82, 320, 240, 1.0f, zm, 1.0f - (1.0f - trnsl) * 0.25f, trnsl / 3.0f, 0.0f, 1.0f, 0.0f, 1.0f);
        draw_sprite_im(0x82, 320, 240, 1.0f, zm, 1.0f - (1.0f - trnsl) * 0.5f, trnsl / 2.5f, 0.0f, 1.0f, 0.0f, 1.0f);
        draw_sprite_im(0x82, 320, 240, 1.0f, zm, trnsl, trnsl * 0.5f, 0.0f, 1.0f, 0.0f, 1.0f);
    } else {
        draw_sprite(0x84, 320, 240.0f - zm * 345.0f, zm * 1.13f, zm * 0.5f);
        draw_sprite(0x84, 320, zm * 345.0f + 240.0f, zm * 1.13f, zm * 0.5f);
        draw_sprite(0x84, 320, 240, zm * 1.13f, zm);
    }
    draw_sprite(0x83, x1, 0xFA - fy, 1.0f, 1.0f);
    draw_sprite(0x83, x2, 0xFA - fy, 1.0f, 1.0f);
}

void draw_op_bg_begin(int pattern)
{
    static int fy = 0;
    static int n = 0;
    int r;

    r = my_rand();
    op_y = 0;
    op_x = 0;
    if ((r & 0x3F) == 0) {
        op_x = (r & 0x100) ? 1 : -1;
    }
    if (((r >> 6) & 0x3F) == 0) {
        op_y = (r & 0x200) ? 1 : -1;
    }
    if (TotalCounter % 360 == 0) {
        op_y = -16;
    }
    fy = (fy + 8) & 0x1F;
    if (pattern == 0) {
        draw_pre_op(fy);
        if (bg_motion) {
            bg_motion--;
            if (bg_motion != -1) {
                return;
            }
        }
    }
    draw_sprite(0x82, 320, 240, 1.0f, pattern ? 1.0f : 0.5f);
    if (pattern == 0 || pattern == 2) {
        polydraw_FBox(op_x + 0x48, op_y - 0x109, op_x + 0x238, op_y + 0x37, 1.0f, 0xFF000000);
        draw_sprite(pattern ? 0x8C : 0x84, op_x + 320, op_y - 105, 1.0f, 0.5f);
        draw_sprite(pattern ? 0x8C : 0x84, op_x + 320, op_y + 240, 1.0f, 1.0f);
        polydraw_FBox(op_x + 0x48, op_y + 0x1A9, op_x + 0x238, op_y + 0x2E9, 1.0f, 0xFF000000);
        draw_sprite(pattern ? 0x8C : 0x84, op_x + 320, op_y + 0x249, 1.0f, 0.5f);
    } else {
        polydraw_FBox(op_x + 0x48, op_y - 0x109, op_x + 0x238, op_y + 0x37, 1.0f, 0xFF000000);
        draw_sprite(pic_id + misc_info[UseCharacter1p2p[story.self_side] - 1]->nfile + 0x90, op_x + 320, op_y - 105, 1.0f, 0.5f);
        draw_sprite(pic_id + misc_info[UseCharacter1p2p[story.self_side] - 1]->nfile + 0x90, op_x + 320, op_y + 240, 1.0f, 1.0f);
        polydraw_FBox(op_x + 0x48, op_y + 0x1A9, op_x + 0x238, op_y + 0x2E9, 1.0f, 0xFF000000);
        draw_sprite(pic_id + misc_info[UseCharacter1p2p[story.self_side] - 1]->nfile + 0x90, op_x + 320, op_y + 0x249, 1.0f, 0.5f);
    }
    polydraw_FBox(op_x + 0x48, op_y + 0x37, op_x + 0x238, op_y + 0x50, 1.0f, 0xFF000000);
    polydraw_FBox(op_x + 0x48, op_y + 0x190, op_x + 0x238, op_y + 0x1A9, 1.0f, 0xFF000000);
    draw_sprite(0x83, op_x + 0x24, 0xFA - fy, 1.0f, 1.0f);
    draw_sprite(0x83, op_x + 0x25C, 0xFA - fy, 1.0f, 1.0f);
    draw_sprite((n >> 1) + 0x85, op_x + 0x277, op_y + 0x9B, 1.0f, 1.0f);
    draw_sprite((n >> 1) + 0x85, op_x + 0x277, op_y + 0x1F4, 1.0f, 1.0f);
    n = (n + 1) & 7;
    if (pattern == 0 && TotalCounter % 600 == 0) {
        draw_sprite(0x89, op_x + 0x200, op_y + 0x80, 1.0f, 1.0f);
    }
    if (pattern == 2 && TotalCounter % 600 == 0) {
        draw_sprite(r % 3 + 0x8D, op_x + 180 + r % 280, op_y + 130 + r % 220, 1.0f, 1.0f);
    }
}

void draw_op_bg_end(int pattern)
{
    static int a = 0;
    static int a_old = 0;
    static int l = 0;
    int r;

    r = my_rand();
    if (r & 0x1) a++;
    if (r & 0x2) a++;
    if (r & 0x4) a++;
    if (r & 0x8) a++;
    if (r & 0x10) a++;
    if (r & 0x20) a++;
    if (r & 0x40) a++;
    if (r & 0x80) a++;
    a %= 4;
    if (a == a_old) {
        a++;
        a %= 4;
    }
    a_old = a;
    if (pattern == 0 && r % 3 == 0) {
        draw_sprite(a + 0x7E, op_x + 320, op_y - 105, 1.0f, 0.5f);
        draw_sprite(a + 0x7E, op_x + 320, op_y + 240, 1.0f, 1.0f);
        draw_sprite(a + 0x7E, op_x + 320, op_y + 0x249, 1.0f, 0.5f);
    }
    l = (l + 1) % 4;
    polydraw_FBox(0.0f, 0.0f, 640.0f, 480.0f, 1.0f, l < 2 ? 0 : 0x1F000000);
}

void draw_op_message(void)
{
    draw_op_bg_begin(0);
    if (bg_motion) {
        return;
    }
    draw_blackbox_move(320, 240, (60 - black * 2 > 30 ? 30 : 60 - black * 2) * 2.1333334f);
    if (black) {
        black--;
    }
    if (ex_id >= 0) {
        if (motion == 0) {
            draw_sprite(ENEMY.talk[ex_id].file_id + 0x8F, MSG_XPOS(ex_id) + op_x, MSG_YPOS(ex_id) + op_y, 1.0f, 0.75f);
        } else {
            if (motion > 70) {
                if (ex_id > 0) {
                    draw_sprite(ENEMY.talk[ex_id - 1].file_id + 0x8F, MSG_XPOS(ex_id - 1) + op_x, MSG_YPOS(ex_id - 1) + op_y, 1.0f, (motion - 70) / 30.0f);
                }
            } else if (motion <= 30) {
                draw_sprite(ENEMY.talk[ex_id].file_id + 0x8F, MSG_XPOS(ex_id) + motion + op_x, MSG_YPOS(ex_id) + op_y, 1.0f, 0.2f);
                draw_sprite(ENEMY.talk[ex_id].file_id + 0x8F, MSG_XPOS(ex_id) - motion + op_x, MSG_YPOS(ex_id) + op_y, 1.0f, (30 - motion) / 40.0f);
            }
            motion--;
        }
    }
    draw_op_bg_end(0);
}

void fade_op_message(void)
{
    draw_op_bg_begin(0);
    draw_blackbox_move(320, 240, (60 - (30 - black) * 2 > 30 ? 30 : 60 - (30 - black) * 2) * 2.1333334f);
    if (black) {
        black--;
    }
    if (ex_id >= 0) {
        if (motion > 70) {
            draw_sprite(ENEMY.talk[ex_id].file_id + 0x8F, MSG_XPOS(ex_id) + op_x, MSG_YPOS(ex_id) + op_y, 1.0f, (motion - 70) / 30.0f);
        }
        motion--;
    }
    draw_op_bg_end(0);
}

void draw_ed_message(void)
{
    draw_picture_scene();
    if (pic_motion) {
        return;
    }
    if (ex_id - id_offset < 0) {
        return;
    }
    {
        if (motion == 0) {
            if (ENEMY.talk[ex_id].file_id - 1 >= 0) {
                draw_blackbox(MSG_XPOS(ex_id), MSG_YPOS(ex_id));
                draw_sprite(ENEMY.talk[ex_id].file_id + 0x8F, MSG_XPOS(ex_id), MSG_YPOS(ex_id), 1.0f, 0.75f);
            }
        } else {
            if (motion > 70) {
                if (ex_id - id_offset > 0) {
                    draw_blackbox_trnsl(MSG_XPOS(ex_id - 1), MSG_YPOS(ex_id - 1), (motion - 70) / 30.0f * 143.0f);
                    draw_sprite(ENEMY.talk[ex_id - 1].file_id + 0x8F, MSG_XPOS(ex_id - 1), MSG_YPOS(ex_id - 1), 1.0f, (motion - 70) / 30.0f);
                } else {
                    motion = 30;
                }
            } else if (motion <= 30) {
                if (ENEMY.talk[ex_id].file_id - 1 >= 0) {
                    draw_blackbox_move(MSG_XPOS(ex_id), MSG_YPOS(ex_id), (60 - motion * 2 > 30 ? 30 : 60 - motion * 2) * 2.1333334f);
                    draw_sprite(ENEMY.talk[ex_id].file_id + 0x8F, MSG_XPOS(ex_id) + motion, MSG_YPOS(ex_id), 1.0f, 0.2f);
                    draw_sprite(ENEMY.talk[ex_id].file_id + 0x8F, MSG_XPOS(ex_id) - motion, MSG_YPOS(ex_id), 1.0f, (30 - motion) / 40.0f);
                }
            }
            motion--;
        }
    }
}

void fade_picture_scene(void)
{
    float fwidth;
    float fheight;
    int id;
    int i;

    fwidth = fade_picture_scene_width / 640.0f;
    fheight = fade_picture_scene_height / 480.0f;
    id = pic_id + 0x90;
    for (i = 0; i < 16; i++) {
        draw_sprite_uv(id, i * 40 + 20, 240, fwidth * pic_motion / 30.0f, 1.0f, 0.5f, (i * 40) / 640.0f, (i * 40) / 640.0f + fwidth, 0.0f, 1.0f);
    }
    for (i = 0; i < 12; i++) {
        draw_sprite_uv(id, 320, i * 40 + 20, 1.0f, fheight * pic_motion / 30.0f, 0.5f, 0.0f, 1.0f, (i * 40) / 480.0f, (i * 40) / 480.0f + fheight);
    }
    if (pic_motion) {
        pic_motion--;
    }
}

void end_picture_scene(void)
{
    if (bg_motion > 360) {
        draw_sprite(pic_id + 0x90, 320, 240, 1.0f, 1.0f);
    } else if (bg_motion > 240) {
        draw_sprite(pic_id + 0x90, 320, 240, bg_motion / 360.0f, (bg_motion - 240) / 120.0f);
    } else if (bg_motion <= 100) {
        if (bg_motion > 0) {
            draw_sprite(0x8A, op_x + 480, op_y + 360, 1.0f, (100 - bg_motion) / 100.0f);
        } else if (bg_motion == 0) {
            draw_sprite(0x8A, op_x + 480, op_y + 360, 1.0f, 1.0f);
        }
    }
    if (bg_motion) {
        bg_motion--;
    }
}

void opening_scene(void)
{
    int fstat = fade_func();

    if (fstat == 1) {
        change_subjob(talk_scene);
        init_story_battle();
    } else if (motion == 0 && fstat != 0 && !WAIT()) {
        if (ENEMY.talk[ex_id + 1].file_id - 1 < 0) {
            black = 30;
            motion = 100;
            fade_init(fade_op_message, 2);
        } else {
            ex_id++;
            motion = 100;
            wtime = 300;
        }
    } else if (fstat != 0 && IS_SKIP()) {
        fade_init(draw_op_message, 1);
    }
}

void ending_scene(void)
{
    int fstat = fade_func();

    if (fstat == 1) {
        if (ENEMY.talk[ex_id + 1].actno == 3) {
            pic_id = set_gallery_flag(-ENEMY.talk[ex_id + 1].file_id - 1);
            pic_motion = 0x280;
            ex_id++;
            id_offset = ex_id + 1;
            fade_init(draw_ed_message, 0);
            if (UseCharacter1p2p[story.self_side] == 4) {
                BGM_Free();
                BGM_Request(0x20, 1);
            }
        } else if (userinfo.autosave) {
            sc.grpvar[1] = -1;
            JobChange(0x2D);
        } else {
            JobChange(3);
        }
    } else if (motion == 0 && fstat != 0 && !WAIT()) {
        if (ENEMY.talk[ex_id + 1].file_id - 1 < 0) {
            if (ENEMY.talk[ex_id + 1].actno == 3) {
                fade_init(draw_ed_message, 1);
            } else {
                ex_id++;
                motion = 100;
                wtime = 300;
                change_subjob(the_end);
            }
        } else {
            ex_id++;
            motion = 100;
            wtime = 300;
        }
    } else if (fstat != 0 && IS_SKIP()) {
        fade_init(draw_ed_message, 1);
    }
}

void set_fchange(int i, int id)
{
    int n;

    for (n = 0; n < 16; n++) {
        face_pt[i][n].trnsl = 0.0f;
    }
    trnsl[i] = 0.01f;
    fchange[i] = id;
    fmotion = 0;
    if (UseCharacter1p2p[story.self_side] == 15 && (u_char)ENEMY.id == 8 && story.scene.page == 2 && face_id[i] == 0x15 && id == 0x16) {
        fmotion_kind = 4;
        return;
    }
    if (UseCharacter1p2p[story.self_side] == 13 && (u_char)ENEMY.id == 1 && upper_id == 7) {
        fmotion_kind = 3;
        return;
    }
    if (UseCharacter1p2p[story.self_side] == 10 && (u_char)ENEMY.id == 15 && face_id[i] == 8 && id == 0x15) {
        face_id[i] = id;
        fchange[i] = 8;
        fmotion_kind = 2;
        return;
    }
    if (UseCharacter1p2p[story.self_side] == 10 && (u_char)ENEMY.id == 15 && face_id[i] == 10 && id == 0x15) {
        face_id[i] = id;
        fchange[i] = 10;
        fmotion_kind = 2;
        return;
    }
    if (xpos[i] > 640 || id == 0x16 || face_id[i] == 0x16) {
        fmotion_kind = 0;
    } else {
        fmotion = 45;
        fmotion_kind = 1;
    }
}

void talk_scene(void)
{
    if (ENEMY.talk[id + 1].actno == 1) {
        if (ENEMY.talk[id + 1].position == 0) {
            set_fchange(1, ENEMY.talk[id + 1].file_id);
        } else {
            set_fchange(0, ENEMY.talk[id + 1].file_id);
        }
        id++;
    }
    draw_talk_scene();
    if (bg_motion == 0 && fchange[0] == 0 && fchange[1] == 0 && motion == 0 && motion2 == 0 && motion3 == 0 && !WAIT()) {
        if (ENEMY.talk[id + 1].file_id - 1 < 0 || ENEMY.talk[id + 1].actno == 2) {
            set_fchange(0, 0x16);
            set_fchange(1, 0x16);
            middle_id = -1;
            lower_id = -1;
            upper_id = -1;
            close_talk_scene();
        } else {
            set_msgid(++id);
            wtime = 300;
        }
    } else if (IS_SKIP()) {
        close_talk_scene();
    }
}

void close_talk_scene(void)
{
    while (flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].talk[id + 1].file_id - 1 >= 0) {
        id++;
    }
    if (flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].talk[id + 1].file_id - 1 < -1) {
        if (flow_info[UseCharacter1p2p[story.self_side] - 1][story.scene.page - 1][story.scene.line - 1].talk[id + 1].actno == 3) {
            ex_id = id;
            change_subjob(ending_scene);
        } else {
            change_subjob(picture_scene);
        }
    } else {
        change_subjob(go_vs);
    }
    fade_init(draw_talk_scene, 1);
}

void picture_scene(void)
{
    int fstat = fade_func();

    if (fstat == 1) {
        if (draw_func == draw_talk_scene) {
            pic_motion = 0x280;
            pic_id = set_gallery_flag(-ENEMY.talk[id + 1].file_id - 1);
            fade_init(draw_picture_scene, 0);
        } else {
            change_subjob(go_vs);
        }
    } else if (fstat != 0) {
        if (!WAIT() || IS_SKIP()) {
            if (ENEMY.talk[id + 1].actno == 2) {
                change_subjob(the_end);
            } else {
                pic_motion = 30;
                fade_init(fade_picture_scene, 1);
            }
        }
    }
}

void go_vs(void)
{
    if (fade_func() == 1) {
        BGM_Free();
        JobChange(0x15);
    }
}

int WAIT(void)
{
    int ret;

    if (pads[story.self_side].trg & 0x20) {
        wtime = 0;
    }
    if (wtime) {
        ret = wtime--;
    } else {
        ret = 0;
    }
    return ret;
}

int IS_SKIP(void)
{
    return pads[story.self_side].trg & 0x800;
}

void draw_the_end(void)
{
    draw_op_bg_begin(1);
    draw_op_bg_end(1);
    end_picture_scene();
}

void the_end(void)
{
    if (motion) {
        fade_func();
    } else {
        draw_the_end();
    }
    if (IS_SKIP()) {
        fade_init(draw_the_end, 2);
        change_subjob(the_end_is_the_begin);
    }
}

void the_end_is_the_begin(void)
{
    if (fade_func() == 1) {
        if (userinfo.autosave) {
            sc.grpvar[1] = -1;
            JobChange(0x2D);
        } else {
            JobChange(3);
        }
    }
}

void StoryGameOverInit(void)
{
    g_main_End();
    heap_clear();
    fface = FileReadSelect(0x24, 1);
    change_subjob(game_over_init);
    JobNext();
}

void StoryGameOverLoad(void)
{
    if (FileReadCheck()) {
        PXF_SpriteClear();
        FadeInit();
        SubJob = 0;
        SubJobCount = 0;
        JobNext();
    }
}

void StoryGameOverMain(void)
{
    subjob_func();
}

void game_over_init(void)
{
    set_fileid(fface, 0x50);
    fade_init(draw_game_over, 3);
    change_subjob(game_over);
    BGM_Request(0x1A, 0);
}

void game_over(void)
{
    int fstat = fade_func();

    if (fstat == 1) {
        nlObjPutSetFadeColor(1.0f, 1.0f, 1.0f);
        BGM_Free();
        if (userinfo.autosave) {
            sc.grpvar[1] = -1;
            JobChange(0x2D);
        } else {
            JobChange(3);
        }
        g_info.BattleControlFlag &= ~4;
    } else if (fstat != 0) {
        if (!WAIT() || IS_SKIP()) {
            fade_init(draw_game_over, 2);
        }
    }
}

void draw_game_over(void)
{
    draw_op_bg_begin(2);
    draw_op_bg_end(2);
    if (bg_motion <= 100) {
        if (bg_motion > 0) {
            draw_sprite(0x8B, op_x + 450, op_y + 360, 1.0f, (100 - bg_motion) / 100.0f);
        } else if (bg_motion == 0) {
            draw_sprite(0x8B, op_x + 450, op_y + 360, 1.0f, 1.0f);
        }
    }
    if (bg_motion) {
        bg_motion--;
    }
}

void draw_picture_scene(void)
{
    static int width = 80;
    static int height = 80;
    float fwidth;
    float fheight;
    int id;
    int i;
    int x;
    int y;
    float zm;

    fwidth = width / 640.0f;
    fheight = height / 480.0f;
    id = pic_id + 0x90;
    x = 640.0f - (pic_motion * 0.0015625f * pic_motion + pic_motion * 0.0f);
    y = x * 480 / 640.0f;
    zm = (640 - x) / 640.0f + 1.0f;
    if (pic_motion == 0) {
        draw_sprite(id, 320, 240, 1.0f, 1.0f);
    } else {
        for (i = 0; i < 8; i++) {
            int I = 7 - i;
            int X1 = ((i + 1) * 80 > x ? x : (i + 1) * 80) - (640 - x) * i / 5 - 40;
            int X2 = ((I + 1) * 80 < 640 - x ? 640 - x : (I + 1) * 80) + (640 - x) * i / 5 - 40;

            draw_sprite_uv(id, X1, 240, fwidth * zm, zm, zm * 0.4, (i * 80) / 640.0f, (i * 80) / 640.0f + fwidth, 0.0f, 1.0f);
            draw_sprite_uv(id, X2, 240, fwidth * zm, zm, zm * 0.4, (I * 80) / 640.0f, (I * 80) / 640.0f + fwidth, 0.0f, 1.0f);
        }
        for (i = 0; i < 6; i++) {
            int I = 5 - i;
            int Y1 = ((i + 1) * 80 > y ? y : (i + 1) * 80) - (640 - x) * i / 5 - 40;
            int Y2 = ((I + 1) * 80 < 480 - y ? 480 - y : (I + 1) * 80) + (640 - x) * i / 5 - 40;

            draw_sprite_uv(id, 320, Y1, zm, fheight * zm, zm * 0.4, 0.0f, 1.0f, (i * 80) / 480.0f, (i * 80) / 480.0f + fheight);
            draw_sprite_uv(id, 320, Y2, zm, fheight * zm, zm * 0.4, 0.0f, 1.0f, (I * 80) / 480.0f, (I * 80) / 480.0f + fheight);
        }
    }
    pic_motion = pic_motion > 8 ? pic_motion - 8 : 0;
}

extern USER_SETTING userinfo;
extern int rannyuuflag;
extern int StageNo;
extern short int logowidth[];
extern int sndtb[];
void pack_allfree(void);
void sound_load_Common(void);
void BGM_Request(int no, int loop);
void set_bgmvol(void);
void Read_Playdat(int size);
void LinkAddrTb(u_int *addr);
int IST_Load(int *texadr, int sprid, int palid);
void PXF_Free(int id, int pal);
void CockpitLoadInit(void);
void CreditFontLoad(int id);
void *dmalloc(int size, char *name);
void dfree(void *mem);
void FadeInit(void);
void InitCancelFlag(void);
int IsRannyuu(void);
int IsCancelFlag(void);
int IsFadeEnd(void);
void DispCreditMsg(void);
void polydraw_FBox(float x1, float y1, float x2, float y2, float z, unsigned int color1);
void EasySpriteDrawEX(int id, int x, int y, float z, int flg, float trnsl);
unsigned int gStripGetFadeColor(unsigned int color);
void gSpriteDraw_NaomiLibVersion(NL_SPR_ARG *sprite);
void DrawBG(int id, int x, int y, float z);
void strloop(int fms);
void SetFadeSpeed(int frame);
void Rq_Fadein(void);
void Rq_Fadeout(void);
void EnterFight_Main(void);

#define PFBOX(q, w_, zz, c) polydraw_FBox((int)(q).x, (int)(q).y, (int)((q).x + (w_)), (int)((q).y + (q).h), zz, c)

void StgSelMain_story(void)
{
    u_int *p;
    int i;
    int id;
    int bgm;
    static int stg;
    static int stgold;
    static int wait;
    static NL_SPR_ARG *sprite;
    static POLYGN *poly;
    static FUJI *film;
    static float x5;
    static float x6;
    static float y3;
    static int count;
    int offu;
    int offl;
    u_int color_w;
    int enemy_id;
    int fms;

    offu = userinfo.offs_cockpit_upper;
    offl = userinfo.offs_cockpit_lower;
    enemy_id = get_enemy_id();
    if (SubJob == 0) {
        if (enemy_id == 11) {
            bgm = 0x1d;
        } else if (enemy_id == 16) {
            bgm = 0x1e;
        } else {
            bgm = 0x18;
        }
        if (is_end()) {
            bgm = get_end_bgm();
        }
        pack_allfree();
        sound_load_Common();
        BGM_Request(bgm, 1);
        set_bgmvol();
        SubJob++;
    } else if (SubJob == 1) {
        i = 0;
        Read_Playdat(0x100);
        p = (u_int *)fstgsel;
        LinkAddrTb(p);
        FadeInit();
        while (p[i]) {
            IST_Load((int *)&p[i], i, -1);
            i++;
        }
        id = enemy_id;
        PXF_Free(20, 6);
        IST_Load((int *)&p[id + 19], 20, -1);
        CockpitLoadInit();
        CreditFontLoad(20);
        sprite = dmalloc(0x2d0, "StgSpr");
        poly = dmalloc(0xe0, "StgPoly");
        film = dmalloc(0x28, "StgFilm");
        for (i = 0; i < 9; i++) {
            sprite[i].zm_x = 1.0f;
            sprite[i].zm_y = 1.0f;
            sprite[i].ang = 0;
            sprite[i].u0 = 0.0f;
            sprite[i].u1 = 1.0f;
            sprite[i].v0 = 0.0f;
            sprite[i].v1 = 1.0f;
            sprite[i].listType = 1;
            sprite[i].trnsl = 0.0f;
            sprite[i].attr = 13;
        }
        for (i = 0; i < 10; i++) {
            film[i].y = (float)i * 55.0;
        }
        for (i = 0; i < 12; i++) {
            poly[i].w = 640;
            poly[i].h = 2;
            poly[i].x = 0.0f;
        }

        poly[0].y = -37.0f;
        poly[1].y = -31.0f;
        poly[2].y = -24.0f;
        poly[3].y = -14.0f;
        poly[4].y = 0.0f;

        poly[5].y = 480.0f;
        poly[6].y = 494.0f;
        poly[7].y = 504.0f;
        poly[8].y = 511.0f;
        poly[9].y = 517.0f;

        poly[10].h = 25;
        poly[10].x = -640.0f;
        poly[10].y = offu;

        poly[11].h = 25;
        poly[11].x = 640.0f;
        poly[11].y = offl + 455;

        if (StageNo <= 0) {
            StageNo = 1;
        }
        stg = StageNo - 1;
        sprite[0].sprno = StageNo + 60;
        sprite[0].x = 320 - logowidth[stg];
        sprite[0].y = 272.0f;
        sprite[0].z = 13.0f;
        sprite[0].u0 = 0.0f;
        sprite[0].u1 = 1.0f;
        sprite[0].zm_x = 1.0f;

        sprite[5].sprno = 14;
        sprite[5].x = 0.0f;
        sprite[5].y = 50.0f;
        sprite[5].z = 21.0f;
        sprite[5].attr = 13;
        sprite[5].trnsl = 1.0f;
        sprite[5].v0 = 0.0625f;
        sprite[5].v1 = 1.0f;
        sprite[5].zm_y = 1.0f;
        sprite[5].u0 = 0.0f;
        sprite[5].u1 = 1.0f;
        sprite[5].zm_x = 1.0f;
        sprite[5].listType = 0;

        sprite[6].sprno = 14;
        sprite[6].x = 0.0f;
        sprite[6].y = 50.0f;
        sprite[6].z = 21.0f;
        sprite[6].attr = 13;
        sprite[6].trnsl = 1.0f;
        sprite[6].v0 = 0.0625f;
        sprite[6].v1 = 1.0f;
        sprite[6].zm_y = 1.0f;
        sprite[6].u0 = 0.0f;
        sprite[6].u1 = 1.0f;
        sprite[6].zm_x = 1.0f;
        sprite[6].listType = 0;

        sprite[7].sprno = 15;
        sprite[7].x = 0.0f;
        sprite[7].y = 50.0f;
        sprite[7].z = 21.0f;
        sprite[7].attr = 13;
        sprite[7].trnsl = 1.0f;
        sprite[7].v0 = 0.0f;
        sprite[7].v1 = 0.9375f;
        sprite[7].zm_y = 1.0f;
        sprite[7].u0 = 0.0f;
        sprite[7].u1 = 1.0f;
        sprite[7].zm_x = 1.0f;
        sprite[7].listType = 0;

        sprite[8].sprno = 15;
        sprite[8].x = 0.0f;
        sprite[8].y = 50.0f;
        sprite[8].z = 21.0f;
        sprite[8].attr = 13;
        sprite[8].trnsl = 1.0f;
        sprite[8].v0 = 0.0f;
        sprite[8].v1 = 0.9375f;
        sprite[8].zm_y = 1.0f;
        sprite[8].u0 = 0.0f;
        sprite[8].u1 = 1.0f;
        sprite[8].zm_x = 1.0f;
        sprite[8].listType = 0;

        sprite[5].x = -640.0f;
        sprite[7].x = 640.0f;
        sprite[5].y = 240.0f;
        sprite[7].y = 260.0f;
        SubJobCount = 0;
        wait = 0;
        rannyuuflag = 0;
        InitCancelFlag();
        SubJob++;
    } else if (SubJob == 2) {
        Read_Playdat(0x80);
        rannyuuflag |= IsRannyuu();
        if (subjob_func == talk_scene && IS_SKIP()) {
            StgSelFree_story();
        }
        DispCreditMsg();
        if (SubJobCount < 15) {
            sprite[5].x += 45.714285714285715;
            sprite[7].x -= 45.714285714285715;
        } else if (SubJobCount >= 15 && SubJobCount < 76) {
            sprite[5].x = 0.0f;
            sprite[7].x = 0.0f;
        } else if (SubJobCount >= 77 && SubJobCount < 91) {
            sprite[5].y -= 6.25;
            sprite[7].y += 6.25;
        } else if (SubJobCount >= 91) {
            sprite[5].y = 140.0f;
            sprite[7].y = 360.0f;
        }
        if (SubJobCount >= 21 && SubJobCount < 29) {
            for (i = 0; i < 5; i++) {
                poly[i].y += 10.875;
            }
            for (i = 0; i < 5; i++) {
                poly[i + 5].y -= 10.875;
            }
        } else if (SubJobCount >= 30) {
            poly[0].y = 50.0f;
            poly[1].y = 56.0f;
            poly[2].y = 63.0f;
            poly[3].y = 73.0f;
            poly[4].y = 87.0f;
            poly[5].y = 391.0f;
            poly[6].y = 405.0f;
            poly[7].y = 415.0f;
            poly[8].y = 422.0f;
            poly[9].y = 428.0f;
        }
        if (SubJobCount >= 45 && SubJobCount < 60) {
            sprite[0].trnsl += 0.0625;
        } else if (SubJobCount >= 61) {
            sprite[0].trnsl = 1.0f;
        }
        if (SubJobCount >= 31 && SubJobCount < 45) {
            poly[10].x += 45.714285714285715;
            poly[11].x -= 45.714285714285715;
        } else if (SubJobCount >= 46) {
            poly[10].x = 0.0f;
            poly[11].x = 0.0f;
        }
        color_w = gStripGetFadeColor(0xffffffff);
        gStripGetFadeColor(0xff000838);
        if (SubJobCount >= 20) {
            for (i = 0; i < 10; i++) {
                PFBOX(poly[i], poly[i].w, 19.0f, color_w);
            }
        }
        gSpriteDraw_NaomiLibVersion(&sprite[5]);
        gSpriteDraw_NaomiLibVersion(&sprite[7]);
        if (SubJobCount >= 21) {
            DrawBG(stg + 1, 0, 140, 27.0f);
        }
        polydraw_FBox(0.0f, 87.0f, 640.0f, sprite[5].y - 20.0f, 25.0f, 0xff000000);
        polydraw_FBox(0.0f, sprite[7].y - 10.0f, 640.0f, 391.0f, 25.0f, 0xff000000);
        SubJobCount++;
        if (SubJobCount >= 96) {
            SubJobCount = 0;
            SubJob++;
        }
    } else if (SubJob == 3) {
        if (SubJobCount == 0) {
            sprite[1].sprno = 18;
            sprite[1].x = 320.0f;
            sprite[1].y = 240.0f;
            sprite[1].z = 15.0f;
            sprite[1].attr = 0x20a;
            sprite[2].sprno = 19;
            sprite[2].x = 320.0f;
            sprite[2].y = 240.0f;
            sprite[2].z = 17.0f;
            sprite[2].attr = 0x20a;

            sprite[3].sprno = 59;
            sprite[3].x = 0.0f;
            sprite[3].y = 50.0f;
            sprite[3].z = 23.0f;
            sprite[3].attr = 5;
            sprite[3].trnsl = 1.0f;
            sprite[3].v0 = (sprite[5].y - 50.0f) / 191.0;
            sprite[3].zm_y = (sprite[5].y - 50.0f) / 191.0;
            sprite[3].listType = 0;
            sprite[4].sprno = 59;
            sprite[4].x = 0.0f;
            sprite[4].y = 428.0f;
            sprite[4].z = 23.0f;
            sprite[4].attr = 13;
            sprite[4].trnsl = 1.0f;
            sprite[4].v0 = (428.0f - sprite[7].y) / 191.0;
            sprite[4].zm_y = (428.0f - sprite[7].y) / 191.0;
            sprite[4].listType = 0;

            poly[12].w = 320;
            poly[13].w = 320;
            poly[12].h = 1;
            poly[13].h = 1;
            poly[12].x = 0.0f;
            poly[13].x = 320.0f;
            poly[12].y = 240.0f;
            poly[13].y = 240.0f;

            y3 = 550.0f;
            x5 = 20.0f;
            x6 = 507.0f;
            count = 0;
            sprite[6].x = 640.0f;
            sprite[6].y = sprite[5].y;
            sprite[8].y = sprite[7].y;
            sprite[8].x = -640.0f;
        }
        Read_Playdat(0x80);
        fms = SubJobCount + 96;
        rannyuuflag |= IsRannyuu();
        if (subjob_func == talk_scene && IS_SKIP()) {
            StgSelFree_story();
        }
        DispCreditMsg();
        sprite[5].x -= 2.909090909090909;
        sprite[6].x -= 2.909090909090909;
        sprite[7].x += 2.909090909090909;
        sprite[8].x += 2.909090909090909;
        if (sprite[5].x <= -640.0f) {
            sprite[5].x = 636.0f;
        }
        if (sprite[6].x <= -640.0f) {
            sprite[6].x = 636.0f;
        }
        if (sprite[7].x >= 640.0f) {
            sprite[7].x = -636.0f;
        }
        if (sprite[8].x >= 640.0f) {
            sprite[8].x = -636.0f;
        }
        sprite[6].y = sprite[5].y;
        sprite[8].y = sprite[7].y;
        sprite[1].trnsl += 0.03125;
        sprite[2].trnsl += 0.01875;
        sprite[1].ang += 60;
        sprite[2].ang += 60;
        if (fms & 1) {
            sprite[3].attr = sndtb[count & 3] | 5;
            sprite[4].attr = sndtb[count & 3] | 13;
            count++;
        }
        y3 -= 17.1875;
        if (fms >= 106) {
            poly[12].x -= 20.0;
            poly[13].x += 20.0;
        }
        color_w = gStripGetFadeColor(0xffffffff);
        gStripGetFadeColor(0xff000838);
        for (i = 0; i < 10; i++) {
            PFBOX(poly[i], poly[i].w, 19.0f, color_w);
        }
        gSpriteDraw_NaomiLibVersion(&sprite[5]);
        gSpriteDraw_NaomiLibVersion(&sprite[6]);
        gSpriteDraw_NaomiLibVersion(&sprite[7]);
        gSpriteDraw_NaomiLibVersion(&sprite[8]);
        DrawBG(stg + 1, 0, 140, 27.0f);
        gSpriteDraw_NaomiLibVersion(&sprite[3]);
        gSpriteDraw_NaomiLibVersion(&sprite[4]);
        for (i = 0; i < 10; i++) {
            if (i == 2 || i == 7) {
                id = 56;
            } else {
                id = 57;
            }
            if (i == 4 || i == 9) {
                id = 56;
            } else {
                id = 57;
            }
        }
        SubJobCount++;
        if (SubJobCount >= 32) {
            SubJobCount = 0;
            SubJob++;
        }
    } else if (SubJob == 4) {
        if (SubJobCount == 0) {
            sprite[0].trnsl = 1.0f;
            sprite[1].trnsl = 0.6f;
        }
        Read_Playdat(0x80);
        if (subjob_func == talk_scene && IS_SKIP()) {
            StgSelFree_story();
        }
        DispCreditMsg();
        sprite[5].x -= 2.909090909090909;
        sprite[6].x -= 2.909090909090909;
        sprite[7].x += 2.909090909090909;
        sprite[8].x += 2.909090909090909;
        sprite[6].y = sprite[5].y;
        sprite[8].y = sprite[7].y;
        if (sprite[5].x <= -640.0f) {
            sprite[5].x = 636.0f;
        }
        if (sprite[6].x <= -640.0f) {
            sprite[6].x = 636.0f;
        }
        if (sprite[7].x >= 640.0f) {
            sprite[7].x = -636.0f;
        }
        if (sprite[8].x >= 640.0f) {
            sprite[8].x = -636.0f;
        }
        sprite[1].ang += 60;
        sprite[2].ang += 60;
        if (SubJobCount & 1) {
            sprite[3].v0 = (191.0 - (sprite[5].y - 50.0) + 65.0 + (float)(SubJobCount & 3)) / 256.0;
            sprite[3].zm_y = (sprite[5].y - 50.0) / 256.0;
            sprite[4].v0 = (191.0 - (428.0 - sprite[7].y) + 65.0 + (float)(SubJobCount & 3)) / 256.0;
            sprite[4].zm_y = (428.0 - sprite[7].y) / 191.0;
            sprite[4].attr = sndtb[count & 3] | 13;
            sprite[3].attr = sndtb[count & 3] | 5;
            count++;
        }
        for (i = 0; i < 10; i++) {
            film[i].y += 6.875;
            if (film[i].y >= 550.0f) {
                film[i].y = 0.0f;
            }
        }
        if (stg != stgold) {
            stgold = stg;
        }
        rannyuuflag |= IsRannyuu();
        color_w = gStripGetFadeColor(0xffffffff);
        gStripGetFadeColor(0xff000838);
        for (i = 0; i < 10; i++) {
            PFBOX(poly[i], poly[i].w, 19.0f, color_w);
        }
        gSpriteDraw_NaomiLibVersion(&sprite[5]);
        gSpriteDraw_NaomiLibVersion(&sprite[6]);
        gSpriteDraw_NaomiLibVersion(&sprite[7]);
        gSpriteDraw_NaomiLibVersion(&sprite[8]);
        DrawBG(stg + 1, 0, 140, 27.0f);
        gSpriteDraw_NaomiLibVersion(&sprite[3]);
        gSpriteDraw_NaomiLibVersion(&sprite[4]);
        for (i = 0; i < 10; i++) {
            if (i == 2 || i == 7) {
                id = 56;
            } else {
                id = 57;
            }
            if (i == 4 || i == 9) {
                id = 56;
            } else {
                id = 57;
            }
        }
        if (SubJobCount < 280) {
            strloop(SubJobCount);
        }
        SubJobCount++;
        if (bg_motion) {
            bg_motion--;
        }
    } else if (SubJob == 5) {
        if (SubJobCount == 0) {
            stgold = stg;
            sprite[5].x = 0.0f;
            sprite[7].x = 0.0f;
            SubJobCount = 276;
        }
        Read_Playdat(0x80);
        rannyuuflag |= IsRannyuu();
        if (subjob_func == talk_scene && IS_SKIP()) {
            StgSelFree_story();
        }
        if (SubJobCount & 1) {
            sprite[4].attr = sndtb[count & 3] | 13;
            sprite[3].attr = sndtb[count & 3] | 5;
            count++;
        }
        sprite[3].v0 = (191.0 - (sprite[5].y - 50.0)) / 191.0;
        sprite[3].zm_y = (sprite[5].y - 50.0) / 191.0;
        sprite[4].v0 = (191.0 - (428.0 - sprite[7].y)) / 191.0;
        sprite[4].zm_y = (428.0 - (sprite[7].y - 10.0)) / 191.0;
        if (SubJobCount < 280) {
            x5 -= 28.0;
            x6 += 28.0;
        } else if (SubJobCount >= 281) {
            x5 = -120.0f;
            x6 = 641.0f;
        }
        if (SubJobCount < 288) {
            sprite[5].y += 8.333333333333334;
            sprite[7].y -= 8.333333333333334;
        } else if (SubJobCount >= 289 && SubJobCount < 296) {
            sprite[5].y = 240.0f;
            sprite[7].y = 260.0f;
        } else if (SubJobCount >= 297) {
            sprite[5].y -= 40.0;
            sprite[7].y += 40.0;
        }
        color_w = gStripGetFadeColor(0xffffffff);
        gStripGetFadeColor(0xff000838);
        if (SubJobCount >= 20) {
            for (i = 0; i < 10; i++) {
                PFBOX(poly[i], poly[i].w, 19.0f, color_w);
            }
        }
        gSpriteDraw_NaomiLibVersion(&sprite[5]);
        gSpriteDraw_NaomiLibVersion(&sprite[7]);
        DrawBG(stg + 1, 0, 140, 27.0f);
        gSpriteDraw_NaomiLibVersion(&sprite[3]);
        gSpriteDraw_NaomiLibVersion(&sprite[4]);
        for (i = 0; i < 10; i++) {
            if (i == 2 || i == 7) {
                id = 56;
            } else {
                id = 57;
            }
            if (i == 4 || i == 9) {
                id = 56;
            } else {
                id = 57;
            }
        }
        if (SubJobCount >= 297) {
            polydraw_FBox(0.0f, sprite[5].y, 640.0f, sprite[7].y - 20.0f, 3.0f, 0xff000000);
        }
        SubJobCount++;
        if (rannyuuflag && SubJobCount >= 308) {
            SubJobCount = 307;
        }
        if (SubJobCount >= 308) {
            SubJob++;
            SubJobCount = 0;
        }
    } else if (SubJob == 6) {
        if (SubJobCount == 0) {
            if (IsCancelFlag()) {
                goto cancel;
            }
            SetFadeSpeed(20);
            Rq_Fadein();
            SubJobCount++;
        }
        if (!IsFadeEnd()) {
            Read_Playdat(0x80);
            EasySpriteDrawEX(16, 240, 252, 7.0f, 0x20d, 1.0f);
            DispCreditMsg();
            return;
        }
        if (SubJobCount < 21) {
            Read_Playdat(0x80);
            EasySpriteDrawEX(16, 240, 252, 7.0f, 0x20d, 1.0f);
            DispCreditMsg();
            SubJobCount++;
        } else {
            EasySpriteDrawEX(16, 240, 252, 7.0f, 0x20d, 1.0f);
            SetFadeSpeed(20);
            Rq_Fadeout();
            SubJob++;
            SubJobCount = 0;
        }
    } else {
        if (!IsFadeEnd()) {
            Read_Playdat(0x80);
            EasySpriteDrawEX(16, 240, 252, 7.0f, 0x20d, 1.0f);
            DispCreditMsg();
            return;
        }
    cancel:
        StgSelFree_story();
        dfree(sprite);
        dfree(poly);
        dfree(film);
        sprite = 0;
        poly = 0;
        film = 0;
        return;
    }
    if (rannyuuflag) {
        EnterFight_Main();
    }
}
