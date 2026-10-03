# EA Tiburon builds with debug info (for the UIS / IStudio library), 2026-09-26

Why: TW2005 GC's source paths put UISEvent.c and UIStudio.c in `Code/Tiburon/IStudio/` (and
TibExt.c in `Code/Tiburon/Eassdk/`): the UI script system is EA Tiburon's library, not the Tiger
team's. TW06/TW07 replaced it with Apt, so our reference builds have none of it. A research agent
(web only, nothing downloaded) looked for Tiburon builds with symbols.

## Verified (listings read)

- **debugging.games GameCube**: `[GC] NASCAR 2005 - Chase for the Cup (USA) [GN4E69] (DWARF).7z`,
  2 MB (added 2023-11-17). EA Tiburon, released 2004-08-31, CodeWarrior. No Madden, NCAA, NASCAR
  Thunder or Tiger Woods GC entries. Other EA GC entries (MOH Frontline, European Assault: DWARF+
  SYMTAB; Harry Potter GoF: DWARF) are other studios.
- **debugging.games PS2** (no debug-type labels): Madden NFL 2003 [SLUS-20529] 2002-06-06 (13 MB;
  Hidden Palace's page for the same date says "full debug symbols"), NCAA Football 2003 [SLUS-20530]
  2002-06-05 (2 MB), NASCAR Thunder demo [SLUS-20535] (1 MB). TW06 PS2 (already used).
- RetroReversing's GC list: no EA/Tiburon entries. GitHub: no UIStudio/UISScreen/UISEvent/TibExt
  outside this repo. Hidden Palace (503) and TCRF (403) could not be read.

## Ranking

1. **NASCAR 2005 GC (DWARF)**: same compiler family, Tiburon, 2004 (one year after TW2004).
   Likely contains IStudio (inference: same studio, year, platform). DWARF gives function names,
   types, locals and per-file line info: EA's names and structures for UIS, and maybe its locals'
   declaration order.
2. Madden NFL 2003 PS2 prototype (2002, "full debug symbols"): may predate IStudio; ee-gcc, so
   names and structures only.
3. NCAA Football 2003 PS2 / NASCAR Thunder demo.

Next: the owner decides on fetching GN4E69 (2 MB; game data, so it stays off git: the PC, like the
other reference builds, with only derived evidence committed). Then grep its DWARF for IStudio,
UIS*, TibExt and pair its UIS functions with ours.

Sources: https://debugging.games/GameCube/, https://debugging.games/PlayStation%202/,
https://hiddenpalace.org/Madden_NFL_2003_(Jun_6,_2002_prototype)
