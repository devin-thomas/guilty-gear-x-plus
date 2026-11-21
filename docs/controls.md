# Controls

The game sees two PS2 pads, exactly as a PS2 would deliver them. Keyboards,
gamepads and arcade sticks are mapped onto them, and
every mapping can be changed.

## The game's buttons

The PS2 pad's face buttons are X, Circle, Square and Triangle. Guilty Gear X
Plus attacks, as the game's own KEY CONFIG sets them at first:

| Attack | PS2 button |
| --- | --- |
| P (Punch) | Square |
| K (Kick) | X |
| S (Slash) | Triangle |
| H (Heavy Slash) | Circle |
| D (Dust) | R1 |

Change them in the game's OPTIONS > KEY CONFIG as on a PS2; the port's own
mappings (below) decide which key or pad button is which PS2 button.

## Default mappings

Gamepad buttons are by position: "south" is the bottom face button (X on a
PlayStation pad, A on Xbox, B on Switch), "east" the right
one (Circle, B, A), "west" the left (Square, X, Y), "north" the top
(Triangle, Y, X).

| PS2 | Keyboard (player 1) | Gamepad (either player) | Arcade stick with no gamepad mapping |
| --- | --- | --- | --- |
| directions | arrow keys | d-pad, or the left stick past half way | first hat; axes 0 and 1 |
| X | Z | south | button 1 |
| Circle | X | east | button 2 |
| Square | A | west | button 0 |
| Triangle | S | north | button 3 |
| L1 / R1 | Q / W | left / right shoulder | buttons 4 / 5 |
| L2 / R2 | E / R | left / right trigger | buttons 6 / 7 |
| Start | Return | Start (Menu, +) | button 9 |
| Select | Right Shift | Back (View, Create, Share, −) | button 8 |

## Macros

The game's KEY CONFIG can put two- and three-button presses on a button
(P AND K, S AND HS, P K S, P K S HS), but on a PS2 they take the place of
RESPECT and MEMORY. In the port they are extra bindings, so you can have
all four:

| Macro | Keyboard | Gamepad |
| --- | --- | --- |
| P AND K | D | left stick click |
| S AND HS | F | right stick click |
| P K S | C | touchpad click |
| P K S HS | V | |

A macro presses the buttons the game's KEY CONFIG has for its attacks, on
the same frame, as a player pressing them together.

## Keys for the port

| Key | |
| --- | --- |
| F9, or Select held for a second (any pad) | the port's settings ([Settings](settings.md)) |
| F3 / F4 | remap player 1 / player 2 |
| F5 | swap the two players' controllers |
| F7 / F8 | player 1's / player 2's pad: digital or analog |
| F2 | original text or English ([Settings](settings.md#english-text)) |
| F11, Alt+Enter, or a double-click in the middle of the picture | fullscreen or a window |
| F6 | fit the window, or whole-number scaling |

## X confirms in menus

The Korean and Japanese releases confirm with Circle and go back with X.
The port confirms with **X** and goes back with **Circle**, on the title,
in menus, OPTIONS, pause menus, the gallery, story scenes, results and
name entry, for each player. Nothing changes in a fight: the attack
buttons stay where KEY CONFIG puts them.

A few places keep the game's own meaning:

- KEY CONFIG's button rows take whichever button you press;
- the character select: every face button picks a colour there, and X
  already chooses.

To play with Circle confirming, as on a PS2, change "Confirm in menus with"
to Circle in the settings panel ([Settings](settings.md)), per player.

## Players and controllers

- The first controller connected is player 1's, the next player 2's. F5
  swaps them.
- The keyboard is player 1's, unless you give player 2 keys (F4).
- Two players can share one keyboard: give player 2 its own keys with F4.

## Button prompts

Where the game shows a button (KEY CONFIG, the SOUND TEST, the gallery's
help line), each player sees the button on the controller they used last:
A, B, X, Y on Xbox and Steam pads, the Switch's own letters, the key on a
keyboard. PlayStation pads keep the PS2 symbols.

## Remapping

Press **F3** (player 1) or **F4** (player 2). The window's title asks for
each PS2 button in turn, then each macro, showing what it is now. Press the
key, button or stick direction you want; **Escape** keeps the current one.
A new key replaces only that button's keyboard binding, so remapping a
gamepad leaves the keyboard as it was, and the other way round. The game
pauses while you remap, and the result is saved.

The settings file can bind more than one key or button to a PS2 button, and
pin a controller to a player ([Settings](settings.md#the-settings-file)).

## Arcade sticks and hitboxes

- A stick that presents itself as a gamepad (XInput, PS4 mode) works as one.
- Others are read as plain joysticks, in the button order PS2 adapters
  use. Remap them with F3 or F4.
- Left + right together is neutral and up + down is up, as on a hitbox
  (`socd = hitbox`, the default; `socd = off` passes both through).

## Analog

Each pad starts as a DualShock 2 with its ANALOG light off, as the game
expects. F7 or F8 turns it on: the game then reads directions from the left
stick only. The keyboard and d-pad still steer, by moving the stick.

Rumble follows the game; `rumble = off` in the settings file turns it off.
