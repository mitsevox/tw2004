# State (keep this short: current facts only; history goes to docs/journal.md)

Updated 2026-09-25 ~07:40 CDT. QUIET: no cloud lanes running, everything merged to main,
`main.dol: OK`. Every unit is free.

**Tonight (owner's PC): the lever sweeper.** `ninja build/GW4E69/report.json`, then
`python tools/match/leversweep.py --from-report --min 95 -j 20` (80 functions, ~8 h; docs/workflow.md
"The lever sweeper"). Results in build/leversweep/summary.tsv + <fn>.txt: candidates only; a lane
applies each hit, confirms it in the real build and checks review levers and fake-match labels.
Priority targets (hand + permuter exhausted, ~99.5%+): ska_shared fn_8001FCF4, UStream_Update,
BreakLine_Render, StaticCam_GetFlybyInformation, UObject fn_800488B4, uiText fn_800922A8,
TerrainData fn_8000C278, Rain fn_800B52D4, hlaudmovie fn_800A8AD4, File_ReadAsyncEx, Earnings
fn_800D477C/fn_800D4F14, gocamscripts CameraScript_LagAimMarker/fn_8003F2E0.

**Running (2026-09-25 afternoon):** local Claude on the PC: permuter on Golfer AI_ChooseTarget
(Golfer held). Gemini (`gemini/round2`) and ChatGPT (`codex/round2`) on the owner's Mac, disjoint unit
lists in agents/assign/2026-09-25-gemini-chatgpt.md (those units held). The orchestrator merges.

**2026-09-25 day (cloud):** LLFileIO_Gc and UStream LINKED (45d089d + next). nm-c merged (f55c2a4:
rcmp_mad_codec 15/17). Golfer AI_ChooseTarget research done, not solved (agents/tried/AI_ChooseTarget.md:
declaration orders exhausted, 2 h permuter float-only, compilers ruled out). NEXT for it: a long
permuter run on the owner's PC: `python tools/match/permute.py Golfer AI_ChooseTarget --minutes 480
-j 18 --max-jobs 18` (after the sweep; not while it reads src/). Biggest blockers by link gain per
function: Golfer AI_ChooseTarget, Ball Physics_HandleCollision, Earnings x2, startUp fn_800B0748,
gocamscripts x2, skalib (52 KB data, 5 left), rcmp fn_800B769C/fn_800B8618. hwsBurn is 26/27
(fn_8011172C 1 instruction off). The tried-ledger agents/tried/<fn>.md is the record of all
attempts (brief + roles/matching.md point there). merge.py has an asm gate (asmgate.py).

gemini/hand-matches (2026-09-25): only its two C fixes are merged (c576b92: File_ReadAsyncEx,
UStream_Update exact). The other 13 "matches" (Earnings x2, Golfer AI_ChooseTarget, TerrainData,
hlaudmovie, ska_shared, PsMgr, GoStaticCam, BreakLine_Render, Rain, uiText, MC, GameModeBestBall,
startUp) paste the original assembly under #ifdef __MWERKS__ with the old C in #else: the match
proves nothing about the C, so they are NOT merged (also: extern in .c, data redefined, trial.py's
newline='\n' removed).

What was tried per function: agents/notes/ (cloud-2026-09-24-round1.txt, money-01-notes_cloud1.txt,
the PC's map-*-notes_w9/w10 files). New rules: docs/decomp-notes.md "New from the first cloud lanes".

Follow-ups (audit, not matching lanes): Swing.c's and goballfx.c's file headers still describe code
moved to StateGolfer.c/stateFunc.c/Code8005D2E4.c and GoLightFogEnv.c; DynChain.c/Replay.c prototype
comments say "Ball.c" for the Wind functions; charstate.h/char.c say "Skin.c" for the three functions
moved to Code80037AB8.c. The bone-name strings 0x801871D0-0x801873F0 (.sdata 0x80280E78-0x80281070)
belong to skalib or char, not Skeleton: attach them when that unit links. UISApi/UIStudio under
`-inline auto,deferred` give byte-identical code (kept as they are; reversed versions on
agent/swing-01 db03b3f). Split leads not taken (no boundary evidence): Golfer's club part, Glows;
DiscError needs fn_800B6FCC. Orphan data still parked: Code8009AA28's raw blobs lbl_8018A028
(0x4B0) / lbl_8018A4D8 (0x21F0), probably textures: the owner decides (no art in git; options:
leave unlinked, or link them from main.dol at build time); GoRenderCtx_Gc lbl_80186AF0 (232 B of
button masks, 8-aligned in the original: no type found that aligns it without a fake); .sdata
lbl_80281100 -> .bss lbl_801D4F68 (the screen-copy record) sit between Skin/Code80037AB8 and
GoDynamicCam/GoPostFx in link order, used only by GxUtil and gomainloop: owner unsettled.
Remote branches local/* and reference-build-debug-inventories are
merged: the owner deletes them (the session cannot).

## Numbers (report.json)

| exact functions | matched code | code linked | data linked | game units linked |
|---|---|---|---|---|
| 7,528 / 7,647 | 93.19% | 74.16% | 78.28% | 211 / 259 |

2026-09-24 (the first cloud day, with the owner's PC): from 7,484 / 91.20% / 62.84% / 65.70%.
Left: 139 non-exact functions (110 KB) in 54 game units; 29 units are 1 function from linking, 9
are 2. `python tools/agents/remain.py` lists them; rank by code-bar gain per function.

Audit: function names and comments 100% (tag `audit-baseline-1`). `auditbaseline.py` now: audited
6,356 / draft 171 / changed 83 (all explained: 64 = call sites of the vector-scale parameter fix,
the rest = matching notes added to draft-audited functions) / new 4 files (the SitDev split) /
headers 69 (phase 3, not started).

## First tasks in the cloud (in order)

1. DONE 2026-09-24, cloud box = 4 cores, 15 GB: setup OK; full build from clean 27 s (441 steps,
   `main.dol: OK`); trial.py 0.4 s per variant (Earnings); permuter `-j 2` 3,446 iterations in 5
   min (fn_800D477C). permute.py fixed on Linux (it left its pool workers running and hid the
   iteration count). Lane budget here: CPU, not builds, is the limit: 2 lanes with at most one
   permuter each, 3 if only one permutes at a time.
2. DONE 2026-09-24: tools/match is Linux-clean. Everything platform-specific is in
   `tools/match/hosttools.py` (one Windows block, one Linux block); no tool tests os.name itself.
   Checked on Linux against main: typeaudit `0 0 80 11` (layout 11), symaudit `1 0 18`, lint 141
   (136 long-line + 5 proto-mismatch, all text checks), declcheck finds a planted clash, perm_setup
   output byte-identical to before. Not yet run on Windows: the first local session should run
   `typeaudit.py --count`, `symaudit.py --count` and `lint.py` and compare with these numbers.
3. **Docs staleness audit (phase 2 of the move; the owner approved the plan):** every doc section
   gets a verdict (current / stale-fix / dead-remove / history-move), checked against the code and
   tools; the verdict table goes to the owner BEFORE anything is removed. Then: fold
   decomp-notes' dated "New from ..." sections into its topic sections, dedupe across files, prune
   `agents/notes/` to the functions still not exact (one `agents/tried/<Unit>.md` each).
4. Matching resumes (agents/roles/matching.md), code-bar order: GoGolfCam (2 fns, 99.4/99.5),
   gocamscripts (2), Swing (2 left, one at 99.8), Earnings (2, both 99.8), Ball (1, 98.7),
   Golfer (2), GoTerrainCollision (3), then the 1-function units.

## Parked (need the owner or a later phase)

- Housekeeping (2026-09-25, the PC is going away): the local dashboard is retired (GitHub Pages
  replaces it, its scheduled task disabled); remove tools/dashboard/install_hook.py and the PC's
  post-commit refresh_history hook in a cleanup (refresh_history.py itself stays: backfill and
  pages.py seed use it). Owner: delete the merged remote branch gemini/round2 (and future merged
  agent/codex/gemini branches); clean ~30 old agent worktrees in C:/dev/tw2004-agents once each is
  verified merged; C:/dev/scratch is archive-only. Heavy jobs now go to the PC runner via
  .github/workflows/pc-job.yml (docs/workflow.md "Sending a job to the PC").

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
