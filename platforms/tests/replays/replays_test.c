/* ggx-replays-test: the replay format and its refusals (PORT-013), with no
 * disc: every replay here is made up (rp_synthetic).
 *
 *   ggx-replays-test <scratch dir>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "replays.h"

static int failures;

#define CHECK(c)                                                                                                  \
    do {                                                                                                          \
        if (!(c)) {                                                                                               \
            fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #c);                                       \
            failures++;                                                                                           \
        }                                                                                                         \
    } while (0)

/* a refusal whose cause names what it should */
static void refused(int line, const Replay *r, const RpBuild *b, const char *word)
{
    char why[400] = "";

    if (rp_compatible(r, b, why, sizeof(why)) == 0 || !strstr(why, word)) {
        fprintf(stderr, "%s:%d: expected a refusal naming \"%s\", got \"%s\"\n", __FILE__, line, word, why);
        failures++;
    }
}

static void undecodable(int line, const uint8_t *b, size_t n, const char *word)
{
    char why[400] = "";
    Replay *r = rp_decode(b, n, why, sizeof(why));

    if (r || !strstr(why, word)) {
        fprintf(stderr, "%s:%d: expected the bytes refused naming \"%s\", got \"%s\"\n", __FILE__, line, word, why);
        failures++;
    }
    rp_free(r);
}

/* the file's hash, made right again after a change */
static void rehash(uint8_t *b, size_t n);

int main(int argc, char **argv)
{
    RpBuild here, b;
    Replay *r, *back;
    uint8_t *bytes, *again, *bad;
    size_t n, n2;
    char why[400], path[1024];
    uint32_t i, field, kind;
    int ndirs, nfiles;

    if (argc != 2) {
        fprintf(stderr, "usage: ggx-replays-test <scratch dir>\n");
        return 64;
    }
    memset(&here, 0, sizeof(here));
    here.release = 1;
    for (i = 0; i < 32; i++) {
        here.elf_sha256[i] = (uint8_t)(0xef - i);
    }
    here.state_hash_version = CORE_STATE_HASH_VERSION;
    here.core_build = 0x0123456789abcdefull;
    here.n_extensions = 1;
    snprintf(here.extensions[0], RP_EXT_ID_MAX, "%s", "ggx-test.compatible");

    /* round trip: the same bytes again, the same records */
    r = rp_synthetic(&here, 5000);
    CHECK(r != NULL);
    CHECK(rp_encode(r, &bytes, &n) == 0);
    back = rp_decode(bytes, n, why, sizeof(why));
    CHECK(back != NULL);
    if (!back) {
        fprintf(stderr, "  %s\n", why);
        return 1;
    }
    CHECK(rp_fields(back) == 5000);
    for (i = 0; i < 5000; i++) {
        CHECK(session_input_equal(rp_input(back, i), rp_input(r, i)));
    }
    CHECK(rp_input(back, 5000) == NULL);
    CHECK(rp_check(back, 120, 0x1234567890abcdefull ^ 120, why, sizeof(why)) == 0);
    CHECK(rp_check(back, 120, 1, why, sizeof(why)) != 0 && strstr(why, "field 120"));
    CHECK(rp_check(back, 121, 1, why, sizeof(why)) == 0);   /* no check there */
    CHECK(rp_marks(back) == 1);
    rp_mark_at(back, 0, &field, &kind);
    CHECK(field == 2500 && kind == RP_MARK_FIGHT);
    CHECK(rp_card(back, 0, &ndirs) == 1 && ndirs == 1);
    CHECK(!strcmp(rp_card_dir(back, 0, 0, &nfiles), "BKSLPM-64525GGX") && nfiles == 2);
    CHECK(rp_card(back, 1, &ndirs) == 0);
    CHECK(!strcmp(rp_build(back)->extensions[0], "ggx-test.compatible"));
    CHECK(rp_encode(back, &again, &n2) == 0 && n2 == n && !memcmp(again, bytes, n));
    /* the inputs are kept as runs: 5000 fields in far fewer bytes */
    CHECK(n < 5000 * 4);
    printf("a made-up replay of 5000 fields: %zu bytes, read back the same\n", n);
    free(again);

    /* playing on this build */
    CHECK(rp_compatible(back, &here, why, sizeof(why)) == 0);
    b = here;
    b.release = 2;
    refused(__LINE__, back, &b, "KR (SLPM-645.25) release");
    b = here;
    b.elf_sha256[5] ^= 1;
    refused(__LINE__, back, &b, "boot ELF");
    b = here;
    b.state_hash_version = 2;
    refused(__LINE__, back, &b, "State Hash version");
    b = here;
    b.core_build ^= 1;
    refused(__LINE__, back, &b, "another build of the port's Core");
    b = here;
    b.n_extensions = 0;
    refused(__LINE__, back, &b, "recorded with the extension \"ggx-test.compatible\"");
    b = here;
    snprintf(b.extensions[1], RP_EXT_ID_MAX, "%s", "ggx-test.other");
    b.n_extensions = 2;
    refused(__LINE__, back, &b, "this run has the extension \"ggx-test.other\" active");

    /* files that aren't replays this build reads */
    undecodable(__LINE__, (const uint8_t *)"GGXSAVE\0", 8, "isn't a replay file");
    bad = malloc(n);
    memcpy(bad, bytes, n);
    bad[n / 2] ^= 1;
    undecodable(__LINE__, bad, n, "damaged");
    memcpy(bad, bytes, n);
    bad[8] = 2;
    rehash(bad, n);
    undecodable(__LINE__, bad, n, "replay format version 2, and this build reads version 1");
    memcpy(bad, bytes, n);
    bad[68] ^= 1;   /* the field count */
    rehash(bad, n);
    undecodable(__LINE__, bad, n, "cut short or malformed");
    free(bad);

    /* to a file and back */
    snprintf(path, sizeof(path), "%s/replays-test.ggxreplay", argv[1]);
    {
        FILE *f = fopen(path, "wb");

        CHECK(f && fwrite(bytes, 1, n, f) == n);
        if (f) {
            fclose(f);
        }
    }
    rp_free(back);
    back = rp_open(path, why, sizeof(why));
    CHECK(back && rp_fields(back) == 5000);
    remove(path);

    free(bytes);
    rp_free(back);
    rp_free(r);
    printf("%s\n", failures ? "FAILED" : "PASSED");
    return failures ? 1 : 0;
}

/* XXH3-64 of everything but the last 8 bytes, which hold it: the same rule as rp_decode's */
#define XXH_INLINE_ALL
#include "xxhash.h"

static void rehash(uint8_t *b, size_t n)
{
    uint64_t h = XXH3_64bits(b, n - 8);
    int i;

    for (i = 0; i < 8; i++) {
        b[n - 8 + i] = (uint8_t)(h >> (8 * i));
    }
}
