# Borrowed SDK / C library source

These files come from the The Legend of Zelda: The Wind Waker decompilation project,
https://github.com/zeldaret/tww (commit `ae3b3b0deb6677c8d0c4c36dde26b100968ec13c`), released under CC0-1.0.
Thank you to its contributors.

Only the Dolphin SDK, MSL C library and runtime files that compile to code matching
Tiger Woods PGA Tour 2004 are kept here, with the headers they need.
How they were chosen: `docs/tw2004-notes.md`, "SDK from other decompilations".

Changed for this game (so the units link into the DOL):
- `src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Src/mbstring.c`: `utf8_to_unicode` is written
  (TWW leaves it empty) and it and `unicode_to_UTF8` are declared `inline`: this game's copy is
  built with GC/1.3 (see configure.py), which inlines only those two, into `mbtowc` and `wcstombs`.
  This code is ours, not TWW's.
