# Tiger Woods PGA Tour 07 PS3 DWARF inventory

`Dev_Golf_Release_PS3.self` is an unencrypted debug fSELF (`key_revision 0x8000`). Reconstructing
its PowerPC64 ELF recovers 47 sections, including `.symtab`, `.strtab`, and full DWARF.

The inventory contains:

- `elf-sections.txt`: section names, sizes, and VMAs.
- `dwarf-statistics.json`: LLVM's DWARF quality/count statistics.
- `source-files.txt`: source files and headers across the compilation units.
- `function-symbols.txt`: 35,071 demangled `.text` symbol-table entries with address and size.
- `key-types-and-symbols.txt`: focused DWARF queries for types already useful to TW2004.

Headline DWARF counts:

- 429 compilation units.
- 33,620 functions; 33,472 have locations.
- 116,495 unique source variables.
- 132,668 parameters and 78,126 local variables.
- 623,860 source-line entries.
- 18,221,535 bytes of `.debug_info` plus line, location, range, frame, macro, and string tables.

This is game code as well as middleware. Paths include `Physics.c`, `Wind.c`, `Swing.c`,
`stateFunc.c`, `Replay.c`, `char.c`, `Skin.c`, `CharSliders.c`, `AI_Brain.cpp`,
`StreamManagerHole.cpp`, frontend/Create-a-Player files, cameras, game modes, terrain, and spectator
systems.

The SELF was reconstructed with
[`ps3recomp/tools/unfself.py`](https://github.com/sp00nznet/ps3recomp/blob/master/tools/unfself.py):

```sh
python unfself.py Dev_Golf_Release_PS3.self --output Dev_Golf_Release_PS3.elf
```

LLVM accepts the result as an unstripped ELF with debug information. `llvm-dwarfdump --verify`
reports overlapping or out-of-parent address ranges for some optimized inline DIEs. The compiler's
line/type/name data remains parseable, but an inline range should be checked before using it as a
hard function boundary.

PS3 is 64-bit and TW07 is substantially newer. Names, enums, signatures, field purposes, source
organization, and algorithms are useful evidence; raw byte offsets and class sizes are not directly
portable to the 32-bit GameCube build.

## Per-source-file lists and the pairing with TW2004

- `cu/<SourceFile>.txt`: every function the DWARF places in that source file, in source order (TW07
  was built as unity files; functions are regrouped by their declaring file). One function per
  block: `line  PS3-address  size  [static ]return Name(parameters)`, then its `local` variables
  in declaration order and the functions it `inlines`. Inline-only functions (no address) are
  included. `cu/index.tsv` lists the files, their function counts and EA's source paths. Made by
  `python tools/match/tw07dwarf.py <elf> <out dir>`.
- `pairs.tsv`: a machine pairing of TW2004 functions (GameCube address, our unit and current name)
  with TW07 functions (name, file, line), scored on shared strings, float constants, paired callees
  and callers, size ratio (PS3 code is about 1.8x GameCube) and source-order alignment within a
  unit. `conf` high/med/low; measured precision against names already proven: high 26/26, med 66/68,
  low about 85%. A candidate, never a fact. `pairs_blind.tsv` is the same without our names, for
  blind audit lanes. Snapshot of 2026-09-24; our names change as the audit renames functions.
