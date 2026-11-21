# The text layer's translation files (`.lang`, PORT-014)

Guilty Gear X Plus draws its Japanese and Korean text as pictures: every
story line, win quote, gallery caption and move list is a texture on the
disc, white letters on a transparent background. The game's two string
fonts (`FontDraw`, `FontDrawDC`) draw Latin letters only, and every menu
is already English on both discs. So the English layer replaces those
textures as the renderer decodes them. A `.lang` file says, for each
texture, what English to draw in its place.

A file holds English and ids only, never the original Japanese or Korean.

## Syntax

UTF-8 text, one statement per line. Blank lines are ignored, and so is a
line whose first non-blank character is `#`. The first statement is the
version:

    ggx-lang 1

Then, in any order:

    lang en                   the language (once per set of files)
    include story-sol.lang    read another file (path relative to this one)
    [story-sol]               start a group; the settings below apply to it

Group settings (each one lasts until the next group):

    releases KR JP            the releases whose disc the ids below are for
    file 37                   the disc file (CDDATA.BIN's file id, seekpos)
    font serif                serif | sans | serif-bold | sans-bold (default: the release's)
    layout block              block | rows <pitch>
    align auto                auto | left | center
    size 16                   the font's size in pixels to the em (default: 16 for
                              block, the original's letter height for rows)
    area x0 y0 x1 y1          replace only this part of the picture (inclusive pixels)

Entries:

    <path> <English>
    ?<path> <English>         the same, marked for review
    <fid>/<path> <English>    a texture in another file than the group's

`<path>` is the texture's place in the file: the index in the file's
offset table, then in each nested table, three digits each, joined by `-`
(`000`, `019-003`).

The English is free text. `|` forces a line break; otherwise the line is
wrapped to fit the original's width. A comment line just above an entry is
the place for a note to the reviewer.

A release's default font follows its own lettering: Japanese uses a mincho,
so JP draws English in DejaVu Serif; the Korean disc uses a heavy gothic, so
KR draws Noto Sans, emboldened by a pixel (`sans-bold`). Both are drawn in
one bit per pixel, as the originals are.

## Layout

The renderer reads each original texture before replacing it: which index
is the letters' (the most common index after the background's), and where
the original's lines are (rows with letters), their left edges and centres.

- **block** (story lines, win quotes, captions): the English is set in one
  block centred on the original's block, at the group's size with lines
  one and a half times that apart, aligned as the original is (`auto`: left
  if every original line starts at the same x, else centred). If it doesn't
  fit the texture, the font shrinks until it does.
- **rows `<pitch>`** (move lists): the game draws each line of the texture
  on its own, as a band `<pitch>` pixels tall, so English line *n* goes in
  band *n*, at the original lines' left edge, as tall as the original
  letters; a line too long for its band is drawn smaller.

With `area`, only that rectangle is read and redrawn, and the rest of the
picture is kept: for text set into artwork, such as the Japanese disc's
"coming soon" card, whose logo stays.

The new texture has the original's format, size and palette, so the game's
own drawing (position, scale, fades, palette) applies to it unchanged.
