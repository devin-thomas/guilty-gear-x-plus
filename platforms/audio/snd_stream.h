/* The sound command stream (PORT-011): what the game sends its sound driver.
 *
 * The game talks to SNDRV.IRX, its IOP sound driver, through one SIF RPC
 * server (0x1010101). soundnew.c queues each request (SnDrvEntry) and
 * SnDrvMain sends it with sceSifCallRpc: the command number, with 0x80 set
 * on the last command of a batch, and its arguments. The SIF shim hands
 * every call to the host (CoreHost.snd_rpc) before it writes the reply;
 * that is the stream. audio/README.md lists the commands.
 *
 * A stream file (.ggxs) records it, so a run's sound can be rendered again
 * without the Core (ggx-snd), and a modified PCSX2 can play the same
 * commands through the game's own driver. All
 * values little-endian:
 *
 *   "GGXSND\0\0", u32 version (1), u32 release (1 = KR, 2 = JP)
 *   records, each a tag byte:
 *     'C' u32 field, u32 command, u32 size, then size bytes: an RPC call
 *     'R' u32 field, u32 lsn, u32 sectors: the game read the disc itself
 *         (sceCdRead), which moves the drive the driver streams from
 *     'F' u32 field, u32 vblanks: the field ended, after that many PS2 vblanks
 *     'E' the end
 *
 * A field's commands come before its 'F'. The vblanks are the field's
 * length in time: in a Lockstep Trace replay, the PS2's (a field that took
 * the PS2 two vblanks to run takes two here); in play, one.
 */
#ifndef GGX_SND_STREAM_H
#define GGX_SND_STREAM_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define SND_STREAM_VERSION 1u

/* The driver's commands (sedrv_command in SNDRV.IRX). */
enum {
    SND_INIT = 0x00,       /* initialise the SPU2 */
    SND_MVOL = 0x01,       /* core 0's master volume (not sent by the game) */
    SND_SE_STOP = 0x0a,    /* key off one core-1 voice */
    SND_SE_ALLSTOP = 0x0b, /* key off every core-1 voice */
    SND_SE_VOLUME = 0x0c,  /* the effects' volume (not sent by the game) */
    SND_SE_TRANS = 0x0d,   /* copy an effect into SPU2 RAM and play it */
    SND_SE_AGAIN = 0x0f,   /* play the effect copied last again */
    SND_SE_LOAD_PC = 0x10, /* load a voice bank from the host (development) */
    SND_SE_LOAD_CD = 0x11, /* load a voice bank from the disc into IOP RAM */
    SND_SE_BANK = 0x12,    /* copy an effect from a bank into SPU2 RAM and play it */
    SND_KEY_ON = 0x13,     /* the driver's own: key on what the batch started */
    SND_BGM_LOAD_CD = 0x14, /* stream BGM from the disc */
    SND_BGM_LOAD_PC = 0x15, /* stream BGM from the host (development) */
    SND_BGM_START = 0x16,
    SND_BGM_STOP = 0x17,
    SND_BGM_VOLUME = 0x18, /* volume and stereo or mono */
    SND_BGM_KEEP = 0x19,   /* play BGM already in IOP RAM */
    SND_BGM_STATUS = 0x1f, /* is BGM using the disc? */
    SND_SE_STATUS = 0x20,  /* is a bank still loading? */
    SND_LAST = 0x80        /* flag: the batch's last command */
};

/* A command's name ("se_trans"), or NULL for one the driver ignores. */
const char *snd_cmd_name(uint32_t cmd);
/* One line describing a command and its arguments, for the stream's dump. */
void snd_cmd_describe(uint32_t cmd, const uint8_t *data, uint32_t size, char *buf, size_t cap);

typedef struct SndLog SndLog;

/* Writing a stream file. NULL if it can't be created. */
SndLog *snd_log_create(const char *path, uint32_t release);
void snd_log_cmd(SndLog *l, uint32_t field, uint32_t cmd, const uint8_t *data, uint32_t size);
void snd_log_disc(SndLog *l, uint32_t field, uint32_t lsn, uint32_t sectors);
void snd_log_field(SndLog *l, uint32_t field, uint32_t vblanks);
/* Writes the end record and closes; 0 if every write succeeded. */
int snd_log_close(SndLog *l);

typedef struct {
    char tag;              /* 'C', 'R', 'F' or 'E' */
    uint32_t field;
    uint32_t cmd, size;    /* 'C' */
    const uint8_t *data;   /* 'C': valid until the next read */
    uint32_t lsn, sectors; /* 'R' */
    uint32_t vblanks;      /* 'F' */
} SndEvent;

typedef struct SndReader SndReader;

/* Reading one: NULL with a message in err if it can't be opened. */
SndReader *snd_read_open(const char *path, uint32_t *release, char *err, size_t cap);
/* The next record: 1, or 0 at the end, -1 if the file is cut short or bad. */
int snd_read_next(SndReader *r, SndEvent *ev);
void snd_read_close(SndReader *r);

#endif
