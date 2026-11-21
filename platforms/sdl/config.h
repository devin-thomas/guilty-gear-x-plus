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
 */
#ifndef GGX_CONFIG_H
#define GGX_CONFIG_H

typedef struct Config Config;

/* An empty config bound to path; the file is read if it exists. NULL only
   when out of memory. A line that isn't "key = value" is reported on stderr
   and dropped. */
Config *config_load(const char *path);
void config_free(Config *c);
/* NULL if the key is not set. */
const char *config_get(const Config *c, const char *key);
const char *config_get_or(const Config *c, const char *key, const char *fallback);
void config_set(Config *c, const char *key, const char *value);
/* Writes the file (creating its directory). 0 on success. */
int config_save(const Config *c);
const char *config_path(const Config *c);

#endif
