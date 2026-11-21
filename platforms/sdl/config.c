/* ggx's settings file (config.h). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "fsys.h"

typedef struct {
    char *key, *value;
} Entry;

struct Config {
    char *path;
    Entry *e;
    size_t n, cap;
};

static char *dup(const char *s, size_t n)
{
    char *d = malloc(n + 1);

    if (d) {
        memcpy(d, s, n);
        d[n] = 0;
    }
    return d;
}

static void trim(const char **s, size_t *n)
{
    while (*n && (**s == ' ' || **s == '\t')) {
        (*s)++, (*n)--;
    }
    while (*n && ((*s)[*n - 1] == ' ' || (*s)[*n - 1] == '\t' || (*s)[*n - 1] == '\r')) {
        (*n)--;
    }
}

Config *config_load(const char *path)
{
    Config *c = calloc(1, sizeof(Config));
    FILE *f;
    char line[1024];
    int no = 0;

    if (!c || !(c->path = dup(path, strlen(path)))) {
        free(c);
        return NULL;
    }
    if (!(f = fs_open(path, "rb"))) {
        return c;
    }
    while (fgets(line, sizeof(line), f)) {
        const char *k = line, *v, *eq, *hash = strchr(line, '#');
        size_t kn, vn;
        char *key, *value;

        no++;
        if (hash) {
            *(char *)hash = 0;
        }
        line[strcspn(line, "\n")] = 0;
        eq = strchr(line, '=');
        kn = eq ? (size_t)(eq - line) : strlen(line);
        trim(&k, &kn);
        if (!eq) {
            if (kn) {
                fprintf(stderr, "ggx: %s:%d: not \"key = value\": ignored\n", path, no);
            }
            continue;
        }
        v = eq + 1;
        vn = strlen(v);
        trim(&v, &vn);
        if (!kn) {
            fprintf(stderr, "ggx: %s:%d: no key: ignored\n", path, no);
            continue;
        }
        key = dup(k, kn);
        value = dup(v, vn);
        if (key && value) {
            config_set(c, key, value);
        }
        free(key);
        free(value);
    }
    fclose(f);
    return c;
}

void config_free(Config *c)
{
    size_t i;

    if (!c) {
        return;
    }
    for (i = 0; i < c->n; i++) {
        free(c->e[i].key);
        free(c->e[i].value);
    }
    free(c->e);
    free(c->path);
    free(c);
}

const char *config_get(const Config *c, const char *key)
{
    size_t i;

    for (i = 0; c && i < c->n; i++) {
        if (!strcmp(c->e[i].key, key)) {
            return c->e[i].value;
        }
    }
    return NULL;
}

const char *config_get_or(const Config *c, const char *key, const char *fallback)
{
    const char *v = config_get(c, key);

    return v ? v : fallback;
}

void config_set(Config *c, const char *key, const char *value)
{
    char *v = dup(value, strlen(value));
    size_t i;

    if (!v) {
        return;
    }
    for (i = 0; i < c->n; i++) {
        if (!strcmp(c->e[i].key, key)) {
            free(c->e[i].value);
            c->e[i].value = v;
            return;
        }
    }
    if (c->n == c->cap) {
        size_t cap = c->cap ? c->cap * 2 : 32;
        Entry *e = realloc(c->e, cap * sizeof(Entry));

        if (!e) {
            free(v);
            return;
        }
        c->e = e;
        c->cap = cap;
    }
    c->e[c->n].key = dup(key, strlen(key));
    c->e[c->n].value = v;
    if (!c->e[c->n].key) {
        free(v);
        return;
    }
    c->n++;
}

int config_save(const Config *c)
{
    char dir[1024], tmp[1100];
    const char *slash = strrchr(c->path, '/');
    FILE *f;
    size_t i;
    int rc = 0;

#ifdef _WIN32
    {
        const char *b = strrchr(c->path, '\\');

        if (b && (!slash || b > slash)) {
            slash = b;
        }
    }
#endif
    if (slash && (size_t)(slash - c->path) < sizeof(dir)) {
        memcpy(dir, c->path, (size_t)(slash - c->path));
        dir[slash - c->path] = 0;
        if (dir[0] && fs_mkdirs(dir) != 0) {
            return -1;
        }
    }
    if (fs_join(tmp, sizeof(tmp), c->path, ".part", NULL) != 0 || !(f = fs_open(tmp, "wb"))) {
        return -1;
    }
    fprintf(f, "# ggx settings (docs/settings.md in the source). ggx rewrites this file: values are kept, comments are not.\n");
    for (i = 0; i < c->n; i++) {
        if (fprintf(f, "%s = %s\n", c->e[i].key, c->e[i].value) < 0) {
            rc = -1;
        }
    }
    if (fclose(f) != 0) {
        rc = -1;
    }
    if (rc == 0) {
        rc = fs_replace(tmp, c->path);
    }
    return rc;
}

const char *config_path(const Config *c)
{
    return c->path;
}
