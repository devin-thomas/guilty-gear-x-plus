/* ggx-pad-test: the pad shim's bytes against the PS2's (PORT-010).
 *
 *   ggx-pad-test [--scripts <dir>] <trace.ggxt[.zst]>...
 *
 * For every scePadRead and scePadGetReqState the game made on the PS2 in
 * each Lockstep Trace, it checks what common/input/pad.c would deliver in
 * that field: a successful read gives the PS2's eight bytes for the same
 * buttons (pad_encode) and returns 32; failed reads, busy request states
 * and scePadSetActDirect's returns fall where the actuator alignment
 * requests the game made put them (pad.h, PadRequest). Both ports make the
 * same calls in every trace, so one request model serves both. No disc is
 * needed: the traces hold the calls.
 *
 * The macros (PORT-018, macro.h): each presses the buttons the game's KEY
 * CONFIG has for its parts, and the bytes are those of a pad holding them.
 *
 * Which button confirms (PORT-019, confirm.h): X and Circle swap only where
 * asked, and a press keeps its meaning until released.
 *
 * Then GGScript: every script in --scripts (the corpus's) parses, and a
 * short script plays as the gg-layers fork plays it (facing, letters, PS2
 * names, map, a second pad, sticks, quit).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "confirm.h"
#include "ggscript.h"
#include "macro.h"
#include "pad.h"
#include "trace.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

static int failures;

#define CHECK(cond, ...)                          \
    do {                                          \
        if (!(cond)) {                            \
            if (failures++ < 20) {                \
                fprintf(stderr, "FAIL: " __VA_ARGS__); \
                fputc('\n', stderr);              \
            }                                     \
        }                                         \
    } while (0)

static void check_trace(const char *path)
{
    char err[512];
    Trace *t = trace_open(path, err, sizeof(err));
    static TraceCall calls[65536];
    uint32_t f, reads = 0, failed = 0, busy = 0, requests = 0, v = 0;
    PadRequest req = { 0, 0 };

    if (!t) {
        CHECK(0, "%s", err);
        return;
    }
    for (f = 0; f < trace_fields(t); f++) {
        uint32_t n = trace_field_calls(t, f, calls, 65536), i, r;
        int acted = 0;

        /* v: the vblank count while the field runs, before its own vblanks (as the Core's sceGsSyncV count) */

        for (i = 0; i < n && i < 65536; i++) {
            const TraceCall *c = &calls[i];

            for (r = 0; r < c->rep; r++) {
                if (c->env == TRACE_ENV_scePadRead) {
                    const uint8_t *ps2 = c->outs + 1;   /* the output's length byte, then the bytes */
                    uint8_t mine[32];
                    PadInput in;

                    if (c->ret == 0) {
                        failed++;
                        CHECK(!pad_read_ok(&req, f, v), "%s field %u: the PS2's read failed, the shim's would not", path, f);
                        continue;
                    }
                    reads++;
                    CHECK(pad_read_ok(&req, f, v), "%s field %u: the PS2's read worked, the shim's would fail", path, f);
                    CHECK(c->nout == 1 && c->outs[0] == 8, "%s field %u: not an 8-byte read", path, f);
                    pad_clear(&in);
                    in.buttons = (uint16_t)~(ps2[2] << 8 | ps2[3]);
                    CHECK(pad_encode(&in, mine) == c->ret, "%s field %u: returns %u, the PS2 %u", path, f, 32u, c->ret);
                    CHECK(!memcmp(mine, ps2, 8),
                          "%s field %u: %02x%02x%02x%02x%02x%02x%02x%02x, the PS2 %02x%02x%02x%02x%02x%02x%02x%02x",
                          path, f, mine[0], mine[1], mine[2], mine[3], mine[4], mine[5], mine[6], mine[7], ps2[0], ps2[1],
                          ps2[2], ps2[3], ps2[4], ps2[5], ps2[6], ps2[7]);
                } else if (c->env == TRACE_ENV_scePadGetReqState) {
                    busy += c->ret != 0;
                    CHECK(c->ret == pad_req_state(&req, v), "%s field %u: scePadGetReqState %u, the shim's %u", path, f,
                          c->ret, pad_req_state(&req, v));
                } else if (c->env == TRACE_ENV_scePadSetActDirect) {
                    /* field 1, before any request, returns 1 on KR and 0 on JP: what libpad held, not modelled */
                    CHECK(f == 1 || c->ret == pad_act_direct(&req, v), "%s field %u: scePadSetActDirect %u, the shim's %u",
                          path, f, c->ret, pad_act_direct(&req, v));
                    acted = 1;
                } else if (c->env == TRACE_ENV_scePadSetActAlign) {
                    pad_request(&req, v, acted);
                    requests++;
                }
            }
        }
        v += trace_field_vblanks(t, f);
    }
    printf("%s: %u reads as the PS2's; %u alignment requests, %u reads failed and %u busy states where they put "
           "them\n", path, reads, requests, failed, busy);
    trace_free(t);
}

static void check_scripts(const char *dir)
{
    int n = 0;
#ifdef _WIN32
    WIN32_FIND_DATAA fd;
    char pat[1024];
    HANDLE h;

    snprintf(pat, sizeof(pat), "%s\\*.txt", dir);
    h = FindFirstFileA(pat, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        CHECK(0, "no scripts in %s", dir);
        return;
    }
    do {
        const char *name = fd.cFileName;
#else
    DIR *d = opendir(dir);
    struct dirent *e;

    if (!d) {
        CHECK(0, "no scripts in %s", dir);
        return;
    }
    while ((e = readdir(d)) != NULL) {
        const char *name = e->d_name;
        size_t len = strlen(name);

        if (len < 5 || strcmp(name + len - 4, ".txt")) {
            continue;
        }
#endif
        {
            char path[1024], err[512];
            GgScript *s;

            snprintf(path, sizeof(path), "%s/%s", dir, name);
            s = ggs_load(path, err, sizeof(err));
            CHECK(s != NULL, "%s", err);
            ggs_free(s);
            n++;
        }
#ifdef _WIN32
    } while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    }
    closedir(d);
#endif
    printf("%s: %d scripts parse\n", dir, n);
}

/* what the fork's GGScript.cpp makes of a script, frame by frame */
static void check_semantics(void)
{
    static const char *text =
        "# comment\n"
        "2 6P\n"                      /* 1-2: forward + Square, facing right */
        "face left\n"
        "1 3K   # trailing comment\n" /* 3: down-forward + Cross, facing left */
        "wait 1\n"                    /* 4 */
        "map D=L2\n"
        "1 Down+Start 2D\n"           /* 5: PS2 names; pad 2: down and D, which the map made L2 */
        "stick 0 255 127 127\n"
        "1 H\n"                       /* 6: Circle, stick moved */
        "quit\n"
        "1 P\n";                      /* never reached */
    static const struct {
        uint16_t p1, p2;
        int st;
    } WANT[] = {
        { PAD_RIGHT | PAD_SQUARE, 0, GGS_PLAYING },
        { PAD_RIGHT | PAD_SQUARE, 0, GGS_PLAYING },
        { PAD_DOWN | PAD_LEFT | PAD_CROSS, 0, GGS_PLAYING },
        { 0, 0, GGS_PLAYING },
        { PAD_DOWN | PAD_START, PAD_DOWN | PAD_L2, GGS_PLAYING },
        { PAD_CIRCLE, 0, GGS_PLAYING },
        { 0, 0, GGS_QUIT },
    };
    char err[256];
    GgScript *s = ggs_parse(text, "semantics", err, sizeof(err));
    size_t i;

    CHECK(s != NULL, "%s", err);
    if (!s) {
        return;
    }
    for (i = 0; i < sizeof(WANT) / sizeof(WANT[0]); i++) {
        uint16_t b[2];
        uint8_t stick[4];
        int st = ggs_frame(s, b, stick);

        CHECK(st == WANT[i].st && b[0] == WANT[i].p1 && b[1] == WANT[i].p2,
              "script frame %zu: %d %04x %04x, want %d %04x %04x", i + 1, st, b[0], b[1], WANT[i].st, WANT[i].p1,
              WANT[i].p2);
        if (i == 5) {
            CHECK(stick[0] == 0 && stick[1] == 255 && stick[2] == 127, "stick %u %u %u", stick[0], stick[1], stick[2]);
        }
    }
    ggs_free(s);
    CHECK(ggs_parse("1 6X\n", "bad", err, sizeof(err)) == NULL, "a bad letter parsed");
    CHECK(ggs_parse("1 Up+Nope\n", "bad", err, sizeof(err)) == NULL, "a bad name parsed");
    CHECK(pad_button_by_name("cross") == PAD_CROSS && pad_button_by_name("R3") == PAD_R3, "button names");
    {
        PadInput in;
        uint8_t out[32];

        pad_clear(&in);
        in.mode = PAD_MODE_ANALOG;
        in.buttons = PAD_UP | PAD_CROSS;
        in.analog[2] = 0x10;
        CHECK(pad_encode(&in, out) == 32 && out[1] == 0x73 && out[2] == 0xEF && out[3] == 0xBF && out[6] == 0x10 &&
                  out[7] == 0x80 && out[8] == 0,
              "analog encoding");
    }
    printf("GGScript semantics and the analog encoding checked\n");
}

/* A pad holding buttons, as scePadRead's bytes 2-3 (active low). */
static unsigned held_bytes(uint16_t buttons)
{
    PadInput p;
    uint8_t b[32];

    pad_clear(&p);
    p.buttons = buttons;
    pad_encode(&p, b);
    return (unsigned)b[2] << 8 | b[3];
}

static void check_macros(void)
{
    MacroKeys k;
    static const struct {
        int m;
        uint16_t want;
    } DEF[] = {
        { MACRO_P_AND_K, PAD_SQUARE | PAD_CROSS },
        { MACRO_S_AND_HS, PAD_TRIANGLE | PAD_CIRCLE },
        { MACRO_P_K_S, PAD_SQUARE | PAD_CROSS | PAD_TRIANGLE },
        { MACRO_P_K_S_HS, PAD_SQUARE | PAD_CROSS | PAD_TRIANGLE | PAD_CIRCLE },
    };
    size_t i;

    /* the game's defaults: square P, cross K, triangle S, circle HS */
    macro_default_keys(&k);
    for (i = 0; i < sizeof(DEF) / sizeof(DEF[0]); i++) {
        uint16_t b = macro_buttons(DEF[i].m, &k);

        CHECK(b == DEF[i].want, "macro %s: %04x, not %04x", macro_name(DEF[i].m), b, DEF[i].want);
        /* bytes 2-3 as libpad sends them: active low, every part down at once */
        CHECK(held_bytes(b) == (~(unsigned)DEF[i].want & 0xFFFF), "macro %s: bytes %04x", macro_name(DEF[i].m),
              held_bytes(b));
    }
    /* KEY CONFIG changed in the game: P on L2, HS on R1. The macros follow it. */
    k.punch = PAD_L2;
    k.hslash = PAD_R1;
    CHECK(macro_buttons(MACRO_P_AND_K, &k) == (PAD_L2 | PAD_CROSS), "P AND K with P on L2: %04x",
          macro_buttons(MACRO_P_AND_K, &k));
    CHECK(macro_buttons(MACRO_P_K_S_HS, &k) == (PAD_L2 | PAD_CROSS | PAD_TRIANGLE | PAD_R1), "P K S HS: %04x",
          macro_buttons(MACRO_P_K_S_HS, &k));
    /* not one of the eight buttons (the game hasn't loaded its settings, or two bits): the default */
    k.punch = 0;
    k.kick = PAD_CROSS | PAD_CIRCLE;
    k.slash = PAD_START;
    CHECK(macro_buttons(MACRO_P_K_S, &k) == (PAD_SQUARE | PAD_CROSS | PAD_TRIANGLE), "fallback: %04x",
          macro_buttons(MACRO_P_K_S, &k));
    CHECK(macro_by_config_name("s_and_hs") == MACRO_S_AND_HS && macro_by_config_name("p_k") < 0, "config names");
    CHECK(!strcmp(macro_name(MACRO_P_K_S_HS), "P K S HS"), "names as KEY CONFIG writes them");
}

/* PORT-019: X confirms in menus; a press keeps its meaning until released */
static void check_confirm(void)
{
    ConfirmLatch l;

    confirm_reset(&l);
    CHECK(confirm_parse("X") == CONFIRM_X && confirm_parse("circle") == CONFIRM_CIRCLE && confirm_parse("cross") < 0,
          "confirm_parse");
    /* not in a menu: as pressed */
    CHECK(confirm_apply(&l, PAD_CROSS | PAD_LEFT, 0) == (PAD_CROSS | PAD_LEFT), "fight: X stays X");
    CHECK(confirm_apply(&l, 0, 0) == 0, "released");
    /* in a menu: X is Circle and Circle is X, both at once too; the rest unchanged */
    CHECK(confirm_apply(&l, PAD_CROSS, 1) == PAD_CIRCLE, "menu: X confirms");
    CHECK(confirm_apply(&l, PAD_CROSS | PAD_CIRCLE | PAD_START, 1) == (PAD_CROSS | PAD_CIRCLE | PAD_START), "both held");
    confirm_apply(&l, 0, 1);
    CHECK(confirm_apply(&l, PAD_CIRCLE | PAD_SQUARE, 1) == (PAD_CROSS | PAD_SQUARE), "menu: Circle cancels");
    confirm_apply(&l, 0, 1);
    /* X pressed in the pause menu and still held as the fight resumes: still Circle until let go, then X kicks */
    CHECK(confirm_apply(&l, PAD_CROSS, 1) == PAD_CIRCLE, "pressed in the menu");
    CHECK(confirm_apply(&l, PAD_CROSS, 0) == PAD_CIRCLE, "held into the fight");
    CHECK(confirm_apply(&l, 0, 0) == 0, "let go");
    CHECK(confirm_apply(&l, PAD_CROSS, 0) == PAD_CROSS, "pressed again in the fight");
    /* and the other way: held from a fight into a menu stays X */
    CHECK(confirm_apply(&l, PAD_CROSS, 1) == PAD_CROSS, "held into a menu");
    /* the bytes a menu press gives: Circle's bit down (active low), X's up */
    {
        PadInput p;
        uint8_t b[32];

        confirm_reset(&l);
        pad_clear(&p);
        p.buttons = confirm_apply(&l, PAD_CROSS, 1);
        pad_encode(&p, b);
        CHECK(((unsigned)b[2] << 8 | b[3]) == (~(unsigned)PAD_CIRCLE & 0xFFFF), "menu X bytes %02x%02x", b[2], b[3]);
    }
}

int main(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--scripts") && i + 1 < argc) {
            check_scripts(argv[++i]);
        } else {
            check_trace(argv[i]);
        }
    }
    check_semantics();
    check_macros();
    check_confirm();
    if (failures) {
        fprintf(stderr, "ggx-pad-test: %d failures\n", failures);
        return 1;
    }
    printf("ggx-pad-test: ok\n");
    return 0;
}
