# State (keep this short: current facts only; history goes to docs/journal.md)

Updated 2026-09-25 ~21:30 CDT. How the machines, CI, the page and PC jobs fit together:
docs/infrastructure.md. History: docs/journal.md.

**Running:** no cloud lanes. Round 3 (worktrees `r3-*`, all merged) was cut short at ~22:35 CDT
when the disk filled: quicktrial.py leaked a temp folder per trial (fixed in 176f77a; their unwritten
attempts are in agents/tried). Gemini (owner's Mac) works `gemini/round4`: LLPictInt, LLDynTex,
LLTex, LLFont, MC, GameModeBestBall, Code8002EE1C, DepthField, GameMode22, CamSpline, uiArc.
PC: permuter on rcmp madinit, then startUp fn_800B0748. Golfer held (PC).
Unit count: the weather split (Code8006F608.c) replaced two unsorted sweep units, so game units
went 217/259 -> 216/258 with no unit lost.

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
| 7,556 / 7,647 | 94.16% | 75.14% | 78.55% | 216 / 258 |

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
