/* ggx-replay: replays a Lockstep Trace through the Core (PORT-006).
 *
 *   ggx-replay <trace.ggxt> [--data-dir <dir> | --disc <image>] [--log <file>] [--fields N]
 *              [--symbols <map.txt>] [--diff-dir <dir>] [--ps2-dump <file>] [--keep-going]
 *              [--snd-log <file.ggxs>] [--audio-out <file.wav>]
 *
 * The trace was recorded from the real game by the gg-layers PCSX2 fork
 * (common/trace/trace.h). The Core boots and runs one field per
 * recorded field, its SDK calls answered by the PS2's answers from the
 * trace. After each field its State Hash, with the trace's masks, must
 * equal the PS2's. The first field where it doesn't, or where the Core's
 * calls stop being the PS2's, ends the run with exit status 1 and a report:
 * the field, the region (ram, spr), each differing 4 KiB page with the
 * symbol it holds (--symbols), and, given the recorder's dump of that field
 * (GG_TRACE_DUMP, --ps2-dump; it finds <trace>.f<field>.ram itself), the
 * differing bytes. --diff-dir writes the
 * Core's side of each differing page there. --keep-going surveys instead:
 * it reports the first mismatch, carries on through every State Hash
 * mismatch, and lists at the end every page that ever differed. Exit status
 * 2: the trace or the disc doesn't fit this build.
 *
 * Sound (PORT-011): --snd-log records the game's sound command stream, and
 * --audio-out plays it through the sound driver into a WAV file, on the
 * PS2's timeline (each field as many vblanks as it took the PS2). Neither
 * changes what the Core computes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "audio_host.h"
#include "core_host.h"
#include "fsys.h"
#include "trace.h"

#if defined(VERSION_KR)
#define RELEASE "KR"
#define RELEASE_ID 1u
#else
#define RELEASE "JP"
#define RELEASE_ID 2u
#endif

static void usage(void)
{
    fprintf(stderr, "usage: ggx-replay <trace.ggxt> [--data-dir <dir> | --disc <image>] [--log <file>] [--fields N]\n"
                    "                  [--symbols <map.txt>] [--diff-dir <dir>] [--ps2-dump <file>]\n"
                    "                  [--snd-log <file.ggxs>] [--audio-out <file.wav>]\n");
    exit(2);
}

int main(int argc, char **argv)
{
    CoreHostConfig cfg = { 0 };
    AudioHostConfig acfg = { 0 };
    AudioHost *audio = NULL;
    static CoreHost host;
    const char *trace_path = NULL, *log_path = NULL, *symbols = NULL, *diff_dir = NULL, *ps2_dump = NULL;
    char pref[1024], err[512];
    const TraceHeader *th;
    unsigned long limit = 0;
    uint32_t n, fields;
    Trace *t;
    clock_t start;
    int i, rc, keep_going = 0;

    cfg.cards[0] = 1;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--keep-going")) {
            keep_going = 1;
            continue;
        }
        if (argv[i][0] != '-') {
            if (trace_path) {
                usage();
            }
            trace_path = argv[i];
            continue;
        }
        if (i + 1 >= argc) {
            usage();
        }
        if (!strcmp(argv[i], "--data-dir")) {
            cfg.data_dir = argv[++i];
        } else if (!strcmp(argv[i], "--disc")) {
            cfg.disc_path = argv[++i];
        } else if (!strcmp(argv[i], "--log")) {
            log_path = argv[++i];
        } else if (!strcmp(argv[i], "--fields")) {
            limit = strtoul(argv[++i], NULL, 10);
        } else if (!strcmp(argv[i], "--symbols")) {
            symbols = argv[++i];
        } else if (!strcmp(argv[i], "--diff-dir")) {
            diff_dir = argv[++i];
        } else if (!strcmp(argv[i], "--ps2-dump")) {
            ps2_dump = argv[++i];
        } else if (!strcmp(argv[i], "--snd-log")) {
            acfg.snd_log = argv[++i];
        } else if (!strcmp(argv[i], "--audio-out")) {
            acfg.wav = argv[++i];
        } else {
            usage();
        }
    }
    if (!trace_path || (cfg.disc_path && cfg.data_dir)) {
        usage();
    }
    if (!(t = trace_open(trace_path, err, sizeof(err)))) {
        fprintf(stderr, "ggx-replay: %s\n", err);
        return 2;
    }
    th = trace_header(t);
    if (th->release != RELEASE_ID) {
        fprintf(stderr, "ggx-replay: %s is a %s trace; this build runs %s\n", trace_path, th->release == 1 ? "KR" : "JP",
                RELEASE);
        return 2;
    }
    if (!cfg.disc_path && !cfg.data_dir) {
        if (fs_pref_dir(pref, sizeof(pref)) != 0) {
            fprintf(stderr, "ggx-replay: no user data directory: set HOME, or pass --data-dir\n");
            return 2;
        }
        cfg.data_dir = pref;
    }
    if (log_path && !(cfg.log = fopen(log_path, "wb"))) {
        fprintf(stderr, "ggx-replay: cannot write %s\n", log_path);
        return 2;
    }
    if (diff_dir) {
        fs_mkdirs(diff_dir);
    }
    if (core_host_open(&host, &cfg) != 0) {
        return 2;
    }
    if (memcmp(host.elf_sha256, th->elf_sha256, 32) != 0) {
        fprintf(stderr, "ggx-replay: %s was recorded from another boot ELF than this disc's\n", trace_path);
        return 2;
    }
    if ((acfg.snd_log || acfg.wav) && !(audio = audio_host_open(&host, &acfg))) {
        return 2;
    }
    fields = trace_fields(t);
    if (limit && limit < fields) {
        fields = (uint32_t)limit;
    }
    fprintf(stderr, "ggx-replay: %s: %u fields, recorded by %s (%s)%s%s\n", trace_path, trace_fields(t), th->recorder,
            th->settings, th->note[0] ? ", " : "", th->note);
    start = clock();
    trace_begin(t, &host);
    if (keep_going) {
        trace_keep_going(t);
    }
    rc = 0;
    for (n = 0; n < fields; n++) {
        trace_field_begin(t, n);
        if ((n == 0 ? core_host_boot(&host) : core_host_field(&host)) != 0) {
            fprintf(stderr, "ggx-replay: the Core trapped in field %u (the log says where)\n", n);
            rc = 1;
            break;
        }
        audio_host_field(audio, n, trace_field_vblanks(t, n));
        if (trace_field_end(t) != 0) {
            /* the recorder's dump of this field, if it made one (GG_TRACE_DUMP) */
            char dump[1100];
            FILE *f;
            int have_dump = 0;

            snprintf(dump, sizeof(dump), "%s.f%u.ram", trace_path, n);
            if ((f = fopen(dump, "rb"))) {
                fclose(f);
                have_dump = 1;
            }
            if (rc == 0) {
                fprintf(stderr, "ggx-replay: %s: MISMATCH\n", trace_path);
                trace_report(t, stderr, diff_dir, symbols, ps2_dump ? ps2_dump : have_dump ? dump : NULL);
            } else if (keep_going && have_dump) {
                /* surveying: every later mismatching field the recorder dumped too */
                trace_report(t, stderr, NULL, symbols, dump);
            }
            rc = 1;
            if (trace_resume(t) != 0) {
                if (keep_going) {
                    trace_report(t, stderr, NULL, symbols, NULL);
                }
                break;
            }
        }
    }
    trace_survey(t, stderr, symbols);
    if (rc == 0) {
        fprintf(stderr, "ggx-replay: %s: all %u fields match the PS2 (%.1f s)\n", trace_path, fields,
                (double)(clock() - start) / CLOCKS_PER_SEC);
    }
    if (audio_host_close(audio) != 0) {
        fprintf(stderr, "ggx-replay: writing the sound failed\n");
        rc = rc ? rc : 2;
    }
    shim_log_flush(&host);
    core_host_close(&host);
    trace_free(t);
    return rc;
}
