/* fpu_test <vectors>: the EE-FPU Model against a file of results.
 *
 * Each vector is one instruction's operands and the result a reference gave
 * for them: PCSX2's EE FPU interpreter for ee_fpu_vectors.txt.
 * A line `## <title>` starts a section, counted on its own. A vector written
 * `~<op> ...` is one the model is known to disagree with: it must differ, so
 * the note can't go stale. Exit status 1 if the model differs on any other. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ee_fpu.h"

struct op {
    const char *name;
    int arity;
    unsigned long count, bad;
};

static struct op ops[] = {
    {"add", 2, 0, 0}, {"sub", 2, 0, 0}, {"mul", 2, 0, 0}, {"div", 2, 0, 0}, {"sqrt", 1, 0, 0},
    {"rsqrt", 2, 0, 0}, {"madd", 3, 0, 0}, {"msub", 3, 0, 0}, {"madda", 3, 0, 0}, {"msuba", 3, 0, 0},
    {"abs", 1, 0, 0}, {"neg", 1, 0, 0}, {"max", 2, 0, 0}, {"min", 2, 0, 0}, {"c.eq", 2, 0, 0},
    {"c.lt", 2, 0, 0}, {"c.le", 2, 0, 0}, {"cvt.s.w", 1, 0, 0}, {"cvt.w.s", 1, 0, 0},
};
#define NOPS (sizeof(ops) / sizeof(ops[0]))

static uint32_t model(int k, uint32_t a, uint32_t b, uint32_t c)
{
    switch (k) {
    case 0: return ee_fpu_add(a, b);
    case 1: return ee_fpu_sub(a, b);
    case 2: return ee_fpu_mul(a, b);
    case 3: return ee_fpu_div(a, b);
    case 4: return ee_fpu_sqrt(a);
    case 5: return ee_fpu_rsqrt(a, b);
    case 6: return ee_fpu_madd(a, b, c);
    case 7: return ee_fpu_msub(a, b, c);
    case 8: return ee_fpu_madda(a, b, c);
    case 9: return ee_fpu_msuba(a, b, c);
    case 10: return ee_fpu_abs(a);
    case 11: return ee_fpu_neg(a);
    case 12: return ee_fpu_max(a, b);
    case 13: return ee_fpu_min(a, b);
    case 14: return (uint32_t)ee_fpu_c_eq(a, b);
    case 15: return (uint32_t)ee_fpu_c_lt(a, b);
    case 16: return (uint32_t)ee_fpu_c_le(a, b);
    case 17: return ee_fpu_cvt_s_w((int32_t)a);
    default: return (uint32_t)ee_fpu_cvt_w_s(a);
    }
}

#define NSECTIONS 16

struct section {
    char title[96];
    unsigned long count, bad, disagree;
};

static struct section sections[NSECTIONS];
static int nsections;

int main(int argc, char **argv)
{
    char line[256], name[16];
    unsigned long total = 0, bad = 0, lineno = 0, stale = 0;
    struct section *sec = NULL;
    FILE *f;
    size_t k;
    int j;

    if (argc != 2) {
        fprintf(stderr, "usage: fpu_test <vectors>\n");
        return 2;
    }
    f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 2;
    }
    while (fgets(line, sizeof line, f)) {
        unsigned long v[4] = {0, 0, 0, 0};
        uint32_t x[4], got;
        int n, i, disagree = line[0] == '~';
        const char *row = disagree ? line + 1 : line;

        lineno++;
        if (!strncmp(line, "## ", 3)) {
            if (nsections == NSECTIONS) {
                fprintf(stderr, "%s:%lu: too many sections\n", argv[1], lineno);
                return 2;
            }
            sec = &sections[nsections++];
            sscanf(line + 3, "%95[^\n]", sec->title);
            continue;
        }
        if (line[0] == '#' || line[0] == '\n')
            continue;
        n = sscanf(row, "%15s %lx %lx %lx %lx", name, &v[0], &v[1], &v[2], &v[3]);
        for (k = 0; k < NOPS && strcmp(ops[k].name, name); k++)
            ;
        if (k == NOPS || n != ops[k].arity + 2) {
            fprintf(stderr, "%s:%lu: bad vector: %s", argv[1], lineno, line);
            return 2;
        }
        for (i = 0; i < 4; i++)
            x[i] = (uint32_t)v[i];
        got = model((int)k, x[0], ops[k].arity >= 2 ? x[1] : 0, ops[k].arity == 3 ? x[2] : 0);
        if (sec)
            sec->count++;
        if (disagree) {
            if (sec)
                sec->disagree++;
            if (got == x[ops[k].arity]) {
                fprintf(stderr, "%s:%lu: model now agrees (drop the ~): %s", argv[1], lineno, line);
                stale++;
            } else {
                printf("known disagreement, model gives %08x: %s", got, row);
            }
            continue;
        }
        ops[k].count++;
        total++;
        if (got != x[ops[k].arity]) {
            if (bad++ < 20)
                fprintf(stderr, "%s:%lu: model gives %08x: %s", argv[1], lineno, got, line);
            ops[k].bad++;
            if (sec)
                sec->bad++;
        }
    }
    fclose(f);
    for (k = 0; k < NOPS; k++)
        if (ops[k].count)
            printf("%-8s %6lu vectors, %lu differ\n", ops[k].name, ops[k].count, ops[k].bad);
    for (j = 0; j < nsections; j++)
        printf("%5lu vectors, %lu differ, %lu known disagreements: %s\n", sections[j].count - sections[j].disagree,
               sections[j].bad, sections[j].disagree, sections[j].title);
    printf("%lu vectors, %lu differ\n", total, bad);
    return bad != 0 || stale != 0 || total == 0;
}
