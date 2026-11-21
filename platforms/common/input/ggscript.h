/* GGScript input scripts, played through the port's input layer (PORT-010).
 *
 * The format is the gg-layers PCSX2 fork's (pcsx2/GGScript.cpp), so a script
 * the recorder plays plays here too:
 *
 *   map D=R1 P=Square        letters P K S H D -> PS2 buttons (defaults P=Square K=Cross S=Triangle H=Circle D=R1)
 *   face right|left          numpad directions are relative to the facing (6 = forward); face2 for pad 2
 *   <frames> <input> [<input2>]  hold pad 1's input (and pad 2's) for that many frames: a numpad digit and/or
 *                            letters ("3", "2D", "6PK"), or PS2 names joined with '+' ("Down+Cross", "Start")
 *   wait <frames>            nothing held
 *   stick <lx> <ly> <rx> <ry>  pad 1's sticks as the bytes the pad reports, held until the next stick line
 *   shot <name>              a screenshot (ggx writes the frame shown; ggx-headless ignores it)
 *   quit                     end the run
 *   turbo, rec, save, log    accepted; the port has no use for them
 *
 * A frame is a PS2 vblank. The port advances the script one frame per vblank
 * the Core counts (sceGsSyncV), before each field, so in play, where a field
 * is a vblank, the timing is the PS2's. Loading differs: the port reads the
 * disc at once where the PS2 takes many vblanks, so a script timed on PCSX2
 * reaches the same screens sooner after a load (waits in the corpus scripts
 * allow for that).
 */
#ifndef GGX_GGSCRIPT_H
#define GGX_GGSCRIPT_H

#include <stddef.h>
#include <stdint.h>

#include "pad.h"

typedef struct GgScript GgScript;

/* Loads a script; NULL with a message (file and line) in err if it can't. */
GgScript *ggs_load(const char *path, char *err, size_t cap);
GgScript *ggs_parse(const char *text, const char *name, char *err, size_t cap);
void ggs_free(GgScript *s);

enum { GGS_PLAYING = 0, GGS_DONE = 1, GGS_QUIT = 2 };

/* Plays one frame: the buttons held on each pad (pad.h bits) and pad 1's
   sticks (lx, ly, rx, ry). After the last step nothing is held and it
   returns GGS_DONE; a quit step returns GGS_QUIT. */
int ggs_frame(GgScript *s, uint16_t buttons[2], uint8_t stick[4]);
/* Plays up to the frame of the vblank a field starts in (the Core's vblank
   count + 1), putting the held buttons in both pads and pad 1's sticks in
   its analog bytes; the result is ggs_frame's for the last frame played. */
int ggs_play_to(GgScript *s, uint64_t frame, PadInput pads[2]);
/* The shot names reached in the last ggs_play_to, NUL-separated, ending in
   an empty name; "" if none. */
const char *ggs_shots(const GgScript *s);
/* Frames played so far, and the script line now playing. */
uint64_t ggs_frames(const GgScript *s);
uint32_t ggs_line(const GgScript *s);

#endif
