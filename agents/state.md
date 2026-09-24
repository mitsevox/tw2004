# State (keep this short: current facts only; history goes to docs/journal.md)

Updated 2026-09-24 ~17:10 CDT. RUNNING (cloud, to 18:00 CDT, no permuter):
- swing-01 round 2: GoStaticCam, UObject, uiText, TerrainData, GoShaderObject_Rain_Gc
- cam-01 round 2: GameMode26, LLFileIO_Gc, startUp, GoDynObj
Round 1 merged (notes: agents/notes/cloud-2026-09-24-round1.txt, money-01-notes_cloud1.txt): linked
uiLoadFile, gbacable, target (the owner's PC), ShaderObjectsData; exact BreakLine_Reset,
Stream_ParseBufs, Glows fn_800981D0; closer Skin (1 instruction), hwsRender, AI_ChooseTarget, PictInt.

Held by the owner's PC (closer lanes on local/<name> branches, wins handed over, never pushed to main):
to ~17:15 CDT hlaudmovie, streammanagerhole, uiProcessInterface; to ~17:45 LLDisp_Gc, LogoTexture,
GameMode22, GameHoleContests, GoShaderObjectCommon_MorphAnimManager_Gc, UISApi, UISScreen, GoTerrain.
Branch local/target-link is merged; the session cannot delete remote branches (owner deletes it).

## Numbers (report.json)

| exact functions | matched code | code linked | data linked | game units linked |
|---|---|---|---|---|
| 7,484 / 7,647 | 91.20% | 62.84% | 65.70% | 183 / 247 |

Left: 163 non-exact functions (133 KB) in 64 game units; 30 units are 1 function from linking, 17
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
