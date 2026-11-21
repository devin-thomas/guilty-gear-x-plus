# Replays

Every fight that ends is saved as a replay, while `replays = on` (the
default; [Settings](settings.md#the-settings-file)).

## Where they go

`replays/` in the port's data folder
([Installing](install.md#where-the-port-keeps-your-data)), one file per
fight, named by the date, the time and the release:
`20261031-140512-KR.ggxreplay`.

A replay holds the inputs from the moment the game started, so it plays
every fight before that one too, plus what the memory card held and which
unlocks were on. That is enough to play the game again exactly. It holds
no pictures or sound from the game: those come from your disc when it
plays.

## Watching one

Start the port with `--play` and the replay's file:

    ggx-kr --play replays/20261031-140512-KR.ggxreplay

It runs quickly to the start of the fight and then plays it at normal
speed, with picture and sound, and stops at the end. Close the window to
stop sooner. `--from start` plays the whole session from the boot instead.
The controllers have no effect while a replay plays, and nothing reaches
your saves.

On Windows, run it from the port's folder (`ggx-kr.exe --play ...`); on a
Mac, run `"/Applications/GGX Plus.app/Contents/MacOS/ggx-kr" --play ...`
in Terminal. 1.0.0 has no file association for replays.

`ggx-replays list` lists the replays in your data folder, and
`ggx-replays info <file>` shows one replay's details: the release, its
length and what it needs to play.

## Why a replay may refuse to play

The port checks its State Hash against the one the replay recorded every
second while it plays, and says so if they ever differ. Before it starts, it
checks that it can play the replay at all, and names the cause if not:

| The message says | Meaning |
| --- | --- |
| "it was recorded on the JP (SLPS-201.37) release, and this build of the port runs KR" | the other release: play it with the build for that disc |
| "it was recorded with another boot ELF" | your disc's files differ from the recording's: a different disc |
| "it was recorded with another build of the port's Core" | an update of the port changed the game code it runs, which could play the same inputs differently |
| "it was recorded with the extension ..." / "this run has the extension ... active" | a mod was on when it was recorded and isn't now, or the other way round |

A replay plays on any operating system: one recorded on Windows plays on a
Mac, as long as both run the same version of the port.

A replay is tied to the build of the port that recorded it. Replays
recorded with 1.0.0 play on 1.0.0. A later release that changes the game
code the port runs will refuse them ("recorded with another build of the
port's Core") rather than play them differently; keep the 1.0.0 package if
you want to watch them after updating.
