# Name pairing survey: how many of EA's names the reference builds give back (2026-09-27, lane n1)

A survey, not a rename: nothing in `src/`, `include/`, `symbols.txt` or `name_sources.tsv` changed.
Tool: `tools/match/pairnames.py` (reusable; its docstring has the full method and every code).
Proposals: `2026-09-27-name-pairing.tsv` (one row per proposal, columns below).

## The counts

Functions (game code, from the split objects of the current build; runtime/MSL units are SDK code
and have no `fn_` left):

| | functions |
|---|---:|
| game functions in the objects | 6,610 |
| still `fn_...` (incl. tier-3 `fn_ADDR_Guess` names: 46 in the scored units) | 5,275 |
| of them in lane n2's units (not scored, below) | 3 |
| **scored** | **5,272** |
| A: T1 candidate (position + signature + size, most with a run or an independent line) | **197** |
| B: T2 candidate (position + agreeing signature + a second line) | **375** |
| C: provisional (the alignment's best guess, weak) | 1,345 |
| none (no reference function fits) | 3,355 |

So **572 confident (A+B) EA names**, 10.8% of the scored `fn_` functions, plus 1,345 provisional.
All but 21 come from TW07 (PS3 DWARF); 21 A/B come from TW06 PS2's EA_DASH copies of EA's shared
packages (SharedFileIO, llSharedFileIO, TagFile). Where the 5,272 sit:

- 2,940 in units that have a reference file of their own (the unit's file name, a `<file>.c`
  string in it, or anchors): 572 A/B, 725 C, 1,643 none.
- 1,575 in units whose reference file is only guessed from paired callers/callees (capped at C
  unless bracketed): 620 C, 955 none.
- 757 in units with no reference file at all (GameCube-only renderer/driver code: `*_Gc`,
  `hws*`, `LLDisp_Gc`, `LLPict_Gc`, `GoShaderObject_*`, `gbacable`, `DynamicRenderingBuffer`, ...).

Globals (`lbl_` in .data/.sdata/.bss/.sbss, strings left out): **1,276**, 1,128 used by game code.
**0 confident names from the inventories in the repo**: no TW07 or TW06 global list for game code is
exported (the TW07 inventory is functions only; the TW06 Xbox PDB export is types and modules), and
the TW06 PS2 globals of the shared packages are already named or differ (`gCRC` vs TW06's
`_ChecksumCRC32_State`). 311 of the 1,128 are used by a function that has a reference name (anchor or
A/B pair): those would become nameable with a TW07 globals export (below).

Struct fields: **1,477** placeholder-named fields with a known offset in top-level structs of
`include/` (prefix + hex offset, `nC38`, `f2C`; 2,281 placeholder-named members counting nested
ones; 480 `unk*` padding not counted). Only 7 of our structs pair with a reference layout in the repo
(TW06 Xbox key types, TW06 PS2 EA_DASH types); they hold 31 placeholders; **1 A, 0 B, 3 C**:
`PgaStatCounts.n44` = TW06 `GM_Pga_StatCounts.monthlyWinnings` (A, anchors on both sides agree on a
+4 shift); `Tournament.nC`/`n10` = `scenario`/`isEuro` (C, one-sided); `Ball.n6C` = `lie` (C: the
header already says TW06 has five lie fields there and this game three). The key types were already
mined by hand; what is left is where EA reordered or resized (Player vs GamePlayer: "only blocks
are matched").

Lane n2's units, not scored: UIS*, UIStudio, rcmp_mad_codec, runtime/MSL. n2's names reached main
while this ran (77 `fn_` there this morning, 3 left in rcmp_mad_codec). TibExt (3 `fn_`, Tiburon's extension) is scored: 2 A, both from TW07's
SharedFileIOCallbacks.c (`SFIO_vSetAttrCallback`, `SFIO_vFlushCallback`).

## How right are they

1. **Hold-out test** (`--holdout 0.5`, three seeds): half of the 826 functions already named after a
   reference name are hidden (renamed `fn_`), the pairing re-run, and the hidden name compared:

   | seed | A right / wrong | B right / wrong | C right / wrong |
   |---|---|---|---|
   | 1 | 167 / 1 (99.4%) | 92 / 8 (92.0%) | 46 / 28 (62.2%) |
   | 2 | 177 / 2 (98.9%) | 93 / 9 (91.2%) | 47 / 28 (62.7%) |
   | 3 | 175 / 0 (100%) | 109 / 4 (96.5%) | 51 / 29 (63.8%) |

   Optimistic: those functions sit in units that keep the other half of their anchors.
2. **Hand check, 30 random A/B pairs of the final run** (seed 23; both sides read: our C, TW07's
   signature, locals and inlines): **27 agree with the code** (parameters, locals and body; a few
   are short accessors where agreement is all one can say), e.g.
   `CalcScoreRankingsForCutEntrants`, `Seq_Step(pperf, step, debounce)`, `OnTrackSetStream`,
   `EVENT_PredictedGameBreakerStarted` calling `Gaud_InitGameBreaker(nPlayer, 1)` = TW07's
   `predicted`), **1 consistent but thin** (`Character_FreeTXRInfo`: clears 4 buffers, frees nothing),
   **1 wrong** (`fn_8010A788` -> `DynTex_GetTextureData`: ours is a 3x3 colour transform; B from a
   run of 4 through TW07's C++ `DynTex::` methods), **1 thin** (`EVENT_PracticeSwing`, a two-line
   handler inside a run of 14). **Error rate 1/30 wrong (3%), 2/30 (7%) counting the doubtful one.**
   All 12 A pairs in the sample were right.
   Two earlier samples on earlier rules (60 pairs) found 3 wrong and 4 doubtful; each wrong one
   produced a rule now in the tool: B needs a second line (`weak`), inline-only TW07 functions
   are B only inside a run, names of systems TW2004 lacks are C (`era`: `GO_vRenderApt`,
   `GO_vUpdateApt` sat in the right place but name TW07's Apt Flash UI), and the TW06 PS2 parser
   had dropped functions that have locals (it shifted `_SFIOProcessOpen` by one).
3. **Against the blind audit**: 11 of our `fn_ADDR_Guess` tier-3 functions got an A/B proposal; all
   11 agree with the audit's behaviour guess (`fn_8016EF90_ContinueSeekReadWrite` ->
   `_SFIOProcessSeekReadWrite`, `fn_8005BA94_MishitAngle` -> `SW_vCalculateMishitAngle`,
   `fn_80174DF0_Delete` -> `TagFileDeleteStart`, `fn_80171294_GetChecksumInterface` ->
   `SFIOGetSignatureMethod`, ...).

Known failure modes (why C is only ~60%): neighbours with the same signature in a wide gap
(`Stream_StreamLoadFixedSize` / `Stream_LocalLoadFixedSize`, `Ter_GetSupportingGroundNormal` /
`...WorldNormal`); TW07 C++ classes whose 2003 C ancestor had a different function set (`DynTex::`,
`GameModeDriver.cpp`); functions TW07 inlined everywhere; units whose file is only a neighbour
guess.

## Coverage per unit (units with 4+ A/B; all rows in the TSV)

| unit | fn_ | A | B | C | none | reference file(s) |
|---|---:|---:|---:|---:|---:|---|
| event | 90 | 36 | 28 | 17 | 9 | event.c event.h event.h.2 |
| GoDynamicCam | 53 | 14 | 22 | 12 | 5 | GoDynamicCam.c GoDynamicCam.h |
| GameAudio | 105 | 7 | 28 | 46 | 24 | GameAudio.c GameAudio.h (4 headers) |
| FE_CrAPDB | 87 | 2 | 32 | 34 | 19 | FE_CrAPDB.c FE_CrAPDB.h |
| FEgolferanim | 92 | 5 | 22 | 40 | 25 | FEgolferanim.c FEgolferanim.h FEgolferanim.h.2 |
| char | 144 | 2 | 21 | 56 | 65 | char.c char.h char_tex_manager.c |
| gomainloop | 54 | 6 | 17 | 12 | 19 | GoMainLoop.c |
| hlaudtrackseq | 31 | 6 | 14 | 6 | 5 | HLAudTrackSeq.c HLAudTrackSeq.h |
| emotion | 20 | 18 | 0 | 0 | 2 | Emotion.c |
| GameEffects | 22 | 11 | 7 | 2 | 2 | GameEffects.c GameEffects.h GameEffects.h.2 |
| LLDynTex | 39 | 0 | 17 | 11 | 11 | LLDynTex.c lldyntex.h |
| PGATourSimulation | 35 | 8 | 9 | 14 | 4 | PGATourSimulation.c |
| Swing | 26 | 7 | 9 | 6 | 4 | Swing.c |
| GoCamCont | 23 | 5 | 10 | 4 | 4 | GoCamCont.c GoCamCont.h GoCamCont.h.2 |
| GoDynObj | 38 | 1 | 12 | 11 | 14 | GoDynObj.c |
| GoComicCam | 15 | 8 | 4 | 3 | 0 | GoComicCam.c |
| NGC/SharedFileIO/llSharedFileIO | 15 | 9 | 2 | 0 | 4 | TW06 PS2 llSharedFileIO.c |
| gocamscripts | 25 | 3 | 7 | 8 | 7 | GoCamScripts.c GoCamScripts.h |
| target | 20 | 4 | 6 | 2 | 8 | Target.c |
| SitDevMisc | 13 | 6 | 4 | 2 | 1 | SitDevMisc.c |
| MC | 28 | 2 | 7 | 13 | 6 | MC.c MC.h |
| Common/SharedFileIO/SharedFileIO | 11 | 9 | 0 | 0 | 2 | TW06 PS2 SharedFileIO.c |
| FE_Manager | 49 | 2 | 6 | 24 | 17 | FE_CrAPUtils.c FE_Manager.c FE_Movies.c |
| Earnings | 54 | 6 | 2 | 12 | 34 | Earnings.c |
| LLTexGrp | 21 | 1 | 6 | 9 | 5 | LLTexGrp.c |
| animblender | 25 | 1 | 6 | 8 | 10 | AnimBlender.c AnimBlender.h |
| UAudContainers | 11 | 0 | 7 | 4 | 0 | UAudContainers.c |
| GoGolfCam | 70 | 4 | 3 | 12 | 51 | GoGolfCam.c |
| Skeleton | 32 | 0 | 5 | 8 | 19 | Skeleton.c/.cpp/.h, skeleton_shared.c |
| GoTerrainCollision | 34 | 2 | 3 | 8 | 21 | GoTerrainCollision.c, ..._Headgate.c |
| Wind | 5 | 0 | 5 | 0 | 0 | Wind.c |
| hlaudemitter | 25 | 1 | 4 | 9 | 11 | HLAudEmitter.c |
| Ball | 31 | 1 | 3 | 11 | 16 | Physics.c |
| GameModeDriverPGATour | 34 | 3 | 1 | 13 | 17 | GameModeDriver_PGATour.cpp |
| FE_LogoDesign | 17 | 1 | 3 | 8 | 5 | FE_LogoDesign.c |

62 units get at least one A/B. The big units with none, and why:

- **FE_MessageTable** (463 `fn_`, the front end's message handlers registered by fn_80079EA8),
  **FE_CrAPMessages** (89), **GameUICommands** (227), **GameMessages** (55): TW07 rebuilt the UI on
  Apt (Flash); its `APT_*`/`FE_CrAPMessages.c` files have other functions (17 in
  FE_CrAPMessages.c against our 89). Almost nothing to pair.
- **startUp** (103): our file name is wrong for it (it is the AX/MIX audio voice driver); TW07's
  `startUp.c` has 2 functions. The tool does not trust a file-name match without an anchor.
- **GameModeN** units, **GameRound**, **GameModeDriver**, **HoleScore**, **EASportsBio**: EA turned
  the modes into C++ classes (`GameMode_*.cpp`) with other function sets; no anchors to hold an
  alignment. Mostly C.
- **EASB / EASBStorage** (135): not in TW07 (EA Sports Bio); the TW06 PS2 dashboard has only the C++
  SharedLogin wrapper.
- **GameCube-only code** (`*_Gc`, `hws*`, `LLDisp_Gc`, `LLPict_Gc`, `GoShaderObject_*`, `gbacable`,
  `DynamicRenderingBuffer`, `GoGrass`, `Code8001*`): PS3/PS2 have different renderers and drivers.
- **streammanagerhole** (60): TW07 has a 5-method C++ `StreamManagerHole` class.

## Method (short; the tool's docstring is the reference)

Per unit: functions already carrying a reference name (826 "anchors") fix the unit's reference
file(s) and positions (CodeWarrior emits a file's functions in source order; TW07's `cu/` lists are
in source order). Every other function is scored against the free reference functions of those
files (parameter count and kinds with byte/halfword widths, return kind, size ratio, callees the
reference inlines, paired callers whose reference inlines it, EA strings naming it, agreement with
`tw06-names.md` and TW07 `pairs.tsv`), then a monotone alignment inside each gap between anchors
picks the pairs; the margin is what the gap's best alignment loses without the pair. Runs of
consecutive pairs with agreeing signatures count as position evidence. A/B pairs become anchors for
the next round (up to 6). Holding out anchors measures the precision (above).

Evidence codes in the TSV follow docs/style.md: `E2b` (TW07 name), `E2` (TW06 name), `E1` (EA's text),
then the machine lines: `sig`/`sigw`/`sig~`/`sig!`, `size`/`size~`/`size?`/`size!`/`inl-only`,
`inl<n>`, `nbr`, `p07h/m/l` (TW07 pairs.tsv agrees), `007`, `pos=1:1/both/one/none`, `coh`,
`run<n>`, `weak`, `dup`, `era`.

TSV columns: `address unit current_name proposed source ref_file ref_line ref_signature
our_signature codes confidence score margin gap round` (`gap` = ours:theirs in the aligned gap,
`round` = the round an A/B was accepted; `ref_line` is the TW07 source line, or the EA_DASH address
for TW06 PS2). `current_name` is `fn_` for every row, so the file is blind except the 32 rows
with a tier-3 `fn_ADDR_Guess` name.

Not used, and why: TW2003/TW2005 GameCube `functions.tsv` (address pairs only, no names: they can
carry a name to the other game, not find one); 007 (its SKA candidates are in `shared_functions.tsv`
and CLAUDE.md already; ska_shared is C here); Madden 2003 (lane n2).

## Recommendation for applying

Under the owner's naming policy of 2026-09-27 (agents/state.md: evidence first, then provisional
descriptive names, light verification, a name must never contradict the code):

1. **A (197): apply as EA names (T1, codes `E2b`/`E2` + the tool's codes), whole units per batch
   (20-40 functions), after one reader checks each name against its body** (the hand check's
   question: do the parameters, locals and body fit every word of the name?). Expect 1-3% rejects.
2. **B (375): the same, plus an independent reviewer sampling 1 in 5.** Expect 5-10% rejects.
   Check a unit's run together: an alignment error is usually an off-by-one that also moves its
   neighbours, so one rejected pair should send the rest of that gap back to C, and the unit
   re-run.
3. **C (1,345): not a name by itself.** It is evidence for the descriptive-naming pass: when the
   reader's own reading agrees with the TW07 candidate, take EA's name (it then becomes B-grade);
   otherwise name from the code. Re-run `pairnames.py` after each applied batch (new anchors narrow
   the gaps and raise some C to B; a rejected pair must not come back as an anchor).
4. Start with the units where the runs are long and the files match: event, GoDynamicCam, emotion,
   GameEffects, GoComicCam, hlaudtrackseq, SharedFileIO/llSharedFileIO, PGATourSimulation, Swing,
   GoCamCont (about 250 of the 572).

To get globals and fields (owner's PC, where the TW07 ELF is): export TW07's compilation-unit-level
variables (name, type, size, declaring file) and full struct member lists (name, type, order) with
the DWARF reader behind `tools/match/tw07dwarf.py`. Globals then pair through the 311 `lbl_` used by
already-paired functions (same file, same users, type/size); fields pair by member order and type
(PS3 offsets are 64-bit, so order and type, not offsets). With TW07's PS3 disassembly a global's
users could be read directly (TOC references), which would make that pairing much stronger.
