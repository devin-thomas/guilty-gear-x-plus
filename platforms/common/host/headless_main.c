/* ggx-headless: the Core with no window, stepped one field at a time.
 *
 *   ggx-headless --extract <image> [--data-dir <dir>]
 *   ggx-headless [--data-dir <dir> | --disc <image>] [--elf <boot elf>] [--fields N] [--until <job>]
 *                [--log <file> | --no-log] [--card1 store|empty|none] [--card2 empty|none] [--bench]
 *                [--save-dir <dir>] [--extension <id>]...
 *                [--hash-log <file>] [--save-at N --save <file>] [--load <file> [--poison]]
 *                [--mash SEED | --script <file>] [--forbid <job>[,<job>...]]
 *                [--snd-log <file.ggxs>] [--audio-out <file.wav>]
 *                [--unlock all|none|<item>,...] [--record <file.ggxreplay>]
 *   ggx-headless --play <file.ggxreplay> [--data-dir <dir> | --disc <image>] [--log <file> | --no-log] ...
 *
 * --extract identifies the player's disc image (ISO, BIN/CUE or CHD) and
 * extracts its files into the user data directory (fsys.h; --data-dir
 * names another), then exits. Running it again checks the extraction and
 * repairs anything missing or damaged.
 *
 * Otherwise it boots the game from the extracted assets of this build's
 * release, or with --disc straight from an image (for development: the
 * log is the same), then runs GameField up to N times (default 3600, one
 * minute of fields). With --until, it stops once the named job has run for
 * a whole field, and fails if that never happens; --until A,B,C waits for
 * each in turn (a completion run: GameMain,WinnerMain,SelectMain is a
 * Versus match played to its end). TitleMain is the title
 * screen: on first boot the game passes through it for one field and goes
 * to the opening (SkipTitle), then comes back to it about 46 seconds later.
 * Memory card slot 1 holds an empty formatted card and slot 2 none, unless
 * --card1/--card2 say otherwise: "store" is the native save store
 * (PORT-012, common/saves/), in --save-dir or saves/ in the user data
 * directory, as ggx uses it. --extension names an extension or mod active
 * in the run (repeatable); every save the game writes records it, and a
 * save so marked can't be exported to a PS2 card (ADR-012).
 *
 * The log (stdout unless --log or --no-log) gets every shim call, every job
 * change, and at the end a hash of EE RAM and the scratchpad; both are the
 * same on every OS for the same disc and build. --bench times the field
 * loop alone on a monotonic clock and reports it on stderr, so the log
 * stays the same.
 *
 * Core state (core_state.h): --hash-log writes the State Hash after the
 * boot and after every field. --save-at N --save <file> writes a snapshot
 * once N fields have run. --load <file> starts from a snapshot instead of
 * booting, and --fields then counts the fields after it; with --poison the
 * memory outside the state regions is poisoned first and checked at the
 * end, along with every page the Core wrote. --mash SEED makes both pads
 * mash buttons as a function of the seed and the field (mash.h).
 *
 * --script plays a GGScript input script (common/input/ggscript.h) on both
 * pads, one script frame per vblank, through the same pad encoding as ggx:
 * the port's completion runs. A "quit" step ends the
 * run. --forbid fails the run (status 5) if any of the jobs listed runs:
 * Story's game over screen in a run that must win.
 *
 * Sound (PORT-011): --snd-log records the game's sound command stream and
 * --audio-out plays it through the sound driver into a WAV file, a field
 * to a 59.94 Hz tick as in ggx. Neither changes anything the Core computes:
 * the log, the hashes and every --hash-log line are the same with them.
 *
 * Every field runs through a session (session.h), as in ggx: --unlock asks
 * for unlocks from the start (PORT-020's "unlock ="), made at the title;
 * GOHAN on the password screen works as in ggx. Replays (PORT-013,
 * replays.h): --record writes the run as a replay when it ends, from the
 * boot, with what the memory cards held at the boot, the unlocks and the
 * extensions. --play plays one: the cards, unlocks and extensions are the
 * replay's, every field's input is the replay's, and the State Hash is
 * checked wherever the replay recorded one. It stops at the replay's end;
 * --fields or --until stop it sooner. Exit status 0 if every check agreed,
 * 1 if one didn't (the field and both hashes are on stderr), 6 if the
 * replay can't play on this build (the cause is on stderr).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio_host.h"
#include "core_host.h"
#include "fsys.h"
#include "ggscript.h"
#include "mash.h"
#include "replays.h"
#include "save_file.h"
#include "session.h"
#include "unlock.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <shellapi.h>
#else
#include <time.h>
#endif

#if defined(VERSION_KR)
#define RELEASE "KR"
#else
#define RELEASE "JP"
#endif

/* the NTSC field rate the Core must keep up with */
#define FIELD_HZ 59.94

static double seconds(void)
{
#ifdef _WIN32
    LARGE_INTEGER f, c;

    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart / (double)f.QuadPart;
#else
    struct timespec t;

    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + t.tv_nsec * 1e-9;
#endif
}

static uint64_t fnv1a(const uint8_t *p, size_t n, uint64_t h)
{
    size_t i;

    for (i = 0; i < n; i++) {
        h = (h ^ p[i]) * 0x100000001b3ull;
    }
    return h;
}

static void usage(void)
{
    fprintf(stderr, "usage: ggx-headless --extract <image> [--data-dir <dir>]\n"
                    "       ggx-headless [--data-dir <dir> | --disc <image>] [--elf <boot elf>] [--fields N] "
                    "[--until <job>]\n"
                    "                    [--log <file> | --no-log] [--card1 store|empty|none] [--card2 empty|none] "
                    "[--save-dir <dir>]\n"
                    "                    [--extension <id>]... [--bench]\n"
                    "                    [--hash-log <file>] [--save-at N --save <file>] [--load <file> [--poison]]\n"
                    "                    [--mash SEED | --script <file>] [--forbid <job>[,<job>...]]\n"
                    "                    [--snd-log <file.ggxs>] [--audio-out <file.wav>]\n"
                    "                    [--unlock all|none|<item>,...] [--record <file.ggxreplay>]\n"
                    "       ggx-headless --play <file.ggxreplay> [--data-dir <dir> | --disc <image>] [--log <file> | --no-log]\n"
                    "                    [--fields N] [--until <job>] [--hash-log <file>] [--snd-log ...] [--audio-out ...]\n");
    exit(64);
}

/* A --hash-log line: the field, its State Hash, and the State Hash of EE RAM
   and the scratchpad alone, which does not depend on the Core's compiler. */
static void log_hashes(CoreHost *h, FILE *f)
{
    static const uint32_t ee[] = { CORE_REGION_RAM, CORE_REGION_SPR };

    fprintf(f, "f%06u %016llx ram+spr %016llx\n", h->field, (unsigned long long)core_hash(h),
            (unsigned long long)core_hash_regions(h, ee, 2));
}

static int save_file(CoreHost *h, const char *path)
{
    CoreSnapshot *s = core_snapshot_new(h);
    FILE *f;
    int rc = -1;

    if (s && core_save(h, s) == 0 && (f = fopen(path, "wb")) != NULL) {
        if (fwrite(core_snapshot_data(s), 1, core_state_size(h), f) == core_state_size(h)) {
            rc = 0;
        }
        rc |= fclose(f);
        if (rc == 0) {
            static const uint32_t ee[] = { CORE_REGION_RAM, CORE_REGION_SPR };

            fprintf(stderr, "ggx: saved field %u to %s, State Hash %016llx (ram+spr %016llx)\n", h->field, path,
                    (unsigned long long)core_snapshot_hash(s), (unsigned long long)core_hash_regions(h, ee, 2));
        }
    }
    if (rc != 0) {
        fprintf(stderr, "ggx: cannot save the Core to %s\n", path);
    }
    core_snapshot_free(s);
    return rc;
}

static int load_file(CoreHost *h, const char *path)
{
    CoreSnapshot *s = core_snapshot_new(h);
    size_t size = core_state_size(h);
    uint8_t *data = malloc(size + 1);
    FILE *f = fopen(path, "rb");
    int rc = CORE_LOAD_FORMAT;

    if (s && data && f && fread(data, 1, size + 1, f) == size && core_snapshot_set_data(s, data, size) == 0) {
        rc = core_load(h, s);
    }
    if (rc == 0) {
        fprintf(stderr, "ggx: loaded field %u from %s, State Hash %016llx\n", h->field, path,
                (unsigned long long)core_snapshot_hash(s));
    } else {
        fprintf(stderr, "ggx: cannot load %s: %s\n", path, core_load_error(rc));
    }
    if (f) {
        fclose(f);
    }
    free(data);
    core_snapshot_free(s);
    return rc;
}

/* The next job of an --until list ("A,B,C") into want; 0 when none is left. */
static int until_next(const char **list, char *want, size_t cap)
{
    const char *p = *list, *comma;
    size_t n;

    want[0] = 0;
    if (!p || !*p) {
        return 0;
    }
    comma = strchr(p, ',');
    n = comma ? (size_t)(comma - p) : strlen(p);
    if (n >= cap) {
        n = cap - 1;
    }
    memcpy(want, p, n);
    want[n] = 0;
    *list = comma ? comma + 1 : p + strlen(p);
    return 1;
}

#ifdef _WIN32
/* The command line as UTF-8, so paths outside the ANSI code page work. */
static char **utf8_argv(int *argc)
{
    wchar_t **w = CommandLineToArgvW(GetCommandLineW(), argc);
    char **v;
    int i;

    if (!w) {
        return NULL;
    }
    v = calloc((size_t)*argc + 1, sizeof(char *));
    for (i = 0; v && i < *argc; i++) {
        int n = WideCharToMultiByte(CP_UTF8, 0, w[i], -1, NULL, 0, NULL, NULL);

        v[i] = n > 0 ? malloc((size_t)n) : NULL;
        if (!v[i] || WideCharToMultiByte(CP_UTF8, 0, w[i], -1, v[i], n, NULL, NULL) != n) {
            v = NULL;
        }
    }
    LocalFree(w);
    return v;
}
#endif

int main(int argc, char **argv)
{
    CoreHostConfig cfg = { 0 };
    AudioHostConfig acfg = { 0 };
    AudioHost *audio = NULL;
    static CoreHost host;
    char pref[1024], save_dir[1100];
    const char *extensions[CORE_MAX_EXTENSIONS];
    int n_ext = 0;
    const char *until = NULL, *log_path = NULL, *extract = NULL, *hash_path = NULL, *save_path = NULL,
               *load_path = NULL, *script_path = NULL, *forbid = NULL;
    GgScript *script = NULL;
    const char *record_path = NULL, *play_path = NULL, *unlock_opt = NULL;
    Replay *rec = NULL, *play = NULL;
    Session sess;
    int unlocks = 0, fields_given = 0;
    char want[64];
    unsigned long fields = 3600, n, save_at = 0;
    int i, rc = 0, reached = 0, was = 0, no_log = 0, bench = 0, poison = 0, mash = 0;
    uint64_t seed = 0;
    FILE *hash_log = NULL;
    double start = 0, elapsed;
    uint64_t hash;

#ifdef _WIN32
    argv = utf8_argv(&argc);
    if (!argv) {
        fprintf(stderr, "ggx: cannot read the command line\n");
        return 1;
    }
#endif
    cfg.cards[0] = CORE_CARD_EMPTY;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--no-log")) {
            no_log = 1;
            continue;
        }
        if (!strcmp(argv[i], "--bench")) {
            bench = 1;
            continue;
        }
        if (!strcmp(argv[i], "--poison")) {
            poison = 1;
            continue;
        }
        if (i + 1 >= argc) {
            usage();
        }
        if (!strcmp(argv[i], "--extract")) {
            extract = argv[++i];
        } else if (!strcmp(argv[i], "--data-dir")) {
            cfg.data_dir = argv[++i];
        } else if (!strcmp(argv[i], "--disc")) {
            cfg.disc_path = argv[++i];
        } else if (!strcmp(argv[i], "--elf")) {
            cfg.elf_path = argv[++i];
        } else if (!strcmp(argv[i], "--fields")) {
            fields = strtoul(argv[++i], NULL, 10);
            fields_given = 1;
        } else if (!strcmp(argv[i], "--record")) {
            record_path = argv[++i];
        } else if (!strcmp(argv[i], "--play")) {
            play_path = argv[++i];
        } else if (!strcmp(argv[i], "--unlock")) {
            unlock_opt = argv[++i];
            if ((unlocks = unlock_parse(unlock_opt)) < 0) {
                usage();
            }
        } else if (!strcmp(argv[i], "--until")) {
            until = argv[++i];
        } else if (!strcmp(argv[i], "--log")) {
            log_path = argv[++i];
        } else if (!strcmp(argv[i], "--hash-log")) {
            hash_path = argv[++i];
        } else if (!strcmp(argv[i], "--save-at")) {
            save_at = strtoul(argv[++i], NULL, 10);
        } else if (!strcmp(argv[i], "--save")) {
            save_path = argv[++i];
        } else if (!strcmp(argv[i], "--load")) {
            load_path = argv[++i];
        } else if (!strcmp(argv[i], "--mash")) {
            mash = 1;
            seed = strtoull(argv[++i], NULL, 0);
        } else if (!strcmp(argv[i], "--script")) {
            script_path = argv[++i];
        } else if (!strcmp(argv[i], "--forbid")) {
            forbid = argv[++i];
        } else if (!strcmp(argv[i], "--snd-log")) {
            acfg.snd_log = argv[++i];
        } else if (!strcmp(argv[i], "--audio-out")) {
            acfg.wav = argv[++i];
        } else if (!strcmp(argv[i], "--card1") || !strcmp(argv[i], "--card2")) {
            int slot = argv[i][6] - '1';
            const char *v = argv[++i];

            if (core_card_kind(v) < 0 || (slot == 1 && core_card_kind(v) == CORE_CARD_STORE)) {
                usage();
            }
            cfg.cards[slot] = core_card_kind(v);
        } else if (!strcmp(argv[i], "--save-dir")) {
            cfg.save_dir = argv[++i];
        } else if (!strcmp(argv[i], "--extension")) {
            if (n_ext == CORE_MAX_EXTENSIONS) {
                usage();
            }
            extensions[n_ext++] = argv[++i];
        } else {
            usage();
        }
    }
    if ((no_log && log_path) || (cfg.disc_path && (cfg.data_dir || extract)) || (!save_path != !save_at) ||
        (poison && !load_path) || (mash && script_path) || (record_path && (load_path || play_path)) ||
        (play_path && (load_path || mash || script_path || unlock_opt || n_ext || cfg.cards[0] != CORE_CARD_EMPTY ||
                       cfg.cards[1] != CORE_CARD_NONE))) {
        usage();
    }
    if (!cfg.disc_path && !cfg.data_dir) {
        if (fs_pref_dir(pref, sizeof(pref)) != 0) {
            fprintf(stderr, "ggx: no user data directory: set HOME, or pass --data-dir\n");
            return 1;
        }
        cfg.data_dir = pref;
    }
    if (!cfg.save_dir) {
        if (save_store_dir(save_dir, sizeof(save_dir), cfg.data_dir) != 0) {
            fprintf(stderr, "ggx: no user data directory for saves: set HOME, or pass --save-dir\n");
            return 1;
        }
        cfg.save_dir = save_dir;
    }
    cfg.extensions = extensions;
    cfg.n_extensions = n_ext;
    if (extract) {
        ExtractResult res;

        if (assets_extract(extract, cfg.data_dir, &res, stderr) != 0) {
            return 1;
        }
        if (strcmp(res.release->id, RELEASE) != 0) {
            fprintf(stderr, "ggx: this build of the port runs the %s release; build it with -DGGX_VERSION=%s to run "
                            "these assets\n", RELEASE, res.release->id);
        }
        return 0;
    }
#ifdef _WIN32
    /* the same bytes as on the other OSes: no CRLF */
    _setmode(_fileno(stdout), _O_BINARY);
#endif
    if (!no_log) {
        cfg.log = log_path ? fopen(log_path, "wb") : stdout;
        if (!cfg.log) {
            fprintf(stderr, "ggx: cannot write %s\n", log_path);
            return 1;
        }
    }
    if (hash_path && !(hash_log = fopen(hash_path, "wb"))) {
        fprintf(stderr, "ggx: cannot write %s\n", hash_path);
        return 1;
    }
    if (script_path) {
        char err[512];

        if (!(script = ggs_load(script_path, err, sizeof(err)))) {
            fprintf(stderr, "ggx: %s\n", err);
            return 1;
        }
    }
    if (play_path) {
        char why[512];
        static const char *ids[RP_MAX_EXTENSIONS];

        if (!(play = rp_open(play_path, why, sizeof(why)))) {
            fprintf(stderr, "ggx: %s can't be played: %s\n", play_path, why);
            return 6;
        }
        /* the replay's cards and extensions */
        rp_host_config(play, cfg.cards, ids, &cfg.n_extensions);
        cfg.extensions = ids;
        if (!fields_given) {
            fields = rp_fields(play);
        }
    }
    if (core_host_open(&host, &cfg) != 0) {
        return 1;
    }
    if ((acfg.snd_log || acfg.wav) && !(audio = audio_host_open(&host, &acfg))) {
        return 1;
    }
    session_init(&sess, &host, unlocks);
    if (play) {
        RpBuild here;
        char why[512];

        rp_build_of(&host, cfg.extensions, cfg.n_extensions, &here);
        if (rp_compatible(play, &here, why, sizeof(why)) != 0) {
            fprintf(stderr, "ggx: %s can't be played on this build: %s\n", play_path, why);
            return 6;
        }
        if (rp_start(play, &sess) != 0) {
            fprintf(stderr, "ggx: %s: the memory card it records doesn't fit on a card\n", play_path);
            return 6;
        }
    }
    if (record_path && !(rec = rp_record(&sess, unlocks, cfg.extensions, cfg.n_extensions))) {
        return 1;
    }
    for (i = 0; i < 2; i++) {
        pad_clear(&host.pads[i]);
    }
    want[0] = 0;
    until_next(&until, want, sizeof(want));
    if (load_path) {
        if (poison) {
            core_state_poison(&host);
        }
        if (load_file(&host, load_path) != 0) {
            return 1;
        }
    } else if (core_host_boot(&host) != 0) {
        rc = 2;
    }
    audio_host_field(audio, 0, 1);
    if (hash_log) {
        log_hashes(&host, hash_log);
    }
    start = seconds();
    for (n = 0; rc == 0 && n < fields; n++) {
        SessionInput in;

        if (mash) {
            mash_input(seed, host.field, host.pads);
        }
        if (script && ggs_play_to(script, (uint64_t)host.vsyncs + 1, host.pads) == GGS_QUIT) {
            shim_log(&host, "script quit at frame %llu", (unsigned long long)ggs_frames(script));
            break;
        }
        memset(&in, 0, sizeof(in));
        in.pads[0] = host.pads[0];
        in.pads[1] = host.pads[1];
        if (play) {
            const SessionInput *p = rp_input(play, host.field);
            char why[256];

            if (!p) {
                break;
            }
            in = *p;
            if (host.field % RP_CHECK_EVERY == 0 && rp_check(play, host.field, core_hash(&host), why, sizeof(why))) {
                fprintf(stderr, "ggx: %s doesn't play as recorded: %s\n", play_path, why);
                rc = 1;
                break;
            }
        }
        if (session_field(&sess, &in) != 0) {
            rc = 2;
            break;
        }
        rp_watch(rec, &host);
        audio_host_field(audio, host.field - 1, 1);
        if (hash_log) {
            log_hashes(&host, hash_log);
        }
        if (save_path && host.field == save_at && save_file(&host, save_path) != 0) {
            rc = 1;
            break;
        }
        if (forbid) {
            const char *name = ggx_func_name(ee_r32(&host, ggx_sym_jobfunc + 4 * ee_r32(&host, ggx_sym_JobMode)));
            const char *p = strstr(forbid, name);
            size_t n = strlen(name);

            if (p && (p == forbid || p[-1] == ',') && (p[n] == ',' || !p[n])) {
                shim_log(&host, "forbidden job %s", name);
                fprintf(stderr, "ggx: field %u: %s ran, which --forbid lists\n", host.field, name);
                rc = 5;
                break;
            }
        }
        if (want[0]) {
            uint32_t job = ee_r32(&host, ggx_sym_JobMode);
            int is = !strcmp(ggx_func_name(ee_r32(&host, ggx_sym_jobfunc + 4 * job)), want);

            /* it was the job when this field began and still is */
            if (is && was) {
                shim_log(&host, "reached %s", want);
                fprintf(stderr, "ggx: field %u: reached %s\n", host.field, want);
                was = 0;
                if (!until_next(&until, want, sizeof(want))) {
                    reached = 1;
                    break;
                }
            } else {
                was = is;
            }
        }
    }
    elapsed = seconds() - start;
    if (play && rc == 0) {
        char why[256];

        if (host.field == rp_fields(play)) {
            if (rp_check(play, host.field, core_hash(&host), why, sizeof(why)) != 0) {
                fprintf(stderr, "ggx: %s doesn't play as recorded: %s\n", play_path, why);
                rc = 1;
            } else {
                fprintf(stderr, "ggx: %s: all %u fields played as recorded, every State Hash checked agrees\n",
                        play_path, host.field);
            }
        } else {
            fprintf(stderr, "ggx: %s: %u of its %u fields played as recorded\n", play_path, host.field,
                    rp_fields(play));
        }
    }
    if (rec && rc == 0) {
        char why[512];

        if (rp_write(rec, record_path, why, sizeof(why)) != 0) {
            fprintf(stderr, "ggx: cannot write the replay: %s\n", why);
            rc = 1;
        } else {
            fprintf(stderr, "ggx: wrote %s: %u fields from the boot\n", record_path, host.field);
        }
    }
    hash = fnv1a(ee_ptr(&host, 0, EE_RAM_SIZE), EE_RAM_SIZE, 0xcbf29ce484222325ull);
    hash = fnv1a(ee_ptr(&host, EE_SPR_BASE, EE_SPR_SIZE), EE_SPR_SIZE, hash);
    shim_log(&host, "end fields=%u vsyncs=%u ram_fnv1a64=%016llx", host.field, host.vsyncs, (unsigned long long)hash);
    if (bench) {
        double fps = elapsed > 0 ? host.field / elapsed : 0;

        fprintf(stderr, "ggx: %u fields in %.3f s: %.0f fields/s, %.1fx real time (%s), ram_fnv1a64=%016llx\n",
                host.field, elapsed, fps, fps / FIELD_HZ, cfg.log ? "logging" : "no log",
                (unsigned long long)hash);
    }
    if (poison) {
        uint64_t bad = core_state_poison_check(&host);
        int pages = core_state_scan(&host, "after the run");

        fprintf(stderr, "ggx: poison check: %llu bytes outside the state regions changed, "
                        "%d pages outside them written\n", (unsigned long long)bad, pages);
        if (bad || pages) {
            rc = 4;
        }
    }
    if (audio_host_close(audio) != 0) {
        fprintf(stderr, "ggx: writing the sound failed\n");
        rc = rc ? rc : 1;
    }
    core_host_close(&host);
    if (log_path) {
        fclose(cfg.log);
    }
    if (hash_log) {
        fclose(hash_log);
    }
    if (rc == 0 && want[0] && !reached) {
        fprintf(stderr, "ggx: %s not reached in %lu fields\n", want, fields);
        rc = 3;
    }
    ggs_free(script);
    rp_free(rec);
    rp_free(play);
    return rc;
}
