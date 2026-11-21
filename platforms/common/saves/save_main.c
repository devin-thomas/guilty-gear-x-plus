/* ggx-save: the native save store and PS2 memory card images (PORT-012, ADR-012).
 *
 *   ggx-save list [--save-dir <dir>]
 *   ggx-save info <file.ggxsave>
 *   ggx-save card <card>
 *   ggx-save import <card> [--release kr|jp] [--replace] [--save-dir <dir>]
 *   ggx-save export <card.ps2|card.mcd> [--release kr|jp] [--replace] [--save-dir <dir>]
 *
 * The store is saves/ in the user data directory, beside ggx.cfg, unless
 * --save-dir names another (ggx and ggx-headless take the same option).
 *
 * A card is a PCSX2 memory card: an image (.ps2, .mcd: 8 MB or larger, with
 * or without ECC) or a folder memory card (a folder; import only). "card"
 * lists what a card holds.
 *
 * import brings every Guilty Gear X Plus directory on the card into the
 * store: the settings and progress of either release (BKSLPM-64525GGX,
 * BISLPS-20137GGX) and the Versus replays (...GGXRP0 to RP9). Each must have
 * the original save layout. A directory already in the store is kept
 * unless --replace is given.
 *
 * export writes the store's directories onto a card image, after the
 * compatibility check (save_file.h save_check_export): a save that an
 * extension or mod touched, one whose release doesn't match its directory,
 * one in a newer format, or one that doesn't have the original layout is
 * refused, with the cause named, and then nothing is written. A missing
 * image is created as a new formatted 8 MB card, and so is an image that
 * holds nothing (a new PCSX2 card, all 0xFF, not yet formatted). A save the
 * card already holds is replaced only with --replace. The image is written
 * to a temporary file and renamed over the old one. Close the card in
 * PCSX2 (or quit PCSX2) first.
 *
 * Exit status: 0 done, 1 refused or failed, 64 usage.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fsys.h"
#include "ps2mc.h"
#include "save_file.h"

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

#define MAX_CARD_DIRS 512

static void usage(void)
{
    fprintf(stderr, "usage: ggx-save list [--save-dir <dir>]\n"
                    "       ggx-save info <file.ggxsave>\n"
                    "       ggx-save card <card>\n"
                    "       ggx-save import <card> [--release kr|jp] [--replace] [--save-dir <dir>]\n"
                    "       ggx-save export <card.ps2|card.mcd> [--release kr|jp] [--replace] [--save-dir <dir>]\n");
    exit(64);
}

/* the game's directories for a release, or both (release 0): the system one, then RP0-RP9 */
static int game_dirs(uint32_t release, char (*out)[SAVE_NAME_MAX])
{
    uint32_t r;
    int n = 0, i;

    for (r = SAVE_RELEASE_KR; r <= SAVE_RELEASE_JP; r++) {
        if (release && r != release) {
            continue;
        }
        snprintf(out[n++], SAVE_NAME_MAX, "%s", save_system_dir(r));
        for (i = 0; i < 10; i++) {
            snprintf(out[n++], SAVE_NAME_MAX, "%sRP%d", save_system_dir(r), i);
        }
    }
    return n;
}

static void print_time(const uint8_t t[8])
{
    printf("%04u-%02u-%02u %02u:%02u:%02u JST", (unsigned)(t[6] | t[7] << 8), t[5], t[4], t[3], t[2], t[1]);
}

/* what a system directory's data file says (USER_SETTING) */
static void print_progress(const SaveDir *d)
{
    const char *sys = save_system_dir(d->release);
    int i = save_dir_find(d, d->name);
    const uint8_t *u;

    if (!sys || strcmp(d->name, sys) != 0 || i < 0 || d->files[i].size < 0x1614) {
        return;
    }
    u = d->files[i].data;
    /* phase: what is unlocked (opt_pass.c raises it to 3, 4, 5); clear: the endings seen; medal */
    printf("    progress: phase %d, clear 0x%04x, medal %d, autosave %s\n", (signed char)u[0x6d],
           (unsigned)(u[0x70] | u[0x71] << 8), (signed char)u[0x6f], u[0x6e] ? "on" : "off");
}

static void print_dir(const SaveDir *d, int detail)
{
    int i;

    printf("  %s  %s", d->name, save_release_name(save_release_of(d->name)));
    if (d->modified[6] | d->modified[7]) {
        printf(", saved ");
        print_time(d->modified);
    }
    printf("\n");
    if (detail) {
        for (i = 0; i < d->nfiles; i++) {
            printf("    %-20s %6u bytes\n", d->files[i].name, d->files[i].size);
        }
        for (i = 0; i < d->next; i++) {
            printf("    written with extension \"%s\"%s\n", d->ext[i].id,
                   d->ext[i].flags & SAVE_EXT_COMPATIBLE ? " (save-compatible)" : "");
        }
    }
    print_progress(d);
}

static int cmd_list(const char *store)
{
    char names[22][SAVE_NAME_MAX], path[1100], why[256];
    int n = game_dirs(0, names), i, found = 0;

    printf("%s:\n", store);
    for (i = 0; i < n; i++) {
        uint64_t size;
        SaveDir d;

        if (save_store_path(path, sizeof(path), store, names[i]) != 0 || fs_stat(path, &size, NULL) != 0) {
            continue;
        }
        found++;
        if (save_read(path, &d, NULL, why, sizeof(why)) != SAVE_OK) {
            printf("  %s  can't be read: %s\n", names[i], why);
            continue;
        }
        print_dir(&d, 0);
        save_dir_free(&d);
    }
    if (!found) {
        printf("  no saves\n");
    }
    return 0;
}

static int cmd_info(const char *path)
{
    char why[256];
    uint32_t version = 0;
    SaveDir d;
    int rc = save_read(path, &d, &version, why, sizeof(why));

    if (rc != SAVE_OK) {
        fprintf(stderr, "ggx-save: %s: %s\n", path, why);
        return 1;
    }
    printf("%s: save format version %u, release %s\n", path, version, save_release_name(d.release));
    print_dir(&d, 1);
    if (save_check_export(&d, why, sizeof(why)) == 0) {
        printf("  can be exported to a PS2 memory card\n");
    } else {
        printf("  can't be exported to a PS2 memory card: %s\n", why);
    }
    save_dir_free(&d);
    return 0;
}

static int cmd_card(const char *path)
{
    static char names[MAX_CARD_DIRS][SAVE_NAME_MAX];
    char why[256];
    Ps2Card *c;
    int n, i;

    if (ps2mc_open(&c, path, why, sizeof(why)) != 0) {
        fprintf(stderr, "ggx-save: %s: %s\n", path, why);
        return 1;
    }
    n = ps2mc_list(c, names, MAX_CARD_DIRS);
    if (n < 0) {
        fprintf(stderr, "ggx-save: %s: its root directory is damaged\n", path);
        ps2mc_close(c);
        return 1;
    }
    if (ps2mc_is_folder(c)) {
        printf("%s: a folder memory card, %d saves\n", path, n);
    } else {
        printf("%s: %d saves, %d KB free\n", path, n, ps2mc_free_kb(c));
    }
    for (i = 0; i < n; i++) {
        SaveDir d;

        if (!save_release_of(names[i])) {
            printf("  %s\n", names[i]);
            continue;
        }
        if (ps2mc_read_dir(c, names[i], &d, why, sizeof(why)) != 0) {
            printf("  %s  can't be read: %s\n", names[i], why);
            continue;
        }
        print_dir(&d, 0);
        save_dir_free(&d);
    }
    ps2mc_close(c);
    return 0;
}

static int cmd_import(const char *card, const char *store, uint32_t release, int replace)
{
    static char names[MAX_CARD_DIRS][SAVE_NAME_MAX];
    char why[256], path[1100];
    int n, i, done = 0, refused = 0;
    Ps2Card *c;

    if (ps2mc_open(&c, card, why, sizeof(why)) != 0) {
        fprintf(stderr, "ggx-save: %s: %s\n", card, why);
        return 1;
    }
    n = ps2mc_list(c, names, MAX_CARD_DIRS);
    if (n < 0) {
        fprintf(stderr, "ggx-save: %s: its root directory is damaged\n", card);
        ps2mc_close(c);
        return 1;
    }
    for (i = 0; i < n; i++) {
        uint32_t r = save_release_of(names[i]);
        uint64_t size;
        SaveDir d;

        if (!r || (release && r != release)) {
            continue;
        }
        if (ps2mc_read_dir(c, names[i], &d, why, sizeof(why)) != 0) {
            fprintf(stderr, "ggx-save: not imported: %s: %s\n", names[i], why);
            refused++;
            continue;
        }
        d.release = r;
        if (save_check_layout(&d, why, sizeof(why)) != 0) {
            fprintf(stderr, "ggx-save: not imported: %s: %s\n", names[i], why);
            save_dir_free(&d);
            refused++;
            continue;
        }
        if (save_store_path(path, sizeof(path), store, d.name) != 0 || fs_mkdirs(store) != 0) {
            fprintf(stderr, "ggx-save: cannot write to %s\n", store);
            save_dir_free(&d);
            ps2mc_close(c);
            return 1;
        }
        if (!replace && fs_stat(path, &size, NULL) == 0) {
            fprintf(stderr, "ggx-save: not imported: %s: the store already has it (%s); --replace overwrites it\n",
                    d.name, path);
            save_dir_free(&d);
            refused++;
            continue;
        }
        if (save_write(path, &d, why, sizeof(why)) != SAVE_OK) {
            fprintf(stderr, "ggx-save: %s\n", why);
            save_dir_free(&d);
            ps2mc_close(c);
            return 1;
        }
        printf("imported:");
        print_dir(&d, 0);
        save_dir_free(&d);
        done++;
    }
    ps2mc_close(c);
    if (!done && !refused) {
        fprintf(stderr, "ggx-save: %s holds no Guilty Gear X Plus save%s\n", card,
                release ? (release == SAVE_RELEASE_KR ? " for KR" : " for JP") : "");
        return 1;
    }
    printf("%d imported into %s", done, store);
    if (refused) {
        printf(", %d not", refused);
    }
    printf("\n");
    return refused ? 1 : 0;
}

/* 1 if every byte of the file is 0xFF: a card PCSX2 created and nothing formatted */
static int blank_card(const char *path)
{
    FILE *f = fs_open(path, "rb");
    uint8_t buf[65536];
    size_t n, i, total = 0;
    int blank = 1;

    if (!f) {
        return 0;
    }
    while (blank && (n = fread(buf, 1, sizeof(buf), f)) > 0) {
        for (i = 0; i < n; i++) {
            if (buf[i] != 0xFF) {
                blank = 0;
                break;
            }
        }
        total += n;
    }
    fclose(f);
    return blank && total == 8650752;
}

static int cmd_export(const char *card, const char *store, uint32_t release, int replace)
{
    static SaveDir dirs[22];
    char names[22][SAVE_NAME_MAX], path[1100], why[256];
    int n = game_dirs(release, names), i, nd = 0, refused = 0;
    uint64_t size;
    Ps2Card *c;
    int created = 0;

    for (i = 0; i < n; i++) {
        uint32_t version = 0;
        int rc;

        if (save_store_path(path, sizeof(path), store, names[i]) != 0 || fs_stat(path, &size, NULL) != 0) {
            continue;
        }
        rc = save_read(path, &dirs[nd], &version, why, sizeof(why));
        if (rc != SAVE_OK) {
            fprintf(stderr, "ggx-save: refused: %s: %s\n", names[i], why);
            refused++;
            continue;
        }
        if (save_check_export(&dirs[nd], why, sizeof(why)) != 0) {
            fprintf(stderr, "ggx-save: refused: %s: %s\n", names[i], why);
            save_dir_free(&dirs[nd]);
            refused++;
            continue;
        }
        nd++;
    }
    if (refused) {
        fprintf(stderr, "ggx-save: nothing was written to %s\n", card);
        for (i = 0; i < nd; i++) {
            save_dir_free(&dirs[i]);
        }
        return 1;
    }
    if (!nd) {
        fprintf(stderr, "ggx-save: %s holds no save%s to export\n", store,
                release ? (release == SAVE_RELEASE_KR ? " for KR" : " for JP") : "");
        return 1;
    }
    if (fs_stat(card, &size, NULL) != 0 || blank_card(card)) {
        created = 1;
        if (ps2mc_create(&c, why, sizeof(why)) != 0) {
            fprintf(stderr, "ggx-save: %s\n", why);
            return 1;
        }
    } else if (ps2mc_open(&c, card, why, sizeof(why)) != 0) {
        fprintf(stderr, "ggx-save: %s: %s\n", card, why);
        return 1;
    }
    for (i = 0; i < nd; i++) {
        if (ps2mc_write_dir(c, &dirs[i], replace, why, sizeof(why)) != 0) {
            fprintf(stderr, "ggx-save: not exported: %s: %s; nothing was written to %s\n", dirs[i].name, why, card);
            for (; i < nd; i++) {
                save_dir_free(&dirs[i]);
            }
            ps2mc_close(c);
            return 1;
        }
    }
    if (ps2mc_save(c, card, why, sizeof(why)) != 0) {
        fprintf(stderr, "ggx-save: %s\n", why);
        ps2mc_close(c);
        return 1;
    }
    for (i = 0; i < nd; i++) {
        printf("exported:");
        print_dir(&dirs[i], 0);
        save_dir_free(&dirs[i]);
    }
    printf("%d exported to %s%s, %d KB free\n", nd, card, created ? " (a new formatted 8 MB card)" : "",
           ps2mc_free_kb(c));
    ps2mc_close(c);
    return 0;
}

#ifdef _WIN32
/* The command line as UTF-8, so paths outside the ANSI code page work (as ggx-headless). */
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
    const char *cmd, *target = NULL, *store = NULL;
    char store_buf[1100];
    uint32_t release = 0;
    int replace = 0, i;

#ifdef _WIN32
    argv = utf8_argv(&argc);
    if (!argv) {
        fprintf(stderr, "ggx-save: cannot read the command line\n");
        return 1;
    }
#endif
    if (argc < 2) {
        usage();
    }
    cmd = argv[1];
    for (i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "--replace")) {
            replace = 1;
        } else if (!strcmp(argv[i], "--save-dir") && i + 1 < argc) {
            store = argv[++i];
        } else if (!strcmp(argv[i], "--release") && i + 1 < argc) {
            i++;
            release = !strcmp(argv[i], "kr") || !strcmp(argv[i], "KR")   ? SAVE_RELEASE_KR
                      : !strcmp(argv[i], "jp") || !strcmp(argv[i], "JP") ? SAVE_RELEASE_JP
                                                                         : 0;
            if (!release) {
                usage();
            }
        } else if (argv[i][0] == '-' || target) {
            usage();
        } else {
            target = argv[i];
        }
    }
    if (!store) {
        if (save_store_dir(store_buf, sizeof(store_buf), NULL) != 0) {
            fprintf(stderr, "ggx-save: no user data directory: set HOME, or pass --save-dir\n");
            return 1;
        }
        store = store_buf;
    }
    if (!strcmp(cmd, "list") && !target) {
        return cmd_list(store);
    }
    if (!target) {
        usage();
    }
    if (!strcmp(cmd, "info")) {
        return cmd_info(target);
    }
    if (!strcmp(cmd, "card")) {
        return cmd_card(target);
    }
    if (!strcmp(cmd, "import")) {
        return cmd_import(target, store, release, replace);
    }
    if (!strcmp(cmd, "export")) {
        return cmd_export(target, store, release, replace);
    }
    usage();
    return 64;
}
