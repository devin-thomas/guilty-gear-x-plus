/* The vblanks of the display a window is on (vblank.h). */
#include "vblank.h"

#include "pace.h"

#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define COBJMACROS
#include <windows.h>
#include <dxgi.h>
#endif

struct VBlank {
    SDL_Mutex *lock;
    VGrid grid;                 /* performance counter ticks */
    uint64_t fake_start, fake_period;   /* --fake-vblank: a grid from here, every period */
    SDL_AtomicInt dead;         /* no source that waits: why says why */
    char why[200];
#if defined(_WIN32)
    HMONITOR monitor;
    SDL_AtomicInt stop;
    FILE *log;
    uint64_t expect;            /* a refresh by the display mode, in counter ticks */
#endif
};

static const char *prefer = "auto";

void vblank_prefer(const char *source)
{
    prefer = source;
}

const char *vblank_why(VBlank *v)
{
    return v && SDL_GetAtomicInt(&v->dead) ? v->why : NULL;
}

VBlank *vblank_fake(double hz)
{
    VBlank *v = hz > 0 ? calloc(1, sizeof(*v)) : NULL;

    if (v) {
        v->fake_period = (uint64_t)((double)SDL_GetPerformanceFrequency() / hz + 0.5);
        v->fake_start = SDL_GetPerformanceCounter();
    }
    return v;
}

int vblank_get(VBlank *v, uint64_t *last, uint64_t *period)
{
    uint64_t now = SDL_GetPerformanceCounter();
    int ok;

    if (!v) {
        return 0;
    }
    if (v->fake_start) {
        *last = v->fake_start + (now - v->fake_start) / v->fake_period * v->fake_period;
        *period = v->fake_period;
        return 1;
    }
    SDL_LockMutex(v->lock);
    ok = vgrid_at(&v->grid, now, last, period);
    SDL_UnlockMutex(v->lock);
    return ok;
}

#if defined(_WIN32)

/* The DXGI output that shows this monitor. */
static IDXGIOutput *find_output(HMONITOR m)
{
    IDXGIFactory1 *f = NULL;
    IDXGIAdapter1 *a;
    IDXGIOutput *o;
    UINT i, j;

    if (FAILED(CreateDXGIFactory1(&IID_IDXGIFactory1, (void **)&f))) {
        return NULL;
    }
    for (i = 0; IDXGIFactory1_EnumAdapters1(f, i, &a) == S_OK; i++) {
        for (j = 0; IDXGIAdapter1_EnumOutputs(a, j, &o) == S_OK; j++) {
            DXGI_OUTPUT_DESC d;

            if (SUCCEEDED(IDXGIOutput_GetDesc(o, &d)) && d.Monitor == m) {
                IDXGIAdapter1_Release(a);
                IDXGIFactory1_Release(f);
                return o;
            }
            IDXGIOutput_Release(o);
        }
        IDXGIAdapter1_Release(a);
    }
    IDXGIFactory1_Release(f);
    return NULL;
}

/* The kernel's vblank wait for one output (a video present source of an
   adapter), which gdi32 exports. Its structures are d3dkmthk.h's, declared
   here so the build needs no WDK header; the functions are looked up at run
   time. On one test PC IDXGIOutput::WaitForVBlank returns at once for the second
   output (a CRT beside a primary TV on one GPU), 1.4 million times a
   run; this waits per output. */
typedef struct {
    HDC hdc;
    UINT adapter;
    LUID luid;
    UINT source;
} KmtOpenFromHdc;

typedef struct {
    UINT adapter;
    UINT device;
    UINT source;
} KmtWaitVBlank;

typedef struct {
    UINT adapter;
} KmtClose;

typedef LONG(APIENTRY *KmtOpenFn)(KmtOpenFromHdc *);
typedef LONG(APIENTRY *KmtWaitFn)(const KmtWaitVBlank *);
typedef LONG(APIENTRY *KmtCloseFn)(const KmtClose *);

typedef struct {
    const char *name;
    IDXGIOutput *output;        /* DXGI */
    KmtWaitFn wait;             /* the kernel's, with its adapter and source */
    KmtCloseFn close;
    KmtWaitVBlank w;
} Source;

static int kmt_open(HMONITOR m, Source *s)
{
    HMODULE gdi = GetModuleHandleW(L"gdi32.dll");
    KmtOpenFn open = gdi ? (KmtOpenFn)(void (*)(void))GetProcAddress(gdi, "D3DKMTOpenAdapterFromHdc") : NULL;
    MONITORINFOEXW mi;
    KmtOpenFromHdc o;
    HDC dc;
    LONG st;

    memset(s, 0, sizeof(*s));
    s->name = "D3DKMTWaitForVerticalBlankEvent";
    s->wait = gdi ? (KmtWaitFn)(void (*)(void))GetProcAddress(gdi, "D3DKMTWaitForVerticalBlankEvent") : NULL;
    s->close = gdi ? (KmtCloseFn)(void (*)(void))GetProcAddress(gdi, "D3DKMTCloseAdapter") : NULL;
    memset(&mi, 0, sizeof(mi));
    mi.cbSize = sizeof(mi);
    if (!open || !s->wait || !s->close || !GetMonitorInfoW(m, (MONITORINFO *)&mi) ||
        !(dc = CreateDCW(mi.szDevice, NULL, NULL, NULL))) {
        return 0;
    }
    memset(&o, 0, sizeof(o));
    o.hdc = dc;
    st = open(&o);
    DeleteDC(dc);
    if (st < 0) {
        return 0;
    }
    s->w.adapter = o.adapter;
    s->w.source = o.source;
    return 1;
}

static int dxgi_open(HMONITOR m, Source *s)
{
    memset(s, 0, sizeof(*s));
    s->name = "IDXGIOutput::WaitForVBlank";
    return (s->output = find_output(m)) != NULL;
}

static int src_wait(Source *s)
{
    if (!strcmp(prefer, "nowait")) {
        return 1;   /* --vblank-source nowait: as if the source returned at once */
    }
    return s->output ? SUCCEEDED(IDXGIOutput_WaitForVBlank(s->output)) : s->wait(&s->w) >= 0;
}

static void src_close(Source *s)
{
    if (s->output) {
        IDXGIOutput_Release(s->output);
    } else if (s->close) {
        KmtClose c;

        c.adapter = s->w.adapter;
        s->close(&c);
    }
}

static void destroy(VBlank *v)
{
    if (v->lock) {
        SDL_DestroyMutex(v->lock);
    }
    free(v);
}

/* Follow one source until stopped (1), or until it shows it doesn't wait:
   50 returns in a row sooner than a quarter of a refresh, failing for a
   second (the kernel's wait fails through a display mode change, which takes
   that long), or, more than 12 of the last 60 times, waiting over one and a
   half refreshes: no fixed grid of vblanks to pace to, as on one test TV, whose
   vblanks follow its frames (0, with why). */
static int follow(VBlank *v, Source *s, char *why, size_t n)
{
    int fast = 0, fails = 0, nlong = 0, ilong = 0;
    uint8_t longs[60] = { 0 };
    uint64_t prev = 0;

    while (!SDL_GetAtomicInt(&v->stop)) {
        uint64_t t0 = SDL_GetPerformanceCounter(), t;

        if (!src_wait(s)) {
            if (++fails == 100) {
                snprintf(why, n, "%s fails", s->name);
                return 0;
            }
            SDL_Delay(10);
            continue;
        }
        t = SDL_GetPerformanceCounter();
        fast = v->expect && t - t0 < v->expect / 4 ? fast + 1 : 0;
        if (fast == 50) {
            snprintf(why, n, "%s returns at once instead of waiting for a vblank", s->name);
            return 0;
        }
        if (prev && v->expect) {
            int late = t - prev > v->expect + v->expect / 2;

            nlong += late - longs[ilong];
            longs[ilong] = (uint8_t)late;
            ilong = (ilong + 1) % 60;
            if (nlong > 12) {
                snprintf(why, n, "%s shows no regular vblanks (a variable refresh rate?)", s->name);
                return 0;
            }
        }
        prev = t;
        SDL_LockMutex(v->lock);
        vgrid_seen(&v->grid, t);
        SDL_UnlockMutex(v->lock);
        if (v->log) {
            fprintf(v->log, "wake,%llu\n", (unsigned long long)t);
        }
    }
    return 1;
}

/* The thread owns the VBlank once started: vblank_close only asks it to
   stop, since a wait for a vblank of a display that has gone to sleep may
   not come back soon, and the window mustn't wait for it. The kernel's wait
   first, then DXGI's; a source that doesn't wait is dropped, and with none
   left the window keeps the clock. */
static int watch(void *ctx)
{
    VBlank *v = ctx;
    int (*open[2])(HMONITOR, Source *) = { kmt_open, dxgi_open };
    char why[2][96] = { "", "" };
    int i, stopped = 0;

    SDL_SetCurrentThreadPriority(SDL_THREAD_PRIORITY_HIGH);
    for (i = 0; i < 2 && !stopped; i++) {
        Source s;

        if (!strcmp(prefer, i ? "kernel" : "dxgi")) {
            snprintf(why[i], sizeof(why[i]), "not tried (--vblank-source %s)", prefer);
        } else if (!open[i](v->monitor, &s)) {
            snprintf(why[i], sizeof(why[i]), "%s can't be opened for this display", s.name);
        } else {
            uint64_t min_gap;

            if (v->log) {
                fprintf(v->log, "source,%s\n", s.name);
            }
            stopped = follow(v, &s, why[i], sizeof(why[i]));
            src_close(&s);
            /* what it gave isn't vblanks: start the grid again */
            SDL_LockMutex(v->lock);
            min_gap = v->grid.min_gap;
            memset(&v->grid, 0, sizeof(v->grid));
            v->grid.min_gap = min_gap;
            SDL_UnlockMutex(v->lock);
        }
        if (!stopped && v->log) {
            fprintf(v->log, "dropped,%s\n", why[i]);
        }
    }
    if (!stopped) {
        snprintf(v->why, sizeof(v->why), "no vblank source: %s; %s", why[0], why[1]);
        SDL_SetAtomicInt(&v->dead, 1);
        while (!SDL_GetAtomicInt(&v->stop)) {
            SDL_Delay(50);   /* the window reads why, then closes it */
        }
    }
    destroy(v);
    return 0;
}

VBlank *vblank_open(SDL_Window *win, double hz, FILE *log)
{
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(win), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    SDL_Thread *t;
    VBlank *v;

    if (!hwnd || !(v = calloc(1, sizeof(*v)))) {
        return NULL;
    }
    v->monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    v->log = log;
    if (hz > 0) {
        v->expect = (uint64_t)((double)SDL_GetPerformanceFrequency() / hz);
        v->grid.min_gap = v->expect / 2;
    }
    if (!(v->lock = SDL_CreateMutex()) || !(t = SDL_CreateThread(watch, "ggx vblank", v))) {
        destroy(v);
        return NULL;
    }
    SDL_DetachThread(t);
    return v;
}

void vblank_close(VBlank *v)
{
    if (!v) {
        return;
    }
    if (v->fake_start) {
        free(v);
        return;
    }
    SDL_SetAtomicInt(&v->stop, 1);
}

#else

VBlank *vblank_open(SDL_Window *win, double hz, FILE *log)
{
    (void)win, (void)hz, (void)log;
    return NULL;
}

void vblank_close(VBlank *v)
{
    free(v);
}

#endif
