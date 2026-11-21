# Your disc

The port contains none of the game. It needs an image of your own Guilty
Gear X Plus disc, and copies the files it needs from it once.

## Supported discs

| Release | Serial |
| --- | --- |
| Guilty Gear X Plus (Korea) | SLPM-645.25 |
| Guilty Gear X Plus (Japan) | SLPS-201.37 |

Both are PS2 CDs. Other games, other versions of Guilty Gear X (the PS2
Guilty Gear X, SLUS-202.87, among them) and modified discs aren't supported.

The image can be:

- **BIN/CUE:** a raw copy of the CD, the `.bin` with its `.cue` sheet
  (choose the `.cue`). A `.bin` without its sheet works too.
- **ISO:** the CD's data, 2,048 bytes a sector.
- **CHD:** MAME's compressed format, made with `chdman createcd` from a
  BIN/CUE (or `createdvd` from an ISO). The smallest, and the slowest to
  extract (tens of seconds).

## Making an image of your disc

You need a PC or Mac with a CD or DVD drive that reads PS2 discs (most do).

- **Windows:** a disc imaging tool that writes BIN/CUE, such as ImgBurn
  (Create image file from disc) or CDRTools.
- **macOS:** a disc imaging tool that writes BIN/CUE or ISO.

If you have a dump checked against Redump's database, that is exactly what
the port expects. The port checks every file of the image itself, so a bad
dump is caught either way (below).

## The first run

Start the port. With no disc files extracted yet, it shows its own screen
before the game:

1. **Choose the disc image.** Press X (the south face button on any pad),
   Enter or click, and choose the image in the file dialog. You can also
   drop the image on the window.
2. **The port checks it.** It identifies the release by the disc's files,
   so a renamed image works. The image must hold exactly the release's
   files, with their sizes.
3. **It extracts.** A bar shows the file and the megabytes copied. Every
   file is checked against the release's own SHA-256 as it is copied. From
   a BIN or ISO this takes a few seconds; from a CHD, longer.
4. **"Ready", and the game starts.** Every later launch goes straight to
   the game.

The files go into `assets/kr/` or `assets/jp/` in the port's data folder
([Installing](install.md#where-the-port-keeps-your-data)), about 515 MB, with
a `manifest.txt` listing each file's hash. The image itself isn't needed
afterwards, but keep it: the port uses it to repair the files (below).

### If the image is refused

The screen says why, then lists the two supported releases. You can choose
another image, or quit (Circle or Escape).

| The message says | Meaning |
| --- | --- |
| "is not a disc image the port can read" | not an ISO, BIN/CUE or CHD, or the file is damaged |
| "has no ISO 9660 file system" / "no PS2 boot file" | not a PS2 game disc |
| "which boots SLUS_202.87: another game" (or another serial) | a different game |
| "it boots SLPM_645.25 ... but its files differ: CDDATA.BIN is ... bytes" | a modified or incomplete copy of the right game |
| "its CDDATA.BIN is not Guilty Gear X Plus (Korea)'s (SHA-256 ...), so the image is modified or a bad dump" | same files and sizes, different contents: a modified image or a read error while dumping. Dump the disc again |
| "which the port supports, but this copy of it is the build for ..." | the other release's disc, with a package that plays one release only ([Installing](install.md#which-program-to-start)) |

Nothing is left half-extracted: a file goes into place only once it has
been checked.

### Without a file dialog

If the file dialog can't open, put the image (and its `.cue`) in a folder
named `disc` in the port's data folder, for example
`%APPDATA%\ggx-plus\disc\` on Windows, and press X on the first-run screen: the port uses the image it finds there. If
the dialog fails, the screen says so and names that folder.

## Repairs

Every launch checks the extracted files against the manifest (quickly: a
file is hashed again only if it changed on disk). If a file is missing,
cut short or changed, the port extracts it again from the image the
manifest names, without asking, showing the same screen as the first run.
If that image has moved, it asks for the image again.

## Playing both releases

Each release's files are kept apart (`assets/kr/`, `assets/jp/`), so both
can be extracted. Which one plays depends on the program you start
([Installing](install.md#which-program-to-start)). Saves and replays are
per release: a KR save or replay isn't used by the JP build, and the other
way round.
