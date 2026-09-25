# State (keep this short: current facts only; history goes to docs/journal.md)

Updated 2026-09-25 ~11:30 CDT. How the machines, CI, the page and PC jobs fit together:
docs/infrastructure.md. History: docs/journal.md.

**Running:** ChatGPT (`codex/round2`, owner's Mac) per agents/assign/2026-09-25-gemini-chatgpt.md
(UPDATE section: uiProcessInterface fn_8008F820, SunFlr fn_8009A708, Swing, then the rest; those units
held); Gemini is out of usage. Local Claude on the PC: permuter on Golfer AI_ChooseTarget (Golfer
held). The orchestrator merges, cleans branches, sends PC jobs.

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
| 7,528 / 7,647 | 93.19% | 74.16% | 78.28% | 211 / 259 |

`python tools/agents/remain.py` lists what is left by unit; rank by code-bar gain per function.

Audit: function names and comments 100% (tag `audit-baseline-1`). `auditbaseline.py` now: audited
6,356 / draft 171 / changed 83 (all explained: 64 = call sites of the vector-scale parameter fix,
the rest = matching notes added to draft-audited functions) / new 4 files (the SitDev split) /
headers 69 (phase 3, not started).

## Parked (need the owner or a later phase)

- Housekeeping: remove tools/dashboard/install_hook.py and the PC's post-commit refresh_history
  hook (the local dashboard is retired; refresh_history.py stays, backfill and pages.py use it). Owner:
  ~30 old agent worktrees in C:/dev/tw2004-agents (delete each once verified merged); C:/dev/scratch
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
