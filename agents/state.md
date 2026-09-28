# State (keep this short: current facts only; history goes to docs/journal.md)

Updated 2026-09-25 ~21:30 CDT. How the machines, CI, the page and PC jobs fit together:
docs/infrastructure.md. History: docs/journal.md.

**100% (2026-09-27 ~17:45 UTC, f568ee7):** 7,647/7,647 functions exact; matched code 100%; code linked
100%; data linked 100%; 453/453 units linked; `main.dol: OK`. The last function was UISStack.c's
fn_80166098 (EA's UISStackProcess). objdiff "matched data" reads 79.95% (a scoring artifact: see the
hygiene list below). Nothing is running. Next: the owner's after-100% list (bottom of this file).
Nintendo SDK data (AXComp/DSPCode) and art/audio blobs are linked via C generated from main.dol at
build time (tools/build/gendata.py), never committed.

**Naming phase (owner, 2026-09-27 evening): the one focus is names + comments, most-called first.**
Measure: call sites to named functions (`python tools/match/hotnames.py`): 42.94% -> 61.92% after
orchestrator batches 1-2 (76 names). Territory: `hotnames.py --units` (named / commented / done per
file). Pipeline: `tools/match/name.py` (all or nothing); playbook `agents/roles/naming.md`. Lanes:
one area each, 15-25 functions per batch, fresh agent per batch. Comments go in the same pass.
Cleanup done before it: 18 stale remote branches deleted (gemini/round4 and 2 young pc-results left),
matching-era records removed (in history at 6839245), docs/README.md index.
Owner 2026-09-28: the readability plan is APPROVED; continue it. The matched-data scoring lane is ON
HOLD (decomp.dev already shows 100%; objdiff's local "matched data" 79.95% is per-symbol scoring).
Progress log: config/GW4E69/readability_progress.tsv (`hotnames.py --units --log` after every round);
per-file work lists: `hotnames.py --unit X --todo`. CHECKPOINT (2026-09-28 ~23:30 UTC): round 17 landed and logged (d5ea131): 3606 functions reviewed = 54.3% (+315), 96 files through the pass, coverage 84.11%. The FRONT-END area is DONE (16 files; naming-leads "Round 17"). EA bugs labelled: lock kinds 20 / 24 (FE_Manager.c), MAD_ReadNextFile end test (Code800B90F4.c); register updated. OWNER QUESTION (parked): rename fe_movies.c -> uiProcessPolygon.c, Trax.c -> uiEATrax.c, uiobject.c -> uiObject.c (EA names proven), and fold LogoTexture.c (+ sweep_8010FF5C.c) into FE_LogoDesign.c; Code800B90F4.c split / rcmp_mad_codec.c naming needs more evidence. Nothing running. NEXT: round 18 = the audio area (per plan-readability order: HUD/front end -> audio -> cameras -> terrain -> saves -> library -> unsorted); pick files with hotnames --units, plus a header lane on naming-leads "Round 17" still-open list. Rules: copy round17_common.md. CHECKPOINT (2026-09-28 ~19:30 UTC): round 16 landed and logged (613c421): 3291 functions reviewed = 49.6% (+404), 80 files through the pass, coverage 82.10%. FE_MessageTable.c DONE (464/464, header rewritten). Four cross-lane name clashes settled on replay (naming-leads "Round 16"; fixed batch copies in /home/user/scratch/tw/r16/). rp1 header scripts replayed last; MC.c MemoryRequired functions now s32 (same bytes). EA bug register: 8 open items added, file names updated to the renames. Nothing running. NEXT: round 17 = the HUD / front-end area: FE_Manager (56), fe_movies (31), uiProcessInterface (31), UISScreen (24 left of review), uiLoadFile (23), fe_craputils (22), UISApi (21), FE_LogoDesign (17), uiTransform / UISEvent / uiText / uiobject / uiArc, gbacable (45), plus a header lane on naming-leads "Round 16" still-open list (the front-end state structs: fe.h FEState / FEProfile / FEScreen, golfer.h GameOptions; its globals lbl_801D7148 / lbl_80281ED4). Rules: copy round16_common.md. CHECKPOINT (2026-09-28 ~17:00 UTC): round 15 landed and logged: 2887 functions reviewed = 43.5% (+302), 79 files through the pass, coverage 81.98%. The round-flow area is DONE (PGATourSimulation, FourBall, Calendar, fe_stats; FE_CrAPMessages and FE_MessageTable's first 60 too). EA bugs: agents/findings/2026-09-28-ea-bug-register.md (owner: labels must not get lost; naming.md rule 6). DONE before round 16 (owner's order): stub message handlers on one pattern (1e6a36c; naming.md rule 5), 14 files renamed to EA's names (9826752), event.c / SitDev.c split (356313a); report.json unchanged (453/453 units, 7647/7647). Not renamed (no code proof): TW2005's PGATourMode.c is probably 2005's new Tour Mode (owner, 2026-09-28; consistent with TW2003's paths, which have CareerMode.c / tTournamentMode.c and no PGATourMode.c), so do NOT rename GameModeDriverPGATour.c after it; RealtimeMode.c may be GameModeDriverRTE.c (real-time events exist in 2004) but nothing in the code ties it; GameMessages.c maybe TW06 gameui_istudio.c. Other areas' misfiled units (Quaternion, Ball, Glows, TibExt, ...) wait for their areas. PRE-COMPACT CHECKPOINT (2026-09-28 ~17:40 UTC, usage 81%): tree clean, pushed. Round 16 plan (6 lanes, prefix rp): rp1 header lane (naming-leads "Round 15" still-open list); rp2..rp6 FE_MessageTable's 404 left, by address, ~81 each: 0x8007CBCC..0x8007E0BC, 0x8007E0D0..0x8008017C, 0x800801C0..0x8008266C, 0x80082680..0x8008392C, 0x80083930..0x800850AC (file header + globals already done by ro5). Lane rules: copy /home/user/scratch/tw/round15_common.md to round16_common.md (no git merge, rule 6 EA bugs, stub-name pattern in naming.md rule 5). Replay: /home/user/scratch/tw/replay_lane.sh <lane> <last name.py commit> <gfirst|glast> <globals|-> <batches>; then compare with the lane, take its lint fixes, header scripts LAST (PYTHONPATH with rm1/rn1/ro1 helper dirs). After the round: hotnames --units --log, leads, EA bug register, state, push. Round 16 LANDED (see the 19:30 checkpoint). NEXT: round 16: FE_MessageTable's other 404 functions (4-5 lanes by address) + a header lane (naming-leads "Round 15"). Owner decisions 2026-09-28 (naming-leads.md "Round 12"): misfiled files get EA's names in one batch after the round-flow area (list: naming-leads "Round 12" + "Round 13"). NEXT: round 14 (6 lanes): GameUICommands (227) split across 3 lanes by address, GameMode4 + GameModeReplay + GameMode2 remainders, FE_PGATourMessages + CalendarScreen, plus a header lane (naming-leads "Round 13" still-open list). Replay helper: /home/user/scratch/tw/replay_lane.sh <lane> <last name.py commit> <gfirst|glast> <globals.tsv|-> <batches...> (then read conflicts, compare with the lane, take the lane's lint fixes). Prompts state only sourced facts (agents/README.md). Hand edits: `tools/agents/merge_lane.sh <lane> [globals.tsv...]` with BASE=<the lane's last name.py commit>; merge_pick.py resolves conflicts per side. Round procedure (orchestrator): new_agent.py <lane>; prompt = plan-readability.md + brief.md +
roles/naming.md, files + `hotnames.py --unit X --todo` cap ~40, deliver batch files + hand-edit list,
never git reset; on return: replay batches with name.py in order (`--by "<lane> (replayed by
orchestrator)"`), rename.py for globals, then hand edits by `BASE=<lane's last name.py commit> tools/agents/merge_lane.sh <lane> [globals.tsv...]`
(3-way, names normalized; conflicts: read them, `tools/agents/merge_pick.py <conflict> <dest> <o|t...>`),
then `rename.py name_sources.tsv --refs-only` + wraplong + read `git diff HEAD`; the header lane's saved
scripts run LAST on main; build + lint, commit, push; after the round
`hotnames.py --units --log`, commit the log row, append leads to naming-leads.md, report numbers.
PHASE: readability. Plan and feedback loop: agents/plan-readability.md (one complete pass per file,
areas in order, measure with `hotnames.py --units`: named / commented / reviewed / done).
Matching-era audit rules and tooling retired 2026-09-27 (tag audit-baseline-1 kept as history).
Running: comment lanes cr1 (flagged list), cr2 GameRound + GameUI, cr3 FE_CrAPDB, cr4 audio (the
one-time catch-up for rounds 1-3, which skipped comment review). Nothing new launches until the
owner approves the plan.
Round 1 (nm1-nm4) and round 2 (nm5 FE_CrAPDB, nm6 SkinPart, nm7 GameRound + GameUI, nm8 audio)
landed: coverage 61.92% -> 69.35%. Nothing running. Lanes hand in batch files; the orchestrator
replays them on main with name.py (merging lane branches conflicts on callers in other files).
name_sources.tsv merges with merge=union. Next-batch EA names and the comments lanes think wrong:
agents/findings/2026-09-27-naming-leads.md (start round 3 from it).
Tool follow-ups: merge.py's asm gate should pass a renamed asm signature; prototype trailing
comments in headers drift out of alignment after renames (cosmetic).

**After 100% (2026-09-27 ~19:15 UTC, pre-compact checkpoint): nothing running.** Landed since 100%:
- n2: EA names applied with evidence (name_sources.tsv): 60 IStudio functions (Madden 2003 STABS) +
  their types/fields/params/locals/enums (uistudio.h), MAD decoder (14 fn, 6 globals), ska_shared x2,
  __float_max. 76 functions renamed vs the audit baseline.
- n1: tools/match/pairnames.py + agents/findings/2026-09-27-name-pairing.{md,tsv}: of 5,272 `fn_`,
  A 197 / B 375 (confident, ~3-7% error by hand check) / C 1,345 (evidence only) / none 3,355;
  globals and fields need a TW07 DWARF export of variables and struct members on the owner's PC.
  Recommended start: event, GoDynamicCam, emotion, GameEffects, GoComicCam, hlaudtrackseq,
  SharedFileIO, PGATourSimulation, Swing, GoCamCont.
- n3: src/README.md (the codebase map, 20 subsystems) + agents/findings/2026-09-27-codebase-health.md
  (reviewer-lens recon: grades, top 10, the feedback loop).
- **Merges now need `--allow-renames`** (renames since the baseline are on main; the flag checks each has
  a name_sources.tsv row). Refresh src/README.md names after each rename batch (rename.py skips .md).
- Queued, not started (owner: after the compact): the matched-data scoring lane (objdiff 79.95%;
  biggest: AXVPB/AXOut .bss unscorable, skalib/LLFileIO .bss sizes; a symbols.txt-vs-object size
  script for the tail). Next: plan the feedback loop with the owner.

**Earlier (2026-09-27 ~13:00 UTC):** 7,641/7,647 exact. IStudio hold LIFTED ~14:00 UTC (owner): Madden evidence in (docs/reference-builds/madden2003-ps2); lanes b12 fn_80166098,
b7 fn_80168918, b5 fn_80165670, b4 fn_80169DC4 rewrite toward EA's locals; Gemini works its own branch (compare at the end). Was: hold until the owner's research and the Madden NFL 2003 PS2 lead are in (Madden 2003 prototype, EA Tiburon
2002, has IStudio with .mdebug/STABS: UIStudio.c, UISEvent.c; being extracted to
docs/reference-builds/madden2003-ps2/, binary in /home/user/refs, off git). Open: IStudio x4 (held),
LLFont x2 (b11 lane; Codex fn_8001144C). /dev/null was deleted by a lane again (b11, 09:13); restored
by the orchestrator with the owner's OK.

**Running (BATTLE PLAN, 2026-09-27 02:17 UTC, agents/assign/2026-09-27-battle-plan.md):** one lane = one
function, no permuter, refill from the queue. b1 char MtaLib_SwapAndLink, b2 uiProcessInterface UI_ReadControllers,
b3 hlaudtrackstm Stm_Tick, b4 UISApi fn_80169D90, b5 UISApi fn_80168CD8, b6 UISScreen fn_8016C6C4,
b7 UISScreen fn_8016B188, b8 UISEvent fn_80165ACC. Codex: UIStudio fn_80166098. Gemini: out of usage.
Merged just before: Codex SkinPart SkinPart_ApplySetsToMaterialEntry (fake, SkinPart linked) + skalib AnimLib_SetLeafClipsByName (EA form):
7,600/7,647 exact, 96.86% matched, 88.35% code / 85.76% data linked.
Night: GoGreenGrid linked; Golfer.c split into its 4 original files (ai_brain.c,
Code8002BBB0.c, Code8002C984.c, Golfer.c = the Luck part), all linked; gPlayers/gCurGolferRecord/gGolferTable
defined in Code8002DB80.c, gSession/gszEmpty in Code8002EE1C.c. Golfer.c header fixed; the club-name table 0x80187650 linked (Code8002EE1C.c). Codex/Gemini: agents/assign/2026-09-26-codex-gemini-velocity.md. ENDGAME checkpoint 1 (2026-09-26 ~21:00 CDT) merged: Golfer AI_ChooseTarget exact (Codex,
labelled fakes), startUp exact (Gemini, labelled pragma) and linked; GoStaticCam, Rain, SunFlr, Code8002EE1C,
GameModeBestBall linked; exact also char Character_KeepClubOutOfGround, SkinPart SkinPart_ListOptionTextures, UISScreen fn_8016B4D4, UISApi
fn_80169308, LLFont fn_8001208C, GoGreenGrid GR_BuildGridRenderData. Exact but not linked: Golfer (46/46; its data
proves it was 4 original files: A ai_brain AI_SetShotModifiers..AI_ApplyError, B AI_TargetsInit..AI_ChooseTarget,
C Club_UsableForKind..Shot_FitTargetToClub, D Luck_*; ranges in the g-hoist report, see journal), GoGreenGrid (link
failed on data order). Left: 49 functions in 23 units. Audit follow-ups: LLFont fn_8001208C's comment ("the colour
passed in") and the FE_CrAPDB const comment are stale.
Round 7 (4 lanes, 16:25-17:30 CDT) merged: Swing linked (SW_KillVibration),
exact also Character_SetupForShot, CameraScript_LagAimMarker; AI_ChooseTarget 98.66 -> 99.56 (last miss:
the kept-copy class); GR_BuildGridRenderData 99.22. rasim.py now models spills / later passes. UIS
kept-copy source form not found (3 rounds): parked until the endgame unless new evidence. Levers:
decomp-notes "New from round 7".
Round 6 (5 lanes, 11:40-13:25 CDT) merged: Earnings, LLDynTex, hwsBurn, GoBreakLine
linked; exact also hlaudtrackstm fn_800AB860, UISEvent fn_80165B90 (labelled s64 fake). Dead asserts: no
effect (findings). UIS = Tiburon's IStudio: one library flag set ties the per-file flags (not applied yet:
owner's call); NASCAR 2005 GC DWARF fetched (docs/reference-builds/nascar2005-gc): no IStudio. r6-big broke
/dev/null (rm, against the brief); restored by the orchestrator. Levers: decomp-notes "New from round 6".
Follow-ups: move r6-misc's allocator replay (rasim.py, rasearch.py) into tools/match; include/engine.h
LLFontFile uGlyphs/u18/uBitmap are now s32 (their u prefix: audit rename); Earnings' award-branch nSlot
shadow awaits its own name (audit).
Round 5 (5 lanes, mwcc-debugger on all 80 non-exact functions, 09:35-11:00
CDT) merged 2026-09-26 ~11:10 CDT: uiText, GoTerrain, HLAudMaster + hlaudmovie (split) linked; exact
Particle fn_800951A0, Grass Static_Render, LLTex fn_8000EA1C, SW_vImpact, hlaudmovie fn_800A8AD4
(labelled fake), uiText UIText_Draw, GoTerrain x2. Every lane wrote its debugger readings into the
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
   HwsBurn_CopySetOptions (EA's `add r3,r6,r3` form gives pBurn 29 neighbours: find the form with one temp fewer).
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
linking: SunFlr_Gc (fn_8009A708), uiProcessInterface (UI_ReadControllers), hwsBurn (HwsBurn_CopySetOptions, 1
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
| 7,598 / 7,647 | 96.79% | 86.33% | 80.58% | 234 / 259 |

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

**Goal reframe (owner, 2026-09-27 evening):** reviewers will judge this now: take it from "AI slop" to a
genuinely good decomp (readable, named, understandable to a human or a fresh AI, faithful). Fakes may
stay for now; every change keeps the byte match. Plan of attack = the procedural feedback loop in
agents/findings/2026-09-27-codebase-health.md (per-file checklist, lint baselines, order of attack);
to be fleshed out with the owner after the compaction. The DOL sha1 (`main.dol: OK`), not objdiff, is
the gate: a type change can grow the DOL with every function still "exact" (n2 finding).

**Naming policy (owner, 2026-09-27):** most names can't be proven; name anyway and roll with it.
- Evidence first (EA/TW06/TW07/Madden names, n1's pairing), then descriptive names that state what
  the code demonstrably does (the audited comments are the base), tier T3 "provisional" in
  name_sources.tsv; confidence lives in the log, never in the identifier (no `_maybe`, no `?`).
- EA's own style: subsystem prefix + verb/object (UISMgrInit, GM_BallHit, Physics_ShotImpact,
  Golfer_IsLucky); consistency across the codebase over perfection of any one name.
- Light verification for T3: one naming pass + an independent reviewer sampling; not two blind
  readers for 5,000 functions. Hard rule kept: a name must never contradict the code.
- merge.py `--allow-renames` requires a name_sources.tsv row per renamed function.

Hygiene items added 2026-09-27 (owner: after 100%, not right away):
- **objdiff "matched data" (79.88% vs 99.93% linked):** cosmetic, no byte changes. Fix symbol sizes
  in symbols.txt / declared types so objdiff's per-symbol .bss/.sdata comparison pairs up (biggest:
  AXVPB .bss 70.9 KB unscorable, skalib .bss 52.6 KB at 99.98% fuzzy, LLFileIO_Gc .bss, AXOut .bss).
- **Madden 2003 evidence:** apply EA's names for the 58 paired IStudio functions (pairing.md) and
  replace IStudio fakes with EA forms where the STABS show them; lbl_80281B40 -> MSL __float_max.
- **Fake-match inventory** incl. tonight's: UISStack pA/0x7C ternaries, GameMode22's 64-bit OR that
  reads pPlayer uninitialised when there are no players (the one known UB-flavoured fake), UIS copy
  chains; DiscError/Code800B7210 split point for the three setters is unproven; CharAnim owns the
  0x801D9908 buffer on weak evidence; LLTex may be 2-3 EA files (aligned(8) .sbss gaps).
- **Tools:** sched750.py skips lines without a source line number (hoisted code) and has no FPU/
  call blocks; rasim decl search doesn't renumber split webs.
- **Stale comments** (audit): char.c unity .data start, startup.h lbl_8018F040, fe.h lbl_80281374,
  LLFont fn_8001208C, FE_CrAPDB const.
