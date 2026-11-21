/* GGScript input scripts (ggscript.h), parsed and played as the gg-layers
 * PCSX2 fork's GGScript.cpp does. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fsys.h"
#include "ggscript.h"
#include "pad.h"

enum { STEP_HOLD, STEP_STICK, STEP_SHOT, STEP_QUIT };

typedef struct {
    uint8_t kind;
    uint32_t frames;
    uint16_t buttons[2];
    uint8_t stick[4];
    uint32_t line;
    char *arg;
} Step;

struct GgScript {
    Step *steps;
    size_t n, cap, pos;
    uint32_t left;
    uint16_t held[2];
    uint8_t stick[4];
    uint64_t frame;
    uint32_t line;
    int done;
    char shots[512];
    size_t shots_len;
};

static const char LETTERS[] = "PKSHD";

/* Splits s in place at any of the separators; up to max tokens. */
static int split(char *s, const char *seps, char **tok, int max)
{
    int n = 0;

    while (*s && n < max) {
        while (*s && strchr(seps, *s)) {
            *s++ = 0;
        }
        if (!*s) {
            break;
        }
        tok[n++] = s;
        while (*s && !strchr(seps, *s)) {
            s++;
        }
    }
    return n;
}

static int numpad(char c, int face_right)
{
    int fwd = face_right ? PAD_RIGHT : PAD_LEFT, back = face_right ? PAD_LEFT : PAD_RIGHT;

    switch (c) {
    case '1': return PAD_DOWN | back;
    case '2': return PAD_DOWN;
    case '3': return PAD_DOWN | fwd;
    case '4': return back;
    case '6': return fwd;
    case '7': return PAD_UP | back;
    case '8': return PAD_UP;
    case '9': return PAD_UP | fwd;
    default: return 0;
    }
}

/* One input: a numpad digit and letters, or PS2 names joined with '+'. -1 if
   it isn't one. */
static int parse_input(const char *in, const int letters[5], int face_right)
{
    int mask = 0;

    if (strchr(in, '+') || (strlen(in) > 1 && pad_button_by_name(in))) {
        char buf[128], *names[16];
        int n, i;

        if (strlen(in) >= sizeof(buf) || in[0] == '+' || in[strlen(in) - 1] == '+' || strstr(in, "++")) {
            return -1;
        }
        strcpy(buf, in);
        n = split(buf, "+", names, 16);
        for (i = 0; i < n; i++) {
            int b = pad_button_by_name(names[i]);

            if (!b) {
                return -1;
            }
            mask |= b;
        }
        return mask;
    }
    for (; *in; in++) {
        const char *l = strchr(LETTERS, *in);

        if (*in >= '1' && *in <= '9') {
            mask |= numpad(*in, face_right);
        } else if (l) {
            mask |= letters[l - LETTERS];
        } else {
            return -1;
        }
    }
    return mask;
}

static int to_u32(const char *s, uint32_t *out)
{
    char *end;
    unsigned long v;

    if (!*s || *s == '-' || *s == '+') {
        return -1;
    }
    v = strtoul(s, &end, 10);
    if (*end || v > 0xFFFFFFFFul) {
        return -1;
    }
    *out = (uint32_t)v;
    return 0;
}

static Step *push(GgScript *s, int kind, uint32_t line)
{
    Step *st;

    if (s->n == s->cap) {
        size_t cap = s->cap ? s->cap * 2 : 256;
        Step *p = realloc(s->steps, cap * sizeof(Step));

        if (!p) {
            return NULL;
        }
        s->steps = p;
        s->cap = cap;
    }
    st = &s->steps[s->n++];
    memset(st, 0, sizeof(*st));
    st->kind = (uint8_t)kind;
    st->line = line;
    return st;
}

GgScript *ggs_parse(const char *text, const char *name, char *err, size_t cap)
{
    GgScript *s = calloc(1, sizeof(GgScript));
    int letters[5] = { PAD_SQUARE, PAD_CROSS, PAD_TRIANGLE, PAD_CIRCLE, PAD_R1 };
    int face[2] = { 1, 1 };
    uint32_t line_no = 0;
    const char *p = text;

    if (!s) {
        snprintf(err, cap, "out of memory");
        return NULL;
    }
    memset(s->stick, 127, 4);   /* the fork's rest value */
    while (*p) {
        char line[512], *tok[8], *h;
        const char *nl = strchr(p, '\n');
        size_t len = nl ? (size_t)(nl - p) : strlen(p);
        int nt = 0;
        Step *st = NULL;

        line_no++;
        if (len >= sizeof(line)) {
            snprintf(err, cap, "%s:%u: line too long", name, line_no);
            ggs_free(s);
            return NULL;
        }
        memcpy(line, p, len);
        line[len] = 0;
        p += len + (nl ? 1 : 0);
        if ((h = strchr(line, '#')) != NULL) {
            *h = 0;
        }
        nt = split(line, " \t\r", tok, 8);
        if (nt == 0) {
            continue;
        }
        if (!strcmp(tok[0], "map")) {
            int i;

            for (i = 1; i < nt; i++) {
                const char *l = tok[i][0] ? strchr(LETTERS, tok[i][0]) : NULL;
                int b = l && tok[i][1] == '=' ? pad_button_by_name(tok[i] + 2) : 0;

                if (!b) {
                    goto bad;
                }
                letters[l - LETTERS] = b;
            }
            continue;
        } else if ((!strcmp(tok[0], "face") || !strcmp(tok[0], "face2")) && nt == 2) {
            face[tok[0][4] == '2'] = strcmp(tok[1], "left") != 0;
            continue;
        } else if (!strcmp(tok[0], "wait") && nt == 2) {
            st = push(s, STEP_HOLD, line_no);
            if (!st || to_u32(tok[1], &st->frames) != 0) {
                goto bad;
            }
        } else if (!strcmp(tok[0], "shot") && nt == 2) {
            st = push(s, STEP_SHOT, line_no);
            if (!st || !(st->arg = malloc(strlen(tok[1]) + 1))) {
                goto bad;
            }
            strcpy(st->arg, tok[1]);
        } else if (!strcmp(tok[0], "quit") && nt == 1) {
            if (!push(s, STEP_QUIT, line_no)) {
                goto bad;
            }
        } else if (!strcmp(tok[0], "stick") && nt == 5) {
            int i;

            st = push(s, STEP_STICK, line_no);
            for (i = 0; st && i < 4; i++) {
                uint32_t v;

                if (to_u32(tok[i + 1], &v) != 0 || v > 255) {
                    goto bad;
                }
                st->stick[i] = (uint8_t)v;
            }
        } else if (!strcmp(tok[0], "turbo") || !strcmp(tok[0], "rec") || !strcmp(tok[0], "save") ||
                   !strcmp(tok[0], "log")) {
            continue;   /* PCSX2's side only */
        } else if (nt == 2 || nt == 3) {
            int m1, m2 = 0;

            st = push(s, STEP_HOLD, line_no);
            if (!st || to_u32(tok[0], &st->frames) != 0) {
                goto bad;
            }
            m1 = parse_input(tok[1], letters, face[0]);
            if (nt == 3) {
                m2 = parse_input(tok[2], letters, face[1]);
            }
            if (m1 < 0 || m2 < 0) {
                goto bad;
            }
            st->buttons[0] = (uint16_t)m1;
            st->buttons[1] = (uint16_t)m2;
        } else {
            goto bad;
        }
        continue;
bad:
        snprintf(err, cap, "%s:%u: not understood", name, line_no);
        ggs_free(s);
        return NULL;
    }
    return s;
}

GgScript *ggs_load(const char *path, char *err, size_t cap)
{
    FILE *f = fs_open(path, "rb");
    int64_t n = f ? fs_size(f) : -1;
    char *text = n >= 0 ? malloc((size_t)n + 1) : NULL;
    GgScript *s = NULL;

    if (!text || fs_seek(f, 0) != 0 || fread(text, 1, (size_t)n, f) != (size_t)n) {
        snprintf(err, cap, "cannot read %s", path);
    } else {
        text[n] = 0;
        s = ggs_parse(text, path, err, cap);
    }
    if (f) {
        fclose(f);
    }
    free(text);
    return s;
}

void ggs_free(GgScript *s)
{
    size_t i;

    if (!s) {
        return;
    }
    for (i = 0; i < s->n; i++) {
        free(s->steps[i].arg);
    }
    free(s->steps);
    free(s);
}

int ggs_frame(GgScript *s, uint16_t buttons[2], uint8_t stick[4])
{
    s->frame++;
    while (!s->done && s->left == 0) {
        const Step *st;

        if (s->pos >= s->n) {
            s->held[0] = s->held[1] = 0;
            s->done = GGS_DONE;
            break;
        }
        st = &s->steps[s->pos++];
        s->line = st->line;
        switch (st->kind) {
        case STEP_HOLD:
            s->left = st->frames;
            s->held[0] = st->buttons[0];
            s->held[1] = st->buttons[1];
            break;
        case STEP_STICK:
            memcpy(s->stick, st->stick, 4);
            break;
        case STEP_SHOT: {
            size_t l = strlen(st->arg);

            if (s->shots_len + l + 2 <= sizeof(s->shots)) {
                memcpy(s->shots + s->shots_len, st->arg, l + 1);
                s->shots_len += l + 1;
                s->shots[s->shots_len] = 0;
            }
            break;
        }
        case STEP_QUIT:
            s->held[0] = s->held[1] = 0;
            s->done = GGS_QUIT;
            break;
        }
    }
    if (!s->done) {
        s->left--;
    }
    buttons[0] = s->held[0];
    buttons[1] = s->held[1];
    memcpy(stick, s->stick, 4);
    return s->done;
}

const char *ggs_shots(const GgScript *s)
{
    return s->shots;
}

uint64_t ggs_frames(const GgScript *s)
{
    return s->frame;
}

uint32_t ggs_line(const GgScript *s)
{
    return s->line;
}

int ggs_play_to(GgScript *s, uint64_t frame, PadInput pads[2])
{
    int st = s->done;

    s->shots_len = 0;
    s->shots[0] = 0;
    while (s->frame < frame && st != GGS_QUIT) {
        uint8_t stick[4];
        uint16_t b[2];

        st = ggs_frame(s, b, stick);
        pads[0].buttons = b[0];
        pads[1].buttons = b[1];
        /* PadInput's order is rx, ry, lx, ly */
        pads[0].analog[0] = stick[2], pads[0].analog[1] = stick[3];
        pads[0].analog[2] = stick[0], pads[0].analog[3] = stick[1];
    }
    return st;
}
