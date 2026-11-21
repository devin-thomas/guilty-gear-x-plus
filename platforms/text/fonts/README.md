# The text layer's fonts

The English replaces pictures of Japanese mincho and Korean gothic
lettering, so each release draws it in a face of the same kind
(`../LANG-FORMAT.md`).

| File | Face | Licence | Source |
| --- | --- | --- | --- |
| `DejaVuSerif.ttf` | DejaVu Serif 2.35 | DejaVu fonts licence (Bitstream Vera terms; `LICENSE-DejaVu.txt`) | matplotlib 3's bundled copy |
| `NotoSans-Regular.ttf` | Noto Sans 2.000, Copyright 2015 Google Inc. | SIL Open Font License 1.1 (`LICENSE-NotoSans-OFL.txt`) | PCSX-Redux's bundled copy |

Both are subset to the characters English needs (Basic Latin, Latin-1, the
General Punctuation dashes, quotes and ellipsis, the euro and trade mark
signs), keeping their kerning:

    pyftsubset <font> --unicodes="U+0020-007E,U+00A0-00FF,U+2010-2027,U+2030-2033,U+2039-203A,U+20AC,U+2122" --layout-features=kern --name-IDs='*' --notdef-outline

The names are unchanged; neither contains the reserved words of its
licence.
