/* ggx's settings (PORT-010): one text file in the per-OS user data
 * directory, <data dir>ggx.cfg (fsys.h fs_pref_dir: %APPDATA%\ggx-plus\ on
 * Windows, ~/Library/Application Support/ggx-plus/ on macOS,
 * ~/.local/share/ggx-plus/ on Linux), or --config <file>.
 *
 *   # comments
 *   window = windowed
 *   p1.x = key:Z, pad:a
 *
 * One "key = value" per line. Keys are kept in the order read; set adds
 * new ones at the end, and save writes them all back, so a hand-edited file
 * keeps its order. Nothing in it reaches the Core.
 *
 * The file is port-settings from game-kit (pset.h); these are the names the
 * port has always used for it.
 */
#ifndef GGX_CONFIG_H
#define GGX_CONFIG_H

#include "pset/pset.h"

typedef PortSettings Config;

/* An empty config bound to path; the file is read if it exists. NULL only
   when out of memory. A line that isn't "key = value" is reported on stderr
   and dropped. */
static inline Config *config_load(const char *path)
{
    PsetOptions o = { "ggx", "ggx settings (docs/settings.md in the source). ggx rewrites this file: values are kept, "
                             "comments are not.", NULL, NULL };

    return pset_open(path, &o);
}

static inline void config_free(Config *c) { pset_free(c); }
/* NULL if the key is not set. */
static inline const char *config_get(const Config *c, const char *key) { return pset_get(c, key); }
static inline const char *config_get_or(const Config *c, const char *key, const char *fallback)
{
    return pset_get_or(c, key, fallback);
}
static inline void config_set(Config *c, const char *key, const char *value) { pset_set(c, key, value); }
/* Writes the file (creating its directory). 0 on success. */
static inline int config_save(const Config *c) { return pset_save(c); }
static inline const char *config_path(const Config *c) { return pset_path(c); }

#endif
