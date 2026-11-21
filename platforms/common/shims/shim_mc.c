/* libmc shims (PORT-012): the game's memory card calls, answered from the
 * card in each slot (common/saves/mc_card.h): the native save store, an
 * empty card that lives for the run, or none.
 *
 * libmc is asynchronous: each call registers a command, and sceMcSync later
 * reports it finished with a result. Here every command is carried out when
 * it is made and finishes by the next sceMcSync. A card is a formatted 8 MB
 * PS2 card. With no card at all the game stops on its "NO MEMORY CARD (PS2)
 * / SKIP AUTOLOAD?" screen until a button is pressed, as it does on a PS2.
 *
 * What mcman keeps on the IOP for each card, the current directory and the
 * open files, the shim keeps in libmc's own buffer for the current
 * directory, mcPwd (4 KB of libmc's .bss, which the port never uses
 * otherwise and the Lockstep Traces mask), so it is Core state: snapshots
 * carry it, and a run that never opens a file leaves it zero. The card's
 * contents are not Core state, like the disc's.
 *
 * Determinism: sceMcGetDir writes each entry's creation and modification
 * time as zero. The store records the host's time for an exported card,
 * but the Core never sees it; nothing in the game reads those fields.
 *
 * Replaying a Lockstep Trace, the shims answer from the trace and touch no
 * card (the traces are recorded with no memory cards).
 */
#include <string.h>

#include "core_host.h"
#include "mc_card.h"
#include "trace.h"

/* sceMcFunc numbers, as sceMcSync reports them */
enum { MC_GETINFO = 1, MC_OPEN, MC_CLOSE, MC_READ = 5, MC_WRITE, MC_MKDIR = 0xb, MC_CHDIR, MC_GETDIR,
       MC_DELETE = 0xf, MC_FORMAT };
#define MC_NO_CARD (-10)
#define MC_FAILED (-4)
#define MC_TOO_MANY_FILES (-7)
#define MC_TYPE_PS2 2
/* sceMcOpen's flags */
#define MC_O_READ 1
#define MC_O_WRITE 2
#define MC_O_CREATE 0x200

/* the shim's state in mcPwd: each port's current directory (empty: the
   root), then the open files */
#define CWD_SIZE 64
#define FD_BASE 0x80
#define FD_SIZE 96
#define FD_MAX 8
enum { FD_USED = 0, FD_PORT = 4, FD_FLAGS = 8, FD_POS = 12, FD_WRITTEN = 16, FD_DIR = 32, FD_FILE = 64 };

static u32 queue(CoreHost *h, int func, int result)
{
    h->mc.pending = 1;
    h->mc.func = func;
    h->mc.result = result;
    return 0;
}

static McCard *card(CoreHost *h, u32 port)
{
    return h->cards[port & 1] ? h->card[port & 1] : NULL;
}

static void get_cwd(CoreHost *h, u32 port, char out[CWD_SIZE])
{
    ee_str(h, ggx_sym_mcPwd + (port & 1) * CWD_SIZE, out, CWD_SIZE);
}

static void set_cwd(CoreHost *h, u32 port, const char *dir)
{
    uint8_t *p = ee_ptr(h, ggx_sym_mcPwd + (port & 1) * CWD_SIZE, CWD_SIZE);

    memset(p, 0, CWD_SIZE);
    memcpy(p, dir, strlen(dir) < CWD_SIZE ? strlen(dir) : CWD_SIZE - 1);
}

static u32 fd_addr(int fd)
{
    return ggx_sym_mcPwd + FD_BASE + (u32)fd * FD_SIZE;
}

/* A path, absolute or from the port's current directory, as a directory
   in the root and a name in it. The depth: 0 the root, 1 a directory, 2 a
   file; -1 if it goes deeper or out of the root. */
static int resolve(CoreHost *h, u32 port, const char *path, char dir[SAVE_NAME_MAX], char file[SAVE_NAME_MAX])
{
    char parts[2][SAVE_NAME_MAX], cwd[CWD_SIZE];
    const char *p = path;
    int depth = 0;

    if (*p != '/') {
        get_cwd(h, port, cwd);
        if (cwd[0]) {
            if (snprintf(parts[0], SAVE_NAME_MAX, "%s", cwd) >= SAVE_NAME_MAX) {
                return -1;
            }
            depth = 1;
        }
    }
    while (*p) {
        const char *e;
        size_t n;

        while (*p == '/') {
            p++;
        }
        if (!*p) {
            break;
        }
        e = strchr(p, '/');
        n = e ? (size_t)(e - p) : strlen(p);
        if (n == 1 && p[0] == '.') {
            /* stays */
        } else if (n == 2 && p[0] == '.' && p[1] == '.') {
            if (depth == 0) {
                return -1;
            }
            depth--;
        } else {
            if (depth == 2 || n >= SAVE_NAME_MAX) {
                return -1;
            }
            memcpy(parts[depth], p, n);
            parts[depth][n] = 0;
            depth++;
        }
        p += n;
    }
    snprintf(dir, SAVE_NAME_MAX, "%s", depth >= 1 ? parts[0] : "");
    snprintf(file, SAVE_NAME_MAX, "%s", depth == 2 ? parts[1] : "");
    return depth;
}

/* sceMcGetDir's wildcards: * any run, ? any one character */
static int match(const char *pat, const char *s)
{
    if (!*pat) {
        return !*s;
    }
    if (*pat == '*') {
        return match(pat + 1, s) || (*s && match(pat, s + 1));
    }
    return *s && (*pat == '?' || *pat == *s) && match(pat + 1, s + 1);
}

static void dir_entry(CoreHost *h, u32 table, const char *name, u32 size, u16 mode)
{
    uint8_t *e = ee_ptr(h, table, 64);

    /* sceMcTblGetDir: _Create and _Modify (zero, above), FileSizeByte, AttrFile, then EntryName at 0x20 */
    memset(e, 0, 64);
    e[0x10] = (uint8_t)size;
    e[0x11] = (uint8_t)(size >> 8);
    e[0x12] = (uint8_t)(size >> 16);
    e[0x13] = (uint8_t)(size >> 24);
    e[0x14] = (uint8_t)mode;
    e[0x15] = (uint8_t)(mode >> 8);
    memcpy(e + 0x20, name, strlen(name) < 31 ? strlen(name) : 31);
}

/* the entries of a directory matching a pattern: the count, and up to maxent written to table */
static int get_dir(CoreHost *h, McCard *c, u32 port, const char *path, int maxent, u32 table)
{
    char dir[SAVE_NAME_MAX], pat[SAVE_NAME_MAX], parent[300], none[SAVE_NAME_MAX];
    const char *slash = strrchr(path, '/');
    int depth, n = 0, i;

    /* the pattern is the last part; the rest names the directory to look in */
    snprintf(parent, sizeof(parent), "%.*s", slash ? (int)(slash - path) + 1 : 0, path);
    if (snprintf(pat, sizeof(pat), "%s", slash ? slash + 1 : path) >= (int)sizeof(pat)) {
        return MC_FAILED;
    }
    depth = resolve(h, port, parent[0] ? parent : ".", dir, none);
    if (depth < 0 || depth > 1 || !pat[0]) {
        return MC_FAILED;
    }
    if (depth == 0) {
        for (i = 0; i < mc_card_count(c); i++) {
            const SaveDir *d = mc_card_at(c, i);

            if (match(pat, d->name)) {
                if (n < maxent) {
                    dir_entry(h, table + (u32)n * 64, d->name, 2 + (u32)d->nfiles, d->mode);
                }
                n++;
            }
        }
    } else {
        const SaveDir *d = mc_card_at(c, mc_card_find(c, dir));

        if (!d) {
            return MC_FAILED;
        }
        if (match(pat, ".")) {
            if (n < maxent) {
                dir_entry(h, table + (u32)n * 64, ".", 0, SAVE_MODE_DIR);
            }
            n++;
        }
        if (match(pat, "..")) {
            if (n < maxent) {
                dir_entry(h, table + (u32)n * 64, "..", 0, SAVE_MODE_DIR);
            }
            n++;
        }
        for (i = 0; i < d->nfiles; i++) {
            if (match(pat, d->files[i].name)) {
                if (n < maxent) {
                    dir_entry(h, table + (u32)n * 64, d->files[i].name, d->files[i].size, d->files[i].mode);
                }
                n++;
            }
        }
    }
    return n < maxent ? n : maxent;
}

static int open_file(CoreHost *h, McCard *c, u32 port, const char *path, u32 flags)
{
    char dir[SAVE_NAME_MAX], file[SAVE_NAME_MAX];
    int di, fi, fd;
    uint8_t *p;

    if (resolve(h, port, path, dir, file) != 2 || (di = mc_card_find(c, dir)) < 0) {
        return MC_FAILED;
    }
    fi = save_dir_find(mc_card_at(c, di), file);
    if (fi < 0) {
        int rc;

        if (!(flags & MC_O_CREATE)) {
            return MC_FAILED;
        }
        rc = mc_card_create(c, di, file);
        if (rc != MC_RES_OK) {
            return rc;
        }
    }
    for (fd = 0; fd < FD_MAX && ee_r32(h, fd_addr(fd) + FD_USED); fd++) {
    }
    if (fd == FD_MAX) {
        return MC_TOO_MANY_FILES;
    }
    p = ee_ptr(h, fd_addr(fd), FD_SIZE);
    memset(p, 0, FD_SIZE);
    ee_w32(h, fd_addr(fd) + FD_USED, 1);
    ee_w32(h, fd_addr(fd) + FD_PORT, port & 1);
    ee_w32(h, fd_addr(fd) + FD_FLAGS, flags);
    memcpy(p + FD_DIR, dir, strlen(dir));
    memcpy(p + FD_FILE, file, strlen(file));
    return fd;
}

/* an open file: its card, directory and file index, or NULL */
static McCard *fd_file(CoreHost *h, u32 fd, int *di, int *fi)
{
    char dir[SAVE_NAME_MAX], file[SAVE_NAME_MAX];
    McCard *c;

    if (fd >= FD_MAX || !ee_r32(h, fd_addr((int)fd) + FD_USED)) {
        return NULL;
    }
    c = card(h, ee_r32(h, fd_addr((int)fd) + FD_PORT));
    ee_str(h, fd_addr((int)fd) + FD_DIR, dir, sizeof(dir));
    ee_str(h, fd_addr((int)fd) + FD_FILE, file, sizeof(file));
    if (!c || (*di = mc_card_find(c, dir)) < 0 || (*fi = save_dir_find(mc_card_at(c, *di), file)) < 0) {
        return NULL;
    }
    return c;
}

u32 w2c_env_sceMcInit(struct w2c_env *env)
{
    shim_log(env->host, "sceMcInit() = 0");
    TRACE_RETURN(env->host, sceMcInit, 0);
}

u32 w2c_env_sceMcGetInfo(struct w2c_env *env, u32 port, u32 slot, u32 type, u32 free, u32 format)
{
    CoreHost *h = env->host;
    McCard *c = card(h, port);
    int kb = c ? mc_card_free_kb(c) : 0;

    /* replaying a trace, the PS2's values arrive with the sceMcSync that completes the command, as on the PS2 */
    if (!h->replay) {
        ee_w32(h, type, c ? MC_TYPE_PS2 : 0);
        ee_w32(h, free, (u32)kb);
        ee_w32(h, format, c ? 1 : 0);
    }
    if (!c || kb == MC_CARD_CAPACITY_KB) {
        shim_log(h, "sceMcGetInfo(%u, %u) = 0 %s", port, slot, c ? "empty card" : "no card");
    } else {
        shim_log(h, "sceMcGetInfo(%u, %u) = 0 card, %d KB free", port, slot, kb);
    }
    TRACE_RETURN(env->host, sceMcGetInfo, queue(h, MC_GETINFO, c ? 0 : MC_NO_CARD), port, slot, type, free, format);
}

u32 w2c_env_sceMcSync(struct w2c_env *env, u32 mode, u32 cmd, u32 result)
{
    CoreHost *h = env->host;

    if (!h->mc.pending) {
        shim_log(h, "sceMcSync(%u) = -1", mode);
        TRACE_RETURN(env->host, sceMcSync, (u32)-1, mode, cmd, result);
    }
    h->mc.pending = 0;
    if (cmd) {
        ee_w32(h, cmd, (uint32_t)h->mc.func);
    }
    if (result) {
        ee_w32(h, result, (uint32_t)h->mc.result);
    }
    shim_log(h, "sceMcSync(%u) = 1 cmd=%d result=%d", mode, h->mc.func, h->mc.result);
    TRACE_RETURN(env->host, sceMcSync, 1, mode, cmd, result);
}

u32 w2c_env_sceMcOpen(struct w2c_env *env, u32 port, u32 slot, u32 name, u32 mode)
{
    CoreHost *h = env->host;
    McCard *c = card(h, port);
    char s[300], path[300];
    int r = MC_FAILED;

    if (!h->replay) {
        ee_str(h, name, path, sizeof(path));
        r = c ? open_file(h, c, port, path, mode) : MC_NO_CARD;
    }
    shim_log(h, "sceMcOpen(%u, %u, %s, 0x%x) = 0 -> %d", port, slot, log_str(h, name, s, sizeof(s)), mode, r);
    TRACE_RETURN(env->host, sceMcOpen, queue(h, MC_OPEN, r), port, slot, name, mode);
}

u32 w2c_env_sceMcClose(struct w2c_env *env, u32 fd)
{
    CoreHost *h = env->host;
    int r = MC_FAILED, di, fi;

    if (!h->replay && fd < FD_MAX && ee_r32(h, fd_addr((int)fd) + FD_USED)) {
        McCard *c = fd_file(h, fd, &di, &fi);

        r = 0;
        if (c && ee_r32(h, fd_addr((int)fd) + FD_WRITTEN) && mc_card_commit(c, mc_card_at(c, di)->name) != 0) {
            r = MC_RES_FULL;
        }
        memset(ee_ptr(h, fd_addr((int)fd), FD_SIZE), 0, FD_SIZE);
    }
    shim_log(h, "sceMcClose(%d) = 0 -> %d", (int)fd, r);
    TRACE_RETURN(env->host, sceMcClose, queue(h, MC_CLOSE, r), fd);
}

u32 w2c_env_sceMcRead(struct w2c_env *env, u32 fd, u32 buf, u32 size)
{
    CoreHost *h = env->host;
    char a[16];
    int r = MC_FAILED, di, fi;

    if (!h->replay) {
        McCard *c = fd_file(h, fd, &di, &fi);

        if (c && (ee_r32(h, fd_addr((int)fd) + FD_FLAGS) & MC_O_READ)) {
            const SaveFile *f = &mc_card_at(c, di)->files[fi];
            u32 pos = ee_r32(h, fd_addr((int)fd) + FD_POS), n = 0;

            if (pos < f->size) {
                n = f->size - pos < size ? f->size - pos : size;
                memcpy(ee_ptr(h, buf, n), f->data + pos, n);
            }
            ee_w32(h, fd_addr((int)fd) + FD_POS, pos + n);
            r = (int)n;
        }
    }
    shim_log(h, "sceMcRead(%d, %s, %u) = 0 -> %d", (int)fd, log_addr(buf, a), size, r);
    TRACE_RETURN(env->host, sceMcRead, queue(h, MC_READ, r), fd, buf, size);
}

u32 w2c_env_sceMcWrite(struct w2c_env *env, u32 fd, u32 buf, u32 size)
{
    CoreHost *h = env->host;
    char a[16];
    int r = MC_FAILED, di, fi;

    if (!h->replay) {
        McCard *c = fd_file(h, fd, &di, &fi);

        if (c && (ee_r32(h, fd_addr((int)fd) + FD_FLAGS) & MC_O_WRITE)) {
            u32 pos = ee_r32(h, fd_addr((int)fd) + FD_POS);

            /* ee_ptr marks the pages changed; they aren't, so they hash the same */
            r = mc_card_write(c, di, fi, pos, size ? ee_ptr(h, buf, size) : NULL, size);
            if (r == MC_RES_OK) {
                ee_w32(h, fd_addr((int)fd) + FD_POS, pos + size);
                ee_w32(h, fd_addr((int)fd) + FD_WRITTEN, 1);
                r = (int)size;
            }
        }
    }
    shim_log(h, "sceMcWrite(%d, %s, %u) = 0 -> %d", (int)fd, log_addr(buf, a), size, r);
    TRACE_RETURN(env->host, sceMcWrite, queue(h, MC_WRITE, r), fd, buf, size);
}

u32 w2c_env_sceMcMkdir(struct w2c_env *env, u32 port, u32 slot, u32 name)
{
    CoreHost *h = env->host;
    McCard *c = card(h, port);
    char s[300], path[300], dir[SAVE_NAME_MAX], file[SAVE_NAME_MAX];
    int r = MC_FAILED;

    if (!h->replay) {
        ee_str(h, name, path, sizeof(path));
        if (!c) {
            r = MC_NO_CARD;
        } else if (resolve(h, port, path, dir, file) == 1) {
            r = mc_card_mkdir(c, dir);
            if (r == MC_RES_OK && mc_card_commit(c, dir) != 0) {
                r = MC_RES_FULL;
            }
        }
    }
    shim_log(h, "sceMcMkdir(%u, %u, %s) = 0 -> %d", port, slot, log_str(h, name, s, sizeof(s)), r);
    TRACE_RETURN(env->host, sceMcMkdir, queue(h, MC_MKDIR, r), port, slot, name);
}

u32 w2c_env_sceMcChdir(struct w2c_env *env, u32 port, u32 slot, u32 dir, u32 cur)
{
    CoreHost *h = env->host;
    McCard *c = card(h, port);
    char s[300], a[16], path[300], d[SAVE_NAME_MAX], file[SAVE_NAME_MAX];
    int r = MC_FAILED;

    if (!h->replay) {
        int depth;

        ee_str(h, dir, path, sizeof(path));
        depth = c ? resolve(h, port, path, d, file) : -1;
        if (!c) {
            r = MC_NO_CARD;
        } else if (depth == 0 || (depth == 1 && mc_card_find(c, d) >= 0)) {
            if (cur) {
                char pwd[CWD_SIZE];

                get_cwd(h, port, pwd);
                snprintf(path, sizeof(path), "/%s", pwd);
                memcpy(ee_ptr(h, cur, (u32)strlen(path) + 1), path, strlen(path) + 1);
            }
            set_cwd(h, port, depth ? d : "");
            r = 0;
        }
    }
    shim_log(h, "sceMcChdir(%u, %u, %s, %s) = 0 -> %d", port, slot, log_str(h, dir, s, sizeof(s)), log_addr(cur, a),
             r);
    TRACE_RETURN(env->host, sceMcChdir, queue(h, MC_CHDIR, r), port, slot, dir, cur);
}

u32 w2c_env_sceMcGetDir(struct w2c_env *env, u32 port, u32 slot, u32 name, u32 mode, u32 maxent, u32 table)
{
    CoreHost *h = env->host;
    McCard *c = card(h, port);
    char s[300], a[16], path[300];
    int r = c ? 0 : MC_NO_CARD;

    /* mode 1 would continue a listing; the game always starts one, and its listings have one entry */
    if (!h->replay && c) {
        ee_str(h, name, path, sizeof(path));
        r = get_dir(h, c, port, path, (int)maxent, table);
    }
    shim_log(h, "sceMcGetDir(%u, %u, %s, %u, %d, %s) = 0", port, slot, log_str(h, name, s, sizeof(s)), mode,
             (int)maxent, log_addr(table, a));
    TRACE_RETURN(env->host, sceMcGetDir, queue(h, MC_GETDIR, r), port, slot, name, mode, maxent, table);
}

u32 w2c_env_sceMcDelete(struct w2c_env *env, u32 port, u32 slot, u32 name)
{
    CoreHost *h = env->host;
    McCard *c = card(h, port);
    char s[300], path[300], dir[SAVE_NAME_MAX], file[SAVE_NAME_MAX];
    int r = MC_FAILED;

    if (!h->replay) {
        int depth, di;

        ee_str(h, name, path, sizeof(path));
        depth = c ? resolve(h, port, path, dir, file) : -1;
        di = c && depth >= 1 ? mc_card_find(c, dir) : -1;
        if (!c) {
            r = MC_NO_CARD;
        } else if (di >= 0 && depth == 1) {
            r = mc_card_rmdir(c, di);
        } else if (di >= 0 && depth == 2) {
            r = mc_card_unlink(c, di, save_dir_find(mc_card_at(c, di), file));
        }
        if (r == MC_RES_OK && mc_card_commit(c, dir) != 0) {
            r = MC_RES_FULL;
        }
    }
    shim_log(h, "sceMcDelete(%u, %u, %s) = 0 -> %d", port, slot, log_str(h, name, s, sizeof(s)), r);
    TRACE_RETURN(env->host, sceMcDelete, queue(h, MC_DELETE, r), port, slot, name);
}

u32 w2c_env_sceMcFormat(struct w2c_env *env, u32 port, u32 slot)
{
    CoreHost *h = env->host;
    int r = MC_FAILED;

    /* every card here is formatted (sceMcGetInfo says so), so the game never asks; leave its saves alone */
    if (!h->replay) {
        r = card(h, port) ? 0 : MC_NO_CARD;
    }
    shim_log(h, "sceMcFormat(%u, %u) = 0 -> %d", port, slot, r);
    TRACE_RETURN(env->host, sceMcFormat, queue(h, MC_FORMAT, r), port, slot);
}
