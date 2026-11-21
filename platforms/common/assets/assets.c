#include "assets.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "fsys.h"
#include "iso9660.h"
#include "sha256.h"

/* more files than any release has, so an image with extra files shows */
#define MAX_FILES 64
/* sectors copied per read */
#define CHUNK 256

const Release *release_by_id(const char *id)
{
    int i;

    for (i = 0; i < GGX_RELEASES; i++) {
        if (!strcmp(ggx_releases[i].id, id)) {
            return &ggx_releases[i];
        }
    }
    return NULL;
}

const ReleaseFile *release_file(const Release *r, const char *path)
{
    unsigned i;

    for (i = 0; i < r->nfiles; i++) {
        if (iso_path_eq(r->files[i].path, path)) {
            return &r->files[i];
        }
    }
    return NULL;
}

void release_list(char *buf, size_t cap)
{
    size_t n = 0;
    int i;

    buf[0] = 0;
    for (i = 0; i < GGX_RELEASES && n < cap; i++) {
        int k = snprintf(buf + n, cap - n, "  %s, %s\n", ggx_releases[i].title, ggx_releases[i].serial);

        if (k < 0) {
            break;
        }
        n += (size_t)k;
    }
}

/* snprintf for messages, which may be cut short */
static void say(char *out, size_t cap, const char *fmt, ...)
#if defined(__GNUC__) || defined(__clang__)
    __attribute__((format(printf, 3, 4)))
#endif
    ;

static void say(char *out, size_t cap, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(out, cap, fmt, ap);
    va_end(ap);
}

/* A failure: to msg, and to the watch's why without "ggx: " */
static void report(FILE *msg, const ExtractWatch *w, const char *fmt, ...)
#if defined(__GNUC__) || defined(__clang__)
    __attribute__((format(printf, 3, 4)))
#endif
    ;
static void report(FILE *msg, const ExtractWatch *w, const char *fmt, ...)
{
    char text[2048];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(text, sizeof(text), fmt, ap);
    va_end(ap);
    fputs(text, msg);
    if (w && w->why && w->why_cap) {
        say(w->why, w->why_cap, "%s", strncmp(text, "ggx: ", 5) ? text : text + 5);
    }
}

/* Bytes read so far, for the watch. */
typedef struct {
    const ExtractWatch *w;
    const char *file;
    uint64_t done, total;
} Progress;

static const IsoFile *listed(const IsoFile *files, int n, const char *path)
{
    int i;

    for (i = 0; i < n; i++) {
        if (iso_path_eq(files[i].path, path)) {
            return &files[i];
        }
    }
    return NULL;
}

/* The refusal: what is wrong with the image, then what the port needs. */
static const Release *refuse(char *why, size_t cap, const char *path, const char *what)
{
    char list[256];

    release_list(list, sizeof(list));
    say(why, cap,
        "%s is not a disc this port supports: %s\n"
        "The port needs your own copy of one of these, as an ISO, BIN/CUE or CHD image:\n%s",
        path, what, list);
    return NULL;
}

/* The boot ELF that SYSTEM.CNF names ("BOOT2 = cdrom0:\SLPM_645.25;1"),
   without its path and version. 0 on success. */
static int boot_name(DiscImage *img, const IsoFile *files, int n, char *out, size_t cap)
{
    const IsoFile *cnf = listed(files, n, "SYSTEM.CNF");
    char text[2049], *p, *e;

    if (!cnf || disc_image_read(img, cnf->lsn, 1, text) != 0) {
        return -1;
    }
    text[cnf->size < 2048 ? cnf->size : 2048] = 0;
    p = strstr(text, "cdrom0:");
    if (!p) {
        return -1;
    }
    for (p += 7; *p == '\\' || *p == '/'; p++) {
    }
    for (e = p; *e && *e != ';' && *e != '\r' && *e != '\n' && *e != ' '; e++) {
    }
    say(out, cap, "%.*s", (int)(e - p), p);
    return 0;
}

/* The first way the image's files differ from r's, or "" if they don't. */
static void differ(const Release *r, const IsoFile *files, int n, char *out, size_t cap)
{
    unsigned k;
    int i;

    out[0] = 0;
    for (k = 0; k < r->nfiles; k++) {
        const IsoFile *f = listed(files, n, r->files[k].path);

        if (!f) {
            say(out, cap, "%s is missing", r->files[k].path);
            return;
        }
        if (f->size != r->files[k].size) {
            say(out, cap, "%s is %u bytes, not %u", f->path, f->size, r->files[k].size);
            return;
        }
    }
    for (i = 0; i < n; i++) {
        if (!release_file(r, files[i].path)) {
            say(out, cap, "it has a file the release doesn't, %s", files[i].path);
            return;
        }
    }
}

const Release *assets_identify(DiscImage *img, const char *path, char *why, size_t cap)
{
    IsoFile files[MAX_FILES];
    char volume[40], boot[64], diff[192], what[512];
    int n, i;

    if (iso_volume(disc_image_read, img, volume, NULL) != 0) {
        return refuse(why, cap, path, "it has no ISO 9660 file system, so it is not a PS2 game disc.");
    }
    n = iso_list(disc_image_read, img, files, MAX_FILES);
    if (n < 0 || n > MAX_FILES) {
        return refuse(why, cap, path, n < 0 ? "its file system can't be read; the image is damaged or cut short."
                                            : "it has far more files than Guilty Gear X Plus.");
    }
    if (boot_name(img, files, n, boot, sizeof(boot)) != 0) {
        say(what, sizeof(what), "it is \"%s\", which has no PS2 boot file (SYSTEM.CNF).", volume);
        return refuse(why, cap, path, what);
    }
    for (i = 0; i < GGX_RELEASES; i++) {
        const Release *r = &ggx_releases[i];

        if (!iso_path_eq(boot, r->boot)) {
            continue;
        }
        differ(r, files, n, diff, sizeof(diff));
        if (!diff[0]) {
            return r;
        }
        say(what, sizeof(what), "it boots %s, as %s does, but its files differ: %s. It is modified or "
                 "another version.", boot, r->title, diff);
        return refuse(why, cap, path, what);
    }
    if (volume[0]) {
        say(what, sizeof(what), "it is \"%s\", which boots %s: another game.", volume, boot);
    } else {
        say(what, sizeof(what), "it boots %s: another game.", boot);
    }
    return refuse(why, cap, path, what);
}

int assets_dir(char *buf, size_t cap, const char *data_dir, const Release *r)
{
#ifdef _WIN32
#define SEP "\\"
#else
#define SEP "/"
#endif
    size_t n = strlen(data_dir);
    const char *sep = n && (data_dir[n - 1] == '/' || data_dir[n - 1] == '\\') ? "" : SEP;
    char id[3] = { 0 }, tail[16];

    id[0] = (char)tolower((unsigned char)r->id[0]);
    id[1] = (char)tolower((unsigned char)r->id[1]);
    fs_join(tail, sizeof(tail), "assets" SEP, id, SEP);
    /* leaves room for any release file's name and ".part" after it */
    return cap > 64 && fs_join(buf, cap - 64, data_dir, sep, tail) == 0 ? 0 : -1;
}

int assets_hash_file(const char *path, char hex[65])
{
    FILE *f = fs_open(path, "rb");
    static uint8_t buf[CHUNK * 2048];
    uint8_t digest[32];
    Sha256 s;
    size_t n;

    if (!f) {
        return -1;
    }
    sha256_init(&s);
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        sha256_update(&s, buf, n);
    }
    n = (size_t)ferror(f);
    fclose(f);
    if (n) {
        return -1;
    }
    sha256_final(&s, digest);
    sha256_hex(digest, hex);
    return 0;
}

/* 1 if the file at path is f, byte for byte */
static int intact(const char *path, const ReleaseFile *f)
{
    uint64_t size;
    char hex[65];

    return fs_stat(path, &size, NULL) == 0 && size == f->size && assets_hash_file(path, hex) == 0 &&
           !strcmp(hex, f->sha256);
}

/* Reads the image's copy of a file, hashing it and writing it to out if
   out isn't NULL. 0 on success, -1 if the image can't be read, -2 if out
   can't be written. */
static int stream(DiscImage *img, const IsoFile *src, FILE *out, char hex[65], Progress *pr)
{
    static uint8_t buf[CHUNK * 2048];
    uint32_t lsn = src->lsn, left = src->size;
    uint8_t digest[32];
    Sha256 s;

    sha256_init(&s);
    while (left) {
        uint32_t sectors = (left + 2047) / 2048, n = left < sizeof(buf) ? left : (uint32_t)sizeof(buf);

        if (sectors > CHUNK) {
            sectors = CHUNK;
        }
        if (disc_image_read(img, lsn, sectors, buf) != 0) {
            return -1;
        }
        sha256_update(&s, buf, n);
        if (out && fwrite(buf, 1, n, out) != n) {
            return -2;
        }
        lsn += sectors;
        left -= n;
        if (pr) {
            pr->done += n;
            if (pr->w->progress) {
                pr->w->progress(pr->w->ctx, pr->file, pr->done, pr->total);
            }
            if (pr->w->stop && pr->w->stop(pr->w->ctx)) {
                return -3;
            }
        }
    }
    sha256_final(&s, digest);
    sha256_hex(digest, hex);
    return 0;
}

#define W(pr) ((pr) ? (pr)->w : NULL)

/* Checks the image's copy of f and, if copy is set, copies it to path
   (via path.part, so a file in place is always whole). 0 on success. */
static int extract_file(DiscImage *img, const IsoFile *src, const ReleaseFile *f, const char *path,
                        const Release *r, const char *image_path, int copy, FILE *msg, Progress *pr)
{
    char part[1100], hex[65];
    FILE *out = NULL;
    int rc;

    fs_join(part, sizeof(part), path, ".part", NULL);
    if (copy) {
        out = fs_open(part, "wb");
        if (!out) {
            report(msg, pr ? pr->w : NULL, "ggx: cannot write %s\n", part);
            return -1;
        }
    }
    rc = stream(img, src, out, hex, pr);
    if (out && fclose(out) != 0 && rc == 0) {
        rc = -2;
    }
    if (rc == -1) {
        report(msg, W(pr), "ggx: cannot read %s from %s: the image is damaged or cut short\n", f->path, image_path);
    } else if (rc == -2) {
        report(msg, W(pr), "ggx: cannot write %s (is the disk full?)\n", part);
    } else if (rc == -3) {
        report(msg, W(pr), "ggx: the extraction was stopped\n");
    } else if (strcmp(hex, f->sha256) != 0) {
        char list[256];

        release_list(list, sizeof(list));
        report(msg, W(pr),
                "ggx: %s is not a disc this port supports: its %s is not %s's (SHA-256 %s, expected %s), "
                "so the image is modified or a bad dump.\n"
                "The port needs your own copy of one of these, as an ISO, BIN/CUE or CHD image:\n%s",
                image_path, f->path, r->title, hex, f->sha256, list);
        rc = -1;
    } else if (out && fs_replace(part, path) != 0) {
        report(msg, W(pr), "ggx: cannot move %s into place\n", part);
        rc = -1;
    }
    if (out && rc != 0) {
        fs_remove(part);
    }
    return rc == 0 ? 0 : -1;
}

static int write_manifest(const char *dir, const Release *r, DiscImage *img, const char *image_path, FILE *msg,
                          const ExtractWatch *w)
{
    char path[1100], part[1100], file[1100];
    unsigned i;
    FILE *out;
    int ok;

    fs_join(path, sizeof(path), dir, "manifest.txt", NULL);
    fs_join(part, sizeof(part), dir, "manifest.txt", ".part");
    out = fs_open(part, "wb");
    if (!out) {
        report(msg, w, "ggx: cannot write %s\n", part);
        return -1;
    }
    fprintf(out, "ggx-assets 1\nrelease %s %s %s\nimage %s %s\n", r->id, r->serial, r->title,
            disc_image_format(img), image_path);
    for (i = 0; i < r->nfiles; i++) {
        const ReleaseFile *f = &r->files[i];
        int64_t mtime = 0;

        fs_join(file, sizeof(file), dir, f->path, NULL);
        if (fs_stat(file, NULL, &mtime) != 0) {
            fclose(out);
            fs_remove(part);
            report(msg, w, "ggx: %s vanished during extraction\n", file);
            return -1;
        }
        fprintf(out, "file %u %u %lld %s %s\n", f->lsn, f->size, (long long)mtime, f->sha256, f->path);
    }
    ok = fclose(out) == 0;
    if (!ok || fs_replace(part, path) != 0) {
        fs_remove(part);
        report(msg, w, "ggx: cannot write %s\n", path);
        return -1;
    }
    return 0;
}

int assets_extract(const char *path, const char *data_dir, ExtractResult *res, FILE *msg)
{
    return assets_extract_watched(path, data_dir, res, msg, NULL);
}

int assets_extract_watched(const char *path, const char *data_dir, ExtractResult *res, FILE *msg,
                           const ExtractWatch *w)
{
    static const ExtractWatch none;
    DiscImage *img = NULL;
    Progress pr;
    IsoFile files[MAX_FILES];
    char why[1024], dir[1024], file[1100];
    const Release *r;
    unsigned i;
    int n, rc = -1, manifest_gone = 0;

    memset(res, 0, sizeof(*res));
    memset(&pr, 0, sizeof(pr));
    pr.w = w ? w : &none;
    if (disc_image_open(&img, path, why, sizeof(why)) != 0) {
        char list[256];

        release_list(list, sizeof(list));
        report(msg, w, "ggx: %s is not a disc image the port can read: %s\n"
                     "The port needs your own copy of one of these, as an ISO, BIN/CUE or CHD image:\n%s",
                path, why, list);
        return -1;
    }
    r = assets_identify(img, path, why, sizeof(why));
    if (!r) {
        report(msg, w, "ggx: %s", why);
        goto done;
    }
    res->release = r;
    for (i = 0; i < r->nfiles; i++) {
        pr.total += r->files[i].size;
    }
    if (pr.w->begin) {
        pr.w->begin(pr.w->ctx, r, pr.total);
    }
    fprintf(msg, "ggx: %s (%s) has the files of %s (%s); checking them against its hashes\n", path,
            disc_image_format(img), r->title, r->serial);
    n = iso_list(disc_image_read, img, files, MAX_FILES);
    if (assets_dir(dir, sizeof(dir), data_dir, r) != 0 || fs_mkdirs(dir) != 0) {
        report(msg, w, "ggx: cannot create %s\n", dir);
        goto done;
    }
    for (i = 0; i < r->nfiles; i++) {
        const ReleaseFile *f = &r->files[i];
        const IsoFile *src = listed(files, n, f->path);
        const char *slash = strrchr(f->path, '/');
        int copied;

        fs_join(file, sizeof(file), dir, f->path, NULL);
        copied = !intact(file, f);
        if (copied && !manifest_gone) {
            /* the extraction is incomplete until every file is in place */
            char m[1100];

            fs_join(m, sizeof(m), dir, "manifest.txt", NULL);
            fs_remove(m);
            manifest_gone = 1;
        }
        if (slash) {
            char sub[1100];

            fs_join(sub, sizeof(sub), dir, f->path, NULL);
            sub[strlen(dir) + (size_t)(slash - f->path)] = 0;
            fs_mkdirs(sub);
        }
        pr.file = f->path;
        if (extract_file(img, src, f, file, r, path, copied, msg, &pr) != 0) {
            goto done;
        }
        if (copied) {
            res->extracted++;
        } else {
            res->kept++;
        }
        fprintf(msg, "  %-14s %s\n", f->path, copied ? "extracted" : "kept");
    }
    rc = write_manifest(dir, r, img, path, msg, w);
    for (i = 0; rc == 0 && i < r->nfiles; i++) {
        /* what an interrupted run left behind */
        fs_join(file, sizeof(file), dir, r->files[i].path, ".part");
        fs_remove(file);
    }
    if (rc == 0) {
        fprintf(msg, "ggx: %s assets in %s: %u extracted, %u already in place\n", r->id, dir, res->extracted,
                res->kept);
    }
done:
    disc_image_close(img);
    return rc;
}

int assets_manifest_image(const char *data_dir, const Release *r, char *image, size_t cap)
{
    char dir[1024], path[1100], line[1200];
    FILE *f;
    int found = -1;

    if (cap) {
        image[0] = 0;
    }
    if (assets_dir(dir, sizeof(dir), data_dir, r) != 0 || fs_join(path, sizeof(path), dir, "manifest.txt", NULL) != 0 ||
        !(f = fs_open(path, "rb"))) {
        return -1;
    }
    if (fgets(line, sizeof(line), f) && !strncmp(line, "ggx-assets ", 11)) {
        found = 0;
    }
    while (found == 0 && fgets(line, sizeof(line), f)) {
        size_t n = strlen(line);
        char *p;

        if (strncmp(line, "image ", 6) != 0) {
            continue;
        }
        while (n && (line[n - 1] == '\n' || line[n - 1] == '\r')) {
            line[--n] = 0;
        }
        /* "image <format> <path>": the format has spaces of its own ("CHD (CD, MODE2_RAW)"), so the path is
           the first tail after a space that names a file */
        for (p = line + 6; (p = strchr(p, ' ')) != NULL; p++) {
            if (fs_stat(p + 1, NULL, NULL) == 0 && strlen(p + 1) < cap) {
                memcpy(image, p + 1, strlen(p + 1) + 1);
                break;
            }
        }
        break;
    }
    fclose(f);
    return found;
}
