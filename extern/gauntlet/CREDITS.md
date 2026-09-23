# Borrowed SDK / C library source

These files come from the Gauntlet: Dark Legacy decompilation project,
https://github.com/sabishii-bit/Gauntlet-Dark-Legacy-Decompilation (commit `5d5e5bc572c764821806e238a2f6f95f62d05fee`).
The repository does not state a license. Thank you to its contributors.

Only files that compile to code matching Tiger Woods PGA Tour 2004 are kept (`db/odenotstub.c`),
with the headers they need. They are unchanged. (`MSL/atanf.c` was removed: the
one function of it that was used, the 8-byte fabs at 0x8000AE94, is EA's own, in UMemPool.c.
`ax/AXVPB.c` was replaced by FFCC's, whose parameter block has this game's size.)
How they were chosen: `docs/tw2004-notes.md`, "SDK from other decompilations".
