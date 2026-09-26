# State (keep this short: current facts only; history goes to docs/journal.md)

Updated 2026-09-25 ~21:30 CDT. How the machines, CI, the page and PC jobs fit together:
docs/infrastructure.md. History: docs/journal.md.

**Running (round 6, launched 2026-09-26 11:40 CDT, checkpoint 14:00 CDT):** r6-uis (UISScreen's
shared copy pattern, UIS parameter order, UIStudio/UISEvent/UISApi, uiProcessInterface, uiArc), r6-args
(scheduling misses + the parameter-order lever: StaticCam, BreakLine, Rain, hlaudtrackstm, startUp,
SunFlr, rcmp, CamSpline, gocamscripts, GoGreenGrid, DepthField, PictInt, MC), r6-assert (the dead-assert
lane: Grass Static_Init, hwsBurn, Golfer, char, SkinPart, Earnings, Swing, skalib, Session, BestBall,
GameMode22; finished 12:04, merged: hwsBurn linked, dead asserts no effect), r6-misc (Particle, LLFont, LLTex,
goballfx, LLDynTex, LogoTexture), r6-big (Earnings, char, Swing). Weekly usage 19% at launch.
UIS = EA Tiburon's IStudio library (TW2005 paths: Code/Tiburon/IStudio/): r6-uis redirected to a
library-wide flag set + macro-style temporaries; PC job libflags (tools/match/libflags.py) on the four
UIS units, 240 min; a research agent looks for Tiburon builds with DWARF/STABS/maps. First cloud run:
one set for all four (-O4,p -inline auto,deferred -pragma "pool_data on" -str readonly) keeps all 33 exact.
Round 5 (5 lanes, mwcc-debugger on all 80 non-exact functions, 09:35-11:00
CDT) merged 2026-09-26 ~11:10 CDT: uiText, GoTerrain, HLAudMaster + hlaudmovie (split) linked; exact
Particle fn_800951A0, Grass Static_Render, LLTex fn_8000EA1C, SW_vImpact, hlaudmovie fn_800A8AD4
(labelled fake), uiText fn_800922A8, GoTerrain x2. Every lane wrote its debugger readings into the
ledgers; the levers are in docs/decomp-notes.md "New from round 5". Gemini round 4 (findings only)
merged. Worktrees r5-* can be reused. Follow-up leads: parameter order vs argument schedule (ledgers
that say "argument order"); UISScreen's copy-propagation pattern (unsolved); src/FE_CrAPDB.c:139's
comment still says uistudio.h has `const s32*` (now non-const): audit fix.
Rule change (owner, 2026-09-26): EA's code exactly as EA wrote it; portability is not a constraint
(CLAUDE.md, agents/brief.md). Leads to revisit with it: file-loading / fixup / address-math code.
Unit count: 259 (the weather split replaced two sweep units: 258; the HLAudMaster split added one).

**Next (owner-approved 2026-09-26, after a context compaction):**
1. **mwcc-debugger** (recommended by #match-help's Mrkol: "regswaps are solvable, use
   mwcc-debugger"): installed 2026-09-26 in the cloud (`tools/match/mwccdbg.py`, docs/workflow.md).
   Next: point it at the register-only near misses (24 functions at 99%+). First read: hwsBurn
   fn_8011172C (EA's `add r3,r6,r3` form gives pBurn 29 neighbours: find the form with one temp fewer).
2. **Dead-assert lane** (1-2 lanes to start): EA's compiled-away asserts/debug code still change
   register choice (a variable used in a dead `if (!p) { if (DEBUG) {...} }` counts as used more)
   and stack size (dead buffers). TW07's PS3 debug build keeps EA's asserts: map where EA asserted
   in our stuck functions and try the dead-macro form there. First: register-order functions, and
   the two stack-size misses (UIStudio fn_80166098 frame 0x70 vs our 0x60; Grass Static_Init 0x100
   vs our 0x110). A dead assert is EA's form (rule 1), not a fake.
3. Flag audit and fake-match inventory: after 100% (CLAUDE.md), mark only.
#match-help notes (2026-09-26): per-file flags are unrealistic (per-library is); a per-function
pragma is a fake match; "fake" = anything a SWE would be unlikely to write; dead-stripped
functions shift helper order (our StrippedFn stand-ins); `static const` debug flags / dead asserts
affect codegen (NFSMW, Mario Party, Prime).

**Parked for the owner:**
- EA names for rcmp_mad_codec (14 functions, 5 globals) and ska_shared (SKAUtil_EulerAnglesToQTs8,
  SKA_LoadFromMem) have two sources each (agents/findings/2026-09-25-mad-names.tsv,
  docs/reference-builds/007eon-ps2) but are NOT applied: the audit gate refuses any rename after
  the baseline. Apply them after 100% (or when the owner approves a rename path for the gate).
- OBFData split: GoShaderObjectContainer_OBFData_Gc.c looks like two EA files (a weather file
  0x8006F608..0x8006FCDC with the course table lbl_80188900, then OBFData proper): the string sits
  after 4 zero bytes at 0x80188DF0. The table's initialiser is in the r2-render lane's scratch.
- Audit: UIStudio fn_80166098 case 0x79's comment ("a text's buffer size, as a float") is wrong
  since the match: EA stores the int word.

**New verified levers (2026-09-25 evening, for docs/decomp-notes.md):** a parameter copied into a
local through `void*` (`T* p = (T*)(void*)pArg;`) reproduces EA's `mr r0,r3 ... mr rN,r0`
(char x3); index loops (`p[i]`) instead of pointer walks give EA's strength-reduced registers
(LLFont, LLDynTex); giving each job its own local (SkinPart, char, UISApi); reusing multiply-assigned
locals (TerrainData, Ball); random declaration orders + a climb find gains greedy climbs miss
(Particle fn_80094B84); `#pragma opt_lifetimes off` reproduces Ball Physics_HandleCollision's
float registers (lead, not kept).

**Biggest blockers** by link gain per function: Golfer AI_ChooseTarget (98.53%, registers only:
agents/tried/AI_ChooseTarget.md), Ball Physics_HandleCollision, Earnings x2, startUp fn_800B0748,
gocamscripts x2, skalib (52 KB data, 5 left), rcmp fn_800B769C/fn_800B8618. One function from
linking: SunFlr_Gc (fn_8009A708), uiProcessInterface (fn_8008F820), hwsBurn (fn_8011172C, 1
instruction). Every attempt is in agents/tried/<fn>.md; new rules in docs/decomp-notes.md.

Follow-ups (audit, not matching lanes): Swing.c's and goballfx.c's file headers still describe code
moved to StateGolfer.c/stateFunc.c/Code8005D2E4.c and GoLightFogEnv.c; DynChain.c/Replay.c prototype
comments say "Ball.c" for the Wind functions; charstate.h/char.c say "Skin.c" for the three functions
moved to Code80037AB8.c. The bone-name strings 0x801871D0-0x801873F0 (.sdata 0x80280E78-0x80281070)
belong to skalib or char, not Skeleton: attach them when that unit links. UISApi/UIStudio under
`-inline auto,deferred` give byte-identical code (kept as they are; the reversed
versions were byte-identical, their branch is gone). Split leads not taken (no boundary evidence): Golfer's club part, Glows;
DiscError needs fn_800B6FCC. Orphan data still parked: Code8009AA28's raw blobs lbl_8018A028
(0x4B0) / lbl_8018A4D8 (0x21F0), probably textures: the owner decides (no art in git; options:
leave unlinked, or link them from main.dol at build time); GoRenderCtx_Gc lbl_80186AF0 (232 B of
button masks, 8-aligned in the original: no type found that aligns it without a fake); .sdata
lbl_80281100 -> .bss lbl_801D4F68 (the screen-copy record) sit between Skin/Code80037AB8 and
GoDynamicCam/GoPostFx in link order, used only by GxUtil and gomainloop: owner unsettled.

## Numbers (report.json)

| exact functions | matched code | code linked | data linked | game units linked |
|---|---|---|---|---|
| 7,575 / 7,647 | 95.49% | 80.42% | 80.21% | 223 / 259 |

`python tools/agents/remain.py` lists what is left by unit; rank by code-bar gain per function.

Audit: function names and comments 100% (tag `audit-baseline-1`). `auditbaseline.py` now: audited
6,356 / draft 171 / changed 83 (all explained: 64 = call sites of the vector-scale parameter fix,
the rest = matching notes added to draft-audited functions) / new 4 files (the SitDev split) /
headers 69 (phase 3, not started).

## Parked (need the owner or a later phase)

- Housekeeping: the PC's post-commit refresh_history hook can go (the local dashboard is retired;
  refresh_history.py stays, backfill and pages.py use it). Owner: ~30 old agent worktrees in C:/dev/tw2004-agents (delete each once verified merged); C:/dev/scratch
  is archive-only. The public page's history before 2026-09-24 11:49 (the repository's rewrite) is
  only in the PC's build/dashboard_history.json: `pages.py merge` adds it (docs/infrastructure.md).
- Docs staleness audit (owner-approved plan): a verdict per doc section (current / fix / remove /
  history), shown to the owner before removing anything; fold decomp-notes' dated "New from ..."
  sections into its topics.

- Orphan data left for linked owners (~14 KB): GoGrass (0xA0 slot vs 0x90 type), MC_Gc small
  globals (padding), GoRenderCtx_Gc table (8-align), Skeleton strings, Code8009AA28 10 KB raw blobs
  (needs a data-file convention), sun-flare 7 KB (owner unclear). The rest (~276 KB) waits on its
  owners linking: streammanagerhole 87 KB, skalib 51 KB, Golfer ~53 KB, Earnings 11 KB.
- PsBallFx: 8.3 KB of `.data` tables at 0x8018C868-0x8018E978 still unowned (values needed).
- SitDev split: fn_800BCB88..fn_800BCD5C left in SitDevStateVector without data proof; tables at
  0x801910F8-0x80191230 unowned.
- `src/unsorted/` (31 tiny units, 2.2 KB, all linked): place them in their home files with TW07 file
  order + neighbour references; sweep_80155F40 (critical regions) is C library code -> sdk category;
  sweep_800055D8 is `main()`.
- Misfiled units found by the audit (agents/findings/audit-reports): rcmp_mad_codec, startUp,
  OBFData (weather), particles (= OS heap + LLTime), goballfx (= GoLightFogEnv), hlaudmovie, Glows
  (2 files), Quaternion (= MathQuat.c), TibExt tail (= SharedFileIOCallbacks.c), Code8009A928
  (= GlowMgr.c), Code8009AA28 (= SunFlr.c), Ball (= Physics.c + Wind.c).
- Audit follow-ups: ~300 TW07 names only lane 2 matched (ledger notes) need a lane-1 E2b pass
  (GoDynamicCam ~37, Swing ~35, MC/hlaudtrackseq ~70, FE_CrAPDB ~52, GoGolfCam ~62, ...).
- Docs: renames not yet reflected in docs/ (e.g. game-data.md FE_GolferAttributes).

## After 100% match (the owner's order)

See `../CLAUDE.md` "After 100% match": partition by audit status, phase 3 headers, **full cleanup of
docs/gameplay.md** (unaudited behaviour claims), port-hazards doc, misfiled units.
