/* ggx-solve: finds inputs that play a mode to its end, and writes them as a
 * GGScript (PORT-010). A developer tool: the scripts it writes are what the
 * completion runs replay, through the same pad path
 * as a player's controller.
 *
 *   ggx-solve --symbols common/core/symbols/kr.txt --prefix <menus.txt> --until <job>[,<job>...]
 *             --out <script.txt> [--data-dir <dir>] [--card1 none] [--fields N]
 *             [--p2 idle|mash:SEED] [--fights N] [--advance <button>] [--horizon N] [--title <text>]
 *             [--fight-fields N --suffix <script>] [--stuck-fields N --stuck-button <button>]
 *
 * It boots the Core, plays the prefix script (the menus: mode, character),
 * then plays on until each job of --until has run in turn (ggx-headless's
 * --until):
 *
 * - In a fight (GameMain with PlayerWork allocated), pad 1 is searched. Every
 *   decision tries each move of a fixed set (walks, jumps, normals, the
 *   motions of specials, a throw) from a snapshot of the Core, follows it
 *   with --horizon fields of blocking, and keeps the move that leaves the
 *   most damage dealt for the least taken (core_save / core_load: PORT-004).
 *   After --fights fights pad 1 stops fighting (Survival: the run ends when
 *   it loses).
 * - With --suffix, once pad 1 has fought --fight-fields fields, the suffix
 *   script plays from where the game is (Training: Start, the pause menu's
 *   last item, Circle), and the search carries on after it.
 * - Anywhere else pad 1 taps --advance (default Circle) every 20 fields:
 *   win screens, story text, VS screens. A screen that has kept the same job
 *   for --stuck-fields gets --stuck-button instead (Story's "THE END" card
 *   waits for Start).
 * - Pad 2 is idle, or (--p2 mash:SEED) mashes in fights as a function of the
 *   seed and the field, like mash.h, so a search sees the same pad 2 as the
 *   final run.
 *
 * --report plays the prefix (a whole script) and nothing else, reporting
 * each job change, fight and round as the solver does: what a completion
 * run did, from its script.
 *
 * Every field's pads go into the output, one script frame per vblank, so
 * `ggx-headless --script <out> --until <same>` replays the same game and
 * reaches the same jobs. The Core is deterministic, so that replay is the
 * same on every host.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core_host.h"
#include "core_state.h"
#include "fsys.h"
#include "ggscript.h"
#include "mash.h"

#define CW_SIZE 0x11c          /* CHARACTER_WORK */
#define CW_HP 0x16
#define CW_PADID 0x1f
#define CW_POSX 0xc8
#define MAX_FRAMES 4000000u

typedef struct {
    uint32_t PlayerWork, g_info;
} Syms;

static Syms syms;

/* the frames written so far: both pads, one entry per vblank */
static uint32_t *frames;
static uint32_t nframes;

static uint32_t sym(const char *path, const char *name)
{
    FILE *f = fs_open(path, "rb");
    char line[256];
    uint32_t a = 0;

    while (f && fgets(line, sizeof(line), f)) {
        char n[128];
        unsigned v;

        if (sscanf(line, "%127s 0x%x", n, &v) == 2 && !strcmp(n, name)) {
            a = v;
            break;
        }
    }
    if (f) {
        fclose(f);
    }
    if (!a) {
        fprintf(stderr, "ggx-solve: %s is not in %s\n", name, path);
        exit(1);
    }
    return a;
}

static const char *job_name(CoreHost *h)
{
    uint32_t job = ee_r32(h, ggx_sym_JobMode);

    return ggx_func_name(ee_r32(h, ggx_sym_jobfunc + 4 * job));
}

static int16_t hp(CoreHost *h, uint32_t pw, int side)
{
    return (int16_t)ee_r16(h, pw + side * CW_SIZE + CW_HP);
}

static int32_t posx(CoreHost *h, uint32_t pw, int side)
{
    return (int32_t)ee_r32(h, pw + side * CW_SIZE + CW_POSX);
}

/* one field, pads as given; 0 or the Core's trap */
static int step(CoreHost *h, uint16_t p1, uint16_t p2)
{
    h->pads[0].buttons = p1;
    h->pads[1].buttons = p2;
    return core_host_field(h);
}

/* the field's pads into the output: every vblank up to the one it starts in */
static void record(CoreHost *h, uint16_t p1, uint16_t p2)
{
    uint32_t to = h->vsyncs + 1;

    if (to > MAX_FRAMES) {
        fprintf(stderr, "ggx-solve: too many frames\n");
        exit(1);
    }
    while (nframes < to) {
        frames[nframes++] = p1 | (uint32_t)p2 << 16;
    }
}

/* ------------------------------------------------------------------ moves */

/* A move: up to 16 fields of pad 1, directions relative to the opponent
   ('6' toward). Digits are numpad directions; letters are P K S H D held
   with them (the game's default layout: Square X Triangle Circle R1). */
typedef struct {
    const char *name;
    const char *steps[16];   /* "<fields> <input>" */
} Move;

static const Move MOVES[] = {
    { "walk", { "8 6" } },
    { "back", { "8 4" } },
    { "block-low", { "8 1" } },
    { "dash", { "1 6", "1 5", "6 6" } },
    { "jump", { "2 9", "12 5" } },
    { "jump-K", { "2 9", "6 5", "2 5K", "6 5" } },
    { "jump-S", { "2 9", "6 5", "2 5S", "6 5" } },
    { "jump-H", { "2 9", "8 5", "2 5H", "4 5" } },
    { "5P", { "2 5P", "4 5" } },
    { "5K", { "2 5K", "4 5" } },
    { "5S", { "2 5S", "6 5" } },
    { "5H", { "2 5H", "8 5" } },
    { "2P", { "2 2P", "4 2" } },
    { "2K", { "2 2K", "4 2" } },
    { "2S", { "2 2S", "6 2" } },
    { "2H", { "2 2H", "8 2" } },
    { "2D", { "2 2D", "8 2" } },
    { "6P", { "2 6P", "6 5" } },
    { "6H", { "2 6H", "6 5" } },
    { "5D", { "2 5D", "8 5" } },
    { "236P", { "2 2", "2 3", "2 6P", "6 5" } },
    { "236K", { "2 2", "2 3", "2 6K", "6 5" } },
    { "236S", { "2 2", "2 3", "2 6S", "6 5" } },
    { "236H", { "2 2", "2 3", "2 6H", "6 5" } },
    { "214P", { "2 2", "2 1", "2 4P", "6 5" } },
    { "214K", { "2 2", "2 1", "2 4K", "6 5" } },
    { "214S", { "2 2", "2 1", "2 4S", "6 5" } },
    { "214H", { "2 2", "2 1", "2 4H", "6 5" } },
    { "623P", { "2 6", "2 2", "2 3P", "6 5" } },
    { "623S", { "2 6", "2 2", "2 3S", "6 5" } },
    { "623H", { "2 6", "2 2", "2 3H", "6 5" } },
    { "41236H", { "2 4", "2 1", "2 2", "2 3", "2 6H", "6 5" } },
    { "63214S", { "2 6", "2 3", "2 2", "2 1", "2 4S", "6 5" } },
};
#define NMOVES (int)(sizeof(MOVES) / sizeof(MOVES[0]))

/* pad bits for a numpad digit and letters, toward = +1 if the opponent is
   to the right */
static uint16_t input_bits(const char *in, int toward)
{
    uint16_t b = 0;
    int fwd = toward > 0 ? PAD_RIGHT : PAD_LEFT, back = toward > 0 ? PAD_LEFT : PAD_RIGHT;

    for (; *in; in++) {
        switch (*in) {
        case '1': b |= PAD_DOWN | back; break;
        case '2': b |= PAD_DOWN; break;
        case '3': b |= PAD_DOWN | fwd; break;
        case '4': b |= back; break;
        case '6': b |= fwd; break;
        case '7': b |= PAD_UP | back; break;
        case '8': b |= PAD_UP; break;
        case '9': b |= PAD_UP | fwd; break;
        case 'P': b |= PAD_SQUARE; break;
        case 'K': b |= PAD_CROSS; break;
        case 'S': b |= PAD_TRIANGLE; break;
        case 'H': b |= PAD_CIRCLE; break;
        case 'D': b |= PAD_R1; break;
        default: break;
        }
    }
    return b;
}

/* a move as pad 1's state per field; returns the field count */
static int expand(const Move *m, int toward, uint16_t *out, int cap)
{
    int n = 0, i;

    for (i = 0; i < 16 && m->steps[i]; i++) {
        int k = atoi(m->steps[i]);
        const char *in = strchr(m->steps[i], ' ') + 1;
        uint16_t b = input_bits(in, toward);

        while (k-- > 0 && n < cap) {
            out[n++] = b;
        }
    }
    return n;
}

/* ------------------------------------------------------------------ the search */

typedef struct {
    int me;               /* pad 1's side */
    int p2_mash;
    uint64_t p2_seed;
    int horizon;
} Fight;

static uint16_t p2_pad(const Fight *ft, CoreHost *h, int in_fight)
{
    PadInput pads[2];

    if (!ft->p2_mash || !in_fight) {
        return 0;
    }
    mash_input(ft->p2_seed, h->field, pads);
    return pads[1].buttons & (uint16_t)~(PAD_START | PAD_SELECT);
}

/* damage dealt minus taken, weighted, after a move and the horizon */
static long score(CoreHost *h, uint32_t pw, int me, int16_t my0, int16_t op0)
{
    int16_t my = hp(h, pw, me), op = hp(h, pw, me ^ 1);
    long s = (long)(op0 - op) * 10 - (long)(my0 - my) * 13;

    if (op <= 0 && op0 > 0) {
        s += 100000;
    }
    if (my <= 0 && my0 > 0) {
        s -= 100000;
    }
    return s;
}

static int fight_decision(CoreHost *h, CoreSnapshot *snap, const Fight *ft, uint16_t *best, int cap)
{
    uint32_t pw = ee_r32(h, syms.PlayerWork);
    int toward = posx(h, pw, ft->me ^ 1) >= posx(h, pw, ft->me) ? 1 : -1;
    int16_t my0 = hp(h, pw, ft->me), op0 = hp(h, pw, ft->me ^ 1);
    long best_score = -0x7fffffffL;
    int best_n = 0, m;
    uint16_t seq[256];

    if (core_save(h, snap) != 0) {
        fprintf(stderr, "ggx-solve: save failed\n");
        exit(2);
    }
    for (m = 0; m < NMOVES; m++) {
        int n = expand(&MOVES[m], toward, seq, 256), i;
        uint16_t guard = input_bits("1", toward);   /* crouching guard while the move plays out */
        long s;

        if (m > 0 && core_load(h, snap) != 0) {
            fprintf(stderr, "ggx-solve: load failed\n");
            exit(2);
        }
        for (i = 0; i < n + ft->horizon; i++) {
            uint32_t pwn;

            if (step(h, i < n ? seq[i] : guard, p2_pad(ft, h, 1)) != 0) {
                exit(2);
            }
            pwn = ee_r32(h, syms.PlayerWork);
            if (pwn != pw) {
                break;   /* the fight ended */
            }
        }
        s = score(h, pw, ft->me, my0, op0);
        /* ties: the earlier (simpler) move */
        if (s > best_score) {
            best_score = s;
            best_n = n < cap ? n : cap;
            memcpy(best, seq, sizeof(uint16_t) * (size_t)best_n);
        }
    }
    if (core_load(h, snap) != 0) {
        fprintf(stderr, "ggx-solve: load failed\n");
        exit(2);
    }
    return best_n;
}

/* --report: what a script did, field by field: job changes, fights, rounds */
typedef struct {
    uint32_t job, pw;
    int16_t hp[2];
    int me, fights, won, lost, rounds_won, rounds_lost;
} Report;

static void observe(CoreHost *h, Report *r)
{
    uint32_t job = ee_r32(h, ggx_sym_JobMode), pw = ee_r32(h, syms.PlayerWork);
    int fighting = pw && !strcmp(job_name(h), "GameMain");

    if (job != r->job) {
        fprintf(stderr, "ggx-solve: field %u: job %u %s\n", h->field, job, job_name(h));
        r->job = job;
    }
    if (fighting && !r->pw) {
        r->pw = pw;
        r->me = ee_r32(h, pw + CW_PADID) == 0 ? 0 : 1;
        r->hp[0] = hp(h, pw, 0), r->hp[1] = hp(h, pw, 1);
        r->rounds_won = r->rounds_lost = 0;
        fprintf(stderr, "ggx-solve: field %u: fight %d, pad 1 is side %d\n", h->field, r->fights + 1, r->me);
    } else if (!fighting && r->pw) {
        int winner = (int8_t)h->mem[syms.g_info + 6];

        r->pw = 0;
        r->fights++;
        r->won += winner == r->me;
        r->lost += winner != r->me;
        fprintf(stderr, "ggx-solve: field %u: fight %d over, winner side %d; rounds won %d, lost %d\n", h->field,
                r->fights, winner, r->rounds_won, r->rounds_lost);
    }
    if (fighting && r->pw == pw) {
        int16_t a = hp(h, pw, r->me), b = hp(h, pw, r->me ^ 1);

        if (r->hp[0 ^ r->me] > 0 && a <= 0) {
            r->rounds_lost++;
            fprintf(stderr, "ggx-solve: field %u: round lost (%d left)\n", h->field, b);
        } else if (r->hp[1 ^ r->me] > 0 && b <= 0) {
            r->rounds_won++;
            fprintf(stderr, "ggx-solve: field %u: round won (%d left)\n", h->field, a);
        }
        r->hp[r->me] = a, r->hp[r->me ^ 1] = b;
    }
}

/* ------------------------------------------------------------------ output */

static void put_input(FILE *f, uint16_t b)
{
    int i, first = 1;

    if (!b) {
        fputc('5', f);   /* hold nothing: GGScript's numpad neutral */
        return;
    }
    for (i = 0; i < PAD_NBUTTONS; i++) {
        if (b & (1u << i)) {
            fprintf(f, "%s%s", first ? "" : "+", pad_button_name(i));
            first = 0;
        }
    }
}

static int write_script(const char *path, const char *title, const char *prefix, const char *args)
{
    FILE *f = fs_open(path, "wb");
    uint32_t i = 0;

    if (!f) {
        return -1;
    }
    fprintf(f, "# %s\n# generated by ggx-solve (platforms/common/host/solve_main.c) from %s\n# %s\n", title, prefix,
            args);
    fprintf(f, "# every frame from the boot; PS2 button names, so facing doesn't matter\n");
    while (i < nframes) {
        uint32_t v = frames[i], n = 1;

        while (i + n < nframes && frames[i + n] == v) {
            n++;
        }
        if (!v) {
            fprintf(f, "wait %u\n", n);
        } else {
            fprintf(f, "%u ", n);
            put_input(f, (uint16_t)v);
            if (v >> 16) {
                fputc(' ', f);
                put_input(f, (uint16_t)(v >> 16));
            }
            fputc('\n', f);
        }
        i += n;
    }
    fprintf(f, "quit\n");
    return fclose(f);
}

static void usage(void)
{
    fprintf(stderr, "usage: ggx-solve --symbols <map> --prefix <script> --until <job>[,<job>...] --out <script>\n"
                    "       ggx-solve --symbols <map> --prefix <script> --report [--data-dir <dir>] [--card1 none]\n"
                    "                 [--data-dir <dir>] [--card1 empty|none] [--fields N] [--p2 idle|mash:SEED]\n"
                    "                 [--fights N] [--advance <button>] [--horizon N] [--title <text>]\n"
                    "                 [--fight-fields N --suffix <script>] [--stuck-fields N --stuck-button <button>]\n");
    exit(64);
}

int main(int argc, char **argv)
{
    CoreHostConfig cfg = { 0 };
    static CoreHost host;
    char pref[1024], err[512], want[64], args[1024] = "";
    const char *symbols = NULL, *prefix_path = NULL, *until = NULL, *out = NULL, *title = "ggx-solve";
    const char *suffix_path = NULL;
    GgScript *suffix = NULL;
    unsigned long fight_fields = 0, fought = 0;
    const char *list;
    GgScript *prefix;
    Fight ft = { 0, 0, 0, 40 };
    unsigned long max_fields = 400000;
    int fights_max = 1000000, fights = 0, i, was = 0, in_fight = 0, wins = 0, losses = 0, report = 0;
    Report rep;
    uint16_t advance = PAD_CIRCLE, stuck_button = PAD_START;
    unsigned long stuck_fields = 0, job_since = 0;
    CoreSnapshot *snap;

    cfg.cards[0] = 1;
    memset(&rep, 0, sizeof(rep));
    rep.job = 0xffffffffu;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--report")) {
            report = 1;
            continue;
        }
        if (i + 1 >= argc) {
            usage();
        }
        if (strlen(args) + strlen(argv[i]) + strlen(argv[i + 1]) + 3 < sizeof(args) && strcmp(argv[i], "--data-dir") &&
            strcmp(argv[i], "--out") && strcmp(argv[i], "--symbols") && strcmp(argv[i], "--prefix")) {
            strcat(args, argv[i]);
            strcat(args, " ");
            strcat(args, argv[i + 1]);
            strcat(args, " ");
        }
        if (!strcmp(argv[i], "--symbols")) {
            symbols = argv[++i];
        } else if (!strcmp(argv[i], "--prefix")) {
            prefix_path = argv[++i];
        } else if (!strcmp(argv[i], "--until")) {
            until = argv[++i];
        } else if (!strcmp(argv[i], "--out")) {
            out = argv[++i];
        } else if (!strcmp(argv[i], "--data-dir")) {
            cfg.data_dir = argv[++i];
        } else if (!strcmp(argv[i], "--card1")) {
            cfg.cards[0] = !strcmp(argv[++i], "empty");
        } else if (!strcmp(argv[i], "--fields")) {
            max_fields = strtoul(argv[++i], NULL, 10);
        } else if (!strcmp(argv[i], "--p2")) {
            const char *v = argv[++i];

            if (!strncmp(v, "mash:", 5)) {
                ft.p2_mash = 1;
                ft.p2_seed = strtoull(v + 5, NULL, 0);
            } else if (strcmp(v, "idle")) {
                usage();
            }
        } else if (!strcmp(argv[i], "--fights")) {
            fights_max = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--advance")) {
            advance = (uint16_t)pad_button_by_name(argv[++i]);
            if (!advance) {
                usage();
            }
        } else if (!strcmp(argv[i], "--horizon")) {
            ft.horizon = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--stuck-fields")) {
            stuck_fields = strtoul(argv[++i], NULL, 10);
        } else if (!strcmp(argv[i], "--stuck-button")) {
            stuck_button = (uint16_t)pad_button_by_name(argv[++i]);
            if (!stuck_button) {
                usage();
            }
        } else if (!strcmp(argv[i], "--fight-fields")) {
            fight_fields = strtoul(argv[++i], NULL, 10);
        } else if (!strcmp(argv[i], "--suffix")) {
            suffix_path = argv[++i];
        } else if (!strcmp(argv[i], "--title")) {
            title = argv[++i];
        } else {
            usage();
        }
    }
    if (!symbols || !prefix_path || (!report && (!until || !out))) {
        usage();
    }
    syms.PlayerWork = sym(symbols, "PlayerWork");
    syms.g_info = sym(symbols, "g_info");
    if ((!suffix_path != !fight_fields) || (suffix_path && !(suffix = ggs_load(suffix_path, err, sizeof(err))))) {
        fprintf(stderr, "ggx-solve: %s\n", suffix_path ? err : "--fight-fields and --suffix go together");
        return 1;
    }
    if (!(prefix = ggs_load(prefix_path, err, sizeof(err)))) {
        fprintf(stderr, "ggx-solve: %s\n", err);
        return 1;
    }
    if (!cfg.data_dir) {
        if (fs_pref_dir(pref, sizeof(pref)) != 0) {
            return 1;
        }
        cfg.data_dir = pref;
    }
    frames = calloc(MAX_FRAMES, sizeof(uint32_t));
    if (!frames || core_host_open(&host, &cfg) != 0 || core_host_boot(&host) != 0) {
        return 1;
    }
    for (i = 0; i < 2; i++) {
        pad_clear(&host.pads[i]);
    }
    snap = core_snapshot_new(&host);
    /* the prefix: menus, played as a script */
    for (;;) {
        PadInput pads[2];
        int st;

        memcpy(pads, host.pads, sizeof(pads));
        st = ggs_play_to(prefix, (uint64_t)host.vsyncs + 1, pads);
        if (st != GGS_PLAYING) {
            break;
        }
        record(&host, pads[0].buttons, pads[1].buttons);
        if (step(&host, pads[0].buttons, pads[1].buttons) != 0) {
            return 2;
        }
        if (report) {
            observe(&host, &rep);
        }
    }
    fprintf(stderr, "ggx-solve: prefix done at field %u (%s)\n", host.field, job_name(&host));
    if (report) {
        fprintf(stderr, "ggx-solve: %d fights: %d won, %d lost\n", rep.fights, rep.won, rep.lost);
        return 0;
    }
    list = until;
    want[0] = 0;
    {
        const char *c = strchr(list, ',');
        size_t n = c ? (size_t)(c - list) : strlen(list);

        memcpy(want, list, n < sizeof(want) ? n : sizeof(want) - 1);
        want[n < sizeof(want) ? n : sizeof(want) - 1] = 0;
        list = c ? c + 1 : list + strlen(list);
    }
    while (host.field < max_fields) {
        static uint32_t last_job = 0xffffffffu;
        uint32_t pw = ee_r32(&host, syms.PlayerWork);
        int fighting = pw && !strcmp(job_name(&host), "GameMain");
        int16_t a = fighting ? hp(&host, pw, ft.me) : 0, b = fighting ? hp(&host, pw, ft.me ^ 1) : 0;
        int is;

        if (ee_r32(&host, ggx_sym_JobMode) != last_job) {
            last_job = ee_r32(&host, ggx_sym_JobMode);
            job_since = host.field;
            fprintf(stderr, "ggx-solve: field %u: job %u %s\n", host.field, last_job, job_name(&host));
        }
        if (fighting && !in_fight) {
            in_fight = 1;
            ft.me = ee_r32(&host, pw + CW_PADID) == 0 ? 0 : 1;   /* PlayerWork[0].padid */
            fprintf(stderr, "ggx-solve: field %u: fight %d, pad 1 is side %d\n", host.field, fights + 1, ft.me);
        } else if (!fighting && in_fight) {
            in_fight = 0;
            fights++;
            fprintf(stderr, "ggx-solve: field %u: fight %d over, winner side %d; rounds won %d, lost %d\n", host.field,
                    fights, (int8_t)host.mem[syms.g_info + 6], wins, losses);
            wins = losses = 0;
        }
        if (suffix && fighting && fought >= fight_fields) {
            /* the suffix, from here: its frames count from this field's vblank */
            uint64_t base = host.vsyncs;

            fprintf(stderr, "ggx-solve: field %u: playing %s\n", host.field, suffix_path);
            for (;;) {
                PadInput pads[2];

                memcpy(pads, host.pads, sizeof(pads));
                if (ggs_play_to(suffix, (uint64_t)host.vsyncs + 1 - base, pads) != GGS_PLAYING) {
                    break;
                }
                record(&host, pads[0].buttons, pads[1].buttons);
                if (step(&host, pads[0].buttons, pads[1].buttons) != 0) {
                    return 2;
                }
            }
            ggs_free(suffix);
            suffix = NULL;
            continue;
        }
        if (fighting && fights < fights_max && hp(&host, pw, 0) > 0 && hp(&host, pw, 1) > 0) {
            uint16_t seq[256];
            int n = fight_decision(&host, snap, &ft, seq, 256), k;

            for (k = 0; k < n; k++) {
                uint16_t p2 = p2_pad(&ft, &host, 1);

                record(&host, seq[k], p2);
                if (step(&host, seq[k], p2) != 0) {
                    return 2;
                }
            }
            fought += (unsigned long)n;
        } else {
            /* menus and screens: tap --advance; in a fight past --fights, or between rounds, nothing */
            int stuck = stuck_fields && host.field - job_since >= stuck_fields;
            uint16_t p1 = (!fighting && host.field % 20 < 2) ? (stuck ? stuck_button : advance) : 0;
            uint16_t p2 = p2_pad(&ft, &host, fighting);

            record(&host, p1, p2);
            if (step(&host, p1, p2) != 0) {
                return 2;
            }
        }
        if (fighting && ee_r32(&host, syms.PlayerWork) == pw) {
            if (a > 0 && hp(&host, pw, ft.me) <= 0) {
                losses++;
                fprintf(stderr, "ggx-solve: field %u: round lost (%d left)\n", host.field, hp(&host, pw, ft.me ^ 1));
            } else if (b > 0 && hp(&host, pw, ft.me ^ 1) <= 0) {
                wins++;
                fprintf(stderr, "ggx-solve: field %u: round won (%d left)\n", host.field, hp(&host, pw, ft.me));
            }
        }
        is = !strcmp(job_name(&host), want);
        if (is && was) {
            fprintf(stderr, "ggx-solve: field %u: reached %s\n", host.field, want);
            was = 0;
            if (!*list) {
                break;
            } else {
                const char *c = strchr(list, ',');
                size_t n = c ? (size_t)(c - list) : strlen(list);

                memcpy(want, list, n < sizeof(want) ? n : sizeof(want) - 1);
                want[n < sizeof(want) ? n : sizeof(want) - 1] = 0;
                list = c ? c + 1 : list + strlen(list);
            }
        } else {
            was = is;
        }
        if (host.field % 1000 == 0) {
            fprintf(stderr, "ggx-solve: field %u: %s, %d fights\n", host.field, job_name(&host), fights);
        }
    }
    if (host.field >= max_fields) {
        fprintf(stderr, "ggx-solve: %s not reached in %lu fields\n", want, max_fields);
    }
    if (write_script(out, title, prefix_path, args) != 0) {
        fprintf(stderr, "ggx-solve: cannot write %s\n", out);
        return 1;
    }
    fprintf(stderr, "ggx-solve: wrote %s: %u frames, %d fights\n", out, nframes, fights);
    core_snapshot_free(snap);
    core_host_close(&host);
    ggs_free(prefix);
    return host.field >= max_fields ? 3 : 0;
}
