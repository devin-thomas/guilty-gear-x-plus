/* ggx's first run (firstrun.h). */
#include "firstrun.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include "assets.h"
#include "fsys.h"
#include "png_write.h"

#if defined(VERSION_KR)
#define OWN "KR"
#define OTHER "JP"
#else
#define OWN "JP"
#define OTHER "KR"
#endif

#define HANDED_OVER "GGX_HANDED_OVER"

/* ---- the picture ---- */

typedef struct {
    uint8_t *rgba;
    uint8_t r, g, b;
} Pen;

static void plot(void *ctx, int x, int y)
{
    Pen *p = ctx;
    uint8_t *o;

    if (x < 0 || y < 0 || x >= FR_W || y >= FR_H) {
        return;
    }
    o = p->rgba + 4 * ((size_t)y * FR_W + (size_t)x);
    o[0] = p->r, o[1] = p->g, o[2] = p->b, o[3] = 255;
}

static void fill(uint8_t *rgba, int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b)
{
    int x, y;

    for (y = y0 < 0 ? 0 : y0; y < y1 && y < FR_H; y++) {
        for (x = x0 < 0 ? 0 : x0; x < x1 && x < FR_W; x++) {
            uint8_t *o = rgba + 4 * ((size_t)y * FR_W + (size_t)x);

            o[0] = r, o[1] = g, o[2] = b, o[3] = 255;
        }
    }
}

static uint32_t utf8(const char **s)
{
    const unsigned char *p = (const unsigned char *)*s;
    uint32_t c = *p++;
    int more = c >= 0xf0 ? 3 : c >= 0xe0 ? 2 : c >= 0xc0 ? 1 : 0;

    if (more) {
        c &= 0x3fu >> more;
        while (more-- && (*p & 0xc0) == 0x80) {
            c = c << 6 | (*p++ & 0x3fu);
        }
    }
    *s = (const char *)p;
    return c;
}

typedef struct {
    TlFonts *f;
    int font;
    float px, scale, tracking;   /* tracking: extra pixels after each letter */
    TlMetrics m;
} Face;

static Face face(TlFonts *f, int font, float px, float tracking)
{
    Face fc;

    fc.f = f, fc.font = font, fc.px = px, fc.tracking = tracking;
    tl_font_metrics(f, font & 0xff, px, &fc.m);
    fc.scale = fc.m.scale;
    return fc;
}

/* the width of n bytes of s */
static float width(const Face *fc, const char *s, size_t n)
{
    const char *e = s + n;
    float w = 0;

    while (s < e) {
        uint32_t c = utf8(&s), next = 0;

        if (s < e) {
            const char *t = s;

            next = utf8(&t);
        }
        w += tl_font_advance(fc->f, fc->font & 0xff, fc->scale, c, next) + fc->tracking + (fc->font & TL_BOLD ? 1 : 0);
    }
    return w;
}

static void text(const Face *fc, uint8_t *rgba, float x, int baseline, const char *s, size_t n, uint8_t r, uint8_t g,
                 uint8_t b, int shadow)
{
    const char *e = s + n;
    Pen pen;

    pen.rgba = rgba;
    while (s < e) {
        uint32_t c = utf8(&s), next = 0;
        float adv;

        if (s < e) {
            const char *t = s;

            next = utf8(&t);
        }
        if (shadow) {
            pen.r = 0x30, pen.g = 0, pen.b = 0;
            tl_font_glyph(fc->f, fc->font, fc->scale, c, x + 2, baseline + 2, plot, &pen);
        }
        pen.r = r, pen.g = g, pen.b = b;
        tl_font_glyph(fc->f, fc->font, fc->scale, c, x, baseline, plot, &pen);
        adv = tl_font_advance(fc->f, fc->font & 0xff, fc->scale, c, next) + fc->tracking + (fc->font & TL_BOLD ? 1 : 0);
        x += adv;
    }
}

static void centred(const Face *fc, uint8_t *rgba, int baseline, const char *s, uint8_t r, uint8_t g, uint8_t b,
                    int shadow)
{
    float w = width(fc, s, strlen(s));

    text(fc, rgba, (FR_W - w) / 2, baseline, s, strlen(s), r, g, b, shadow);
}

/* s wrapped to w pixels from (x, baseline), a line every lh; returns the
   baseline after the last line. A word wider than a line is broken. */
static int wrapped(const Face *fc, uint8_t *rgba, int x, int baseline, int w, int lh, const char *s, uint8_t r,
                   uint8_t g, uint8_t b, int max_y)
{
    while (*s && baseline <= max_y) {
        const char *line = s, *brk = NULL, *p = s;

        /* the longest run of whole words that fits */
        while (*p && *p != '\n') {
            const char *q = p;

            while (*q && *q != ' ' && *q != '\n') {
                q++;
            }
            if (width(fc, line, (size_t)(q - line)) > (float)w) {
                break;
            }
            brk = q;
            p = *q == ' ' ? q + 1 : q;
        }
        if (!brk) {
            /* one word too long for the line: as many characters as fit */
            const char *q = line, *t;

            while (*q && *q != '\n') {
                t = q;
                utf8(&t);
                if (width(fc, line, (size_t)(t - line)) > (float)w && q > line) {
                    break;
                }
                q = t;
            }
            brk = q;
        }
        text(fc, rgba, (float)x, baseline, line, (size_t)(brk - line), r, g, b, 0);
        baseline += lh;
        s = brk;
        while (*s == ' ') {
            s++;
        }
        if (*s == '\n') {
            s++;
            if (*s == '\n') {
                /* a blank line: half a line */
                baseline += lh / 2;
                s++;
            }
        }
    }
    return baseline;
}

/* a button prompt: the label in a light disc, then what it does */
static float prompt(const Face *fc, uint8_t *rgba, float x, int baseline, const char *label, const char *does)
{
    float lw = width(fc, label, strlen(label)), pad = 6;
    int top = baseline - (int)fc->m.ascent - 3, bottom = baseline + (int)fc->m.descent + 3;

    fill(rgba, (int)x, top, (int)(x + lw + 2 * pad), bottom, 0xe8, 0xe0, 0xd8);
    text(fc, rgba, x + pad, baseline, label, strlen(label), 0x20, 0x08, 0x08, 0);
    x += lw + 2 * pad + 8;
    text(fc, rgba, x, baseline, does, strlen(does), 0xf0, 0xe8, 0xe0, 0);
    return x + width(fc, does, strlen(does)) + 28;
}

void fr_draw(TlFonts *fonts, const FrScreen *s, uint8_t *rgba)
{
    Face title = face(fonts, TL_FONT_SERIF | TL_BOLD, 30, 3), head = face(fonts, TL_FONT_SANS | TL_BOLD, 16, 1.5f);
    Face body = face(fonts, TL_FONT_SANS, 14, 0), small = face(fonts, TL_FONT_SANS, 13, 0);
    int y, x;

    /* the game's colours: black into a deep red, a red band under the title */
    for (y = 0; y < FR_H; y++) {
        int t = y * 256 / FR_H;

        fill(rgba, 0, y, FR_W, y + 1, (uint8_t)(0x0c + (0x48 * t >> 8)), (uint8_t)(0x04 + (0x06 * t >> 8)),
             (uint8_t)(0x06 + (0x08 * t >> 8)));
    }
    for (y = 0; y < FR_H; y += 2) {
        /* every other line a shade darker, as the game's menus are */
        for (x = 0; x < FR_W; x++) {
            uint8_t *o = rgba + 4 * ((size_t)y * FR_W + (size_t)x);

            o[0] = (uint8_t)(o[0] * 7 / 8), o[1] = (uint8_t)(o[1] * 7 / 8), o[2] = (uint8_t)(o[2] * 7 / 8);
        }
    }
    fill(rgba, 0, 96, FR_W, 99, 0xc0, 0x10, 0x18);
    fill(rgba, 0, 100, FR_W, 101, 0x60, 0x08, 0x0c);
    centred(&title, rgba, 66, "GUILTY GEAR X PLUS", 0xff, 0xf8, 0xf0, 1);
    centred(&small, rgba, 88, "NATIVE PORT", 0xd0, 0x90, 0x80, 0);
    if (s->heading) {
        centred(&head, rgba, 132, s->heading, 0xff, 0xd8, 0x60, 1);
    }
    y = wrapped(&body, rgba, 72, 168, FR_W - 144, 21, s->body ? s->body : "", 0xf0, 0xe8, 0xe0,
                s->kind == FR_SCREEN_EXTRACT ? 300 : 372);
    if (s->kind == FR_SCREEN_EXTRACT) {
        int x0 = 72, x1 = FR_W - 72, by = 318;
        int fillx = s->total ? x0 + (int)((double)(x1 - x0) * (double)s->done / (double)s->total) : x0;
        char line[160];

        fill(rgba, x0 - 2, by - 2, x1 + 2, by + 14, 0x80, 0x70, 0x68);
        fill(rgba, x0, by, x1, by + 12, 0x18, 0x08, 0x08);
        fill(rgba, x0, by, fillx, by + 12, 0xd8, 0x28, 0x20);
        fill(rgba, x0, by, fillx, by + 3, 0xff, 0x80, 0x60);
        snprintf(line, sizeof(line), "%s", s->file ? s->file : "");
        text(&small, rgba, (float)x0, by + 34, line, strlen(line), 0xd0, 0xc8, 0xc0, 0);
        snprintf(line, sizeof(line), "%llu of %llu MB  %d%%", (unsigned long long)(s->done >> 20),
                 (unsigned long long)(s->total >> 20), s->total ? (int)(s->done * 100 / s->total) : 0);
        text(&small, rgba, x1 - width(&small, line, strlen(line)), by + 34, line, strlen(line), 0xd0, 0xc8, 0xc0, 0);
    }
    (void)y;
    if (s->confirm_does || s->back_does) {
        float px = 72;

        fill(rgba, 0, 396, FR_W, 397, 0x60, 0x08, 0x0c);
        if (s->confirm_does) {
            px = prompt(&body, rgba, px, 424, s->confirm ? s->confirm : "X", s->confirm_does);
        }
        if (s->back_does) {
            prompt(&body, rgba, px, 424, s->back ? s->back : "CIRCLE", s->back_does);
        }
    }
}

/* ---- builds and the hand-over ---- */

int fr_build_of(const Release *r, char *out, size_t cap)
{
    const char *base = SDL_GetBasePath();
    char name[16];

#ifdef _WIN32
    snprintf(name, sizeof(name), "ggx-%c%c.exe", r->id[0] | 0x20, r->id[1] | 0x20);
#else
    snprintf(name, sizeof(name), "ggx-%c%c", r->id[0] | 0x20, r->id[1] | 0x20);
#endif
    if (!base || fs_join(out, cap, base, name, NULL) != 0 || fs_stat(out, NULL, NULL) != 0) {
        if (cap) {
            out[0] = 0;
        }
        return -1;
    }
    return 0;
}

/* 1 if r's extraction in data_dir opens (quietly: its refusal is the first
   run's to say) */
static int extracted(const char *data_dir, const Release *r)
{
    AssetStore *s = NULL;
    FILE *sink;
    char image[8];
    int ok;

    if (assets_manifest_image(data_dir, r, image, sizeof(image)) != 0) {
        return 0;
    }
#ifdef _WIN32
    sink = fopen("NUL", "w");
#else
    sink = fopen("/dev/null", "w");
#endif
    ok = asset_store_open(&s, data_dir, r, sink ? sink : stderr) == 0;
    if (sink) {
        fclose(sink);
    }
    if (ok) {
        asset_store_close(s);
    }
    return ok;
}

int fr_needed(const char *data_dir, char *other, size_t cap)
{
    const Release *own = release_by_id(OWN), *oth = release_by_id(OTHER);
    char image[8];

    if (cap) {
        other[0] = 0;
    }
    if (extracted(data_dir, own)) {
        return 0;
    }
    if (assets_manifest_image(data_dir, own, image, sizeof(image)) != 0 && !SDL_getenv(HANDED_OVER) &&
        extracted(data_dir, oth) && fr_build_of(oth, other, cap) == 0) {
        return 0;
    }
    return 1;
}

int fr_handoff(const char *path, int argc, char **argv, int *code)
{
    const char **args = calloc((size_t)argc + 1, sizeof(char *));
    int i, n = 0;

    if (!args) {
        return -1;
    }
    args[n++] = path;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--first-run") && i + 1 < argc) {
            i++;
            continue;
        }
        args[n++] = argv[i];
    }
    args[n] = NULL;
    SDL_setenv_unsafe(HANDED_OVER, "1", 1);
    fflush(stdout);
    fflush(stderr);
#ifdef _WIN32
    {
        SDL_Process *p = SDL_CreateProcess(args, false);

        free(args);
        if (!p) {
            fprintf(stderr, "ggx: cannot start %s: %s\n", path, SDL_GetError());
            return -1;
        }
        SDL_WaitProcess(p, true, code);
        SDL_DestroyProcess(p);
        return 0;
    }
#else
    (void)code;
    execv(path, (char *const *)args);
    fprintf(stderr, "ggx: cannot start %s\n", path);
    free(args);
    return -1;
#endif
}

/* ---- the run ---- */

typedef struct {
    FirstRun *fr;
    TlFonts *fonts;
    uint8_t *rgba;
    int dumped;
    /* the extraction, on its thread */
    SDL_Mutex *lock;
    SDL_AtomicInt stop, finished;
    const Release *release;
    const char *file;
    uint64_t done, total;
    int rc;
    char why[2048];
    char image[1100];
    /* the dialog's answer, from whatever thread SDL calls it on */
    SDL_AtomicInt answered;
    char chosen[1100];
    char dialog_error[256];
    /* the device used last, for the prompts */
    SDL_JoystickID pad;
} Run;

static void dump(Run *r)
{
    char path[1100], name[32];

    if (!r->fr->dump_dir) {
        return;
    }
    snprintf(name, sizeof(name), "first-run-%d.png", r->dumped++);
    if (fs_join(path, sizeof(path), r->fr->dump_dir, "/", name) == 0) {
        png_write_rgba(path, r->rgba, FR_W, FR_H);
    }
}

static void show(Run *r, const FrScreen *s, int dump_it)
{
    fr_draw(r->fonts, s, r->rgba);
    if (r->fr->show) {
        r->fr->show(r->fr->show_ctx, r->rgba);
    }
    if (dump_it) {
        dump(r);
    }
}

static void w_begin(void *ctx, const Release *rel, uint64_t total)
{
    Run *r = ctx;

    SDL_LockMutex(r->lock);
    r->release = rel, r->total = total;
    SDL_UnlockMutex(r->lock);
}

static void w_progress(void *ctx, const char *file, uint64_t done, uint64_t total)
{
    Run *r = ctx;

    SDL_LockMutex(r->lock);
    r->file = file, r->done = done, r->total = total;
    SDL_UnlockMutex(r->lock);
}

static int w_stop(void *ctx)
{
    return SDL_GetAtomicInt(&((Run *)ctx)->stop);
}

static int SDLCALL extract_thread(void *ctx)
{
    Run *r = ctx;
    ExtractWatch w;
    ExtractResult res;

    memset(&w, 0, sizeof(w));
    w.ctx = r, w.begin = w_begin, w.progress = w_progress, w.stop = w_stop;
    w.why = r->why, w.why_cap = sizeof(r->why);
    r->rc = assets_extract_watched(r->image, r->fr->data_dir, &res, stderr, &w);
    SDL_SetAtomicInt(&r->finished, 1);
    return 0;
}

static void SDLCALL dialog_done(void *ctx, const char *const *files, int filter)
{
    Run *r = ctx;

    (void)filter;
    if (!files) {
        snprintf(r->dialog_error, sizeof(r->dialog_error), "%s", SDL_GetError());
        r->chosen[0] = 0;
    } else {
        snprintf(r->chosen, sizeof(r->chosen), "%s", files[0] ? files[0] : "");
    }
    SDL_SetAtomicInt(&r->answered, 1);
}

/* The prompts for the device used last: the pad's own labels (X confirms:
   the south button), or the keyboard's keys. */
static void labels(const Run *r, FrScreen *s)
{
    SDL_Gamepad *g = r->pad ? SDL_GetGamepadFromID(r->pad) : NULL;

    s->confirm = "ENTER", s->back = "ESC";
    if (g) {
        static const char *const names[] = { "?", "A", "B", "X", "Y", "X", "CIRCLE", "SQUARE", "TRIANGLE" };
        SDL_GamepadButtonLabel c = SDL_GetGamepadButtonLabel(g, SDL_GAMEPAD_BUTTON_SOUTH);
        SDL_GamepadButtonLabel b = SDL_GetGamepadButtonLabel(g, SDL_GAMEPAD_BUTTON_EAST);

        s->confirm = (unsigned)c < sizeof(names) / sizeof(names[0]) && c ? names[c] : "X";
        s->back = (unsigned)b < sizeof(names) / sizeof(names[0]) && b ? names[b] : "CIRCLE";
    }
}

/* the player's path, shortened to its file name */
static const char *base_name(const char *p)
{
    const char *a = strrchr(p, '/'), *b = strrchr(p, '\\');

    return a > b ? a + 1 : b ? b + 1 : a ? a + 1 : p;
}

/* "ggx: <full path> is not ..." as the player reads it: the file's name only */
static void tidy(char *why, const char *image)
{
    size_t n = strlen(image);
    char *at = n ? strstr(why, image) : NULL;
    const char *bn = base_name(image);

    while (at) {
        memmove(at, bn, strlen(bn));
        memmove(at + strlen(bn), at + n, strlen(at + n) + 1);
        at = strstr(at + strlen(bn), image);
    }
    n = strlen(why);
    while (n && why[n - 1] == '\n') {
        why[--n] = 0;
    }
}

enum { ST_ASK, ST_DIALOG, ST_EXTRACT, ST_REFUSED, ST_DONE };

/* Identifies the image: 0 to extract (r->release set), else the refusal is
   in r->why. */
static int identify(Run *r, const char *path)
{
    const Release *own = release_by_id(OWN);
    DiscImage *img = NULL;
    const Release *found;
    char other[1100], list[256];

    snprintf(r->image, sizeof(r->image), "%s", path);
    if (disc_image_open(&img, path, r->why, sizeof(r->why)) != 0) {
        char err[1024];

        snprintf(err, sizeof(err), "%s", r->why);
        release_list(list, sizeof(list));
        snprintf(r->why, sizeof(r->why),
                 "%s is not a disc image the port can read: %s\n"
                 "The port needs your own copy of one of these, as an ISO, BIN/CUE or CHD image:\n%s",
                 path, err, list);
        fprintf(stderr, "ggx: %s", r->why);
        tidy(r->why, path);
        return -1;
    }
    found = assets_identify(img, path, r->why, sizeof(r->why));
    disc_image_close(img);
    if (!found) {
        fprintf(stderr, "ggx: %s", r->why);
        tidy(r->why, path);
        return -1;
    }
    r->release = found;
    if (found != own && (SDL_getenv(HANDED_OVER) || fr_build_of(found, other, sizeof(other)) != 0)) {
        snprintf(r->why, sizeof(r->why),
                 "%s is %s (%s), which the port supports, but this copy of it is the build for %s (%s).\n"
                 "Choose an image of that disc, or play this one with the port's %s build (ggx-%c%c).",
                 path, found->title, found->serial, own->title, own->serial, found->id, found->id[0] | 0x20,
                 found->id[1] | 0x20);
        fprintf(stderr, "ggx: %s\n", r->why);
        tidy(r->why, path);
        return -1;
    }
    fprintf(stderr, "ggx: first run: %s is %s (%s)\n", path, found->title, found->serial);
    return 0;
}

/* an image in <data dir>/disc/, for a desktop without a working file
   dialog (Steam Deck's Game Mode) */
typedef struct {
    char *out;
    size_t cap;
    const char *dir;
    int bin;      /* the second look: a BIN with no CUE */
} Look;

static void look_each(void *ctx, const char *name)
{
    Look *l = ctx;
    const char *dot = strrchr(name, '.');
    char ext[8];
    size_t i;

    if (l->out[0] || !dot || strlen(dot) > 4) {
        return;
    }
    for (i = 0; dot[i + 1]; i++) {
        ext[i] = (char)(dot[i + 1] | 0x20);
    }
    ext[i] = 0;
    /* a CUE before its BIN: the sheet says how to read the BIN */
    if (l->bin ? !strcmp(ext, "bin") : !strcmp(ext, "iso") || !strcmp(ext, "chd") || !strcmp(ext, "cue")) {
        fs_join(l->out, l->cap, l->dir, name, NULL);
    }
}

static int disc_folder(const char *data_dir, char *dir, size_t cap)
{
    size_t n = strlen(data_dir);

    return fs_join(dir, cap, data_dir, n && (data_dir[n - 1] == '/' || data_dir[n - 1] == '\\') ? "disc" : "/disc",
#ifdef _WIN32
                   "\\"
#else
                   "/"
#endif
    );
}

static int look_in_folder(const char *data_dir, char *out, size_t cap)
{
    char dir[1100];
    Look l;

    out[0] = 0;
    if (disc_folder(data_dir, dir, sizeof(dir)) != 0) {
        return -1;
    }
    l.out = out, l.cap = cap, l.dir = dir, l.bin = 0;
    fs_list(dir, look_each, &l);
    if (!out[0]) {
        l.bin = 1;
        fs_list(dir, look_each, &l);
    }
    return out[0] ? 0 : -1;
}

int fr_run(FirstRun *fr)
{
    static const SDL_DialogFileFilter filters[] = {
        { "Disc images (ISO, BIN/CUE, CHD)", "iso;bin;cue;chd" },
        { "All files", "*" },
    };
    const Release *own = release_by_id(OWN), *oth = release_by_id(OTHER);
    Run r;
    SDL_Thread *th = NULL;
    char err[512], ask[1400], heading[160], body[2400], folder[1100], image[1100];
    int state = ST_ASK, drawn_state = -1, result = FR_QUIT, damaged;
    uint64_t last_draw = 0;
    int repair = 0, extract_kept = 0, asked = 0;

    memset(&r, 0, sizeof(r));
    r.fr = fr;
    fr->other[0] = 0;
    if (!(r.fonts = tl_fonts_load(fr->fonts_dir, err, sizeof(err))) || !(r.rgba = malloc((size_t)FR_W * FR_H * 4)) ||
        !(r.lock = SDL_CreateMutex())) {
        fprintf(stderr, "ggx: first run: %s\n", r.fonts ? "out of memory" : err);
        tl_fonts_free(r.fonts);
        free(r.rgba);
        return FR_QUIT;
    }
    disc_folder(fr->data_dir, folder, sizeof(folder));
    damaged = assets_manifest_image(fr->data_dir, own, image, sizeof(image)) == 0 && !extracted(fr->data_dir, own);
    if (damaged) {
        fprintf(stderr, "ggx: the extracted files in %s are damaged or incomplete\n", fr->data_dir);
    }
    {
        char list[256];

        release_list(list, sizeof(list));
        snprintf(ask, sizeof(ask),
                 "%sThe port plays your own copy of the game. Choose its disc image: an ISO, BIN/CUE or CHD of one "
                 "of these:\n%s\nIts files are copied into %s, once.",
                 damaged ? "The files extracted from your disc are damaged or incomplete, so they will be "
                           "extracted again.\n\n"
                 : extracted(fr->data_dir, oth) ? "Your other disc's files are already extracted, but this copy of "
                                                  "the port plays the other release.\n\n"
                                                : "",
                 list, fr->data_dir);
    }
    if (fr->image) {
        snprintf(r.chosen, sizeof(r.chosen), "%s", fr->image);
        SDL_SetAtomicInt(&r.answered, 1);
        state = ST_DIALOG;
        if (fr->dump_dir) {
            FrScreen s;

            memset(&s, 0, sizeof(s));
            s.kind = FR_SCREEN_ASK, s.heading = "FIRST RUN", s.body = ask;
            labels(&r, &s);
            s.confirm_does = "CHOOSE THE DISC IMAGE", s.back_does = "QUIT";
            show(&r, &s, 1);
        }
    } else if (damaged && image[0]) {
        /* redone from the image it came from, without asking */
        snprintf(r.chosen, sizeof(r.chosen), "%s", image);
        SDL_SetAtomicInt(&r.answered, 1);
        state = ST_DIALOG;
        repair = 1;
    }
    for (;;) {
        SDL_Event e;
        int confirm = 0, back = 0, quit = 0;
        uint64_t now = SDL_GetTicks();
        FrScreen s;

        while (fr->win && SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_EVENT_QUIT:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                quit = 1;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (e.key.repeat) {
                    break;
                }
                r.pad = 0;
                if (e.key.key == SDLK_RETURN || e.key.key == SDLK_KP_ENTER || e.key.key == SDLK_SPACE) {
                    confirm = 1;
                } else if (e.key.key == SDLK_ESCAPE) {
                    back = 1;
                }
                drawn_state = -1;
                break;
            case SDL_EVENT_GAMEPAD_ADDED:
                SDL_OpenGamepad(e.gdevice.which);
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                r.pad = e.gbutton.which;
                drawn_state = -1;
                if (e.gbutton.button == SDL_GAMEPAD_BUTTON_SOUTH || e.gbutton.button == SDL_GAMEPAD_BUTTON_START) {
                    confirm = 1;
                } else if (e.gbutton.button == SDL_GAMEPAD_BUTTON_EAST) {
                    back = 1;
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    confirm = 1;
                }
                break;
            case SDL_EVENT_DROP_FILE:
                if ((state == ST_ASK || state == ST_REFUSED) && e.drop.data) {
                    snprintf(r.chosen, sizeof(r.chosen), "%s", e.drop.data);
                    SDL_SetAtomicInt(&r.answered, 1);
                    state = ST_DIALOG;
                }
                break;
            default:
                break;
            }
        }
        if (quit) {
            if (th) {
                SDL_SetAtomicInt(&r.stop, 1);
                SDL_WaitThread(th, NULL);
                th = NULL;
            }
            result = FR_QUIT;
            break;
        }
        if (!fr->win && state == ST_ASK) {
            /* offscreen, nobody can choose: what --first-run named was no image */
            fprintf(stderr, "ggx: first run: no disc image to extract\n");
            result = FR_REFUSED;
            break;
        }
        if ((state == ST_ASK || state == ST_REFUSED) && back) {
            result = FR_QUIT;
            break;
        }
        if ((state == ST_ASK || state == ST_REFUSED) && confirm) {
            r.dialog_error[0] = 0;
            if (look_in_folder(fr->data_dir, r.chosen, sizeof(r.chosen)) == 0) {
                SDL_SetAtomicInt(&r.answered, 1);
            } else {
                SDL_SetAtomicInt(&r.answered, 0);
                SDL_ShowOpenFileDialog(dialog_done, &r, fr->win, filters, 2, NULL, false);
                asked = 1;
            }
            state = ST_DIALOG;
        }
        if (state == ST_DIALOG && SDL_GetAtomicInt(&r.answered)) {
            SDL_SetAtomicInt(&r.answered, 0);
            if (fr->win && asked) {
                /* the keyboard back from the dialog (macOS leaves it nowhere) */
                SDL_RaiseWindow(fr->win);
                asked = 0;
            }
            if (!r.chosen[0]) {
                if (r.dialog_error[0]) {
                    snprintf(r.why, sizeof(r.why),
                             "The file dialog didn't open (%s).\nPut the disc image in %s and press the button again.",
                             r.dialog_error, folder);
                    state = ST_REFUSED;
                } else {
                    state = ST_ASK;   /* cancelled */
                }
            } else if (identify(&r, r.chosen) != 0) {
                state = ST_REFUSED;
            } else {
                SDL_SetAtomicInt(&r.stop, 0);
                SDL_SetAtomicInt(&r.finished, 0);
                r.done = 0, r.total = 0, r.file = NULL, r.why[0] = 0;
                th = SDL_CreateThread(extract_thread, "ggx-extract", &r);
                if (!th) {
                    snprintf(r.why, sizeof(r.why), "cannot start the extraction: %s", SDL_GetError());
                    state = ST_REFUSED;
                } else {
                    state = ST_EXTRACT;
                }
            }
            if (state == ST_REFUSED && !fr->win) {
                show(&r, &(FrScreen){ FR_SCREEN_REFUSED, "THIS IMAGE CAN'T BE USED", r.why, NULL, 0, 0, "X", "CIRCLE",
                                      "CHOOSE ANOTHER", "QUIT" }, 1);
                result = FR_REFUSED;
                break;
            }
            if (repair && state == ST_REFUSED) {
                /* the image it came from is no good now either: ask */
                repair = 0;
            }
        }
        if (state == ST_EXTRACT && SDL_GetAtomicInt(&r.finished)) {
            SDL_WaitThread(th, NULL);
            th = NULL;
            if (r.rc != 0) {
                tidy(r.why, r.image);
                state = ST_REFUSED;
                if (!fr->win) {
                    show(&r, &(FrScreen){ FR_SCREEN_REFUSED, "THIS IMAGE CAN'T BE USED", r.why, NULL, 0, 0, "X",
                                          "CIRCLE", "CHOOSE ANOTHER", "QUIT" }, 1);
                    result = FR_REFUSED;
                    break;
                }
            } else {
                state = ST_DONE;
            }
        }
        /* the screen */
        memset(&s, 0, sizeof(s));
        labels(&r, &s);
        switch (state) {
        case ST_ASK:
            s.kind = FR_SCREEN_ASK, s.heading = "FIRST RUN", s.body = ask;
            s.confirm_does = "CHOOSE THE DISC IMAGE", s.back_does = "QUIT";
            break;
        case ST_DIALOG:
            s.kind = FR_SCREEN_DIALOG, s.heading = "FIRST RUN";
            s.body = "Choose the disc image in the window that opened.\n\nYou can also drop the image on this window.";
            break;
        case ST_EXTRACT: {
            const Release *rel;

            SDL_LockMutex(r.lock);
            rel = r.release;
            s.file = r.file, s.done = r.done, s.total = r.total;
            SDL_UnlockMutex(r.lock);
            snprintf(heading, sizeof(heading), repair ? "REPAIRING THE EXTRACTED FILES" : "EXTRACTING");
            snprintf(body, sizeof(body),
                     "%s (%s), from %s.\n\nEach file is checked against the release's own hashes as it is copied.",
                     rel ? rel->title : "", rel ? rel->serial : "", base_name(r.image));
            s.kind = FR_SCREEN_EXTRACT, s.heading = heading, s.body = body;
            break;
        }
        case ST_REFUSED:
            s.kind = FR_SCREEN_REFUSED, s.heading = r.dialog_error[0] ? "NO FILE DIALOG" : "THIS IMAGE CAN'T BE USED";
            s.body = r.why, s.confirm_does = "CHOOSE ANOTHER", s.back_does = "QUIT";
            break;
        default:
            snprintf(body, sizeof(body), "%s (%s) is ready.%s", r.release ? r.release->title : "",
                     r.release ? r.release->serial : "",
                     r.release && r.release != own ? "\n\nStarting the port's build for it." : "");
            s.kind = FR_SCREEN_READY, s.heading = "READY", s.body = body;
            break;
        }
        if (state != drawn_state || state == ST_EXTRACT || now - last_draw > 250) {
            /* each screen is kept as it first shows, and the extraction's once, halfway */
            int keep = state == ST_EXTRACT ? !extract_kept && s.total && s.done * 2 >= s.total : state != drawn_state;

            if (state == ST_EXTRACT && keep) {
                extract_kept = 1;
            }
            show(&r, &s, keep && fr->dump_dir);
            drawn_state = state;
            last_draw = now;
        }
        if (state == ST_DONE) {
            if (fr->win) {
                SDL_Delay(900);
            }
            if (r.release == own) {
                result = FR_PLAY;
            } else {
                fr_build_of(r.release, fr->other, sizeof(fr->other));
                result = FR_HANDOFF;
            }
            break;
        }
        SDL_Delay(fr->win ? 16 : 50);
    }
    tl_fonts_free(r.fonts);
    free(r.rgba);
    SDL_DestroyMutex(r.lock);
    return result;
}
