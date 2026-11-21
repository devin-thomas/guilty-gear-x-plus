/* ggx-replays: the player's replays (PORT-013, replays.h).
 *
 *   ggx-replays list [--data-dir <dir>]
 *   ggx-replays info <file.ggxreplay>
 *
 * list shows the replays in the user data directory's replays/ folder (or
 * --data-dir's), where ggx writes one at the end of each fight; info shows
 * one replay's header: the release and Core build it plays on, the
 * extensions, the unlocks, the memory cards, its fields and its fights.
 * To play one: ggx --play <file> (in a window), or ggx-headless --play
 * <file>, which checks every State Hash it recorded.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fsys.h"
#include "replays.h"
#include "unlock.h"

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

static const char *release_name(uint32_t r)
{
    return r == 1 ? "KR (SLPM-645.25)" : r == 2 ? "JP (SLPS-201.37)" : "unknown";
}

/* "characters,colours" for a mask, "none", or "all" */
static const char *unlock_names(int m, char *buf, size_t cap)
{
    size_t o = 0;
    int i;

    if (m == UNLOCK_ALL) {
        return "all";
    }
    if (!m) {
        return "none";
    }
    buf[0] = 0;
    for (i = 0; i < 4; i++) {
        if (m & (1 << i)) {
            o += (size_t)snprintf(buf + o, cap - o, "%s%s", o ? "," : "", unlock_name(1 << i));
        }
    }
    return buf;
}

static int fights(const Replay *r)
{
    uint32_t f, k;
    int i, n = 0;

    for (i = 0; i < rp_marks(r); i++) {
        rp_mark_at(r, i, &f, &k);
        n += k == RP_MARK_FIGHT;
    }
    return n;
}

static int info(const char *path)
{
    char why[512], u[64];
    Replay *r = rp_open(path, why, sizeof(why));
    const RpBuild *b;
    int i, s;

    if (!r) {
        fprintf(stderr, "ggx-replays: %s: %s\n", path, why);
        return 1;
    }
    b = rp_build(r);
    printf("%s\n", path);
    printf("  release      %s\n", release_name(b->release));
    printf("  boot ELF     ");
    for (i = 0; i < 32; i++) {
        printf("%02x", b->elf_sha256[i]);
    }
    printf("\n  Core build   %016llx (State Hash version %u)\n", (unsigned long long)b->core_build,
           b->state_hash_version);
    printf("  extensions   ");
    for (i = 0; i < b->n_extensions; i++) {
        printf("%s%s", i ? ", " : "", b->extensions[i]);
    }
    printf("%s\n", b->n_extensions ? "" : "none");
    printf("  unlocks      %s, from the start\n", unlock_names(rp_unlocks(r), u, sizeof(u)));
    for (s = 0; s < 2; s++) {
        int ndirs, d;

        if (!rp_card(r, s, &ndirs)) {
            printf("  card %d       none\n", s + 1);
            continue;
        }
        printf("  card %d       %d %s", s + 1, ndirs, ndirs == 1 ? "save" : "saves");
        for (d = 0; d < ndirs; d++) {
            int nfiles;

            printf("%s%s", d ? ", " : ": ", rp_card_dir(r, s, d, &nfiles));
        }
        printf("\n");
    }
    printf("  fields       %u from the boot (%.1f minutes)\n", rp_fields(r), rp_fields(r) / 59.94 / 60);
    printf("  marks       ");
    for (i = 0; i < rp_marks(r); i++) {
        uint32_t f, k;

        rp_mark_at(r, i, &f, &k);
        printf(" %s at %u", k == RP_MARK_FIGHT ? "fight" : "end", f);
    }
    printf("%s\n", rp_marks(r) ? "" : " none");
    rp_free(r);
    return 0;
}

typedef struct {
    char (*names)[260];
    int n, cap;
} Names;

static void collect(void *ctx, const char *name)
{
    Names *l = ctx;
    size_t n = strlen(name);

    if (n < 10 || strcmp(name + n - 10, ".ggxreplay") != 0 || n >= 260) {
        return;
    }
    if (l->n == l->cap) {
        l->cap = l->cap ? 2 * l->cap : 64;
        l->names = realloc(l->names, (size_t)l->cap * sizeof(*l->names));
        if (!l->names) {
            exit(1);
        }
    }
    memcpy(l->names[l->n++], name, n + 1);
}

static int cmp_name(const void *a, const void *b)
{
    return strcmp(a, b);
}

static int list(const char *data_dir)
{
    char dir[1100];
    Names l = { 0 };
    int i;

    if (rp_dir(dir, sizeof(dir), data_dir) != 0) {
        fprintf(stderr, "ggx-replays: no user data directory: set HOME, or pass --data-dir\n");
        return 1;
    }
    if (fs_list(dir, collect, &l) != 0) {
        printf("no replays: %s doesn't exist yet\n", dir);
        return 0;
    }
    qsort(l.names, (size_t)l.n, sizeof(*l.names), cmp_name);
    printf("%d %s in %s\n", l.n, l.n == 1 ? "replay" : "replays", dir);
    for (i = 0; i < l.n; i++) {
        char path[1400], why[512];
        Replay *r;

        snprintf(path, sizeof(path), "%s/%s", dir, l.names[i]);
        if (!(r = rp_open(path, why, sizeof(why)))) {
            printf("  %-40s  %s\n", l.names[i], why);
            continue;
        }
        printf("  %-40s  %s, %u fields, %d %s\n", l.names[i], rp_build(r)->release == 1 ? "KR" : "JP", rp_fields(r),
               fights(r), fights(r) == 1 ? "fight" : "fights");
        rp_free(r);
    }
    free(l.names);
    return 0;
}

static int usage(void)
{
    fprintf(stderr, "usage: ggx-replays list [--data-dir <dir>]\n"
                    "       ggx-replays info <file.ggxreplay>\n");
    return 64;
}

int main(int argc, char **argv)
{
#ifdef _WIN32
    /* the command line as UTF-8 */
    wchar_t **w = CommandLineToArgvW(GetCommandLineW(), &argc);
    static char *v[64];
    int k;

    for (k = 0; w && k < argc && k < 64; k++) {
        int n = WideCharToMultiByte(CP_UTF8, 0, w[k], -1, NULL, 0, NULL, NULL);

        v[k] = malloc((size_t)(n > 0 ? n : 1));
        if (!v[k] || WideCharToMultiByte(CP_UTF8, 0, w[k], -1, v[k], n, NULL, NULL) != n) {
            return 1;
        }
    }
    argv = v;
#endif
    if (argc == 2 && !strcmp(argv[1], "list")) {
        return list(NULL);
    }
    if (argc == 4 && !strcmp(argv[1], "list") && !strcmp(argv[2], "--data-dir")) {
        return list(argv[3]);
    }
    if (argc == 3 && !strcmp(argv[1], "info")) {
        return info(argv[2]);
    }
    return usage();
}
