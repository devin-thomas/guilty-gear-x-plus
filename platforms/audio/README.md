# audio/: the sound driver (PORT-011)

The game plays its sound through SNDRV.IRX, an IOP module on the disc, which drives the SPU2 through Sony's
libsd (LIBSD.IRX). The port runs neither the IOP nor the SPU2 (ADR-011). This directory is the same driver
re-implemented on an SPU2 model, fed by the commands the game sends it:

| File | What |
| --- | --- |
| `snd_stream.{h,c}` | the command stream: the commands' names and arguments, and the `.ggxs` file that records them |
| `audio_host.{h,c}` | the layer on a Core host: takes the stream from the SIF shim (`CoreHost.snd_rpc`), records it, renders it |
| `sndrv.{h,c}` | SNDRV.IRX: its RPC server (sndrv.c), effects and voices (se.c), BGM streaming (bgm.c), and the libsd calls it makes |
| `spu2.{h,c}`, `spu2_regs.c` | the SPU2, bit-exact to PCSX2 2.8.2's: 2 cores of 24 voices, 2 MB of sound RAM, ADPCM, Gaussian interpolation, ADSR, volumes, mixing, reverb, noise; driven through its registers |
| `snd_main.c` | `ggx-snd`: dumps a stream, or renders it to WAV without the Core (also as stems, for the clip analysis, and as an SPU2 tap) |
| `spu2_tap.{h,c}`, `spu2_replay_main.c`, `spu2_compare_main.c` | the SPU2 lockstep's tools: the tap format, `ggx-spu2-replay` (a tap of PCSX2's SPU2 into the model, sample for sample) and `ggx-spu2-compare` (the port's driver against the game's, at the SPU2) |

`sdl/audio_out.c` plays the samples in ggx. Nothing here writes Core memory: the shim hands the audio layer a read-only pointer to the
bytes the game sent (`ee_rptr`, which doesn't even mark the pages touched), and the driver keeps all its state
in its own structure.

## Where the game talks to its driver

All of it is in `src/ggxeuc/soundnew.c`. `SnDrvEntry` queues a command (up to 64 per batch, 128 KB each) and
`SnDrvMain` sends the queue with `sceSifCallRpc` to the RPC server SNDRV.IRX registers (0x1010101), which
`SnDrvInit` binds. The last command of a batch has 0x80 set. Two calls are sent asynchronously (mode 1) and end
the batch: 0x8D, which carries an effect's samples, and 0x92. `SnDrvMain` runs from `SnDrvEntry` and from the
game's loop; while an asynchronous call is still running (`sceSifCheckStatRpc`) it sends nothing, so on a PS2
a batch can spread over fields. Every field it also asks for the BGM's state (0x1F) and, while a voice bank
loads, for the bank's (0x20). `sound.c` and `cmn_se.c` are the callers (`sound_request_main`, `voice_request`,
`BGM_Request`...). The SDK's own SIF code is not compiled into the port; the shim (`common/shims/shim_sif.c`)
is the boundary, and the stream is what crosses it. The CD shim adds the game's own disc reads (`sceCdRead`'s
LSN and length, `CoreHost.snd_cd_read`): they don't go to the driver, but they move the drive it streams the
BGM from, so the stream records them ('R') and the driver's drive model follows them.

## The commands

The driver's dispatcher (`sedrv_command`) masks the command with 0x7F. Arguments are 32-bit words.

| Cmd | Sent by | Arguments | What the driver does |
| ---: | --- | --- | --- |
| 0x00 | `SnDrvInit` | | `sndrv_init`: both cores' switches, volumes and voices, reverb off, effects' mixing (below) |
| 0x01 | (not sent) | volume | core 0's master volume |
| 0x0A | `SnDrvStop` | voice | an effect voice: volume 0, key off. `sound_request_main` stops the next voice in turn after every request |
| 0x0B | `SnDrvAllStop` | | key off every core-1 voice |
| 0x0C | (not sent) | left, right | the effects' volume |
| 0x0D (0x8D) | `SnDrvSeTrans` | voice, size, EE address, then at byte 0x40 the effect's samples | copies the effect into sound RAM and plays it: from an effect file the game keeps in EE RAM (the common effects, both characters' voices) |
| 0x0F | `SnDrvSeTrans`, `SnDrvSeVReq` | voice, 0 | plays the effect copied last again, on another voice (no copy) |
| 0x10 | `SnDrvSeVLoadPC` | bank, file name | loads a bank from the development host: never on a player's disc |
| 0x11 | `SnDrvSeVLoadCD` | bank, LSN, bytes | reads a bank of a character's private effects from the disc into IOP RAM (bank 0 or 1, 256 KB each) |
| 0x12 (0x92) | `SnDrvSeVReq` | voice, bank, effect | copies effect *n* of a bank into sound RAM and plays it |
| 0x14 | `SnDrvBgmLoadCD` | LSN, sectors | the next BGM, streamed from the disc |
| 0x15 | `SnDrvBgmLoadPC` | file name | BGM from the host: development only |
| 0x16 | `SnDrvBgmStart` | loop | starts it |
| 0x17 | `SnDrvBgmStop` | | stops it |
| 0x18 | `SnDrvBgmVol`, `SnDrvBgmMode` | volume, mono | the BGM's volume (the game sends its 0-15 setting << 10) and stereo or mono; the fades send it every field |
| 0x19 | `SnDrvBgmKeep` | loop, bytes | plays BGM already in IOP RAM (the VS screen's jingle) |
| 0x1F | `SnDrvMain` | | answers 1 while BGM is loading or streaming from the disc |
| 0x20 | `SnDrvMain` | | answers the bank load's state: 3 reading, 0 done, -1 failed |

Any command with 0x80 also queues the driver's own 0x13: key on every voice the batch started, at once. The
driver ignores 0x02-0x09, 0x0E, 0x13 and 0x1A-0x1E from the game.

`ggx-snd dump <stream>` prints them; a KR Arcade run from the boot sends about 8,700 calls in 8,000 fields:

    f001959 vb002117 bgm_load_cd lsn 92282, 880 sectors (last)
    f001959 vb002117 bgm_start loop 0 (last)
    f002662 vb002821 se_trans voice 4, 3504 bytes from EE 0x01e2c4a4 (last)
    f002663 vb002822 se_stop voice 5 (last)
    f002672 vb002831 se_again voice 5 (last)

### The replies stay zeros

The shim answers every call with a zeroed reply, as before PORT-011: no BGM playing, every bank loaded. A
PS2 answers 0x1F with 1 while the BGM streams (the Lockstep Traces record it), and the game uses that in
`IOPLock`, which stops the music and waits before the game reads the disc itself, and in `BGM_Request`, which
doesn't restart the song already playing. During a trace replay the shim gives the PS2's answers, so the
stream the driver gets is the PS2's, call for call. In play the replies don't follow the driver: making them
would change what the Core computes (and the completion runs' expectations), so audio could no longer be
off without changing the game. 1.0.0 keeps the zeros.

## How the driver was derived

SNDRV.IRX keeps its symbols (sndrv.c, se.c, bgm.c and their functions and variables), and was compiled
without optimisation, so its code reads like its C. It was disassembled and decompiled (Ghidra, for reading)
and re-written here function for function, in the same order and with the same names: `sndrv_init`,
`sedrv_command`, `se_command`, `se_main`, `se_trans`, `se_stop`, `se_load_cd`, `bgm_main` (as a state machine,
below), `bgm_irq`, `bgm_vol_out`. LIBSD.IRX, which has no symbols, was read through its export table (the
indices SNDRV.IRX imports by) to get what each libsd call does to the SPU2's registers: `sceSdSetParam`,
`sceSdSetSwitch`, `sceSdSetAddr`, `sceSdGetAddr`, `sceSdSetCoreAttr`, `sceSdVoiceTrans`,
`sceSdSetEffectAttr`, and what `sceSdInit` leaves. No code from either module is in the port, only what they
do. Some of what that showed:

- **Effects.** Core 1's voices 1-16, in turn, at pitch 0x555 (16 kHz), ADSR 81FFh/DFE1h, volume 1FFFh. An
  effect from EE RAM (0x0D) is copied to the next place in a ring of sound RAM from 0xD000 to 0x1F0000. The
  copy is the call's size less 0x50, rounded *down* to 64 bytes plus the 16 bytes the EE sends after the
  samples, so up to 0x3F bytes of what an earlier call left in the RPC buffer go with it; the last 16-byte
  block of the copy is then made an end (header 0, flags 1). The driver keeps its own RPC buffer for that
  reason. An effect whose first byte is negative is skipped ("Voice CutOff").
- **BGM.** 44.1 kHz SPU-ADPCM, stereo, interleaved on the disc in 16 KB chunks (left, then right), each chunk
  beginning with a loop-start block and ending with a loop-end-and-repeat block. Core 0 plays it on voices
  18-21: 18 and 20 the left, 19 and 21 the right, from two 16 KB rings of sound RAM (0x5000 and 0x9000) whose
  loop addresses the driver sets. The BGM thread polls the left and right voices' NAX and refills a ring's
  half as the voices leave it (8 KB transfers, chained by the transfer interrupt), then reads the next 32 KB
  from the disc. If a voice wraps before its half was refilled it restarts the stream ("BGM Miss Found").
  It also writes 3 (1 at the end of a song that doesn't loop) into the *last* byte of each half of its read
  buffer: a sample, not the flags, so it changes two nibbles of each chunk's last block and nothing else.
- **Each BGM channel is played twice** (voices 18 and 20, 19 and 21) because of the envelope: with decay shift
  0 the first decay step takes the level from 7FFFh to 3FFFh before the sustain level (0xF, the top) is
  checked, so every BGM voice sustains at half. Two of them make full level. PCSX2 and DuckStation step the
  same way; checking the level first instead made the port's BGM exactly twice PCSX2's.
- **Reverb is off.** `sndrv_init` calls `sceSdSetEffectAttr` with mode 0 and depth 0 for core 0 (twice: the
  loop's core argument is always 0), clears both cores' effect enable bits and sends no voice to either wet
  mix; nothing turns it on later. So no cue in this game is reverberated, on a PS2 either.
- **Mixing.** Core 0's voices go dry to core 0's output (MMIX 0xC00), which reaches core 1 through its
  external input at volume 7FFFh; core 1 mixes its own voices with that (MMIX 0xC0C). Both main volumes are
  3FFFh. `sndrv_mvolume` calls `sceSdSetSwitch` on the main volume registers, which writes their neighbours
  too; that is modelled (`sd_set_switch`).

## The SPU2 model

`spu2.c` and `spu2_regs.c` are bit-exact to PCSX2 2.8.2's SPU2: replayed the same register writes and
transfers at the same samples, every sample they mix is PCSX2's, which `ggx-spu2-replay` checks on
recordings of the game's driver and of a synthetic corpus. It is
written from PCSX2's observed behaviour and psx-spx's description of the hardware, not from either's code.
Software drives it through the registers (`spu2_write`, `spu2_read`), as the IOP does; libsd in `sndrv.c`
makes the calls LIBSD.IRX makes, write for write.

- **Registers:** an address is two words, the high one first; a voice's SSA, LSA and NAX keep 20 bits,
  8-aligned (a NAX write points past the block header, a LSA write stops the blocks' loop-start flags moving
  it); a 24-bit switch is two words, each replacing its half; KON and KOFF hold what was last written to
  each half until the next sample applies them (key off first), so a second write in one sample replaces
  the first; writing ENDX clears its half; MMIX gates the mixes (core 0 has no external input), the gates
  starting all open but core 0's external and core 1's dry external, whatever MMIX reads as.
- **Decoding:** each sample, whether the voice plays or not, the block at NAX gives its flags, and while the
  voice's FIFO holds 12 samples or fewer NAX moves on a word and four samples are decoded into it; at a
  block's end, a loop-end flag sets ENDX and jumps to LSA (without repeat, the voice stops). A decoded block
  is kept per block of RAM and shared: a voice entering it from the same two previous samples reuses it, and
  one that doesn't decodes it again for every voice reading it; a write from outside the mixer discards it.
  ADPCM: shift 0-15, filters 0-4 (5-15 predict 0).
- **Pitch and interpolation:** the 12-bit pitch counter, clamped at 3FFFh, pitch modulation from the previous
  voice's last output, and the hardware's Gaussian table on the FIFO's four samples, each product shifted
  before the sum.
- **ADSR:** each phase's rate is taken from ADSR1 and ADSR2 when either is written (zero, which ends every
  phase at once, until then); a step, then the target check; the slowest rate (7Fh) still steps, once every
  8000h samples; exponential decreases scale the step by the level.
- **Volumes:** direct (bit 15 clear: the value doubled) or sliding by the envelope's steps, the slowest rate
  holding; reverb return, external and sound data input volumes as signed 16-bit.
- **Mixing:** per core, voices to the dry and wet mixes (VMIX), saturated, then the MMIX gates for voices, the
  sound data input (ADMA buffers) and the external input, the reverb, and the main volume, saturated at each
  core's output. Voices 1 and 3's outputs, the cores' mixes and core 0's output are written into sound RAM.
- **Reverb:** on each core's work area (ESA to EEA's high word with its low 16 bits set; none when ESA is not
  below EEA), the left side on even samples and the right on odd ones, its products wrapping at 32 bits as
  PCSX2's do; the 39-tap resampling filter as PCSX2's SSE4.1 path rounds it (each product to 16 bits, eight
  lanes, saturating); the work area written only while ATTR bit 7 is set. On core 1, DIFF_L_SRC and
  DIFF_R_SRC are the other way round, as in PCSX2's register table.
- **Noise,** on the core's noise clock (ATTR bits 8-13). **Not modelled:** IRQs, and the SPDIF modes that send
  the SPU2's input straight out.
- **Power-on** is PCSX2's: RAM zero but two blocks that loop on themselves (0x2800, where the voices start, and
  0xE870), pitch 3FFFh, the reverb areas at the top of RAM.

## Time

The IOP's threads become one timeline at 48 kHz (`sndrv.h`):

- the RPC thread runs calls in order, each when the last one's DMA transfer is done, at 64 bytes a sample
  (PCSX2's rate of a word every 24 IOP cycles), and the effect thread runs as soon as it waits, so an effect
  is keyed on once its upload has finished; after a key on the effect thread waits for the next sample, so two
  key ons never share one (on the IOP the thread's calls take time, and the BGM thread's loop its share);
- the module's start is the module's: the BGM's volumes, then `sceSdInit` write for write as LIBSD.IRX makes
  them (its two waits, 36 and 37 samples, run on the SPU2 before the driver's clock starts);
- the BGM thread polls every 32 samples, and its 32 KB byte-copy of a kept BGM takes 19 ms (26 unoptimised
  instructions a byte, as PCSX2's IOP runs them);
- the disc drive has a head, a spindle speed and a busy time, as PCSX2 2.8.2 models its drive (measured
  there with a probe build): no seek within 8
  sectors of the last read, 30 ms within 4,371, 100 ms beyond; a ~6 ms start and 0.8 ms a sector; and a
  311-344 ms change of speed (more the further out the head, linear in the LSN between the two
  measurements) when the driver, which reads at the maximum speed, follows the game's own reads, which use
  the nominal one (`file.c`). The game's reads reach the audio layer from the CD shim, read-only, and the
  stream records them.

With these every BGM start follows PCSX2's within 5.5 ms. The first BGM read after the game's reads
(title, select, profile) is the one that pays the speed change; at the stage the driver's own bank loads
already have, unheard. A real PS2's drive is not measured here.

`audio_host.c` gives the driver a field's commands at the field's start and renders 800.8 samples per vblank
(48000 x 1001 / 60000, counted exactly). In a Lockstep Trace replay a field lasts as many vblanks as it took
the PS2; in play, one, the port's 59.94 Hz tick.

## Rollback (PORT-013)

Under rollback the layer follows the session (`audio_host_follow`):
each field plays once, as it first runs; fields run again after a rollback play nothing and only have their
calls collected; when they sent other calls than what played, the playing driver becomes a copy
(`sndrv_copy`) of a second driver kept at the last confirmed field and is fed the fields since, without
playing them. `ggx-rollback-test --audio` checks that every confirmed field's calls reach the driver once,
as a run without rollback's do, and that only fields played while a prediction was wrong sound different.
