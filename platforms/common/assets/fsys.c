#include "fsys.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <shlobj.h>
#include <sys/stat.h>
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#define APP "ggx-plus"

#ifdef _WIN32
/* a malloc'd wide copy of a UTF-8 string, or NULL */
static wchar_t *wide(const char *s)
{
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, -1, NULL, 0);
    wchar_t *w;

    if (n <= 0) {
        return NULL;
    }
    w = malloc(sizeof(wchar_t) * (size_t)n);
    if (w && MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, s, -1, w, n) != n) {
        free(w);
        w = NULL;
    }
    return w;
}
#endif

FILE *fs_open(const char *path, const char *mode)
{
#ifdef _WIN32
    wchar_t *wp = wide(path), *wm = wide(mode);
    FILE *f = wp && wm ? _wfopen(wp, wm) : NULL;

    free(wp);
    free(wm);
    return f;
#else
    return fopen(path, mode);
#endif
}

int fs_seek(FILE *f, uint64_t off)
{
#ifdef _WIN32
    return _fseeki64(f, (long long)off, SEEK_SET);
#else
    return fseeko(f, (off_t)off, SEEK_SET);
#endif
}

int64_t fs_size(FILE *f)
{
#ifdef _WIN32
    if (_fseeki64(f, 0, SEEK_END) != 0) {
        return -1;
    }
    return _ftelli64(f);
#else
    if (fseeko(f, 0, SEEK_END) != 0) {
        return -1;
    }
    return (int64_t)ftello(f);
#endif
}

int fs_stat(const char *path, uint64_t *size, int64_t *mtime)
{
#ifdef _WIN32
    struct _stat64 st;
    wchar_t *wp = wide(path);
    int rc = wp ? _wstat64(wp, &st) : -1;

    free(wp);
#else
    struct stat st;
    int rc = stat(path, &st);
#endif
    if (rc != 0) {
        return -1;
    }
    if (size) {
        *size = (uint64_t)st.st_size;
    }
    if (mtime) {
        *mtime = (int64_t)st.st_mtime;
    }
    return 0;
}

static int make_dir(const char *path)
{
#ifdef _WIN32
    wchar_t *wp = wide(path);
    int rc = wp ? _wmkdir(wp) : -1;

    free(wp);
    return rc;
#else
    return mkdir(path, 0755);
#endif
}

int fs_mkdirs(const char *path)
{
    char buf[1024];
    size_t n = strlen(path), i;

    if (n == 0 || n >= sizeof(buf)) {
        return -1;
    }
    memcpy(buf, path, n + 1);
    for (i = 1; i <= n; i++) {
        if (buf[i] == '/' || buf[i] == '\\' || buf[i] == 0) {
            char c = buf[i];

            /* "C:" alone is not a directory to make */
            if (!(i == 2 && buf[1] == ':')) {
                buf[i] = 0;
                if (make_dir(buf) != 0 && errno != EEXIST && fs_stat(buf, NULL, NULL) != 0) {
                    return -1;
                }
            }
            buf[i] = c;
        }
    }
    return 0;
}

int fs_replace(const char *from, const char *to)
{
#ifdef _WIN32
    wchar_t *wf = wide(from), *wt = wide(to);
    int ok = wf && wt && MoveFileExW(wf, wt, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);

    free(wf);
    free(wt);
    return ok ? 0 : -1;
#else
    return rename(from, to);
#endif
}

int fs_remove(const char *path)
{
#ifdef _WIN32
    wchar_t *wp = wide(path);
    int rc = wp ? _wremove(wp) : -1;

    free(wp);
    return rc;
#else
    return remove(path);
#endif
}

int fs_join(char *out, size_t cap, const char *a, const char *b, const char *c)
{
    const char *parts[3] = { a, b, c };
    size_t n = 0, len;
    int i;

    for (i = 0; i < 3; i++) {
        if (!parts[i]) {
            continue;
        }
        len = strlen(parts[i]);
        if (n + len >= cap) {
            if (cap) {
                out[0] = 0;
            }
            return -1;
        }
        memcpy(out + n, parts[i], len);
        n += len;
    }
    out[n] = 0;
    return 0;
}

int fs_pref_dir(char *buf, size_t cap)
{
    int n;
#ifdef _WIN32
    wchar_t w[MAX_PATH];
    char base[MAX_PATH * 3];

    if (SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, w) != S_OK ||
        WideCharToMultiByte(CP_UTF8, 0, w, -1, base, (int)sizeof(base), NULL, NULL) <= 0) {
        return -1;
    }
    n = snprintf(buf, cap, "%s\\" APP "\\", base);
#else
    const char *home = getenv("HOME");
#ifdef __APPLE__
    if (!home || !*home) {
        return -1;
    }
    n = snprintf(buf, cap, "%s/Library/Application Support/" APP "/", home);
#else
    const char *xdg = getenv("XDG_DATA_HOME");

    if (xdg && *xdg) {
        n = snprintf(buf, cap, "%s/" APP "/", xdg);
    } else if (home && *home) {
        n = snprintf(buf, cap, "%s/.local/share/" APP "/", home);
    } else {
        return -1;
    }
#endif
#endif
    return n > 0 && (size_t)n < cap ? 0 : -1;
}

int fs_list(const char *dir, void (*each)(void *ctx, const char *name), void *ctx)
{
#ifdef _WIN32
    char pat[2048];
    wchar_t *w;
    WIN32_FIND_DATAW fd;
    HANDLE h;

    if (snprintf(pat, sizeof(pat), "%s\\*", dir) >= (int)sizeof(pat) || !(w = wide(pat))) {
        return -1;
    }
    h = FindFirstFileW(w, &fd);
    free(w);
    if (h == INVALID_HANDLE_VALUE) {
        return -1;
    }
    do {
        char name[MAX_PATH * 3];

        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            WideCharToMultiByte(CP_UTF8, 0, fd.cFileName, -1, name, (int)sizeof(name), NULL, NULL) > 0) {
            each(ctx, name);
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return 0;
#else
    DIR *dp = opendir(dir);
    struct dirent *de;

    if (!dp) {
        return -1;
    }
    while ((de = readdir(dp)) != NULL) {
        char full[4096];
        struct stat st;

        if (snprintf(full, sizeof(full), "%s/%s", dir, de->d_name) < (int)sizeof(full) && stat(full, &st) == 0 &&
            S_ISREG(st.st_mode)) {
            each(ctx, de->d_name);
        }
    }
    closedir(dp);
    return 0;
#endif
}
