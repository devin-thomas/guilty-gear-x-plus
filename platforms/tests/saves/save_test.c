/* ggx-save-test: the native save format, the export check and PS2 memory card
 * images (PORT-012), with no disc: every save here is made up, in the
 * original layout's shape.
 *
 *   ggx-save-test <scratch dir>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fsys.h"
#include "mc_card.h"
#include "ps2mc.h"
#include "save_file.h"

static int failures;

#define CHECK(c)                                                                                                  \
    do {                                                                                                          \
        if (!(c)) {                                                                                               \
            fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c);                                       \
            failures++;                                                                                           \
        }                                                                                                         \
    } while (0)

/* a refusal whose cause names what it should */
#define REFUSED(call, word)                                                                                       \
    do {                                                                                                          \
        char why_[300] = "";                                                                                      \
        int rc_ = (call);                                                                                         \
        if (rc_ == 0 || !strstr(why_, word)) {                                                                    \
            fprintf(stderr, "%s:%d: expected a refusal naming \"%s\", got %d \"%s\"\n", __FILE__, __LINE__, word, \
                    rc_, why_);                                                                                   \
            failures++;                                                                                           \
        }                                                                                                         \
    } while (0)

/* a system directory in the original layout: settings with a valid checksum, icon.bin, icon.sys */
static void make_save(SaveDir *d, uint32_t release, uint8_t fill)
{
    static uint8_t data[0x1615], icon[0xF1D8], sys[964];
    static const uint8_t t[8] = { 0, 1, 2, 3, 4, 5, 0xe9, 0x07 };
    uint8_t sum = 0;
    unsigned i;

    for (i = 0; i < sizeof(data) - 1; i++) {
        data[i] = (uint8_t)(fill + i * 7);
        sum = (uint8_t)(sum + data[i]);
    }
    data[sizeof(data) - 1] = (uint8_t)(1 - sum);
    for (i = 0; i < sizeof(icon); i++) {
        icon[i] = (uint8_t)(i * 13 + fill);
    }
    memset(sys, 0, sizeof(sys));
    memcpy(sys, "PS2D", 4);
    memcpy(sys + 0x104, "icon.bin", 8);
    save_dir_init(d, save_system_dir(release));
    memcpy(d->created, t, 8);
    memcpy(d->modified, t, 8);
    save_dir_put(d, d->name, SAVE_MODE_FILE, t, t, data, sizeof(data));
    save_dir_put(d, "icon.bin", SAVE_MODE_FILE, t, t, icon, sizeof(icon));
    save_dir_put(d, "icon.sys", SAVE_MODE_FILE, t, t, sys, sizeof(sys));
}

static int same_files(const SaveDir *a, const SaveDir *b)
{
    int i;

    if (a->nfiles != b->nfiles || strcmp(a->name, b->name) != 0) {
        return 0;
    }
    for (i = 0; i < a->nfiles; i++) {
        int j = save_dir_find(b, a->files[i].name);

        if (j < 0 || a->files[i].size != b->files[j].size ||
            memcmp(a->files[i].data, b->files[j].data, a->files[i].size) != 0) {
            return 0;
        }
    }
    return 1;
}

static void test_format(const char *dir)
{
    char path[1100], why[300];
    uint8_t *buf;
    size_t len;
    uint32_t version;
    SaveDir d, back;

    make_save(&d, SAVE_RELEASE_KR, 3);
    CHECK(d.release == SAVE_RELEASE_KR);
    CHECK(save_check_layout(&d, why, sizeof(why)) == 0);
    CHECK(save_check_export(&d, why, sizeof(why)) == 0);

    /* the file round trip, through a temporary file and a rename */
    CHECK(snprintf(path, sizeof(path), "%s/%s.ggxsave", dir, d.name) < (int)sizeof(path));
    CHECK(save_write(path, &d, why, sizeof(why)) == SAVE_OK);
    CHECK(save_read(path, &back, &version, why, sizeof(why)) == SAVE_OK);
    CHECK(version == SAVE_FORMAT_VERSION && back.release == SAVE_RELEASE_KR && same_files(&d, &back));
    CHECK(snprintf(path, sizeof(path), "%s/%s.ggxsave.tmp", dir, d.name) < (int)sizeof(path));
    CHECK(fs_stat(path, NULL, NULL) != 0);
    save_dir_free(&back);

    /* damage, and a newer format */
    CHECK(save_encode(&d, &buf, &len) == SAVE_OK);
    buf[200] ^= 1;
    CHECK(save_decode(buf, len, &back, NULL, why, sizeof(why)) == SAVE_EBAD && strstr(why, "hash"));
    buf[200] ^= 1;
    buf[8] = 2;
    CHECK(save_decode(buf, len, &back, &version, why, sizeof(why)) == SAVE_ENEWER && version == 2 &&
          strstr(why, "version 2"));
    CHECK(save_decode(buf, 40, &back, NULL, why, sizeof(why)) != SAVE_OK);
    free(buf);

    /* every cause export names */
    REFUSED((d.release = SAVE_RELEASE_JP, save_check_export(&d, why_, sizeof(why_))), "JP");
    REFUSED((d.release = 7, save_check_export(&d, why_, sizeof(why_))), "release 7");
    d.release = SAVE_RELEASE_KR;
    CHECK(save_dir_touch(&d, "widescreen-hud", SAVE_EXT_COMPATIBLE) == 0);
    CHECK(save_check_export(&d, why, sizeof(why)) == 0);
    CHECK(save_dir_touch(&d, "new-character", 0) == 0);
    REFUSED(save_check_export(&d, why_, sizeof(why_)), "\"new-character\"");
    /* a mod that once wrote without declaring compatibility stays incompatible */
    CHECK(save_dir_touch(&d, "widescreen-hud", 0) == 0 && d.next == 2 && d.ext[0].flags == 0);
    d.next = 0;
    d.files[0].data[10] ^= 0x40;
    REFUSED(save_check_export(&d, why_, sizeof(why_)), "checksum");
    d.files[0].data[10] ^= 0x40;
    CHECK(save_dir_put(&d, "cheats.bin", SAVE_MODE_FILE, NULL, NULL, "x", 1) == 0);
    REFUSED(save_check_export(&d, why_, sizeof(why_)), "cheats.bin");
    save_dir_remove(&d, save_dir_find(&d, "cheats.bin"));
    CHECK(save_dir_put(&d, "icon.bin", SAVE_MODE_FILE, NULL, NULL, "x", 1) == 0);
    REFUSED(save_check_export(&d, why_, sizeof(why_)), "icon.bin");
    save_dir_free(&d);

    CHECK(save_release_of("BISLPS-20137GGXRP9") == SAVE_RELEASE_JP);
    CHECK(save_release_of("BKSLPM-64525GGXRP") == 0 && save_release_of("BASLUS-20436GGXX") == 0);
}

static void test_ecc(void)
{
    uint8_t chunk[128], e[3];

    /* an erased quarter page, as every BIOS-formatted card has them */
    memset(chunk, 0xFF, sizeof(chunk));
    ps2mc_ecc(chunk, e);
    CHECK(e[0] == 0x77 && e[1] == 0x7f && e[2] == 0x7f);
}

static void test_card(const char *dir)
{
    char path[1100], why[300], names[8][SAVE_NAME_MAX];
    Ps2Card *c;
    SaveDir kr, jp, back;
    FILE *f;
    uint8_t spare[16];
    static const uint8_t bios_sb_ecc[12] = { 0x07, 0x34, 0x4b, 0x77, 0x7f, 0x7f, 0x25, 0x71, 0x0e, 0x77, 0x7f, 0x7f };
    int free0;

    make_save(&kr, SAVE_RELEASE_KR, 5);
    make_save(&jp, SAVE_RELEASE_JP, 9);
    CHECK(ps2mc_create(&c, why, sizeof(why)) == 0);
    free0 = ps2mc_free_kb(c);
    CHECK(free0 == 8134);
    CHECK(ps2mc_write_dir(c, &kr, 0, why, sizeof(why)) == 0);
    CHECK(ps2mc_write_dir(c, &jp, 0, why, sizeof(why)) == 0);
    /* each: 3 clusters of entries, 6 + 61 + 1 of data; and the root's second cluster */
    CHECK(ps2mc_free_kb(c) == free0 - 2 * 71 - 1);
    REFUSED(ps2mc_write_dir(c, &kr, 0, why_, sizeof(why_)), "already holds");
    kr.files[0].data[0] ^= 0xFF;
    kr.files[0].data[1] ^= 0xFF;
    CHECK(ps2mc_write_dir(c, &kr, 1, why, sizeof(why)) == 0);
    CHECK(ps2mc_free_kb(c) == free0 - 2 * 71 - 1);
    snprintf(path, sizeof(path), "%s/card.ps2", dir);
    CHECK(ps2mc_save(c, path, why, sizeof(why)) == 0);
    ps2mc_close(c);

    /* what a PCSX2 8 MB card is: 16384 pages of 528 bytes; the superblock's ECC as the BIOS writes it */
    f = fs_open(path, "rb");
    CHECK(f && fs_size(f) == 8650752);
    if (f) {
        CHECK(fs_seek(f, 512) == 0 && fread(spare, 1, 16, f) == 16 && memcmp(spare, bios_sb_ecc, 12) == 0);
        fclose(f);
    }

    CHECK(ps2mc_open(&c, path, why, sizeof(why)) == 0);
    if (c) {
        CHECK(ps2mc_list(c, names, 8) == 2);
        CHECK(ps2mc_read_dir(c, kr.name, &back, why, sizeof(why)) == 0 && same_files(&kr, &back) &&
              back.release == SAVE_RELEASE_KR);
        save_dir_free(&back);
        CHECK(ps2mc_read_dir(c, jp.name, &back, why, sizeof(why)) == 0 && same_files(&jp, &back));
        save_dir_free(&back);
        CHECK(ps2mc_read_dir(c, "BASLUS-20436GGXX", &back, why, sizeof(why)) == -1);
        ps2mc_close(c);
    }

    /* a folder memory card: a folder per save */
    snprintf(path, sizeof(path), "%s/folder/%s", dir, jp.name);
    CHECK(fs_mkdirs(path) == 0);
    {
        int i;

        for (i = 0; i < jp.nfiles; i++) {
            char p[1300];

            snprintf(p, sizeof(p), "%s/%s", path, jp.files[i].name);
            f = fs_open(p, "wb");
            CHECK(f && fwrite(jp.files[i].data, 1, jp.files[i].size, f) == jp.files[i].size);
            if (f) {
                fclose(f);
            }
        }
        snprintf(path, sizeof(path), "%s/folder/%s/_pcsx2_index", dir, jp.name);
        f = fs_open(path, "wb");
        if (f) {
            fclose(f);
        }
    }
    snprintf(path, sizeof(path), "%s/folder", dir);
    CHECK(ps2mc_open(&c, path, why, sizeof(why)) == 0 && ps2mc_is_folder(c));
    if (c) {
        CHECK(ps2mc_list(c, names, 8) == 1 && !strcmp(names[0], jp.name));
        CHECK(ps2mc_read_dir(c, jp.name, &back, why, sizeof(why)) == 0 && same_files(&jp, &back));
        save_dir_free(&back);
        REFUSED(ps2mc_write_dir(c, &kr, 0, why_, sizeof(why_)), "folder");
        ps2mc_close(c);
    }
    save_dir_free(&kr);
    save_dir_free(&jp);
}

/* the card the shim uses: the store read back, writes committed, an unreadable save left alone */
static void test_mc_card(const char *dir)
{
    char store[1100], path[1200], why[300];
    static const char *const ext[] = { "test-mod" };
    McCard *c;
    SaveDir d, back;
    uint8_t *buf;
    size_t len;
    FILE *f;

    CHECK(snprintf(store, sizeof(store), "%s/store", dir) < (int)sizeof(store));
    snprintf(path, sizeof(path), "%s/BKSLPM-64525GGX.ggxsave", store);
    fs_remove(path);
    c = mc_card_open(store, SAVE_RELEASE_KR, NULL);
    if (!c) {
        failures++;
        return;
    }
    CHECK(c && mc_card_count(c) == 0 && mc_card_free_kb(c) == MC_CARD_CAPACITY_KB);
    CHECK(mc_card_mkdir(c, "BKSLPM-64525GGX") == MC_RES_OK);
    CHECK(mc_card_mkdir(c, "BKSLPM-64525GGX") == MC_RES_NO_ENTRY);
    CHECK(mc_card_create(c, 0, "icon.sys") == MC_RES_OK);
    CHECK(mc_card_write(c, 0, 0, 4, (const uint8_t *)"abcd", 4) == MC_RES_OK);
    CHECK(mc_card_count(c) == 1 && mc_card_at(c, 0)->files[0].size == 8);
    CHECK(mc_card_rmdir(c, 0) == MC_RES_NOT_EMPTY);
    mc_card_set_extensions(c, ext, NULL, 1);
    CHECK(mc_card_commit(c, "BKSLPM-64525GGX") == 0);
    mc_card_close(c);

    c = mc_card_open(store, SAVE_RELEASE_KR, NULL);
    CHECK(c && mc_card_count(c) == 1 && mc_card_at(c, 0)->files[0].size == 8 && mc_card_at(c, 0)->next == 1 &&
          !strcmp(mc_card_at(c, 0)->ext[0].id, "test-mod"));
    CHECK(mc_card_unlink(c, 0, 0) == MC_RES_OK && mc_card_rmdir(c, 0) == MC_RES_OK);
    CHECK(mc_card_commit(c, "BKSLPM-64525GGX") == 0);
    CHECK(fs_stat(path, NULL, NULL) != 0);
    mc_card_close(c);

    /* a save in a newer format: not shown to the game, and not overwritten */
    make_save(&d, SAVE_RELEASE_KR, 1);
    CHECK(save_encode(&d, &buf, &len) == SAVE_OK);
    buf[8] = 2;
    f = fs_open(path, "wb");
    CHECK(f && fwrite(buf, 1, len, f) == len);
    if (f) {
        fclose(f);
    }
    c = mc_card_open(store, SAVE_RELEASE_KR, NULL);
    CHECK(c && mc_card_count(c) == 0 && mc_card_mkdir(c, "BKSLPM-64525GGX") == MC_RES_DENIED);
    mc_card_close(c);
    CHECK(save_read(path, &back, NULL, why, sizeof(why)) == SAVE_ENEWER);
    free(buf);
    save_dir_free(&d);

    /* an empty card in memory writes nothing */
    c = mc_card_open(NULL, SAVE_RELEASE_JP, NULL);
    CHECK(c && mc_card_mkdir(c, "BISLPS-20137GGX") == MC_RES_OK && mc_card_commit(c, "BISLPS-20137GGX") == 0);
    mc_card_close(c);
}

int main(int argc, char **argv)
{
    char dir[1100];

    if (argc != 2) {
        fprintf(stderr, "usage: ggx-save-test <scratch dir>\n");
        return 64;
    }
    snprintf(dir, sizeof(dir), "%s/saves-test.tmp", argv[1]);
    if (fs_mkdirs(dir) != 0) {
        fprintf(stderr, "ggx-save-test: cannot make %s\n", dir);
        return 1;
    }
    test_format(dir);
    test_ecc();
    test_card(dir);
    test_mc_card(dir);
    if (failures) {
        fprintf(stderr, "ggx-save-test: %d failed\n", failures);
        return 1;
    }
    printf("ggx-save-test: all passed\n");
    return 0;
}
