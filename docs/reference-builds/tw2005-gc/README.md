# Tiger Woods PGA Tour 2005, GameCube (USA)

First pass, 2026-09-25, on the owner's PC. The disc images stay on the PC; only the derived evidence
below is in the repo (file names, strings with their addresses, function address pairs).

## What the discs have

Two discs, 367 and 378 files, the same main.dol on both (2.18 MiB). `sys/`, EA's `.hog` / `.gcb` /
`.fxg` archives, audio (`AudioStm_GC.sab`), streamed character files (`.sas`), movies (`.ngc`). No
linker map, ELF, debug or symbol file, no dev configs or logs. (`.hog` archives not opened.)

main.dol: entry 0x80003100; text 0x80003100 (0x3A0) and 0x800034A0 (0x1F4140); data 0x801F75E0,
0x801F7660, 0x80214D60, 0x802F1FE0, 0x802F4920; bss 0x8022DD20 (0xC6BF4). A different layout from
TW2004 (dtk's own analysis of it stops on overlapping functions; the header above is read directly).

## Compiler

**A different compiler generation or flag set from TW2004.** No paired-single saves of f14-f31 at all
(TW2004: 1,060), 3.7x as many `stmw` saves (3,540 vs 948). TW2006 has the same fingerprint as TW2005.
So TW2005's copies of TW2004 functions compile differently: function-level matching is weak.

## What is useful

- `strings.tsv`: **EA's source paths** (`__FILE__`), e.g.
  `c:/dev/Golf_2005/Tigercode/Code/Golf/Cameras/GoGolfCam.c`: 437 path strings, 148 distinct files,
  in EA's folder tree (Golf/Animation, Golf/UI Core, Golf/Lo-Rendering, Golf/Hi-Rendering,
  Legacy/Ll, Golf/Sfx, Legacy/Lib, Golf/TextureFX, Golf/GameMode, Golf/Cameras, ...). This is the
  file-to-folder layout of the engine TW2004 shares: which files sat together, for splitting and
  naming units. Also 38 messages not in TW2004 (camera, terrain, animation, UI warnings naming
  functions such as Ter_ComputeLODBinCounts and XDivX_OpenFile) and 127 identifier-like names
  (course enums, particle-system field names). Console libraries and the C runtime are left out.
- `functions.tsv`: 376 TW2004 functions found exactly once with the same opcodes and registers
  (masked immediates). All are already-named functions (mostly runtime/library code): no new names
  from here. Matching with registers ignored, or on bodies without prologue/epilogue, finds no more
  (375 and 273), so the game code itself changed or reschedules.
