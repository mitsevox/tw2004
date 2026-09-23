# Borrowed C library source

These files come from the Sonic Heroes decompilation project,
https://github.com/Jovinull/sonicheroes (commit `4243c6c679fbd24fd1b24bcaacbda7c7f9d3d6b9`), released under the MIT license
(see `LICENSE` in this folder). Thank you to its contributors.

Only files that compile to code matching Tiger Woods PGA Tour 2004 are kept (`strtold.c`,
`strtoul.c`), with the headers they need.
How they were chosen: `docs/tw2004-notes.md`, "SDK from other decompilations".

Changed for this game (so the unit links into the DOL):
- `src/MSL_C/strtoul.c`: calls `__StringRead` and reads `__ctype_map` / `__upper_map` by name instead
  of by Sonic Heroes addresses, and defines its functions in the order `__strtoul`, `__strtoull`,
  `strtoul`, `atoi` (the compiler emits them in reverse, which is this game's order). The code is unchanged.
