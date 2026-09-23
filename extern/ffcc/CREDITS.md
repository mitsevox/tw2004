# Borrowed SDK / C library source

These files come from the Final Fantasy Crystal Chronicles decompilation project,
https://github.com/zcanann/FFCC-Decomp (commit `99164f08e275550d532c75199b80657e04168faf`), released under CC0-1.0.
Thank you to its contributors.

Only the Dolphin SDK, MSL C library and runtime files that compile to code matching
Tiger Woods PGA Tour 2004 are kept here, with the headers they need.
How they were chosen: `docs/tw2004-notes.md`, "SDK from other decompilations".

Changed for this game (so the units link into the DOL):
- `include/dolphin/gx/__gx.h`: the GX state pointer is `__GXData` (FFCC calls it `gx`), the name
  the rest of this game's GX library uses; `gx` is kept as a macro for it.
- `src/ax/AX.c`: the release build string is this game's AX build (Sep  5 2002 05:34:31).
- `src/axfx/reverb_hi.c`: this game's `DoCrossTalk` scales by 0.6 (`value0_6`), not by a 1.0
  constant; `value1_0` is gone and `value0_6` comes before `value0_3`, the game's constant order.
