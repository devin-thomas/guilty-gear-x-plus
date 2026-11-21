# Settings

## The settings panel

Press **F9**, or hold **Select** for a second on any pad, to open the
port's settings over the game. The game pauses while it is open.

- **Up / Down** pick a row; **Left / Right** (or X / Circle) change it.
- **Start**, F9 or Escape closes it and saves.

| Row | |
| --- | --- |
| Confirm in menus with: 1P, 2P | X (the default) or Circle, per player ([Controls](controls.md#x-confirms-in-menus)) |
| Unlocks | below |

On the first launch a line over the title says that X confirms and how to
open the panel.

## Unlocks

Everything the game unlocks by playing (the hidden characters, GG and EX
characters, every colour, the gallery's pictures and its challenges) can be
unlocked at once. There are three ways:

- **The settings panel's Unlocks:** everything, or the characters, colours,
  gallery or challenges. Press X on a row; it says what it gives.
- **The password GOHAN:** on OPTIONS > PASSWORD, enter `GOHANAAA` (the
  screen wants eight letters). It unlocks everything and says "EX PASSWORD 2
  INPUT COMPLETED".
- **`unlock = all`** in the settings file, applied each time the title
  screen comes up.

An unlock only ever adds: nothing you have unlocked is taken away, and the
game saves it to your memory card as its own progress. It happens at the
title screen or in OPTIONS, where the game reads it; asked for elsewhere, it
waits for the title. Off by default.

## English text

Guilty Gear X Plus never had an English release. Its menus, mode names and
pause screens are in English on both discs already; the story's dialogue
and narration, the win quotes, the gallery's captions and the move lists are
Korean or Japanese.

**F2** switches between the original text and an English translation of all
of those, drawn in place of the original. It changes only what you see: the
game runs the same either way. The original text is the default; start with
`--text en` to begin in English.

## The display

| | |
| --- | --- |
| Fullscreen or a window | F11, Alt+Enter, or a double-click in the middle of the picture (`window =`) |
| Scaling | F6: **fit** fills the window at 4:3; **integer** uses whole multiples of the PS2's 448 lines, for even pixels (`scaling =`) |
| Which display | `display = 2`, or a part of its name; `--display list` lists them |
| Frame timing | `present = vsync` (the default) waits for the display; `mailbox` and `immediate` don't, which on a variable refresh rate display lets the game set the pace |
| Internal resolution | `--scale N` draws at N times 640x448 (1 to 8) |

The picture is always 4:3, the game's shape; widescreen isn't supported.
The game runs at the PS2's 59.94 frames a second whatever the display's
rate, and each frame is shown as it was drawn, with nothing blended in.

## Sound

`audio = off` in the settings file turns the sound off; `--audio off` for
one run.

## The settings file

`ggx.cfg`, in the port's data folder
([Installing](install.md#where-the-port-keeps-your-data)). The port writes
it when you change something in the game window or the panel; you can also
edit it while the port isn't running. A command-line option overrides it
for that run.

    window = windowed      # or fullscreen
    scaling = fit          # or integer
    present = vsync        # or mailbox, immediate
    display = 2            # the display's number, or part of its name; unset, the system chooses
    confirm = x            # or circle; p1.confirm and p2.confirm per player
    unlock = none          # or all, or characters,colours,gallery,challenges
    replays = on           # or off: save each fight as a replay
    audio = on             # or off
    rumble = on            # or off
    socd = hitbox          # or off: left+right and up+down as they are
    prompts = auto         # or playstation, xbox, steam, nintendo, keyboard, other
    p1.analog = off        # or on (F7, F8)
    p1.x = key:Z, pad:south
    p2.device = 0300...    # pin a controller to player 2 (its SDL GUID)

A binding is a list: `key:<key name>`, `pad:<button>` (south, east, west,
north, dpup, dpdown, dpleft, dpright, leftshoulder, rightshoulder, start,
back, l4, r4, l5, r5), `pad:<axis>+` or `-` (leftx, lefty, rightx, righty,
lefttrigger, righttrigger), `joy:b<n>`, `joy:h<n>.up` (down, left, right),
`joy:a<n>+` or `-`, or `none`. The buttons are `up`, `down`, `left`,
`right`, `x`, `circle`, `square`, `triangle`, `l1`, `r1`, `l2`, `r2`,
`l3`, `r3`, `start`, `select`, and the macros `p_and_k`, `s_and_hs`,
`p_k_s`, `p_k_s_hs`.
F3 and F4 write these for you ([Controls](controls.md#remapping)).

Deleting `ggx.cfg` returns every setting to its default.
