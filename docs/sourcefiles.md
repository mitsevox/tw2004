Source file map
===============

The original source files of the game in link order, with the evidence for each name and boundary (2026-09-23). Produced by a one-off analysis of the DOL (duplicate compiler constants, EA's assert file names, 8-byte data alignment) and the TW06 references ([`tw06-names.md`](tw06-names.md)). The sweep files in `src/unsorted/<File>/` are sorted by this map; see "Using this for splits.txt" below for which parts are safe to act on. The machine-readable copy is `config/GW4E69/filemap.json` (used by `tools/match/mapunits.py`).

**Read this first.** The map separates three questions for every file: *is there a file boundary here at all* (column **Ext**), *exactly where is it* (column **Left edge**: the window it can lie in; `.text` shows the best guess), and *what was the file called* (column **Name conf**). Strong = backed by EA's own strings or by duplicate compiler constants; medium = TW06 (2005) names or a single binary witness; guess = inferred. Anything marked guess should be checked before it is used for splits.

## Summary

- **378 files** in link order: 197 EA (CodeWarrior game code 0x80005520-0x8012E950, EA UI Studio 0x801654D0-0x8016C718, EA GCC file library 0x8016C718-0x80175F54) and 181 Dolphin SDK / MSL / MetroTRK / MusyX files (taken from the existing source units).
- EA files by name confidence: strong 89, medium 32, guess 34, unnamed 42.
- **The EA file count is a lower bound.** Boundaries are found where the binary proves one (a constant or string the compiler would have pooled appears twice, EA's assert file names change, a data section is padded to 8 bytes). Code that uses no floats or strings gives no such evidence, so long unnamed rows (e.g. `8007CACC` 37 KB, `80127D84` 27 KB, `800A3E3C` 15 KB) may hold several files. 124 separate copies of `0.0f` in `.sdata2` alone mean at least 124 EA files use floats.

### Coverage of EA `.text` (1285812 bytes)

| Name confidence | certain core, boundaries strong | certain core, a boundary medium/guess | boundary margin (could belong to the neighbour) | total |
|---|--:|--:|--:|--:|
| strong | 402236 (31.3%) | 96284 (7.5%) | 116312 (9.0%) | 614832 (47.8%) |
| medium | 129272 (10.1%) | 67264 (5.2%) | 52648 (4.1%) | 249184 (19.4%) |
| guess | 48292 (3.8%) | 84076 (6.5%) | 53760 (4.2%) | 186128 (14.5%) |
| unnamed | 89204 (6.9%) | 70832 (5.5%) | 75632 (5.9%) | 235668 (18.3%) |

SDK `.text` (255812 bytes) comes from the existing source units and is not re-derived here.

### Coverage of EA data

Data ranges are derived from the text map: every data object is given to the file whose functions reference it, keeping link order, with file starts on 8-byte boundaries (every CodeWarrior object's data sections are 8-aligned; checked by compiling a test file). "Anchored" = between the first and last object of that file that its code references; "slack" = unreferenced padding/objects at the edges that could belong to the neighbour. Checked against the 27 linked (Matching) EA units that carry data ranges: with those ranges hidden, the method put 25 exactly on the known range.

| Section | bytes | strong-name file (anchored / slack) | medium | guess | unnamed |
|---|--:|--:|--:|--:|--:|
| `.rodata` | 31324 | 9264 / 4 | 20376 / 0 | 728 / 0 | 952 / 0 |
| `.data` | 62632 | 27130 / 4946 | 5460 / 764 | 4918 / 474 | 18808 / 132 |
| `.bss` | 801696 | 111664 / 118344 | 109688 / 5992 | 133952 / 664 | 320160 / 1232 |
| `.sdata` | 3120 | 1098 / 686 | 569 / 15 | 199 / 17 | 500 / 36 |
| `.sbss` | 2640 | 1250 / 34 | 336 / 16 | 589 / 11 | 388 / 16 |
| `.sdata2` | 8528 | 3888 / 0 | 2432 / 0 | 852 / 4 | 1352 / 0 |
| `.sbss2` | 4 | 4 / 0 | 0 / 0 | 0 / 0 | 0 / 0 |

`.ctors`: none in EA code (C only). `.dtors`: its one entry belongs to the runtime (`global_destructor_chain.c`). `.sbss2`: its only object (0x80285200, 4 bytes) is referenced from 0x80006B4C and belongs to `LLDisp_Gc.c`.

### Sweep files (1,468 `sweep_*.c` + 3 `code_*.c` = 1,471 in `src/unsorted`)

- 1467 sweep files lie wholly inside one file of the map (1 straddles a boundary, see below).
- **310 land in strong-name files inside the certain core with strong boundaries** (safe to move once the file exists); 757 are in strong-name files overall (418 in the core, the rest in a boundary margin).
- Medium-name files: 191 (core 84). Guess-name files: 31. Unnamed files: 488 (their extent is known, their name is not).
- `sweep_8009B314.c` holds two functions that the evidence puts in different files (a duplicate constant forces a boundary between 0x8009B0D0 and 0x8009B340, and medium glue ties 0x8009B320 to the next function).
- The three `code_*.c` units (0x80007BC4-0x800083A4, the camera/visibility block) have no evidence separating them from `LLObj_Gc.c`; the boundary to `GoEntry.c` lies after 0x80007D74 (so the frustum-cull function is not in `GoEntry.c`). Unsure whether the block is the tail of `LLObj_Gc.c` or a file of its own.

### Using this for splits.txt

Only the 5 GCC-library files have a strong name *and* exact edges on both sides . In the CodeWarrior code, edges are exact in only 7 places; elsewhere the evidence gives a window. The safe move is to split each strong-name file to its `text.core` range (always inside the real file, so at worst a slice, which splits.txt allows) and widen it as neighbours are matched. Functions in a margin (between `left.earliest` and `left.latest`) could belong to either neighbour.

## Existing units that disagree with the evidence

| Unit | Range | Finding |
|---|---|---|
| `CharClip.c` | 800176E8-800177A0 | edge 0x800176E8 cuts through one file (shared constants/strings across it): the unit is a slice<br>edge 0x800177A0 cuts through one file (shared constants/strings across it): the unit is a slice |
| `Golfer.c` | 8002A630-8002F1D4 | certain file boundary inside, between 0x8002BBA4 and 0x8002BDEC (dupconst)<br>certain file boundary inside, between 0x8002C8B4 and 0x8002CD20 (dupconst)<br>certain file boundary inside, between 0x8002D6FC and 0x8002DC50 (dupconst+padstrong)<br>certain file boundary inside, between 0x8002E650 and 0x8002EE1C (dupconst+padstrong) |
| `Ball.c` | 80050C2C-8005620C | certain file boundary inside, between 0x80055C1C and 0x80055F88 (dupconst+padstrong) |
| `Swing.c` | 800589F8-80062E40 | certain file boundary inside, between 0x8005CBB0 and 0x8005D44C (dupconst+padstrong)<br>possible file boundary inside near 0x8005D44C (tw06) |
| `CharAnim.c` | 800965DC-80096690 | edge 0x800965DC cuts through one file (shared constants/strings across it): the unit is a slice<br>edge 0x80096690 cuts through one file (shared constants/strings across it): the unit is a slice |
| `HoleScore.c` | 800D0AA0-800D0AF4 | edge 0x800D0AA0 cuts through one file (shared constants/strings across it): the unit is a slice<br>edge 0x800D0AF4 cuts through one file (shared constants/strings across it): the unit is a slice |

What they mean:

- **`Golfer.c` is at least five original files.** Four boundaries are proven by constants the file would have pooled (0.0, 1.0, 2.0, pi, 2pi, 10, 100, the int-to-float double each appear again; 9, 9, 6 and 2 witnesses). TW06 names the first part `golf/ai/ai_brain.cpp` (our `AI_RehearseShot` = TW06 `AIBrain_Think`, strong pair) and the fourth `golf/ai/ai_core.cpp` (two medium pairs, guess); the middle two have no name.
- **`Swing.c` is three files:** `Swing.c` (assert, 0x800589F8-~0x8005CCAC), then TW06's `golf/state engine/stategolfer.c` (`GOLFERSTATE_*`, 11 witnesses) and `statefunc.c` (`STATEFUNC_*`; that last split is TW06-only, guess).
- **`Ball.c` ends too late:** its last functions (from somewhere in 0x80055C1C-0x80055F88 up to 0x8005620C) use their own copies of 0.0, 0.1, 2.0, 5.0 and the int-to-float double, so they belong to the next file, which the map calls `LLPictInt.c` (its assert and TW06 pair are at 0x8005620C). The rest of `Ball.c` is TW06's `golf/physics/physics.c` (22 paired functions).
- **`CharClip.c`, `CharAnim.c`, `HoleScore.c` are slices** of bigger files: shared constants cross both of their edges. They sit inside `char.c` (assert), TW06's `char_state.c`, and TW06's `golf/gamemode/analysisutilities.c` respectively.
- All other hand-made units (GameMode*, GameUI, GameManager, Earnings, GoGolfCam, UStream, skalib, the GCC units, ...) are consistent with the evidence. That is weaker than "confirmed": for float-free code (most GameMode units) the binary says nothing either way, and their edges were used as the preferred boundary positions.

## Other findings

- **LLFileIO_Gc.c vs LLMem.** The only `LLFileIO_Gc.c` asserts are in 0x800065C8; TW06 pairs `Mem_cpy` (0x80005628) with `legacy/ll/xbox/llmem_xbox.c`. So 0x80005628-~0x800065C8 is probably a separate `LLMem_Gc.c` (guess; no constant evidence either way).
- **TW06 names are 2005 names.** Where no assert exists, names come from TW06 functions paired to ours ([`tw06-names.md`](tw06-names.md)) mapped to files with the Xbox PDB line tables (5 unity modules, 13,307 functions -> 575 source files). Several are C++ in 2005 (`ai_brain.cpp`, `streammanagerhole.cpp`, `gamemanager.cpp`); the 2003 file was C, same stem likely. Xbox platform files (`*_xbox.c`) are `*_Gc.c` here. Single TW06 pairs inside a proven file are ignored (`vec4flt_CrossProduct` inside `char.c`, a `char.c` pair inside `statefunc.c`).
- **Header asserts:** `GoShaderObjectCommon_DynamicRenderingBuffer_Gc.h` is asserted from 18 places in one file (0x8006FF2C-0x80070EC4): inline functions from that header; the .c name is not known.
- **A global const pair at 0x80283300** (0.84, 0.7056) is the first `.sdata2` of `physics.c` but is used from three other files (0x8004731C, 0x80061B44, 0x800B1B18); constants are not always file-private.
- **No function padding in `.text`.** CodeWarrior packs every file's `.text` at 4-byte alignment, so text alignment gives no boundary evidence (checked: zero gaps between functions). All data sections are 8-aligned per file, which is what the data-side evidence uses.
- `build/GW4E69/asm` is older than `main` (it still has `GameMode10.s`); only addresses and instructions were taken from it, names from `symbols.txt`.

## Method

Evidence, strongest first:

1. **EA assert strings** (`__FILE__` of 94 files, about 85 of them EA). A string literal is private to its file, so every function referencing it is in that file; different names = different files.
2. **Duplicate constants and strings.** CodeWarrior keeps one copy of each float/double literal and each string per file (checked by compiling a test file with GC/2.5: 1.0f, pi, 0.5f and the int-to-float double used by six functions are emitted once). Two referenced copies of the same value = two files; the boundary lies between the last user of the first copy and the first user of the second. 1,360 such constraints, plus 56 from strings.
3. **Data padding.** Every object's data sections are 8-aligned. Padding that skips past a 4-byte boundary to reach 8 (e.g. a 9-byte string followed by 7 zero bytes) marks a file start in `.data`/`.rodata`/`.sdata`; a `float x, 0` pair in `.sdata2` marks one too. 147 strong + 43 weak markers.
4. **Same-file glue** (a boundary cannot be here): a file-private constant/string used by two functions within 32 KB; two functions whose private data is in reverse order (data follows link order across files, so an inversion must be inside one file); medium glue from static-looking data used only within 2 KB and helper functions called only from within 1 KB. Calibrated on the constraints above: the chosen windows produced 0 and 1 contradictions.
5. **TW06 and the 2002 tree** for names and folders; TW06 file changes between paired functions add weak (guess) boundaries.
6. **Existing hand-made units**: their edges are preferred when the evidence allows a range, and add weak boundaries otherwise, except for the units above that are proven wrong.

Solving: the proven "a boundary lies in this window" constraints are covered with the fewest boundaries (greedy, never on strong glue); each boundary's window is what the evidence allows; its position inside the window prefers existing unit edges, then no medium glue, then TW06 changes, then not splitting a sweep unit, then fewest calls crossing it. Rows with the same name on both sides of weak boundaries are merged (a file is contiguous). Data: dynamic programming per section over all objects, monotone in link order, file starts 8-aligned; linked (Matching) units' data ranges are fixed.

Confidence columns: **Name conf** S = assert (or verified unit), M = TW06 strong pair / 2002 tree, G = TW06 medium pair or hand-made unit name. **Ext** (do the edges exist) S = assert/SDK/verified edge or 2+ independent witnesses, M = one witness, G = TW06/weak padding/unit edge only. **Left edge** gives the window for the start; `=` means exact. 

## Table

Columns: `.text` best guess; **Left edge** window for the start; **Fns** functions; data columns best-guess range per section (blank = nothing there); **Sweep** sweep files in the certain core / in the file; **Unit** existing units overlapping (`*` = unit sticks out of this file). Addresses without `0x`.

| # | File | Evidence | Name conf | Ext | .text | Left edge | Fns | .rodata | .data | .bss | .sdata | .sbss | .sdata2 | Sweep | Unit |
|---|------|----------|:-:|:-:|-------|-----------|--:|---------|-------|------|--------|-------|---------|------:|------|
| 0 | `main.c`<br>legacy/main/main.c | TW06; fn names: main | M | G | 80005520-80005628 | = | 5 |  |  |  |  |  |  | 5/5 |  |
| 1 | `llmem_xbox.c`<br>legacy/ll/xbox/llmem_xbox.c | TW06, TW06 name is the Xbox file; GC file probably llmem_Gc.c; fn names: Mem | M | G | 80005628-800065C8 | = | 15 |  | 801866E0-80186720 | 8019D540-801A2350 |  | 80281B80-80281B88 |  | 0/2 |  |
| 2 | `LLFileIO_Gc.c` | assert | S | G | 800065C8-800067E4 | 80005884..800065C8 | 2 |  | 80186720-80186730 |  |  |  |  |  |  |
| 3 | `LLDisp_Gc.c` | assert | S | S | 800067E4-800073B4 | 800066E4..800067E4 | 24 | 8017DB60-8017DB70 | 80186730-80186768 | 801A2350-801A24A0 |  | 80281B88-80281BB0 | 80282A60-80282A80 | 3/9 |  |
| 4 | `LLObj_Gc.c`<br>Legacy/LL/Gc/LLObj_Gc.c | assert, 2002 tree; fn names: Vec3 | S | S | 800073B4-800083A4 | 800072E0..800073B4 | 36 |  | 80186768-80186778 |  | 80280DA0-80280DA8 |  | 80282A80-80282A88 |  | code_80007BC4.c, code_800080D0.c, code_800082F8.c |
| 5 | `GoEntry.c`<br>Golf/Entry/GoEntry.c | assert, 2002 tree; fn names: Game | S | G | 800083A4-8000977C | 800080D0..800083A4 | 28 | 8017DB70-8017DB80 | 80186778-80186810 |  | 80280DA8-80280DB0 |  | 80282A88-80282AC0 | 0/7 |  |
| 6 | `(unnamed)` |  | - | G | 8000977C-8000A0E8 | 80008820..800097CC | 18 |  | 80186810-80186828 |  |  | 80281BB0-80281BD8 |  | 0/5 |  |
| 7 | `UMemPool.c`<br>Legacy/Lib/UMemPool.c | assert, 2002 tree; fn names: Vec, fabsf | S | S | 8000A0E8-8000B130 | 8000989C..8000A194 | 31 |  | 80186828-80186838 |  |  | 80281BD8-80281BE0 | 80282AC0-80282AE0 | 6/9 | atanf.c |
| 8 | `urandom.c`<br>legacy/lib/urandom.c | TW06; fn names: Rand | M | S | 8000B130-8000B46C | 8000B058..8000B318 | 11 |  |  | 801A24A0-801A25B8 |  | 80281BE0-80281BF0 | 80282AE0-80282AF0 | 1/4 |  |
| 9 | `unetwork.c`<br>legacy/specif/unetwork.c | TW06; fn names: Game, Course | G | G | 8000B46C-8000C624 | 8000B46C..8000BF20 | 43 |  | 80186838-801868A0 | 801A25B8-801A2AC0 | 80280DB0-80280DB8 | 80281BF0-80281C18 | 80282AF0-80282B08 | 0/12 |  |
| 10 | `UStream.c`<br>Legacy/SPECIF/UStream.c | assert, TW06, 2002 tree; fn names: U | S | G | 8000C624-8000E708 | 8000BCA0..8000C968 | 31 |  | 801868A0-801868B0 | 801A2AC0-801A3438 | 80280DB8-80280DC8 | 80281C18-80281C60 |  |  | UStream.c |
| 11 | `LLTex.c`<br>Legacy/LL/LLTex.c | assert, TW06, 2002 tree; fn names: TX | S | S | 8000E708-8000FFAC | 8000CAF8..8000EA1C | 14 |  | 801868B0-801869A8 | 801A3438-801A3458 | 80280DC8-80280DD8 | 80281C60-80281C80 | 80282B08-80282B20 | 2/3 |  |
| 12 | `LLTexGrp.c`<br>Legacy/LL/LLTexGrp.c | assert, 2002 tree | S | S | 8000FFAC-800107F4 | 8000FFAC..800103C0 | 26 |  | 801869A8-801869C0 |  | 80280DD8-80280DE0 |  |  | 0/12 |  |
| 13 | `LLFont.c`<br>Legacy/LL/LLFont.c | assert, TW06, 2002 tree; fn names: FO | S | S | 800107F4-8001256C | 8001049C..800107F4 | 23 |  | 801869C0-80186AD8 | 801A3458-801A34C0 |  | 80281C80-80281C90 | 80282B20-80282B48 | 5/12 |  |
| 14 | `UFont.c`<br>Legacy/SPECIF/UFont.c | assert, 2002 tree | S | S | 8001256C-80012ED0 | 80012438..8001260C | 30 |  |  |  | 80280DE0-80280DF0 |  | 80282B48-80282B58 | 10/14 |  |
| 15 | `GoRenderCtx_Gc.c`<br>Golf/Hi-Rendering/Gc/GoRenderCtx_Gc.c | assert, TW06, 2002 tree; fn names: RC | S | S | 80012ED0-800143B8 | 80012E4C..8001371C | 71 |  | 80186AD8-80186BD8 | 801A34C0-801A3728 |  | 80281C90-80281CA0 | 80282B58-80282B80 | 8/21 |  |
| 16 | `streammanagerhole.cpp`<br>golf/streaming/streammanagerhole.cpp | TW06, TW06 file is C++; the 2003 file was C (probably streammanagerhole.c); fn names: Stream | M | S | 800143B8-80016CB8 | 8001425C..800143B8 | 92 |  | 80186BD8-80186CC8 | 801A3728-801B8A98 | 80280DF0-80280E10 |  | 80282B80-80282BB8 | 25/28 |  |
| 17 | `char_tex_manager.c`<br>golf/animation/char_tex_manager.c | TW06; fn names: s | M | S | 80016CB8-80017508 | 80016B54..80016D18 | 30 |  |  | 801B8A98-801B95C8 |  | 80281CA0-80281CA8 | 80282BB8-80282BC0 | 10/19 |  |
| 18 | `char.c`<br>Golf/Animation/char.c | assert, TW06, 2002 tree; fn names: Character, Char, vec4flt | S | S | 80017508-8001F32C | 800172B4..80017508 | 156 | 8017DB80-8017DD78 | 80186CC8-801871D0 | 801B95C8-801B9668 | 80280E10-80280E78 | 80281CA8-80281CB0 | 80282BC0-80282CA0 | 37/38 | CharClip.c |
| 19 | `mtalib.c`<br>Golf/Animation/mtalib.c | assert, 2002 tree | S | S | 8001F32C-8001FB00 | 8001F084..8001F32C | 15 | 8017DD78-8017DDC8 | 801871D0-801873F0 | 801B9668-801B9730 |  | 80281CB0-80281CC0 | 80282CA0-80282CC0 | 0/4 |  |
| 20 | `ska_shared.c`<br>golf/animation/ska_shared.c | TW06 | G | S | 8001FB00-80021ADC | 8001F42C..8001FB00 | 24 | 8017DDC8-8017E020 |  |  |  | 80281CC0-80281CC8 | 80282CC0-80282CF0 | 4/4 |  |
| 21 | `skalib.c`<br>Golf/Animation/skalib.c | assert, 2002 tree; fn names: Anim, Skalib, Clip | S | S | 80021ADC-80026844 | 80021ADC..80021B78 | 53 | 8017E020-8017E1B0 | 801873F0-80187400 | 801B9730-801C6498 | 80280E78-80281080 | 80281CC8-80281D20 | 80282CF0-80282D10 |  | skalib.c |
| 22 | `Skeleton.c`<br>Golf/Animation/Skeleton.c | assert, 2002 tree; fn names: SKEL | S | M | 80026844-80029FC8 | 800266C8..80026B4C | 56 |  | 80187400-80187490 | 801C6498-801C64A8 | 80281080-802810A8 | 80281D20-80281D28 | 80282D10-80282D60 | 8/9 |  |
| 23 | `(unnamed)` |  | - | M | 80029FC8-8002A630 | 80029BC8..8002A024 | 10 |  |  | 801C64A8-801C64D8 |  | 80281D28-80281D30 | 80282D60-80282D68 | 0/4 |  |
| 24 | `ai_brain.cpp`<br>golf/ai/ai_brain.cpp | TW06, TW06 file is C++; the 2003 file was C (probably ai_brain.c); fn names: AI, Shot, Golfer | M | M | 8002A630-8002BBB0 | 8002A164..8002AE30 | 9 |  | 80187490-801874B0 | 801C64D8-801C65B8 | 802810A8-802810B0 | 80281D30-80281D40 | 80282D68-80282E40 |  | Golfer.c* |
| 25 | `(unnamed)` | part of unit Golfer.c, which spans several files; fn names: AI, Game, Shot | - | S | 8002BBB0-8002C984 | 8002BBA4..8002BDEC | 14 |  |  | 801C65B8-801C66E8 |  | 80281D40-80281D48 | 80282E40-80282E80 |  | Golfer.c* |
| 26 | `(unnamed)` | part of unit Golfer.c, which spans several files; fn names: AI, Shot, Club | - | S | 8002C984-8002D8A8 | 8002C8B4..8002CD20 | 18 |  | 801874B0-80187650 |  |  |  | 80282E80-80282EE8 |  | Golfer.c* |
| 27 | `ai_core.cpp`<br>golf/ai/ai_core.cpp | TW06, TW06 file is C++; the 2003 file was C (probably ai_core.c); fn names: Golfer, Player, Luck | G | S | 8002D8A8-8002EBA4 | 8002D6FC..8002DC50 | 43 |  |  | 801C66E8-801D3950 | 802810B0-802810B8 | 80281D48-80281D50 | 80282EE8-80282F20 |  | Golfer.c* |
| 28 | `GoRenderSurface.c` | assert; fn names: Session, Options | S | S | 8002EBA4-8002F4FC | 8002E650..8002EE1C | 13 |  | 80187650-801876C8 | 801D3950-801D3A30 | 802810B8-802810C0 | 80281D50-80281D58 | 80282F20-80282F30 |  | Golfer.c* |
| 29 | `LLPict_Gc.c`<br>Legacy/LL/Gc/LLPict_Gc.c | assert, 2002 tree | S | S | 8002F4FC-80030254 | 8002F32C..8002F4FC | 22 |  | 801876C8-801876D8 |  | 802810C0-802810C8 |  | 80282F30-80282F48 | 6/13 |  |
| 30 | `GoTerrain.c`<br>Golf/Hi-Rendering/GoTerrain.c | assert, 2002 tree | S | S | 80030254-80035640 | 8003001C..80030254 | 97 | 8017E1B0-8017E1C0 | 801876D8-80187880 | 801D3A30-801D4E78 | 802810C8-802810F0 | 80281D58-80281D70 | 80282F48-80283018 | 14/29 |  |
| 31 | `Skin.c`<br>Golf/Animation/Skin.c | assert, 2002 tree | S | S | 80035640-80037DD8 | 8003541C..800358E0 | 43 | 8017E1C0-8017E690 | 80187880-80187900 | 801D4E78-801D4F68 | 802810F0-80281100 | 80281D70-80281D80 | 80283018-80283028 | 10/11 |  |
| 32 | `GoPostFx.c`<br>golf/hi-rendering/gopostfx.c | assert, TW06; fn names: Post | S | S | 80037DD8-80039454 | 80037AB8..80037DD8 | 26 |  | 80187900-80187910 | 801D4F68-801D5110 |  | 80281D80-80281D88 | 80283028-80283068 | 7/8 |  |
| 33 | `GoDynamicCam.c`<br>Golf/Cameras/GoDynamicCam.c | assert, TW06, 2002 tree; fn names: Dynamic | S | S | 80039454-8003DCE8 | 8003944C..80039554 | 63 | 8017E690-8017E938 | 80187910-80187AA0 |  | 80281100-80281110 | 80281D88-80281D90 | 80283068-802830A8 | 8/12 |  |
| 34 | `gocamscripts.c`<br>golf/cameras/gocamscripts.c | TW06; fn names: Camera, Cam, Terrain | M | S | 8003DCE8-80045660 | 8003DC30..8003DCE8 | 59 | 8017E938-8017E9B8 | 80187AA0-80187B88 |  |  |  | 802830A8-80283148 | 4/7 |  |
| 35 | `depthfield.c`<br>golf/sfx/depthfield.c | TW06; fn names: DF | M | G | 80045660-80045D18 | 800453C8..80045660 | 7 |  |  | 801D5110-801D5228 | 80281110-80281128 | 80281D90-80281D98 | 80283148-80283168 | 1/1 |  |
| 36 | `UObject3D.c`<br>Legacy/Lib/UObject3D.c | assert, 2002 tree | S | G | 80045D18-800461A8 | 80045D18..80045D80 | 20 |  | 80187B88-80187B98 |  |  | 80281D98-80281DA0 |  | 0/11 |  |
| 37 | `GoDynObj.c`<br>Golf/Hi-Rendering/GoDynObj.c | assert, 2002 tree | S | S | 800461A8-800486F4 | 80045E3C..800461A8 | 36 |  | 80187B98-80187D48 |  | 80281128-80281130 | 80281DA0-80281DA8 | 80283168-802831E0 | 5/6 |  |
| 38 | `UObject.c`<br>Legacy/Lib/UObject.c | assert, 2002 tree | S | S | 800486F4-80048AF4 | 80048680..800486F4 | 10 |  | 80187D48-80187D58 |  |  |  | 802831E0-80283200 | 3/4 |  |
| 39 | `UKernel.c`<br>Legacy/SPECIF/UKernel.c | assert, TW06, 2002 tree; fn names: Kernel | S | S | 80048AF4-800496E0 | 80048A84..80048AF4 | 23 |  | 80187D58-80187D68 | 801D5228-801D53A8 |  | 80281DA8-80281DC0 | 80283200-80283208 | 4/4 |  |
| 40 | `(unnamed)` |  | - | S | 800496E0-80049B40 | 800496E0..800497BC | 8 |  | 80187D68-80187DF0 |  |  |  | 80283208-80283218 | 1/2 |  |
| 41 | `goanimalactors.c`<br>golf/hi-rendering/goanimalactors.c | TW06; fn names: Act | M | S | 80049B40-8004ABBC | 80049ACC..80049B40 | 8 |  | 80187DF0-80187E68 |  |  |  | 80283218-80283280 | 0/3 |  |
| 42 | `(unnamed)` |  | - | S | 8004ABBC-8004AFA0 | 8004AAEC..8004ABBC | 5 |  |  |  |  |  | 80283280-80283288 | 1/2 |  |
| 43 | `goterraincollision.c`<br>golf/hi-rendering/goterraincollision.c | TW06; fn names: Ter | M | S | 8004AFA0-80050C2C | 8004AF28..8004AFA0 | 53 | 8017E9B8-80181328 |  | 801D53A8-801D5888 |  | 80281DC0-80281DD0 | 80283288-80283300 | 3/3 |  |
| 44 | `physics.c`<br>golf/physics/physics.c | TW06; fn names: Ball, Physics | M | S | 80050C2C-80055F1C | 80050BD8..80050C2C | 60 | 80181328-801834C0 | 80187E68-80187EF8 | 801D5888-801D5908 | 80281130-80281138 | 80281DD0-80281DE8 | 80283300-80283588 |  | Ball.c* |
| 45 | `LLPictInt.c`<br>Legacy/LL/LLPictInt.c | assert, TW06, 2002 tree; fn names: Wind, Pict | S | S | 80055F1C-800563C4 | 80055C1C..80055F88 | 9 |  | 80187EF8-80187F88 |  |  | 80281DE8-80281DF0 | 80283588-802835B0 |  | Ball.c* |
| 46 | `user.c` | assert | S | G | 800563C4-80056480 | = | 2 |  |  |  | 80281138-80281140 |  |  | 0/1 |  |
| 47 | `passwordmanager.cpp`<br>golf/earnings/passwordmanager.cpp | TW06, TW06 file is C++; the 2003 file was C (probably passwordmanager.c); fn names: Password | M | G | 80056480-80057F18 | 80056454..800564D8 | 9 | 801834C0-80183578 | 80187F88-80188138 | 801D5908-801D5968 | 80281140-80281188 |  |  | 0/1 |  |
| 48 | `fe_craputils.c`<br>golf/ui core/frontend/fe_craputils.c | TW06; fn names: FE | M | G | 80057F18-800589F8 | 80056B8C..80057F18 | 22 |  | 80188138-80188148 |  |  | 80281DF0-80281E00 |  | 0/4 |  |
| 49 | `Swing.c`<br>Golf/AI/Swing.c | assert, TW06, 2002 tree; fn names: Swing, Shot, Vec | S | S | 800589F8-8005CCAC | 80057FBC..800589F8 | 65 | 80183578-801835E0 | 80188148-801883C0 |  | 80281188-802811A8 |  | 802835B0-80283680 |  | Swing.c* |
| 50 | `stategolfer.c`<br>golf/state engine/stategolfer.c | TW06; fn names: GOLFERSTATE | M | G | 8005CCAC-8005D44C | 8005CBB0..8005D44C | 15 |  | 801883C0-801883D8 | 801D5968-801D5AB0 |  | 80281E00-80281E10 |  |  | Swing.c* |
| 51 | `statefunc.c`<br>golf/state engine/statefunc.c | TW06; fn names: STATEFUNC | M | G | 8005D44C-80062E40 | 8005D2DC..8005D44C | 101 | 801835E0-801836B0 | 801883D8-80188528 |  | 802811A8-802811B8 | 80281E10-80281E18 | 80283680-802836E8 |  | Swing.c* |
| 52 | `gocamcont.c`<br>golf/cameras/gocamcont.c | TW06; fn names: Camera, View | M | S | 80062E40-8006449C | 80062DCC..80062E40 | 28 | 801836B0-80183700 | 80188528-801885F8 |  |  |  | 802836E8-80283728 | 5/5 |  |
| 53 | `GoStaticCam.c`<br>Golf/Cameras/GoStaticCam.c | assert, 2002 tree | S | S | 8006449C-80065E9C | 80064478..800644F4 | 32 | 80183700-80183768 | 801885F8-80188628 |  |  | 80281E18-80281E28 | 80283728-80283748 | 3/12 |  |
| 54 | `event.c`<br>golf/eventmanager/event.c | TW06; fn names: EVENT | M | M | 80065E9C-80067710 | 8006596C..80065E9C | 76 |  | 80188628-80188778 |  | 802811B8-802811C0 |  | 80283748-80283770 | 10/14 |  |
| 55 | `(unnamed)` |  | - | M | 80067710-80067B80 | 8006752C..80067710 | 4 |  |  |  |  | 80281E28-80281E30 | 80283770-80283778 |  |  |
| 56 | `target.c`<br>golf/ai/target.c | TW06; fn names: Place | M | S | 80067B80-8006ACF8 | 8006795C..80067B80 | 38 | 80183768-80183848 | 80188778-80188830 | 801D5AB0-801D5F78 | 802811C0-802811D0 | 80281E30-80281E48 | 80283778-80283868 | 2/5 |  |
| 57 | `emotion.c`<br>golf/ai/emotion.c | TW06; fn names: Emotion | M | S | 8006ACF8-8006BED4 | 8006A7A8..8006AD68 | 16 | 80183848-80183858 | 80188830-801888C0 | 801D5F78-801D6030 |  |  | 80283868-802838B8 |  |  |
| 58 | `Replay.c`<br>golf/replay/replay.c | assert, TW06 | S | S | 8006BED4-8006C6F0 | 8006BDC8..8006BF60 | 15 |  | 801888C0-801888D0 | 801D6030-801D6F58 |  | 80281E48-80281E50 | 802838B8-802838C0 | 0/4 |  |
| 59 | `gomainloop.c`<br>golf/mainloop/gomainloop.c | TW06; fn names: GO | M | S | 8006C6F0-8006E150 | 8006C300..8006C968 | 53 |  | 801888D0-801888E0 |  | 802811D0-802811D8 | 80281E50-80281E68 | 802838C0-802838F0 | 4/20 |  |
| 60 | `GoFrameBuf.c`<br>Golf/Hi-Rendering/GoFrameBuf.c | assert, 2002 tree | S | S | 8006E150-8006E2A4 | 8006DBD4..8006E150 | 7 |  | 801888E0-801888F0 |  |  |  | 802838F0-80283900 | 2/3 |  |
| 61 | `GoLighting.c`<br>Golf/Hi-Rendering/GoLighting.c | assert, 2002 tree | S | S | 8006E2A4-8006F154 | 8006E26C..8006E2A4 | 16 |  | 801888F0-80188900 |  | 802811D8-802811E0 |  | 80283900-80283958 | 1/2 |  |
| 62 | `(unnamed)` |  | - | S | 8006F154-8006F438 | 8006EFB8..8006F154 | 5 |  |  |  | 802811E0-802811E8 |  | 80283958-80283988 | 0/2 |  |
| 63 | `GoShaderObjectContainer_OBFData_Gc.c`<br>Golf/Lo-Rendering/Shader/ShaderObjectContainer/Gc/GoShaderObjectContainer_OBFData_Gc.c | assert, 2002 tree | S | S | 8006F438-8006FF2C | 8006F3D0..8006F650 | 27 |  | 80188900-80188E18 |  | 802811E8-802811F8 |  | 80283988-802839B8 | 1/11 |  |
| 64 | `GoShaderObjectCommon_DynamicRenderingBuffer_Gc.h`<br>Golf/Lo-Rendering/Shader/ShaderObject/ShaderObjectCommon/Gc/Headers/GoShaderObjectCommon_DynamicRenderingBuffer_Gc.h | assert(.h), 2002 tree | M | M | 8006FF2C-80070EC4 | 8006FDCC..8007018C | 29 |  | 80188E18-80188E50 |  |  | 80281E68-80281E78 | 802839B8-802839C8 | 0/16 |  |
| 65 | `(unnamed)` |  | - | M | 80070EC4-8007110C | 80070348..80070EC4 | 8 |  |  |  |  | 80281E78-80281E80 | 802839C8-802839D0 | 0/6 |  |
| 66 | `GoShaderObject_PrelitUVAnimation_Gc.c` | assert | S | M | 8007110C-80071AD0 | 80070F00..8007110C | 39 |  | 80188E50-801893F0 |  | 802811F8-80281200 |  | 802839D0-802839E8 | 4/18 |  |
| 67 | `animblender.c`<br>golf/animation/animblender.c | TW06; fn names: Anim | G | S | 80071AD0-80073878 | 80071574..80071C28 | 29 |  |  |  |  | 80281E80-80281EA0 | 802839E8-80283A30 | 6/6 |  |
| 68 | `GoShaderObjectCommon_ShaderObjectsData_Gc.c` | assert | S | S | 80073878-8007531C | 80073878..80074BE0 | 16 |  | 801893F0-80189470 |  |  | 80281EA0-80281EB8 | 80283A30-80283A50 | 1/4 |  |
| 69 | `LLVideo.c`<br>Legacy/LL/LLVideo.c | assert, 2002 tree | S | S | 8007531C-800763BC | 8007524C..800754C0 | 35 |  | 80189470-80189480 |  | 80281200-80281208 | 80281EB8-80281EC0 | 80283A50-80283A70 | 6/9 |  |
| 70 | `GoCamera.c`<br>Golf/Hi-Rendering/GoCamera.c | assert, TW06, 2002 tree | S | S | 800763BC-80076A54 | 80076128..800763BC | 14 |  | 80189480-80189490 |  |  |  | 80283A70-80283A98 | 1/5 |  |
| 71 | `GoViewport.c`<br>Golf/Hi-Rendering/GoViewport.c | assert, 2002 tree | S | S | 80076A54-80076B7C | 80076948..80076A54 | 6 |  | 80189490-801894A0 |  |  |  | 80283A98-80283AA8 | 2/2 |  |
| 72 | `GoShaderObjectCommon_TexAnimManager_Gc.c` | assert | S | S | 80076B7C-80076F24 | = | 10 |  | 801894A0-801894D0 |  |  | 80281EC0-80281EC8 | 80283AA8-80283AC0 | 2/4 |  |
| 73 | `FE_Manager.c`<br>Golf/FrontEnd/FE_Manager.c | assert, 2002 tree; fn names: FE, Golfer | S | G | 80076F24-8007BDFC | 80076E48..80076F80 | 68 | 80183858-80183870 | 801894D0-801896F0 | 801D6F58-801D83B0 | 80281208-802812A0 | 80281EC8-80281ED8 | 80283AC0-80283AD8 | 12/19 |  |
| 74 | `apt_fe_gamemessages.c`<br>golf/ui core/frontend/gamemessages/apt_fe_gamemessages.c | TW06; fn names: Get | M | G | 8007BDFC-8007CACC | 80079E44..8007BE28 | 44 |  |  |  | 802812A0-802812B0 |  |  | 0/19 |  |
| 75 | `(unnamed)` |  | - | G | 8007CACC-80085D04 | 8007BEEC..8007CACC | 409 | 80183870-80183950 | 801896F0-801897E8 | 801D83B0-801D8708 | 802812B0-802812D0 |  | 80283AD8-80283B10 | 103/132 |  |
| 76 | `(unnamed)` |  | - | G | 80085D04-800864F8 | 80084008..80085D04 | 21 |  | 801897E8-801897F8 |  |  | 80281ED8-80281EE0 |  | 0/5 |  |
| 77 | `(unnamed)` |  | - | S | 800864F8-8008AD80 | 80084CFC..800864F8 | 202 | 80183950-80183960 | 801897F8-801899E0 |  | 802812D0-80281330 |  | 80283B10-80283B38 | 53/83 |  |
| 78 | `FEgolferanim.c`<br>Golf/UI runtime/FEgolferanim.c | assert, TW06, 2002 tree; fn names: sFE | S | S | 8008AD80-8008EC30 | 80089C4C..8008AD80 | 93 | 80183960-80183A70 | 801899E0-80189B18 | 801D8708-801D87A8 | 80281330-80281350 | 80281EE0-80281EF0 | 80283B38-80283B78 | 43/44 |  |
| 79 | `uiLoadFile.c`<br>Golf/UI runtime/uiLoadFile.c | assert, 2002 tree | S | S | 8008EC30-8008F3A4 | 8008EBB4..8008EC68 | 24 |  | 80189B18-80189B38 | 801D87A8-801D87C0 | 80281350-80281368 | 80281EF0-80281F18 |  | 8/12 |  |
| 80 | `uiProcessInterface.c`<br>Golf/UI runtime/uiProcessInterface.c | assert, 2002 tree | S | S | 8008F3A4-80090940 | 8008F310..8008F648 | 30 |  | 80189B38-80189C00 | 801D87C0-801D8818 | 80281368-80281378 | 80281F18-80281F20 | 80283B78-80283B90 | 2/7 |  |
| 81 | `fe_movies.c`<br>golf/ui core/frontend/fe_movies.c | TW06 | G | S | 80090940-800922A8 | 800907AC..80090B80 | 36 |  | 80189C00-80189C38 | 801D8818-801D8ED0 | 80281378-80281380 | 80281F20-80281F30 | 80283B90-80283BF0 | 7/9 |  |
| 82 | `(unnamed)` |  | - | S | 800922A8-80092CE8 | 80092250..800922A8 | 9 |  | 80189C38-80189CA0 |  |  | 80281F30-80281F38 | 80283BF0-80283C20 | 0/3 |  |
| 83 | `uiTransform.c`<br>Golf/UI runtime/uiTransform.c | assert, 2002 tree | S | S | 80092CE8-80093524 | 800929E0..80092F8C | 11 |  | 80189CA0-80189CB0 | 801D8ED0-801D9278 |  | 80281F38-80281F40 | 80283C20-80283C40 | 1/2 |  |
| 84 | `goballfx.c`<br>golf/hi-rendering/goballfx.c | TW06; fn names: BFX | M | S | 80093524-80093D3C | 8009349C..800935CC | 14 |  | 80189CB0-80189CC8 | 801D9278-801D96B8 | 80281380-80281390 | 80281F40-80281F50 | 80283C40-80283C68 | 5/6 |  |
| 85 | `(unnamed)` |  | - | S | 80093D3C-8009414C | 80093D14..80093DB8 | 3 |  |  | 801D96B8-801D97E0 | 80281390-802813A8 | 80281F50-80281F58 | 80283C68-80283C80 |  |  |
| 86 | `GoShaderObject_Particle_Gc.c`<br>Golf/Lo-Rendering/Shader/ShaderObject/Gc/GoShaderObject_Particle_Gc.c | assert, 2002 tree | S | S | 8009414C-800957FC | 800940FC..8009414C | 38 |  | 80189CC8-80189CE8 |  | 802813A8-802813B8 | 80281F58-80281F70 | 80283C80-80283CD8 | 4/14 |  |
| 87 | `char_state.c`<br>golf/animation/char_state.c | TW06; fn names: Character, Char | M | M | 800957FC-80097208 | 80095088..800957FC | 16 | 80183A70-80183A98 | 80189CE8-80189D30 |  |  |  | 80283CD8-80283D18 | 4/4 | CharAnim.c |
| 88 | `GoShaderObjectCommon_MorphAnimManager_Gc.c` | assert | S | M | 80097208-800977F8 | = | 12 |  | 80189D30-80189D60 |  |  | 80281F70-80281F78 | 80283D18-80283D28 | 1/5 |  |
| 89 | `GoCamTuningVars.c` | assert | S | S | 800977F8-80097E98 | 800975B0..800977F8 | 1 |  | 80189D60-80189D78 |  |  |  | 80283D28-80283DF8 |  |  |
| 90 | `GoShaderObject_Glows_Gc.c`<br>Golf/Lo-Rendering/Shader/ShaderObject/Gc/GoShaderObject_Glows_Gc.c | assert, 2002 tree | S | S | 80097E98-80098A98 | 80097E98..80097F44 | 21 |  | 80189D78-80189D98 | 801D97E0-801D9A68 |  | 80281F78-80281F88 | 80283DF8-80283E20 | 1/7 |  |
| 91 | `UFstPart.c`<br>Golf/SFX/UFstPart.c | assert, 2002 tree | S | S | 80098A98-80099ED8 | 80098740..80098A98 | 18 |  | 80189D98-80189DA8 | 801D9A68-801DB8A0 |  | 80281F88-80281F90 | 80283E20-80283E70 | 3/4 |  |
| 92 | `(unnamed)` |  | - | M | 80099ED8-8009A250 | 80099C50..8009A16C | 5 | 80183A98-80183AD8 |  | 801DB8A0-801E1470 |  | 80281F90-80281FA0 | 80283E70-80283E78 | 1/2 |  |
| 93 | `SunFlr_Gc.c` | assert | S | M | 8009A250-8009A928 | 8009A180..8009A250 | 9 |  | 80189DA8-80189E78 |  |  |  | 80283E78-80283E90 | 4/5 |  |
| 94 | `(unnamed)` |  | - | M | 8009A928-8009B320 | 8009A844..8009AA28 | 13 |  | 80189E78-8018C6C8 |  | 802813B8-802813C0 |  | 80283E90-80283EC0 | 0/4 |  |
| 95 | `(unnamed)` |  | - | M | 8009B320-8009B630 | 8009B0D0..8009B340 | 9 |  |  |  |  | 80281FA0-80281FA8 | 80283EC0-80283EC8 | 0/1 |  |
| 96 | `GoGreenGrid.c`<br>golf/hi-rendering/gogreengrid.c | assert, TW06; fn names: GR | S | M | 8009B630-8009CC00 | 8009B4B4..8009B630 | 11 |  | 8018C6C8-8018C6D8 |  | 802813C0-802813D0 | 80281FA8-80281FB0 | 80283EC8-80283F18 | 1/1 |  |
| 97 | `MC_Gc.c`<br>Golf/Memory Card/Gc/MC_Gc.c | assert, 2002 tree | S | S | 8009CC00-8009F734 | 8009C914..8009CC00 | 61 |  | 8018C6D8-8018C7C8 | 801E1470-801E7100 | 802813D0-802813F0 | 80281FB0-80281FD8 |  | 11/11 |  |
| 98 | `MC.c`<br>Golf/Memory Card/MC.c | assert, 2002 tree | S | S | 8009F734-800A27FC | 8009F6A0..8009FAD0 | 58 |  | 8018C7C8-8018C868 | 801E7100-801F1640 | 802813F0-802813F8 | 80281FD8-80282010 |  | 5/13 |  |
| 99 | `PsMgr.c`<br>Golf/SFX/PsMgr.c | assert, 2002 tree | S | S | 800A27FC-800A2C3C | 800A233C..800A2A80 | 11 |  |  | 801F1640-801F1708 | 802813F8-80281408 |  | 80283F18-80283F28 | 0/3 |  |
| 100 | `PsBallFx.c`<br>Golf/SFX/PsBallFx.c | assert, TW06, 2002 tree; fn names: Ps | S | S | 800A2C3C-800A3E3C | 800A2B34..800A2C3C | 13 |  | 8018C868-8018E988 |  | 80281408-80281418 |  | 80283F28-80283F48 | 1/1 |  |
| 101 | `(unnamed)` |  | - | S | 800A3E3C-800A7AF0 | 800A3D6C..800A41A4 | 105 | 80183AD8-80183B08 | 8018E988-8018EB30 | 801F1708-801F17D0 | 80281418-80281460 | 80282010-80282058 | 80283F48-80283F88 | 11/39 |  |
| 102 | `(unnamed)` |  | - | M | 800A7AF0-800A8604 | 800A67E8..800A7AF0 | 22 |  |  |  |  | 80282058-80282060 | 80283F88-80283FB0 | 1/2 |  |
| 103 | `hlaudmovie.c`<br>golf/audio/engine/hl/hlaudmovie.c | TW06; fn names: Mov | M | M | 800A8604-800A9808 | 800A8524..800A8604 | 39 |  | 8018EB30-8018EB78 | 801F17D0-801F1868 | 80281460-80281470 | 80282060-80282098 | 80283FB0-80283FD0 | 1/14 |  |
| 104 | `(unnamed)` |  | - | M | 800A9808-800AA4BC | 800A8700..800A9BC8 | 21 |  |  | 801F1868-801F1880 |  | 80282098-802820A8 | 80283FD0-80283FD8 | 5/7 |  |
| 105 | `(unnamed)` |  | - | M | 800AA4BC-800AB3A4 | 800AA498..800AA744 | 31 |  |  | 801F1880-801F18B8 |  |  | 80283FD8-80283FF8 | 6/10 |  |
| 106 | `(unnamed)` |  | - | M | 800AB3A4-800ACECC | 800AB32C..800AC6D0 | 41 |  | 8018EB78-8018EC10 | 801F18B8-801F2668 |  | 802820A8-802820B8 | 80283FF8-80284008 | 0/12 |  |
| 107 | `(unnamed)` |  | - | M | 800ACECC-800AE1F8 | 800ACA5C..800AD800 | 36 |  |  | 801F2668-801F5B40 |  |  | 80284008-80284018 | 7/14 |  |
| 108 | `uiobject.c`<br>golf/ui core/istudio runtime/uiobject.c | TW06; fn names: UI | M | S | 800AE1F8-800AF324 | 800ADE70..800AE1F8 | 17 | 80183B08-80183BC8 | 8018EC10-8018F040 | 801F5B40-801F5DA8 | 80281470-80281498 | 802820B8-802820E8 | 80284018-80284070 | 1/6 |  |
| 109 | `startUp.c`<br>Golf/Entry/startUp.c | assert, 2002 tree | S | S | 800AF324-800B2314 | 800AF0A8..800AF324 | 103 |  | 8018F040-8018FF68 | 801F5DA8-801F5E20 | 80281498-802814A8 | 802820E8-80282158 | 80284070-802840B8 | 13/28 |  |
| 110 | `shadow.c`<br>Golf/SFX/shadow.c | assert, TW06, 2002 tree; fn names: SH | S | S | 800B2314-800B34F0 | 800B1A40..800B24E0 | 19 |  | 8018FF68-8018FF78 |  | 802814A8-802814B8 | 80282158-80282178 | 802840B8-802840F8 | 1/8 |  |
| 111 | `GoComicCam.c`<br>Golf/Cameras/GoComicCam.c | assert, 2002 tree | S | S | 800B34F0-800B4B5C | 800B3438..800B34F0 | 15 | 80183BC8-80183C78 | 8018FF78-8018FF88 |  |  | 80282178-80282180 | 802840F8-80284148 | 5/5 |  |
| 112 | `GoShaderObject_Rain_Gc.c`<br>Golf/Lo-Rendering/Shader/ShaderObject/Gc/GoShaderObject_Rain_Gc.c | assert, TW06, 2002 tree; fn names: SD | S | S | 800B4B5C-800B5A14 | 800B4AE0..800B4B5C | 22 |  | 8018FF88-8018FFA8 | 801F5E20-801F6360 | 802814B8-802814C8 |  | 80284148-802841B0 | 3/9 |  |
| 113 | `UAudMemStack.c`<br>Golf/Audio/Engine/Utils/UAudMemStack.c | assert, 2002 tree | S | S | 800B5A14-800B5C40 | 800B58B4..800B5A14 | 10 |  | 8018FFA8-8018FFB8 | 801F6360-801F6680 |  | 80282180-80282188 |  | 0/1 |  |
| 114 | `GoARAM.c` | assert | S | M | 800B5C40-800B6924 | 800B5AAC..800B62DC | 27 |  | 8018FFB8-8018FFE0 |  | 802814C8-802814D0 |  |  | 0/11 |  |
| 115 | `(unnamed)` |  | - | M | 800B6924-800B769C | 800B64D8..800B7210 | 15 |  | 8018FFE0-80190FE0 | 801F6680-801F6858 | 802814D0-802814E8 | 80282188-802821A8 | 802841B0-802841C8 | 0/4 |  |
| 116 | `rcmp_mad_codec.c`<br>Golf/RCMP/rcmp_mad_codec.c | assert, 2002 tree | S | S | 800B769C-800B9FF0 | 800B7490..800B9100 | 45 | 80183C78-80184C68 | 80190FE0-801910F8 | 801F6858-801F8458 | 802814E8-80281508 | 802821A8-802821E8 | 802841C8-802841E0 | 13/14 |  |
| 117 | `(unnamed)` |  | - | S | 800B9FF0-800BA74C | 800B9FF0..800BA1A4 | 17 | 80184C68-80184C78 |  | 801F8458-801FA0F0 | 80281508-80281510 |  | 802841E0-80284238 | 5/7 |  |
| 118 | `(unnamed)` |  | - | S | 800BA74C-800BA940 | 800BA734..800BA74C | 1 |  |  |  |  | 802821E8-802821F8 | 80284238-80284248 |  |  |
| 119 | `(unnamed)` |  | - | S | 800BA940-800BAEB0 | = | 8 |  |  |  | 80281510-80281520 |  | 80284248-80284280 | 1/1 |  |
| 120 | `SitDevFile.c`<br>golf/sitdev/sitdevfile.c | assert, TW06; fn names: Vec, Sit, FE | S | G | 800BAEB0-800BCD68 | 800BAD60..800BAEB0 | 42 |  | 801910F8-801912D0 | 801FA0F0-801FA1E8 |  | 802821F8-80282220 | 80284280-802842C0 | 5/7 |  |
| 121 | `(unnamed)` |  | - | G | 800BCD68-800BD894 | 800BCA60..800BD580 | 11 |  | 801912D0-80191308 |  |  |  |  | 0/2 |  |
| 122 | `GoGolfCam.c`<br>golf/cameras/gogolfcam.c | assert, TW06; fn names: Golf | S | S | 800BD894-800C7480 | 800BCA60..800BD894 | 123 | 80184C78-80184D20 | 80191308-80191440 | 801FA1E8-801FA1F8 | 80281520-80281528 | 80282220-80282228 | 802842C0-802843B8 |  | GoGolfCam.c |
| 123 | `(unnamed)` |  | - | S | 800C7480-800C808C | 800C7450..800C7480 | 8 |  | 80191440-80191480 |  |  |  | 802843B8-802843F8 |  |  |
| 124 | `GoBreakLine.c` | assert; fn names: Break | S | S | 800C808C-800C937C | 800C8068..800C808C | 12 | 80184D20-80184D30 | 80191480-80191490 |  | 80281528-80281530 | 80282228-80282230 | 802843F8-80284460 | 1/1 |  |
| 125 | `AnimStream.c` | assert | S | S | 800C937C-800CC1EC | 800C9310..800C937C | 33 |  | 80191490-80191748 |  | 80281530-80281538 | 80282230-80282238 | 80284460-80284478 | 2/2 |  |
| 126 | `SkinPart.c` | assert | S | G | 800CC1EC-800CF904 | 800CC1EC..800CD404 | 88 |  | 80191748-80191798 |  | 80281538-80281548 | 80282238-80282240 |  | 3/9 |  |
| 127 | `analysisutilities.c`<br>golf/gamemode/analysisutilities.c | TW06; fn names: Hole | M | G | 800CF904-800D1698 | 800CEA30..800D0550 | 34 |  |  |  |  | 80282240-80282248 | 80284478-802844C8 | 0/1 | HoleScore.c |
| 128 | `(unnamed)` | fn names: Hole | - | S | 800D1698-800D3244 | 800D1674..800D1698 | 59 |  | 80191798-801917D0 | 801FA1F8-801FFAE8 | 80281548-80281550 |  | 802844C8-80284510 | 6/12 |  |
| 129 | `Earnings.c` | existing unit name (hand-made, behaviour-based) | G | S | 800D3244-800D9E14 | 800D2994..800D33A8 | 64 |  | 801917D0-80192010 | 801FFAE8-80202828 | 80281550-80281568 | 80282248-80282260 | 80284510-80284560 |  | Earnings.c |
| 130 | `(unnamed)` |  | - | S | 800D9E14-800DAE44 | 800D9350..800DA36C | 17 |  |  | 80202828-80202898 | 80281568-80281588 | 80282260-80282270 | 80284560-80284578 | 2/5 |  |
| 131 | `gameeffects.c`<br>golf/gamemode/gameeffects.c | TW06; fn names: Game | M | S | 800DAE44-800DCAD8 | = | 27 |  |  | 80202898-802028F0 |  |  | 80284578-802845C0 |  | GameEffects.c |
| 132 | `gamemode.c`<br>golf/gamemode/gamemode.c | TW06; fn names: GM | M | G | 800DCAD8-800E0AF0 | 800DCAD8..800DD4B4 | 56 | 80184D30-80184D40 | 80192010-80192030 |  |  |  | 802845C0-80284610 |  | GameManager.c |
| 133 | `gamemanager.cpp`<br>golf/mainloop/gamemanager.cpp | TW06, TW06 file is C++; the 2003 file was C (probably gamemanager.c); fn names: Game, Gimme | M | G | 800E0AF0-800E3B28 | 800E0A84..800E276C | 63 | 80184D40-80184D90 | 80192030-80192558 |  | 80281588-80281640 | 80282270-80282280 | 80284610-80284628 |  | GameRound.c |
| 134 | `GameUI.c` | existing unit name (hand-made, behaviour-based) | G | G | 800E3B28-800E4FFC | 800E2810..800E4364 | 31 | 80184D90-80184DA0 | 80192558-80192588 | 802028F0-80203138 | 80281640-80281648 | 80282280-802822E0 | 80284628-80284630 |  | GameUI.c |
| 135 | `gameui.c`<br>golf/gamemode/gameui.c | TW06 | G | G | 800E4FFC-800E5E54 | = | 55 |  |  | 80203138-80203148 |  | 802822E0-802822E8 | 80284630-80284638 |  | GameMessages.c |
| 136 | `GameAnalysis.c` | existing unit name (hand-made, behaviour-based) | G | G | 800E5E54-800E68F0 | = | 11 |  | 80192588-801925B8 |  |  |  | 80284638-80284648 |  | GameAnalysis.c |
| 137 | `GameModeAlternateShot.c` | existing unit name (hand-made, behaviour-based) | G | G | 800E68F0-800E7980 | = | 15 | 80184DA0-80184DB0 |  |  | 80281648-80281650 |  | 80284648-80284650 |  | GameModeAlternateShot.c |
| 138 | `GameModeBattle.c` | existing unit name (hand-made, behaviour-based) | G | G | 800E7980-800E81C4 | = | 17 |  | 801925B8-801925D0 | 80203148-80203170 | 80281650-80281658 | 802822E8-802822F0 |  |  | GameModeBattle.c |
| 139 | `GameModeBestBall.c` | existing unit name (hand-made, behaviour-based) | G | G | 800E81C4-800E8D58 | = | 13 | 80184DB0-80184DC0 |  |  |  |  | 80284650-80284658 |  | GameModeBestBall.c |
| 140 | `GameModeFourBall.c` | existing unit name (hand-made, behaviour-based) | G | G | 800E8D58-800E9E40 | = | 13 | 80184DC0-80184DD0 |  |  |  |  | 80284658-80284660 |  | GameModeFourBall.c |
| 141 | `GameModeMatch.c` | existing unit name (hand-made, behaviour-based) | G | G | 800E9E40-800EACD8 | = | 11 |  |  |  | 80281658-80281660 |  | 80284660-80284668 |  | GameModeMatch.c |
| 142 | `GameMode5.c` | existing unit name (hand-made, behaviour-based) | G | G | 800EACD8-800ED738 | = | 43 |  | 801925D0-801926A8 | 80203170-80205ED8 | 80281660-80281670 | 802822F0-80282330 | 80284668-80284670 |  | GameMode5.c |
| 143 | `GameMode9.c` | existing unit name (hand-made, behaviour-based) | G | G | 800ED738-800EDD18 | 800E68F0..800EDA74 | 13 | 80184DD0-80184DE0 |  |  |  | 80282330-80282338 | 80284670-80284688 |  | GameMode9.c |
| 144 | `GameMode23.c` | existing unit name (hand-made, behaviour-based) | G | G | 800EDD18-800F0448 | = | 69 |  | 801926A8-80192810 | 80205ED8-8020CF90 | 80281670-80281680 | 80282338-80282348 |  |  | GameMode23.c |
| 145 | `GameMode24.c` | existing unit name (hand-made, behaviour-based) | G | G | 800F0448-800F125C | = | 34 |  |  | 8020CF90-80211D38 | 80281680-80281688 | 80282348-80282360 |  |  | GameMode24.c |
| 146 | `GameModeReplay.c` | existing unit name (hand-made, behaviour-based) | G | G | 800F125C-800F19D4 | = | 12 |  |  |  |  |  | 80284688-802846A0 |  | GameModeReplay.c |
| 147 | `GameTargets.c` | existing unit name (hand-made, behaviour-based) | G | G | 800F19D4-800F2984 | 800F193C..800F266C | 27 |  | 80192810-801928EC | 80211D38-80211FB8 |  | 80282360-80282368 | 802846A0-802846B8 |  | GameTargets.c |
| 148 | `GameMode14.c` | existing unit name (hand-made, behaviour-based) | G | G | 800F2984-800F39F4 | = | 29 |  | 801928EC-80192908 | 80211FB8-802120F8 | 80281688-80281690 | 80282368-80282378 | 802846B8-802846C0 |  | GameMode14.c |
| 149 | `GameMode15.c` | existing unit name (hand-made, behaviour-based) | G | M | 800F39F4-800F4B40 | 800F2810..800F3EBC | 24 |  |  |  | 80281690-80281698 | 80282378-80282390 | 802846C0-802846D0 |  | GameMode15.c |
| 150 | `GameMode16.c` | existing unit name (hand-made, behaviour-based) | G | G | 800F4B40-800F5AAC | 800F4584..800F5014 | 21 |  |  |  | 80281698-802816A0 | 80282390-802823A0 | 802846D0-802846E0 |  | GameMode16.c |
| 151 | `GameMode17.c` | existing unit name (hand-made, behaviour-based) | G | G | 800F5AAC-800F6A60 | = | 22 |  |  |  | 802816A0-802816A8 | 802823A0-802823B0 | 802846E0-802846E8 |  | GameMode17.c |
| 152 | `GameMode13.c` | existing unit name (hand-made, behaviour-based) | G | G | 800F6A60-800F80FC | 800F5A88..800F6ED4 | 28 |  |  |  | 802816A8-802816B0 | 802823B0-802823C0 | 802846E8-80284700 |  | GameMode13.c |
| 153 | `GameMode2.c` | existing unit name (hand-made, behaviour-based) | G | G | 800F80FC-800F944C | = | 14 |  |  |  |  | 802823C0-802823C8 | 80284700-80284708 |  | GameMode2.c |
| 154 | `GameMode6.c` | existing unit name (hand-made, behaviour-based) | G | G | 800F944C-800F9610 | 800F8068..800FB35C | 1 |  |  |  |  |  |  |  | GameMode6.c |
| 155 | `GameMode7.c` | existing unit name (hand-made, behaviour-based) | G | G | 800F9610-800F986C | = | 3 |  |  |  |  |  |  |  | GameMode7.c |
| 156 | `GameMode8.c` | existing unit name (hand-made, behaviour-based) | G | G | 800F986C-800FE1B4 | = | 66 |  | 80192908-80192D00 | 802120F8-80212418 | 802816B0-802816D0 | 802823C8-802823D8 | 80284708-802847A0 |  | GameMode8.c |
| 157 | `GameModeStableford.c` | existing unit name (hand-made, behaviour-based) | G | G | 800FE1B4-800FEAFC | = | 9 | 80184DE0-80184DF0 |  |  | 802816D0-802816D8 |  | 802847A0-802847A8 |  | GameModeStableford.c |
| 158 | `GameMode12.c` | existing unit name (hand-made, behaviour-based) | G | G | 800FEAFC-800FF700 | 800FCBDC..800FEC80 | 20 |  | 80192D00-80192D1C | 80212418-802124B8 |  | 802823D8-802823E0 | 802847A8-802847E0 |  | GameMode12.c |
| 159 | `GameModeStroke.c` | existing unit name (hand-made, behaviour-based) | G | G | 800FF700-800FFF34 | = | 7 | 80184DF0-80184E00 |  |  |  |  | 802847E0-802847E8 |  | GameModeStroke.c |
| 160 | `GameMode11.c` | existing unit name (hand-made, behaviour-based); fn names: Scenario | G | G | 800FFF34-80101FEC | 800FF288..80100508 | 40 | 80184E00-80184E10 | 80192D1C-80193130 |  | 802816D8-802816E0 | 802823E0-80282430 | 802847E8-80284808 |  | GameMode11.c |
| 161 | `GameMode4.c` | existing unit name (hand-made, behaviour-based) | G | G | 80101FEC-80102AC8 | = | 29 |  | 80193130-80193188 | 802124B8-80212B60 | 802816E0-802816E8 | 80282430-80282458 |  |  | GameMode4.c |
| 162 | `(unnamed)` |  | - | G | 80102AC8-801037F8 | 80102874..80103684 | 2 |  | 80193188-80193228 |  |  | 80282458-80282460 | 80284808-80284838 |  |  |
| 163 | `FE_CrAPDB.c`<br>golf/ui core/frontend/fe_crapdb.c | assert, TW06; fn names: FE, s | S | M | 801037F8-80109120 | 80103684..80103EFC | 157 | 80184E10-80184E30 | 80193228-80193AB0 |  | 802816E8-80281760 | 80282460-80282488 | 80284838-80284848 | 24/44 |  |
| 164 | `LLDynTex.c` | assert | S | M | 80109120-8010A6A8 | 801069AC..80109120 | 49 |  | 80193AB0-80193B28 |  |  |  | 80284848-80284868 | 20/28 |  |
| 165 | `(unnamed)` |  | - | S | 8010A6A8-8010C4A0 | 80109EAC..8010A788 | 35 |  |  |  |  | 80282488-80282490 | 80284868-80284890 | 4/8 |  |
| 166 | `CharSliders.c`<br>golf/animation/charsliders.c | assert, TW06; fn names: Char | S | S | 8010C4A0-8010DC94 | 8010BCFC..8010C4A0 | 29 |  | 80193B28-80193C90 |  | 80281760-80281790 | 80282490-802824B0 | 80284890-802848A0 | 12/12 |  |
| 167 | `FE_PGATourMessages.c` | assert | S | S | 8010DC94-8010F6C8 | 8010DC94..8010DCE8 | 36 |  | 80193C90-80193D28 |  | 80281790-802817C8 | 802824B0-802824B8 | 802848A0-802848C8 | 1/7 |  |
| 168 | `FE_LogoDesign.c` | assert | S | M | 8010F6C8-8010FF9C | 8010EEA8..8010F6C8 | 19 |  | 80193D28-80193D58 | 80212B60-80213B60 |  | 802824B8-802824C8 |  | 7/9 |  |
| 169 | `(unnamed)` |  | - | M | 8010FF9C-801104AC | 8010FB70..8010FF9C | 13 |  | 80193D58-80193D80 | 80213B60-80223BB0 | 802817C8-802817D8 | 802824C8-802824E0 |  | 1/4 |  |
| 170 | `hwsBurn.c` | assert | S | S | 801104AC-8011206C | 80110234..801104AC | 29 |  | 80193D80-80193D90 |  |  |  |  | 4/4 |  |
| 171 | `hwsMaterial_Gc.c` | assert | S | S | 8011206C-80112804 | 80111EB0..8011206C | 20 |  | 80193D90-80193DC0 |  | 802817D8-802817E8 |  |  | 4/6 |  |
| 172 | `hwsOverride_Gc.c` | assert | S | S | 80112804-80112B80 | 801127A0..80112848 | 14 |  | 80193DC0-80193DD8 |  |  |  |  | 3/5 |  |
| 173 | `hwsRender_Gc.c` | assert | S | S | 80112B80-80113E60 | 80112A10..80112C64 | 33 |  | 80193DD8-80193DE8 | 80223BB0-80223C48 | 802817E8-802817F8 | 802824E0-802824F8 | 802848C8-802848E8 | 2/14 |  |
| 174 | `DynChain.c`<br>golf/animation/dynchain.c | assert, TW06 | S | M | 80113E60-80116678 | 8011368C..80113E60 | 34 |  | 80193DE8-80193E70 |  |  | 802824F8-80282500 | 802848E8-802849C0 | 3/6 |  |
| 175 | `(unnamed)` |  | - | M | 80116678-80117600 | 801164D4..80116678 | 29 |  | 80193E70-80193F88 | 80223C48-80223C70 | 802817F8-80281840 |  |  | 5/7 |  |
| 176 | `(unnamed)` |  | - | M | 80117600-8011C068 | 80117348..80118710 | 119 |  | 80193F88-80194170 | 80223C70-80250080 | 80281840-80281880 | 80282500-80282508 | 802849C0-80284A18 | 25/31 |  |
| 177 | `SkinMorph.c` | assert | S | G | 8011C068-8011CEB0 | 8011BCFC..8011C27C | 26 |  | 80194170-80194180 |  | 80281880-80281888 |  | 80284A18-80284A30 | 6/7 |  |
| 178 | `fe_stats.c`<br>golf/ui core/frontend/fe_stats.c | TW06; fn names: Print | M | G | 8011CEB0-8011D280 | 8011CADC..8011CEB0 | 7 |  | 80194180-801944F8 |  | 80281888-802818A8 | 80282508-80282510 |  | 2/3 |  |
| 179 | `llrtclock.c`<br>legacy/ll/llrtclock.c | TW06; fn names: RT | M | G | 8011D280-8011E170 | 8011D268..8011D280 | 12 |  | 801944F8-80194598 |  | 802818A8-80281900 |  |  | 1/1 |  |
| 180 | `GoGrass.c`<br>golf/hi-rendering/gograss.c | assert, TW06; fn names: Grass | S | G | 8011E170-80120304 | = | 37 |  | 80194598-80194648 | 80250080-802603B0 | 80281900-80281908 | 80282510-80282518 | 80284A30-80284AD0 | 10/15 |  |
| 181 | `(unnamed)` |  | - | S | 80120304-8012168C | 80120194..80120304 | 22 |  | 80194648-80194748 | 802603B0-80260940 | 80281908-80281928 |  | 80284AD0-80284B00 | 8/8 |  |
| 182 | `TibExt.c`<br>tiburon/eassdk/tibext.c | assert, TW06; fn names: Tib | S | S | 8012168C-80122AF0 | 8012168C..80121C80 | 48 |  | 80194748-80194810 | 80260940-80260D88 | 80281928-80281980 |  | 80284B00-80284B10 | 2/15 |  |
| 183 | `gbacable.c` | assert | S | S | 80122AF0-80124A40 | 80122128..80122CFC | 48 | 80184E30-80184E40 | 80194810-80195340 | 80260D88-80261058 | 80281980-80281988 | 80282518-80282568 |  | 0/9 |  |
| 184 | `EASportsBio.c` | assert | S | S | 80124A40-8012597C | 80123E34..80124B54 | 41 |  | 80195340-801953E0 |  | 80281988-80281998 | 80282568-80282580 | 80284B10-80284B28 | 18/21 |  |
| 185 | `(unnamed)` |  | - | G | 8012597C-801270F0 | 8012597C..80125A24 | 40 |  | 801953E0-80195500 |  | 80281998-802819A8 | 80282580-802825A8 | 80284B28-80284B38 | 10/11 |  |
| 186 | `SkinBurn.c` | assert | S | G | 801270F0-80127D84 | 80127098..801270F0 | 12 |  | 80195500-80195520 |  | 802819A8-802819B0 | 802825A8-802825B0 | 80284B38-80284B40 | 0/1 |  |
| 187 | `(unnamed)` |  | - | S | 80127D84-8012E950 | 80127B10..80128FD4 | 135 | 80184E40-80184FC0 | 80195520-80195750 |  | 802819B0-802819B8 | 802825B0-802825C0 | 80284B40-80284B60 | 20/26 |  |
| 188 | `vi.c` | assert; fn names: VI, get, set | S | S | 8012E950-8013042C | = | 16 |  | 80195750-801959B0 | 80261058-802611A0 | 802819B8-802819C8 | 802825C0-80282618 |  |  | vi.c |
| 189 | `EXIBios.c` | SDK source unit; fn names: EXI, Set, __EXIProbe | S | S | 8013042C-80131D08 | = | 23 |  | 801959B0-80195BF0 | 802611A0-80261260 | 802819C8-802819D0 |  |  |  | EXIBios.c |
| 190 | `EXIUart.c` | SDK source unit; fn names: InitializeUART, WriteUARTN | S | S | 80131D08-80131F78 | = | 2 |  |  |  |  | 80282618-80282628 |  |  | EXIUart.c |
| 191 | `SIBios.c` | SDK source unit; fn names: SI, Complete, __SITransfer | S | S | 80131F78-801337D8 | = | 24 |  | 80195BF0-80195D08 | 80261260-80261460 | 802819D0-802819D8 | 80282628-80282638 |  |  | SIBios.c |
| 192 | `SISamplingRate.c` | SDK source unit; fn names: SI | S | S | 801337D8-801338E0 | = | 2 |  | 80195D08-80195DA0 |  |  | 80282638-80282640 |  |  | SISamplingRate.c |
| 193 | `(unnamed)` | fn names: AMC, Hu | - | S | 801338E0-80133918 | = | 9 |  |  |  |  |  |  | 1/1 |  |
| 194 | `ar.c` | SDK source unit; fn names: AR, __ARHandler, __ARChecksize | S | S | 80133918-8013538C | = | 7 |  |  |  | 802819D8-802819E0 | 80282640-80282660 |  |  | ar.c |
| 195 | `arq.c` | SDK source unit; fn names: ARQ, __ARQServiceQueu, __ARQCallbackHac | S | S | 8013538C-80135810 | = | 6 |  |  |  | 802819E0-802819E8 | 80282660-80282690 |  |  | arq.c |
| 196 | `AX.c` | SDK source unit; fn names: AX | S | S | 80135810-8013584C | = | 1 |  |  |  | 802819E8-802819F0 |  |  |  | AX.c |
| 197 | `AXAlloc.c` | SDK source unit; fn names: __AXGetStackHead, __AXServiceCallb, __AXAllocQuit | S | S | 8013584C-80135BE0 | = | 8 |  |  | 80261460-80261560 |  | 80282690-80282698 |  |  | AXAlloc.c |
| 198 | `AXAux.c` | SDK source unit; fn names: __AXAuxInit, __AXGetAuxAOutpu, __AXProcessAux | S | S | 80135BE0-80136058 | = | 12 |  |  | 80261560-80264260 |  | 80282698-802826D0 |  |  | AXAux.c |
| 199 | `(unnamed)` |  | - | S | 80136058-8013606C | = | 2 |  |  |  |  |  |  | 2/2 |  |
| 200 | `AXCL.c` | SDK source unit; fn names: __AXGetCommandLi, __AXNextFrame, __AXClInit | S | S | 8013606C-80136790 | = | 4 |  | 80195DA0-801979A0 | 80264260-80264860 |  | 802826D0-802826E8 |  |  | AXCL.c |
| 201 | `(unnamed)` |  | - | S | 80136790-80136E08 | = | 9 |  | 801979A0-801998C0 | 80264860-802690C0 | 802819F0-802819F8 | 802826E8-80282708 |  | 3/3 |  |
| 202 | `AXSPB.c` | SDK source unit; fn names: __AXPrintStudio, __AXSPBInit, __AXDepopVoice | S | S | 80136E08-801372C0 | = | 3 |  |  |  |  | 80282708-80282730 |  |  | AXSPB.c |
| 203 | `(unnamed)` |  | - | S | 801372C0-80137C38 | = | 4 |  |  | 802690C0-8027A4C0 |  | 80282730-80282740 |  | 2/2 |  |
| 204 | `AXVPB.c` | SDK source unit; fn names: AX, __AXSetPBDefault | S | S | 80137C38-80138184 | = | 8 |  |  |  |  |  | 80284B60-80284B68 |  | AXVPB.c |
| 205 | `AXProf.c` | SDK source unit; fn names: __AXGetCurrentPr | S | S | 80138184-801381CC | = | 1 |  |  |  |  | 80282740-80282750 |  |  | AXProf.c |
| 206 | `reverb_hi.c` | SDK source unit; fn names: AXFX, ReverbHI, Do | S | S | 801381CC-80138EC8 | = | 5 |  | 801998C0-801998E0 |  |  |  | 80284B68-80284BA8 |  | reverb_hi.c |
| 207 | `delay.c` | SDK source unit; fn names: AXFX | S | S | 80138EC8-80139374 | = | 4 |  |  |  | 802819F8-80281A00 |  |  |  | delay.c |
| 208 | `(unnamed)` |  | - | S | 80139374-801393D0 | = | 3 |  |  |  |  |  |  | 1/1 |  |
| 209 | `PPCArch.c` | SDK source unit; fn names: PPC | S | S | 801393D0-801394C0 | = | 17 |  |  |  |  |  |  |  | PPCArch.c |
| 210 | `db.c` | SDK source unit; fn names: DB, __DBExceptionDes, __DBIsExceptionM | S | S | 801394C0-801395AC | = | 5 |  | 801998E0-801998F8 |  |  | 80282750-80282758 |  |  | db.c |
| 211 | `dsp.c` | SDK source unit; fn names: DSP | S | S | 801395AC-801397FC | = | 8 |  | 801998F8-80199978 |  | 80281A00-80281A08 | 80282758-80282760 |  |  | dsp.c |
| 212 | `dsp_debug.c` | SDK source unit; fn names: __DSP_debug_prin | S | S | 801397FC-8013984C | = | 1 |  |  |  |  |  |  |  | dsp_debug.c |
| 213 | `dsp_task.c` | SDK source unit; fn names: __DSPHandler, __DSP_exec_task, __DSP_boot_task | S | S | 8013984C-8013A0D0 | = | 5 |  | 80199978-80199AB8 |  |  | 80282760-80282778 |  |  | dsp_task.c |
| 214 | `dvdlow.c` | SDK source unit; fn names: DVD, Alarm, __DVDInitWA | S | S | 8013A0D0-8013AF10 | = | 20 |  |  | 8027A4C0-8027A590 | 80281A08-80281A10 | 80282778-802827C0 |  |  | dvdlow.c |
| 215 | `dvdfs.c` | assert; fn names: DVD, __DVDFSInit, cb | S | S | 8013AF10-8013B3C4 | = | 6 |  | 80199AB8-80199BB8 |  | 80281A10-80281A18 | 802827C0-802827D8 |  |  | dvdfs.c |
| 216 | `(unnamed)` |  | - | S | 8013B3C4-8013B3C8 | = | 1 |  |  |  |  |  |  | 1/1 |  |
| 217 | `dvd.c` | assert; fn names: DVD, cb, state | S | S | 8013B3C8-8013DCB4 | = | 46 |  | 80199BB8-80199D38 | 8027A590-8027A610 | 80281A18-80281A38 | 802827D8-80282830 |  |  | dvd.c |
| 218 | `dvdqueue.c` | SDK source unit; fn names: __DVDClearWaitin, __DVDPushWaiting, __DVDPopWaitingQ | S | S | 8013DCB4-8013DEAC | = | 5 |  |  | 8027A610-8027A658 |  |  |  |  | dvdqueue.c |
| 219 | `dvderror.c` | SDK source unit; fn names: Error, __DVDStoreErrorC | S | S | 8013DEAC-8013E044 | = | 2 |  | 80199D38-80199D80 |  |  |  |  |  | dvderror.c |
| 220 | `dvdidutils.c` | SDK source unit; fn names: DVD | S | S | 8013E044-8013E13C | = | 1 |  |  |  |  |  |  |  | dvdidutils.c |
| 221 | `dvdFatal.c` | SDK source unit; fn names: __DVDPrintFatalM | S | S | 8013E13C-8013E16C | = | 1 |  |  |  |  | 80282830-80282838 |  |  | dvdFatal.c |
| 222 | `fstload.c` | SDK source unit; fn names: cb, __fstLoad | S | S | 8013E16C-8013E3AC | = | 2 |  |  | 8027A658-8027A6C8 |  | 80282838-80282848 |  |  | fstload.c |
| 223 | `GXInit.c` | SDK source unit; fn names: __GXDefaultTexRe, __GXDefaultTlutR, GX | S | S | 8013E3AC-8013F57C | = | 4 |  | 80199D80-80199F20 | 8027A6C8-8027AC40 | 80281A38-80281A50 | 80282848-80282858 | 80284BA8-80284BD0 |  | GXInit.c |
| 224 | `GXFifo.c` | SDK source unit; fn names: GX, __GXWriteFifoInt, GXCP | S | S | 8013F57C-8013FF30 | = | 19 |  |  |  |  | 80282858-80282878 |  |  | GXFifo.c |
| 225 | `(unnamed)` |  | - | S | 8013FF30-8013FF38 | = | 1 |  |  |  |  |  |  | 1/1 |  |
| 226 | `GXAttr.c` | SDK source unit; fn names: GX, __GXXfVtxSpecs, __GXSetVCD | S | S | 8013FF38-801410A8 | = | 12 |  | 80199F20-8019A080 |  | 80281A50-80281A60 |  |  |  | GXAttr.c |
| 227 | `GXMisc.c` | SDK source unit; fn names: GX, __GXPEInit | S | S | 801410A8-801415AC | = | 17 |  |  |  |  | 80282878-80282890 |  |  | GXMisc.c |
| 228 | `GXGeometry.c` | SDK source unit; fn names: GX, __GXSetDirtyStat, __GXSendFlushPri | S | S | 801415AC-8014195C | = | 9 |  |  |  |  |  |  |  | GXGeometry.c |
| 229 | `GXFrameBuf.c` | SDK source unit; fn names: GX | S | S | 8014195C-80142440 | = | 14 |  | 8019A080-8019A170 |  |  |  | 80284BD0-80284BD8 |  | GXFrameBuf.c |
| 230 | `GXLight.c` | SDK source unit; fn names: GX | S | S | 80142440-801427D0 | = | 7 |  |  |  |  |  |  |  | GXLight.c |
| 231 | `GXTexture.c` | SDK source unit; fn names: GX, __GetImageTileCo, __SetSURegs | S | S | 801427D0-80143768 | = | 18 |  | 8019A170-8019A398 |  | 80281A60-80281AA0 |  | 80284BD8-80284C00 |  | GXTexture.c |
| 232 | `GXBump.c` | SDK source unit; fn names: GX, __GXUpdateBPMask, __GXFlushTexture | S | S | 80143768-80143AE0 | = | 6 |  |  |  |  |  |  |  | GXBump.c |
| 233 | `GXTev.c` | SDK source unit; fn names: GX | S | S | 80143AE0-801442C4 | = | 16 |  | 8019A398-8019A410 |  |  |  |  |  | GXTev.c |
| 234 | `GXPixel.c` | SDK source unit; fn names: GX | S | S | 801442C4-80144898 | = | 12 |  | 8019A410-8019A430 |  |  |  | 80284C00-80284C30 |  | GXPixel.c |
| 235 | `GXStubs.c` | SDK source unit; fn names: __GXSetRange | S | S | 80144898-8014489C | = | 1 |  |  |  |  |  |  |  | GXStubs.c |
| 236 | `GXDisplayList.c` | SDK source unit; fn names: GX | S | S | 8014489C-80144AA8 | = | 3 |  |  | 8027AC40-8027B160 |  | 80282890-80282898 |  |  | GXDisplayList.c |
| 237 | `GXTransform.c` | SDK source unit; fn names: GX, __GXSetMatrixInd | S | S | 80144AA8-80145140 | = | 15 |  |  |  |  |  | 80284C30-80284C48 |  | GXTransform.c |
| 238 | `GXPerf.c` | SDK source unit; fn names: GX | S | S | 80145140-80145998 | = | 2 |  | 8019A430-8019A520 |  |  |  |  |  | GXPerf.c |
| 239 | `(unnamed)` | fn names: MIX, __MIXSetPan | - | S | 80145998-80147B94 | = | 11 |  | 8019A520-8019B0E0 | 8027B160-8027C960 |  | 80282898-802828A8 |  | 1/1 |  |
| 240 | `mtx.c` | SDK source unit; fn names: PSMTX | S | S | 80147B94-80147DB0 | = | 5 |  |  |  | 80281AA0-80281AA8 |  | 80284C48-80284C50 |  | mtx.c |
| 241 | `mtx44.c` | SDK source unit; fn names: C | S | S | 80147DB0-80147E48 | = | 1 |  |  |  |  |  | 80284C50-80284C60 |  | mtx44.c |
| 242 | `OS.c` | SDK source unit; fn names: OS, Clear, Inquiry | S | S | 80147E48-80148790 | = | 14 |  |  | 8027C960-8027C980 | 80281AA8-80281AB0 | 802828A8-802828D8 |  |  | OS.c |
| 243 | `OSAlarm.c` | SDK source unit; fn names: OS, Decrementer, Insert | S | S | 80148790-80148E40 | = | 7 |  |  |  |  | 802828D8-802828E0 |  |  | OSAlarm.c |
| 244 | `OSAlloc.c` | SDK source unit; fn names: OS, DL | S | S | 80148E40-80149150 | = | 6 |  |  |  | 80281AB0-80281AB8 | 802828E0-802828F0 |  |  | OSAlloc.c |
| 245 | `OSArena.c` | SDK source unit; fn names: OS | S | S | 80149150-80149170 | = | 4 |  |  |  | 80281AB8-80281AC0 | 802828F0-802828F8 |  |  | OSArena.c |
| 246 | `OSAudioSystem.c` | SDK source unit; fn names: __OSInitAudioSys, __OSStopAudioSys | S | S | 80149170-80149404 | = | 2 |  | 8019B0E0-8019B368 |  |  |  |  |  | OSAudioSystem.c |
| 247 | `OSCache.c` | SDK source unit; fn names: DC, IC, LC | S | S | 80149404-8014983C | = | 12 |  |  |  |  |  |  |  | OSCache.c |
| 248 | `OSContext.c` | SDK source unit; fn names: OS, __OSLoadFPUConte, __OSSaveFPUConte | S | S | 8014983C-8014A0AC | = | 13 |  | 8019B368-8019B770 |  |  |  |  |  | OSContext.c |
| 249 | `OSError.c` | SDK source unit; fn names: OS, __OSUnhandledExc | S | S | 8014A0AC-8014A758 | = | 4 |  |  | 8027C980-8027CA00 | 80281AC0-80281AC8 |  |  |  | OSError.c |
| 250 | `OSFont.c` | SDK source unit; fn names: OS | S | S | 8014A758-8014A7B0 | = | 1 |  |  |  | 80281AC8-80281AD0 |  |  |  | OSFont.c |
| 251 | `OSInterrupt.c` | SDK source unit; fn names: OS, __OSSetInterrupt, __OSGetInterrupt | S | S | 8014A7B0-8014B01C | = | 11 |  | 8019B770-8019BAC0 |  |  | 802828F8-80282910 |  |  | OSInterrupt.c |
| 252 | `OSLink.c` | SDK source unit; fn names: __OSModuleInit | S | S | 8014B01C-8014B034 | = | 1 |  |  |  |  |  |  |  | OSLink.c |
| 253 | `OSMemory.c` | SDK source unit; fn names: On, MEM, Config24MB | S | S | 8014B034-8014B314 | = | 6 |  | 8019BAC0-8019BAD0 |  |  |  |  |  | OSMemory.c |
| 254 | `OSMutex.c` | SDK source unit; fn names: OS, __OSUnlockAllMut, __OSCheckMutex | S | S | 8014B314-8014B70C | = | 7 |  |  |  |  |  |  |  | OSMutex.c |
| 255 | `OSReboot.c` | SDK source unit; fn names: Run, __OSReboot | S | S | 8014B70C-8014B8F0 | = | 3 |  |  | 8027CA00-8027CA20 |  | 80282910-80282920 |  |  | OSReboot.c |
| 256 | `OSReset.c` | SDK source unit; fn names: OS, Reset, __OSDoHotReset | S | S | 8014B8F0-8014BCD4 | = | 5 |  |  |  |  | 80282920-80282928 |  |  | OSReset.c |
| 257 | `OSResetSW.c` | SDK source unit; fn names: __OSResetSWInter, OS | S | S | 8014BCD4-8014C060 | = | 2 |  |  |  |  | 80282928-80282948 |  |  | OSResetSW.c |
| 258 | `OSRtc.c` | SDK source unit; fn names: OS, Write, __OSInitSram | S | S | 8014C060-8014C8D4 | = | 12 |  |  | 8027CA20-8027CA78 |  |  |  |  | OSRtc.c |
| 259 | `(unnamed)` | fn names: OS | - | S | 8014C8D4-8014C9FC | = | 3 |  |  |  |  |  |  |  |  |
| 260 | `OSSync.c` | SDK source unit; fn names: System, __OSInitSystemCa | S | S | 8014C9FC-8014CA80 | = | 2 |  |  |  |  |  |  |  | OSSync.c |
| 261 | `(unnamed)` |  | - | S | 8014CA80-8014CA84 | = | 1 |  |  |  |  |  |  | 1/1 |  |
| 262 | `OSThread.c` | SDK source unit; fn names: OS, __OSThreadInit, Unset | S | S | 8014CA84-8014E48C | = | 21 |  | 8019BAD0-8019C2E0 | 8027CA78-8027D470 | 80281AD0-80281AD8 | 80282948-80282958 |  |  | OSThread.c |
| 263 | `OSTime.c` | SDK source unit; fn names: OS, __OSGetSystemTim, Get | S | S | 8014E48C-8014E8B0 | = | 5 |  | 8019C2E0-8019C340 |  |  |  |  |  | OSTime.c |
| 264 | `__ppc_eabi_init.c` | SDK source unit; fn names: __init_user, __init_cpp, _ExitProcess | S | S | 8014E8B0-8014E944 | = | 3 |  |  |  |  |  |  |  | __ppc_eabi_init.c |
| 265 | `PadClamp.c` | SDK source unit; fn names: Clamp, PAD | S | S | 8014E944-8014EB88 | = | 2 | 80184FC0-80184FD0 |  |  |  |  |  |  | PadClamp.c |
| 266 | `Pad.c` | SDK source unit; fn names: PAD, Update, SPEC0 | S | S | 8014EB88-80150658 | = | 19 |  | 8019C340-8019C398 | 8027D470-8027D4C0 | 80281AD8-80281AF8 | 80282958-80282980 |  |  | Pad.c |
| 267 | `ai.c` | SDK source unit; fn names: AI, __AI_set_stream_, __AISHandler | S | S | 80150658-80150F24 | = | 18 |  | 8019C398-8019C3E0 |  | 80281AF8-80281B00 | 80282980-802829C0 |  |  | ai.c |
| 268 | `(unnamed)` |  | - | S | 80150F24-80150F2C | = | 2 |  |  |  |  |  |  | 1/1 |  |
| 269 | `odenotstub.c` | SDK source unit; fn names: DBG, DB, MW | S | S | 80150F2C-801519A4 | = | 12 |  |  |  | 80281B00-80281B08 | 802829C0-802829D8 |  |  | odenotstub.c |
| 270 | `__va_arg.c` | SDK source unit; fn names: __va_arg | S | S | 801519A4-80151A6C | = | 1 |  |  |  |  |  |  |  | __va_arg.c |
| 271 | `global_destructor_chain.c` | SDK source unit; fn names: __destroy_global | S | S | 80151A6C-80151AB4 | = | 1 |  |  |  |  | 802829D8-802829E0 |  |  | global_destructor_chain.c |
| 272 | `runtime.c` | SDK source unit; fn names: __cvt_fp2unsigne, _savefpr_14, _restfpr_14 | S | S | 80151AB4-801520F8 | = | 11 | 80184FD0-80184FE8 |  |  |  |  |  |  | runtime.c |
| 273 | `GCN_mem_alloc.c` | SDK source unit; fn names: __sys_alloc | S | S | 801520F8-801521B0 | = | 1 | 80184FE8-80185060 |  |  |  |  |  |  | GCN_mem_alloc.c |
| 274 | `abort_exit.c` | SDK source unit; fn names: exit | S | S | 801521B0-801522A0 | = | 1 |  |  | 8027D4C0-8027D5C0 |  | 802829E0-802829F0 |  |  | abort_exit.c |
| 275 | `alloc.c` | SDK source unit; fn names: deallocate, free, __pool_free | S | S | 801522A0-80152754 | = | 4 | 80185060-80185078 |  | 8027D5C0-8027D5F8 |  | 802829F0-802829F8 |  |  | alloc.c |
| 276 | `ansi_files.c` | SDK source unit; fn names: __flush_all, __close_all | S | S | 80152754-8015286C | = | 2 |  | 8019C3E0-8019C520 |  |  |  |  |  | ansi_files.c |
| 277 | `(unnamed)` |  | - | S | 8015286C-801538EC | = | 1 | 80185078-80185158 | 8019C520-8019C560 |  |  |  | 80284C60-80284C90 |  |  |
| 278 | `ansi_fp.c` | SDK source unit; fn names: __num2dec, __num2dec_intern, __equals_dec | S | S | 801538EC-80155BD0 | = | 5 |  | 8019C560-8019C688 |  |  |  |  |  | ansi_fp.c |
| 279 | `(unnamed)` |  | - | S | 80155BD0-80155E48 | = | 1 |  |  |  |  |  |  |  |  |
| 280 | `buffer_io.c` | SDK source unit; fn names: __flush_buffer, __prep_buffer | S | S | 80155E48-80155F40 | = | 2 |  |  |  |  |  |  |  | buffer_io.c |
| 281 | `(unnamed)` | fn names: __end_critical_r, __begin_critical, __kill_critical_ | - | S | 80155F40-80155F4C | = | 3 |  |  |  |  |  |  | 1/1 |  |
| 282 | `ctype.c` | SDK source unit; fn names: tolower | S | S | 80155F4C-80155F70 | = | 1 |  | 8019C688-8019C988 |  |  |  |  |  | ctype.c |
| 283 | `(unnamed)` |  | - | S | 80155F70-80155F88 | = | 1 |  |  |  |  |  |  |  |  |
| 284 | `direct_io.c` | SDK source unit; fn names: __fwrite, fwrite | S | S | 80155F88-80156310 | = | 2 |  |  |  |  |  |  |  | direct_io.c |
| 285 | `file_io.c` | SDK source unit; fn names: fflush, fclose | S | S | 80156310-80156604 | = | 2 |  |  |  |  |  |  |  | file_io.c |
| 286 | `FILE_POS.c` | SDK source unit; fn names: fseek, _fseek, ftell | S | S | 80156604-80156954 | = | 3 |  |  |  |  | 802829F8-80282A00 |  |  | FILE_POS.c |
| 287 | `mbstring.c` | SDK source unit; fn names: wcstombs, is | S | S | 80156954-80156C78 | = | 3 |  |  |  |  |  | 80284C90-80284C98 |  | mbstring.c |
| 288 | `mem.c` | SDK source unit; fn names: memcmp, __memrchr, memchr | S | S | 80156C78-80156DE8 | = | 4 |  |  |  |  |  |  |  | mem.c |
| 289 | `mem_funcs.c` | SDK source unit; fn names: __copy_longs_rev, __copy_longs_una | S | S | 80156DE8-80156F54 | = | 2 |  |  |  |  |  |  |  | mem_funcs.c |
| 290 | `(unnamed)` | fn names: __copy_longs_rev, __copy_longs_ali | - | S | 80156F54-801570B8 | = | 2 |  |  |  |  |  |  |  |  |
| 291 | `misc_io.c` | SDK source unit; fn names: __stdio_atexit | S | S | 801570B8-801570C8 | = | 1 |  |  |  |  |  |  |  | misc_io.c |
| 292 | `printf.c` | SDK source unit; fn names: sprintf, vsprintf, vprintf | S | S | 801570C8-8015929C | = | 14 |  | 8019C988-8019CBF0 |  |  |  | 80284C98-80284CA0 |  | printf.c |
| 293 | `(unnamed)` |  | - | S | 8015929C-801594CC | = | 2 |  |  |  |  |  |  |  |  |
| 294 | `sscanf.c` | SDK source unit; fn names: __StringRead | S | S | 801594CC-8015955C | = | 1 |  |  |  |  |  |  |  | sscanf.c |
| 295 | `(unnamed)` |  | - | S | 8015955C-8015A450 | = | 2 | 80185158-801851B0 | 8019CBF0-8019CCC0 |  |  |  |  |  |  |
| 296 | `string.c` | SDK source unit; fn names: strstr, strtok, strchr | S | S | 8015A450-8015A8D0 | = | 9 | 801851B0-801851D0 |  |  | 80281B08-80281B18 |  |  |  | string.c |
| 297 | `strtold.c` | SDK source unit | S | S | 8015A8D0-8015B8DC | = | 1 | 801851D0-80185578 |  |  | 80281B18-80281B44 |  | 80284CA0-80284CB8 |  | strtold.c |
| 298 | `strtoul.c` | SDK source unit; fn names: atoi, __strtoull, __strtoul | S | S | 8015B8DC-8015C120 | = | 3 |  | 8019CCC0-8019CD48 |  |  |  |  |  | strtoul.c |
| 299 | `(unnamed)` |  | - | S | 8015C120-8015C1F8 | = | 2 |  |  |  |  | 80282A00-80282A08 |  | 2/2 |  |
| 300 | `wchar_io.c` | SDK source unit; fn names: fwide | S | S | 8015C1F8-8015C280 | = | 1 |  |  |  |  |  |  |  | wchar_io.c |
| 301 | `e_acos.c` | SDK source unit; fn names: __ieee754_acos | S | S | 8015C280-8015C590 | = | 1 |  |  |  |  |  | 80284CB8-80284D48 |  | e_acos.c |
| 302 | `e_asin.c` | SDK source unit; fn names: __ieee754_asin | S | S | 8015C590-8015C7F4 | = | 1 |  |  |  |  |  | 80284D48-80284DE0 |  | e_asin.c |
| 303 | `e_atan2.c` | SDK source unit; fn names: __ieee754_atan2 | S | S | 8015C7F4-8015CA84 | = | 1 |  |  |  |  |  | 80284DE0-80284E38 |  | e_atan2.c |
| 304 | `e_fmod.c` | SDK source unit; fn names: __ieee754_fmod | S | S | 8015CA84-8015CDC0 | = | 1 | 80185578-80185588 |  |  |  |  |  |  | e_fmod.c |
| 305 | `e_log.c` | SDK source unit; fn names: __ieee754_log | S | S | 8015CDC0-8015D03C | = | 1 |  |  |  |  | 80282A08-80282A10 | 80284E38-80284EB8 |  | e_log.c |
| 306 | `e_pow.c` | SDK source unit; fn names: __ieee754_pow | S | S | 8015D03C-8015D8F0 | = | 1 | 80185588-801855B8 |  |  |  |  | 80284EB8-80284FC8 |  | e_pow.c |
| 307 | `e_rem_pio2.c` | SDK source unit; fn names: __ieee754_rem_pi | S | S | 8015D8F0-8015DC90 | = | 1 | 801855B8-80185740 |  |  |  |  | 80284FC8-80285020 |  | e_rem_pio2.c |
| 308 | `k_cos.c` | SDK source unit; fn names: __kernel_cos | S | S | 8015DC90-8015DD84 | = | 1 |  |  |  |  |  | 80285020-80285068 |  | k_cos.c |
| 309 | `k_rem_pio2.c` | SDK source unit; fn names: __kernel_rem_pio | S | S | 8015DD84-8015EBD8 | = | 1 | 80185740-80185790 |  |  |  |  | 80285068-802850A8 |  | k_rem_pio2.c |
| 310 | `k_sin.c` | SDK source unit; fn names: __kernel_sin | S | S | 8015EBD8-8015EC78 | = | 1 |  |  |  |  |  | 802850A8-802850E0 |  | k_sin.c |
| 311 | `k_tan.c` | SDK source unit; fn names: __kernel_tan | S | S | 8015EC78-8015EE8C | = | 1 | 80185790-801857F8 |  |  |  |  | 802850E0-80285118 |  | k_tan.c |
| 312 | `s_atan.c` | SDK source unit; fn names: atan | S | S | 8015EE8C-8015F0A4 | = | 1 | 801857F8-80185890 |  |  |  |  | 80285118-80285140 |  | s_atan.c |
| 313 | `s_copysign.c` | SDK source unit; fn names: copysign | S | S | 8015F0A4-8015F0CC | = | 1 |  |  |  |  |  |  |  | s_copysign.c |
| 314 | `s_cos.c` | SDK source unit; fn names: cos | S | S | 8015F0CC-8015F1A0 | = | 1 |  |  |  |  |  | 80285140-80285148 |  | s_cos.c |
| 315 | `s_floor.c` | SDK source unit; fn names: floor | S | S | 8015F1A0-8015F2E8 | = | 1 |  |  |  |  |  | 80285148-80285158 |  | s_floor.c |
| 316 | `s_frexp.c` | SDK source unit; fn names: frexp | S | S | 8015F2E8-8015F374 | = | 1 |  |  |  |  |  | 80285158-80285160 |  | s_frexp.c |
| 317 | `s_ldexp.c` | SDK source unit; fn names: ldexp | S | S | 8015F374-8015F538 | = | 1 |  |  |  |  |  | 80285160-80285188 |  | s_ldexp.c |
| 318 | `s_modf.c` | SDK source unit; fn names: modf | S | S | 8015F538-8015F634 | = | 1 |  |  |  |  |  |  |  | s_modf.c |
| 319 | `s_sin.c` | SDK source unit; fn names: sin | S | S | 8015F634-8015F70C | = | 1 |  |  |  |  |  | 80285188-80285190 |  | s_sin.c |
| 320 | `s_tan.c` | SDK source unit; fn names: tan | S | S | 8015F70C-8015F784 | = | 1 |  |  |  |  |  | 80285190-80285198 |  | s_tan.c |
| 321 | `(unnamed)` |  | - | S | 8015F784-8015F8D0 | = | 7 |  |  |  |  |  |  | 1/1 |  |
| 322 | `mainloop.c` | SDK source unit; fn names: TRK | S | S | 8015F8D0-8015F9C8 | = | 1 |  |  |  |  | 80282A10-80282A18 |  |  | mainloop.c |
| 323 | `nubevent.c` | SDK source unit; fn names: TRK | S | S | 8015F9C8-8015FBF0 | = | 5 |  |  | 8027D5F8-8027D920 |  |  |  |  | nubevent.c |
| 324 | `nubinit.c` | SDK source unit; fn names: TRK | S | S | 8015FBF0-8015FD88 | = | 3 |  |  |  |  |  |  |  | nubinit.c |
| 325 | `msg.c` | SDK source unit; fn names: TRK | S | S | 8015FD88-8015FDCC | = | 1 | 80185890-801858E8 |  |  |  |  |  |  | msg.c |
| 326 | `msgbuf.c` | SDK source unit; fn names: TRK | S | S | 8015FDCC-80160610 | = | 15 | 801858E8-80185908 |  | 8027D920-8027F2D8 |  |  |  |  | msgbuf.c |
| 327 | `serpoll.c` | SDK source unit; fn names: TRK | S | S | 80160610-801608C0 | = | 4 | 80185908-80185A98 |  | 8027F2D8-8027F2F0 |  |  |  |  | serpoll.c |
| 328 | `usr_put.c` | SDK source unit; fn names: usr | S | S | 801608C0-8016094C | = | 2 |  |  |  |  |  |  |  | usr_put.c |
| 329 | `dispatch.c` | SDK source unit; fn names: TRK | S | S | 8016094C-80160AC4 | = | 2 | 80185A98-80185AD8 | 8019CD48-8019CDB8 |  |  |  |  |  | dispatch.c |
| 330 | `msghndlr.c` | SDK source unit; fn names: TRK, Output | S | S | 80160AC4-80161A94 | = | 17 | 80185AD8-80185CC0 | 8019CDB8-8019CDF0 | 8027F2F0-8027F2F8 |  |  |  |  | msghndlr.c |
| 331 | `support.c` | SDK source unit; fn names: Handle, TRK | S | S | 80161A94-801621C8 | = | 5 | 80185CC0-80185D68 |  |  |  |  |  |  | support.c |
| 332 | `mutex_TRK.c` | SDK source unit | S | S | 801621C8-801621E0 | = | 3 |  |  |  |  |  |  |  | mutex_TRK.c |
| 333 | `notify.c` | SDK source unit; fn names: TRK | S | S | 801621E0-80162278 | = | 1 |  |  |  |  |  |  |  | notify.c |
| 334 | `flush_cache.c` | SDK source unit; fn names: TRK | S | S | 80162278-801622B0 | = | 1 |  |  |  |  |  |  |  | flush_cache.c |
| 335 | `mem_TRK.c` | SDK source unit; fn names: TRK | S | S | 801622B0-80162378 | = | 3 |  |  |  |  |  |  |  | mem_TRK.c |
| 336 | `targimpl.c` | SDK source unit; fn names: TRK | S | S | 80162378-80163DD0 | = | 29 | 80185D68-80185E00 | 8019CDF0-8019CE20 | 8027F2F8-8027F878 |  |  |  |  | targimpl.c |
| 337 | `mpc_7xx_603e.c` | SDK source unit; fn names: TRK | S | S | 80163DD0-80164140 | = | 2 |  |  |  |  |  |  |  | mpc_7xx_603e.c |
| 338 | `dolphin_trk.c` | SDK source unit; fn names: TRK, Init, __TRK_copy_vecto | S | S | 80164140-801643C8 | = | 6 |  | 8019CE20-8019CE60 | 8027F878-8027F880 |  |  |  |  | dolphin_trk.c |
| 339 | `main_TRK.c` | SDK source unit; fn names: TRK | S | S | 801643C8-80164420 | = | 1 |  |  | 8027F880-8027F888 |  |  |  |  | main_TRK.c |
| 340 | `dolphin_trk_glue.c` | SDK source unit; fn names: TRK, TRKUART, Initialize | S | S | 80164420-80164978 | = | 13 | 80185E00-80185F18 | 8019CE60-8019CE88 | 8027F888-8027F890 |  |  |  |  | dolphin_trk_glue.c |
| 341 | `targcont.c` | SDK source unit; fn names: TRK | S | S | 80164978-801649AC | = | 1 |  |  |  |  |  |  |  | targcont.c |
| 342 | `target_options.c` | SDK source unit | S | S | 801649AC-801649C8 | = | 2 |  |  | 8027F890-8027F898 |  |  |  |  | target_options.c |
| 343 | `mslsupp.c` | SDK source unit; fn names: __TRK_write_cons, __read_console | S | S | 801649C8-80164B40 | = | 2 |  |  |  |  |  |  |  | mslsupp.c |
| 344 | `UDP_Stubs.c` | SDK source unit; fn names: udp | S | S | 80164B40-80164B88 | = | 9 |  |  |  |  |  |  |  | UDP_Stubs.c |
| 345 | `main.c` | SDK source unit; fn names: ddh | S | S | 80164B88-80164ECC | = | 10 | 80185F18-80185FF8 |  | 8027F898-802800B8 |  | 80282A18-80282A20 |  |  | main.c |
| 346 | `CircleBuffer.c` | SDK source unit; fn names: Circle | S | S | 80164ECC-80165134 | = | 4 |  |  |  |  |  |  |  | CircleBuffer.c |
| 347 | `main.c` | SDK source unit; fn names: gdev | S | S | 80165134-80165480 | = | 10 | 80185FF8-801860D8 |  | 802800B8-802805D8 |  | 80282A20-80282A28 |  |  | main.c |
| 348 | `MWTrace.c` | SDK source unit; fn names: MWTRACE | S | S | 80165480-801654D0 | = | 1 |  |  |  |  |  |  |  | MWTrace.c |
| 349 | `(unnamed)` |  | - | M | 801654D0-8016604C | = | 12 | 801860D8-801861A0 | 8019CE88-8019CEB0 |  |  | 80282A28-80282A2C | 80285198-802851A8 | 4/4 |  |
| 350 | `UIStudio.c` | assert | S | M | 8016604C-80168C24 | 8016604C..80166098 | 6 | 801861A0-8018620C | 8019CEB0-8019D118 | 802805D8-80280628 |  |  | 802851A8-802851B8 |  |  |
| 351 | `(unnamed)` |  | - | S | 80168C24-8016A2D4 | 80168644..80168C24 | 21 |  |  |  |  |  | 802851B8-802851C0 | 4/4 |  |
| 352 | `(unnamed)` |  | - | S | 8016A2D4-8016C718 | 8016A2D4..8016BEDC | 24 |  | 8019D118-8019D1FC | 80280628-80280660 |  |  | 802851C0-802851E8 | 1/1 |  |
| 353 | `ChecksumCRC32.c`<br>Common/Checksum/ChecksumCRC32.c | assert, verified unit; fn names: CRC32 | S | S | 8016C718-8016CC20 | = | 10 | 8018620C-80186258 | 8019D1FC-8019D210 |  |  | 80282A2C-80282A30 |  |  | ChecksumCRC32.c |
| 354 | `CipherXOR.c`<br>Common/Cipher/CipherXOR.c | verified unit; fn names: Cipher | S | S | 8016CC20-8016CFF8 | = | 8 | 80186258-80186274 |  |  | 80281B44-80281B54 |  |  |  | CipherXOR.c |
| 355 | `SharedFileIO.c`<br>Common/SharedFileIO/SharedFileIO.c | assert, verified unit; fn names: SFIO | S | S | 8016CFF8-80171308 | = | 50 | 80186274-801863D4 | 8019D210-8019D2C0 |  | 80281B54-80281B5C |  |  |  | SharedFileIO.c |
| 356 | `llSharedFileIO.c`<br>llSharedFileIO.c | assert, filemap.md | S | S | 80171308-801730C8 | = | 15 | 801863D4-8018655C |  |  |  |  |  |  |  |
| 357 | `TagFile.c`<br>Common/TagFile/TagFile.c | assert, verified unit; fn names: Tag | S | S | 801730C8-80175F54 | = | 41 | 8018655C-801866D4 |  |  |  | 80282A30-80282A38 |  |  | TagFile.c |
| 358 | `GBA.c` | assert; fn names: On | S | S | 80175F54-80176280 | = | 7 |  | 8019D2C0-8019D300 | 80280660-80280760 | 80281B5C-80281B68 | 80282A38-80282A40 |  |  | GBA.c |
| 359 | `(unnamed)` |  | - | S | 80176280-8017641C | = | 1 |  |  |  |  |  |  |  |  |
| 360 | `GBAXfer.c` | assert | S | S | 8017641C-80176A38 | 80176088..80176870 | 11 |  | 8019D300-8019D338 | 80280760-80280B60 |  |  |  |  |  |
| 361 | `CARDBios.c` | SDK source unit; fn names: CARD, __CARDDefaultApi, __CARDSyncCallba | S | S | 80176A38-80177DA4 | = | 26 |  | 8019D338-8019D3A0 | 80280B60-80280D80 | 80281B68-80281B70 | 80282A40-80282A42 |  |  | CARDBios.c |
| 362 | `CARDUnlock.c` | SDK source unit; fn names: bitrev, Read, Dummy | S | S | 80177DA4-80179004 | = | 6 |  | 8019D3A0-8019D500 |  | 80281B70-80281B78 |  |  |  | CARDUnlock.c |
| 363 | `CARDRdwr.c` | SDK source unit; fn names: Block, __CARDRead, __CARDWrite | S | S | 80179004-8017929C | = | 5 |  |  |  |  |  |  |  | CARDRdwr.c |
| 364 | `CARDBlock.c` | SDK source unit; fn names: __CARDGetFatBloc, Write, Erase | S | S | 8017929C-801796A0 | = | 6 |  |  |  |  |  |  |  | CARDBlock.c |
| 365 | `CARDDir.c` | SDK source unit; fn names: __CARDGetDirBloc, Write, Erase | S | S | 801796A0-80179904 | = | 4 |  |  |  |  |  |  |  | CARDDir.c |
| 366 | `CARDCheck.c` | SDK source unit; fn names: CARD, __CARDCheckSum, VerifyID | S | S | 80179904-8017A840 | = | 7 |  |  |  |  |  |  |  | CARDCheck.c |
| 367 | `CARDMount.c` | SDK source unit; fn names: CARD, Do, Is | S | S | 8017A840-8017B2B8 | = | 7 |  | 8019D500-8019D540 |  | 80281B78-80281B7B |  |  |  | CARDMount.c |
| 368 | `CARDFormat.c` | SDK source unit; fn names: Format, __CARDFormatRegi, CARD | S | S | 8017B2B8-8017BA9C | = | 3 |  |  |  |  |  |  |  | CARDFormat.c |
| 369 | `CARDOpen.c` | SDK source unit; fn names: CARD, __CARDCompareFil, __CARDAccess | S | S | 8017BA9C-8017C18C | = | 9 |  |  | 80280D80-80280DA0 |  |  |  |  | CARDOpen.c |
| 370 | `CARDCreate.c` | SDK source unit; fn names: Create, CARD | S | S | 8017C18C-8017C4DC | = | 2 |  |  |  |  |  |  |  | CARDCreate.c |
| 371 | `CARDRead.c` | SDK source unit; fn names: __CARDSeek, Read, CARD | S | S | 8017C4DC-8017C908 | = | 3 |  |  |  |  |  |  |  | CARDRead.c |
| 372 | `CARDWrite.c` | SDK source unit; fn names: CARD, Write, Erase | S | S | 8017C908-8017CC84 | = | 4 |  |  |  |  |  |  |  | CARDWrite.c |
| 373 | `CARDDelete.c` | SDK source unit; fn names: CARD, Delete | S | S | 8017CC84-8017CF64 | = | 3 |  |  |  |  |  |  |  | CARDDelete.c |
| 374 | `CARDStat.c` | SDK source unit; fn names: CARD, Update | S | S | 8017CF64-8017D42C | = | 4 |  |  |  |  |  |  |  | CARDStat.c |
| 375 | `CARDRename.c` | SDK source unit; fn names: CARD | S | S | 8017D42C-8017D628 | = | 1 |  |  |  |  |  |  |  | CARDRename.c |
| 376 | `CARDStatEx.c` | SDK source unit; fn names: __CARDSetStatusE, CARD, __CARDGetStatusE | S | S | 8017D628-8017DAD4 | = | 5 |  |  |  |  |  |  |  | CARDStatEx.c |
| 377 | `(unnamed)` | fn names: __CARDEnableGlob | - | S | 8017DAD4-8017DB18 | = | 1 |  |  |  |  |  |  |  |  |

