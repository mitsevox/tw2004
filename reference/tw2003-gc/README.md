# Tiger Woods PGA Tour 2003, GameCube (USA)

First pass, 2026-09-25, on the owner's PC. The disc image stays on the PC; only the derived evidence
below is in the repo (file names, strings with their addresses, function address pairs).

## What the disc has

425 files: `sys/` (main.dol 1.40 MiB, apploader, fst, bi2), EA's `.hog` / `.gcb` / `.fxg` data
archives, audio (`AudioStm_GC.sab`), movies (`.ngc`). No linker map, ELF, debug or symbol file, no
dev configs or logs. (The `.hog` archives are EA's format and were not opened.)

main.dol: entry 0x80003140; .init 0x80003100 (0x24F0), .text 0x80005600 (0x145300), .rodata
0x8014A940, .data 0x80152FA0, .sdata 0x8022F660, .sbss 0x80230440; no .sdata2 section.

## Compiler

Same generation and flags as TW2004: the same prologue style and paired-single saves of f14-f31
(`psq_st`: 1,012 here, 1,060 in TW2004; none in TW2005).

## What is useful

- `functions.tsv`: **1,083 TW2004 functions** (12+ instructions) found exactly once in TW2003 with
  the same opcodes and registers (branch targets and address/offset immediates masked). 498 of them
  are still `fn_` names in TW2004, so any name found for one side names both. 21 of them are functions
  that are not exact in TW2004 yet (fn_8000C278, fn_8000E884, the UISApi/UIStudio fn_8016xxxx group,
  ...): the same C compiled to the same code a year earlier, so EA's source form did not change there.
- `strings.tsv`: source-file names and messages from main.dol's data (console libraries and the C
  runtime left out). Names TW2004's strings lack: CareerMode.c, CourseInfo.c, PlayNowMode.c,
  tTournamentMode.c, FETextureMgr.c, pLLTexGrp.c, char_skin.c, shadow_gc.c, uiEATrax.c (in_tw04 = no).
  Game modes and front-end code TW2004 very likely still has, so candidates for unnamed units.
