# Saves

The game saves exactly as it does on a PS2: from OPTIONS > MEMORY CARD,
with AUTO SAVE, and it loads your save when it starts. The port's memory
card slot 1 holds your saves, kept as files in `saves/` in the port's data
folder ([Installing](install.md#where-the-port-keeps-your-data)): one
`.ggxsave` file per save, written whole each time the game saves.

Each save records its release (KR or JP) and its format version. A save
made while a mod or extension changed the game records that too, so it is
never taken for the original game's.

## Moving saves to and from PCSX2

`ggx-save` converts between the port's saves and PCSX2's memory cards. On
Windows it is `ggx-save.exe` in the port's folder; on a Mac it is
`GGX Plus.app/Contents/MacOS/ggx-save`. There is no menu for it.

Run it from a terminal (Command Prompt or PowerShell on Windows, Terminal
on a Mac, with the full path to the app's copy):

    ggx-save list
    ggx-save card <card>
    ggx-save import <card> [--release kr|jp] [--replace]
    ggx-save export <card.ps2> [--release kr|jp] [--replace]
    ggx-save info <file.ggxsave>

- `list` shows the saves the port has; `card` shows what a memory card
  holds; `info` shows one save file's details.
- **Import** copies the Guilty Gear X Plus save from a PCSX2 memory card:
  a card image (`.ps2` or `.mcd`, 8 MB or larger) or a PCSX2 folder memory
  card. With both releases' saves on the card, `--release` picks one.
  `--replace` overwrites a save the port already has.
- **Export** writes the port's save into a PCSX2 card image, creating and
  formatting the image if it doesn't exist. Close the card in PCSX2 first
  (or close PCSX2).

Export refuses, and writes nothing, if a save couldn't load on a PS2. It
names the reason:

- a newer save format than this `ggx-save` knows;
- the other release (a KR save can't go on as JP, or the other way round);
- a mod or extension that changed what the save holds in a way the PS2
  game can't read;
- data that isn't the original game's save layout.

Saves made with no mods export always.

## Backing up

Copy the `saves/` folder. To restore, put the files back while the port
isn't running.

## Starting over

Quit the port and move the release's save out of `saves/` (its file name
holds the release: `BKSLPM-64525GGX` for KR, `BISLPS-20137GGX` for JP). The
game then starts as on a new memory card.
