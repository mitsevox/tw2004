# Borrowed C library source

These files come from the Pikmin 2 decompilation project,
https://github.com/projectPiki/pikmin2 (commit `7d48e23c4005ea3ac61cd3db3447ad5c9601157f`), released under CC0-1.0.
Thank you to its contributors.

Only files that compile to code matching Tiger Woods PGA Tour 2004 are kept (`src/MSL_C/scanf.c`).
How they were chosen: `docs/tw2004-notes.md`, "SDK from other decompilations".

Changed for this game (so the unit links into the DOL):
- `src/MSL_C/scanf.c` (Pikmin 2's `src/Dolphin/MSL_C/MSL_Common/scanf.c`): uses this repository's
  MSL headers (`extern/sdk/libc`: `__GetChar`, `__UngetChar`, `__CheckForError`), declares the
  `__strtoul`, `__strtoull`, `__strtold` and `mbtowc` it calls, and drops the unused stubs
  (`__FileRead`, `fscanf`, `vscanf`, `scanf`, `vfscanf`, `isspace_string`). In `%c` with a wide
  argument the pointer steps one byte, as in the original (Pikmin 2 writes it as a cast and an
  increment). Built with GC/2.5 (see configure.py). The code is otherwise unchanged.
