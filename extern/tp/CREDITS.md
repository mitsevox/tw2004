# Borrowed SDK / C library source

These files come from the The Legend of Zelda: Twilight Princess decompilation project,
https://github.com/zeldaret/tp (commit `c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0`), released under CC0-1.0.
Thank you to its contributors.

Only the Dolphin SDK, MSL C library and runtime files that compile to code matching
Tiger Woods PGA Tour 2004 are kept here, with the headers they need.
How they were chosen: `docs/tw2004-notes.md`, "SDK from other decompilations".

Changed for this game (so the units link into the DOL):
- `libs/dolphin/src/ar/arq.c`: the release build string is this game's ARQ build
  (Sep  5 2002 05:34:29). The Metroid Prime copy in `extern/sdk` has the right string but lacks
  `ARQRemoveRequest`, which this game has.
- `libs/dolphin/src/card/CARDStatEx.c`: `CARDGetAttributes`, `CARDSetAttributesAsync` and
  `CARDSetAttributes` (ours), which TP's copy lacks; this game's build calls `__CARDGetStatusEx`
  in them instead of inlining it. The file also holds `__CARDVendorID`, `__CARDPermMask` and
  `__CARDEnableGlobal` (ours), as this game's copy of the SDK's CARDNet.c does.
