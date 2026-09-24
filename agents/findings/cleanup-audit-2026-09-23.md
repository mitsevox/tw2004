# Step 2 audit findings (verified = orchestrator checked against code/asm)

## A: game modes on main (GM8/12/4/Stableford/Stroke full; GM2,6,7,9,13-17 diffs)
MUST-FIX (all verified):
- A1 UB: `int fn_800E1788(int)` declared locally in GameModeStroke.c:9, GameModeStableford.c:9, GameModeBestBall.c:10
  and its value used; definition GameRound.c:510 is `void` (tail call to fn_800E19A4, so r3 does carry the value).
  Fix: `int fn_800E1788(int nPlayer) { return fn_800E19A4(nPlayer, 18); }`, one prototype in game.h.
- A2 UB/name: fn_800D3C7C returns the earnings rating 0..25 (game.h:68) but Stroke/Stableford redeclare it as
  "the player's golfer" and index `GolferPrize prize[1]` (Stroke) / `[34]` (Stableford) with it. Real rows:
  Earnings.c `StrokePrize aStrokePrize[26]` at 0x1D4, aSkins at 0x2A4. Fix: one table type, [26], nRating,
  drop GOLFER_PRIZE macro (name-shortening macro).
- A3 drift: lbl_80200538 (0x22F0 .bss) defined locally 6 ways in 12 files with different row type names
  (MatchPrize 0x5E4 GM4, MiniPrize 0x710, GolferPrize, StrokePrize/EarningsTable in Earnings.c). One
  EarningsTable in game.h.
- A4 drift: fn_8000C594() result read as CourseInfo (pin[18] at 0x70) AND as HoleTees (tee[4] at 0xB0, 4 copies)
  and raw `(u8*)pBallCourse + nTeeSet*0x10 + 0xB0` in GM13-17. Can't both be right; resolve from code.
- A5 comment: GameMode8.c:1-4 says 30 s per stroke; prize/scorecard use 3 (SG_Score :1596); the *30 results
  (fn_800FA48C/4B8) are discarded. Also "the time scores" comment in fn_800FA2D0.
- A6 EA bug unmarked (verified asm 800F9CDC `stw r31,0x1e4(r3)` from gPlayers base): GameMode8.c:210-213 player
  1's hole win written into player 0's nModePoints.
- A7 EA bug unmarked (verified asm 800FD664/800FD67C `mr r3,r29` both): fn_800FD534 calls
  GM_EndOfGolferTurn(nPlayer) twice; comment claims the other player's turn ends.
- A8 comment: mulligans. GM_PlayerTakeMulligan: 0 = none, 1 = unlimited, 2 = one per player (unkC28). Comments
  at GameMode12.c:49, GameMode13.c:62, golfer.h:470, GameRound.c:505, Stroke:37, Stableford:39 wrong/incomplete.
SHOULD-FIX:
- pfn234 meaning contradicts (GM8:1692 "hole may end" vs golfer.h:533 "controller pulled").
- GM8:319/333 invented "quit"/"given up" meanings for bits 0x2000 / 0x04000000.
- GM4 aWon / fn_80102874 "won" but GameMode2 Skins calls it on a loss (check asm; rename or EA bug).
- Callback slot types wrong in golfer.h (pfn240 `s32(*)(void)` but called with nPlayer; pfn1E0, pfn1DC, pfn1D8),
  every mode casts. Slot meanings contradict across files (pfn1CC, pfn1D0, pfn1E4, pfn25C). Name slots once
  with TW06 method names.
- gpSaveData declared LadderSave* (GM4), u8* (many), Profile* (Earnings), s32 (GM23); byte 0 named bActive vs b0.
- Player.ball u8[0x64] / ballBefore u8[0x94] but 0xBC copied; raw `*(f32*)(ball+8)`. (agent/headers fixes.)
- Scalars used as arrays: `(&gPlayers[n].nView0)[i]` (GM8, Swing), Vec_Copy into &fBallX. -> nView[2], vBall[4].
- GameMode8.c:10-99: SwingStateDef typedef + sGolferStateEngineTable extern duplicate Swing.c; file order;
  fn_80008370 u8* vs void* (GoGolfCam.c:122); fn_800A7720 also local in GM11; unused extern lbl_802823D4;
  raw +0x34 camera at :1214.
- Unlabelled codegen casts: (s8)GOLFERSTATE_GetCurrentState x21, (u8)nPlayer into GOLFERSTATE_Set x10,
  (s8)Golfer_GetAttribute, p1 & 0xFF. Same trick labelled in Stroke but not Stableford (`h` reuse).
- TW06 names used without a tw06-names.md row: GameModeStroke/Stableford method names, GameModeSkins::.
- Duplicate/misplaced prototypes: game.h:337-341 fn_800FFCCC/fn_800FFD54 twice; fn_80101FEC only in GameRound.c;
  memcpy declared locally in GM4; gNumPlayersSetUp extern in ~20 .c files; lbl_80282278 in 8.
- MiniPrize fields named per mode (nMode13...) though GM12 uses them as its own; 6 copies; stale
  "(see GameMode10.c)" refs in GameMode12.c:15, golfer.h:359, style.md:6.
NITS: event-table bound comment (GM8:538/552 `<= 0x2A` allows one past 42 entries: check reachability);
  unk fields that are used (gSession.unk14, unk5B38, SESSION_OPTIONS->unkC, unkC28); NULL vs 0; empty if
  block GM8:969; blank lines in expressions; GM12:119 aCD4[nCD0++] unbounded [20]; GM2 fn_800F8EDC nBest=5.
PATTERNS: shared data described locally per file; callback slot types wrong -> casts; behaviour comments not
  backed by code; typo-looking asm-confirmed code without `EA bug:`; codegen casts unlabelled; TW06 names
  without doc rows.

## C: GoGolfCam (incl. agent/gamecore), Swing/Golfer/GameManager/Ball/skalib/UStream/CharAnim/CharClip diffs
MUST-FIX (all verified):
- C1 UB (hidden r3 args): Swing.c:3001-3002/3021-3022 declares fn_80012EF0(void) and fn_800171D8(f32 x4) and
  calls them separately; GoGolfCam (branch :827/:831) shows fn_800171D8(fn_80012EF0(fn_80017004(nView)), 0,0,1,1)
  (5 args) and fn_80013EEC(fn_8001614C()). Same at Swing.c:1825-1826/1911-1912. Matches only because r3 survives.
  Fix: real prototypes in engine.h, nested calls.
- C2 UB (branch gamecore): GoGolfCam declares u8 fn_800DC514(int) vs definition s32 fn_800DC514(void)
  (GameEffects.c:663), and u8 GM_ShowPostShotCrowdFlyby(void) vs int (GameManager.c:836). Asm 800C13B0: mr r3,r31;
  bl; clrlwi. -> callers right. Fix the definitions (u8, (int nPlayer) unused), declare in game.h.
- C3 UB: GameManager.c:29 local `u8 fn_800E1CA8(void)` vs `int` definition GameRound.c:553. Fix definition to u8
  (may change codegen; rebuild).
- C4 UB: GameManager.c:191 `void fn_800DCB10(int)` { gpGame->pfn1F8(nPlayer); } but GameEffects.c:86 declares u8
  and tests it (asm 800DB990 bl; clrlwi.). Fix: u8 + return, pfn1F8 type u8(*)(int).
- C5 merge blocker: agent/gamecore raises typeaudit (main 7 12 514 103 -> branch 7 14 516 110): second ShotObj,
  second GameEffects, more externs/typedefs. (Also agent/prephysics 7 12 524 109 raises externs/typedefs.)
SHOULD-FIX:
- GoGolfCam prototypes: fn_8005CB78, fn_80062C10/1C/28 (defined in Swing.c) declared locally; Vec3Copy,
  GameEffects_SetSuperSlowMo duplicate headers; asm-only callees declared differently per file: fn_8003A950,
  fn_8003BDBC (CamSequence* vs int), fn_8003DC78, fn_8001EDF4 (CrAPModel* vs int handle), fn_80063CBC. GoGolfCam's
  own functions declared elsewhere with void* instead of View* (Swing.c, GameManager.c; ~20 more local decls).
- Golfer character object: 4 views (Player.nShotHandle s32 used as handle AND cast to ShotObj*/u8*; Swing raw
  +0x184/+0x17C where ShotObj names them; CharClip raw +0x168.. with u32 vs ShotObj s32 uFlags; CharAnim raw +4;
  branch ShotObj differs). One Character struct in engine.h.
- Player.ball/ballBefore u8[] root cause of (Ball*)/(f32*) casts added today; raw offsets GameManager.c:454
  (ball+0x7C = pBallCourse already exists at 0xB0C), 1226, 1271; Swing.c ~9 sites; GoGolfCam branch 473/542/1313.
  fn_800C6110 tests nBallSurface but indexes gSurfaceTypes with ballBefore's surface (asm-confirmed; comment).
  Golfer.c gSimBall u8[0xBC] + SIM_BALL_* raw macros + (Ball*) casts -> `Ball gSimBall`.
- lbl_80281F78 CamTuning* (GoGolfCam) vs u8* raw +0x1C0/+0x170 (Swing), +0x16C/+0x170 (GameManager).
- lbl_80202898 u8[] with raw +0x28/+0x2C/+0x11 in GameManager.c:171-203; GameEffects struct has n28/n2C/b11.
- skalib.c:2076 handler registered through an incompatible function-pointer cast (UB); LoadedFile 0x20 `nSlot`
  vs engine.h UStreamObject 0x20 `uHash` — one name is wrong.
- GameManager.c: 56 local prototypes identical to headers; duplicate `typedef struct View View;`;
  gReplayData[0xF10] raw vs Replay.bF10.
- View struct defined in Swing.c:82 and GoGolfCam; engine.h only forward-declares.
- gszEmpty (Golfer.c:1752) unsized -> can't match (orig sda21 at 8002EEFC; Session_SetupProfiles 70.9%): size it.
  lbl_80281B40 (Swing.c:403) unsized is DELIBERATE: original uses lis/@ha (fn_8005CC18 100%) -> label it fake
  match. (So symaudit's sda finding for lbl_80281B40 is a correct flag of a deliberate trick: needs a label.)
- UStream.c:67-70 comment stale (pool_data off now). Scope open: arrays sit early in .bss (not COMMON), consistent
  with static or with -common off. Questions -common on itself (gPlayers/gSession also mid-.bss). Needs a build
  experiment once UStream .bss is split.
- GoGolfCam NULL checks inconsistent (p130 tested then dereferenced; pB4) — EA bug notes or "never NULL".
- GoGolfCam unlabelled oddities: (f32)int compare (asm-confirmed), discarded GOLFERSTATE_GetCurrentState result,
  n194=0;n194=1; f190 stored twice; explicit cases returning default. (s8)GOLFERSTATE_GetCurrentState ~40 sites:
  real return type probably s8.
- Comments: "game breaker" (TW06 suggests freeze time), "once the shot has settled", "no shot", "half the time"
  (49%).
- Names: CrAPState.nView holds a body part (0..4); GolfCamera_ClampLookAngle applied from a tw06 "conflict - Not
  applied" row (symbols.txt fec8abc); medium-tier TW06 names applied, tw06-names.md "Now" column stale;
  bPlayerCut vs header comment "only strokes and matchwins confirmed".
- Ball.c Ter_* signature inconsistency; fn_8000C5FC/C5D4 declared in two files; Golfer.c:889 (u32) index on a
  non-player array; UStream.c:111-137 `extern` function prototypes in .c; engine.h fabsf declared double (probably
  really `fabs`).
PATTERNS: caller-side types instead of fixing definitions; casts instead of types; private views of shared objects
  multiplying (View, character, GameEffects, CamTuning, LoadedFile, Replay); asm-only callees declared per file
  with different signatures (hides real r3 args); TW06 medium/conflict names applied without doc update.

## B: GameMode11/Targets/Replay/Earnings full; FourBall/BestBall/AltShot/Battle/Match/GameRound/Messages/Effects/UI/HoleScore/Analysis diffs
MUST-FIX (verified unless noted):
- B1 name/drift (verified): gpGame->holeOrder[] is NOT a hole order: GameRound.c:354-360 sets all 18 to one value
  (gSession.unk5B38, 0 if -1); Game_CurrentHole() (Golfer.c:35) returns it and Ball.c indexes pCourse->pin[] with it.
  So it is a pin-placement index 0..3, and CourseInfo is pin[4] at 0x70 + tee[4] at 0xB0 (not pin[18]) — this also
  resolves A4 (HoleTees). Fix: CourseInfo { pin[4]; tee[4]; }, rename holeOrder/Game_CurrentHole/unk5B38/Replay.nF07,
  delete 4 local HoleTees, fix Earnings "holeOrder" comments. Names/types only.
- B2 = A3 (lbl_80200538 / EarningsTable) — confirmed independently.
- B3 drift (verified): gpSaveData typed Profile* (Earnings), u8* (GM11, GameEffects, GameRound, match modes), s32
  (GameMode23.c:13). `Lesson`/`LessonSave`/`Profile` defined in GameMode11 AND GameMode23 as unrelated structs
  (same names, different data). Profile.n5000 "TOUR card level" (Earnings) vs "lesson finished" (GM11). GameRound.c:351
  raw +0x70. Fix: one SaveProfile (0x10600) in a header.
- B4 UB (types verified; call-site garbage unverified — pfn1E0 has no call in decompiled C and I found no call in asm):
  mode setups store u8 f(int,u8) / u8 f(u8) into pfn1D8/pfn1DC/pfn1E0 declared (int,int)/(int)/(void) with casts
  (Battle, Match, FourBall, AltShot, BestBall...). TW06: HoleFinished(PlayerNumber_t,u8), GameFinished(u8),
  GoToPlayoff(u8). Fix the GameState slot types; may change codegen at GameManager call sites.
- B5 comment (verified): fn_800D9350 "At the end of a hole" — it is called last in GM_PlayerTookShot (GameManager.c:635),
  i.e. after every shot; condition = first shot on par>=4 reached green/hole, not only hole-in-one. golfer.h:245 too.
- B6 comment (verified): GameMode11.c:19 "second table from step 6 on" sits on lbl_80192D98, which fn_801008A8 returns
  for step >= 6 (it's the FIRST table for tries); DC8 is before step 6.
- B7 UB/drift (verified): fn_800D7770 declared `void (int, u8*)` in GameMode4.c:69 and GameMode24.c:47; definition
  `s32 fn_800D7770(int, Award*)` (Earnings.c:573). Also GameMode2.c:11 `fn_800D3D64(void*, int)` passed an int.
  GameMode4.c:336 passes nProfile as nPlayer (unsure EA vs ours).
- B8 name: Earnings.c "TW06: GM_Earnings_RegisterStreamClients/UnRegister..." don't exist in TW06 (PS2 map has
  EarningsInfo::RegisterStreamClients). Header "(TW06's earnings.c)" unsourced; EA's 2002 tree has
  Golf\GameMode\Earnings.c (tw2004-notes.md:618) -> "(EA's name, from the TW2003 source tree)". Other Earnings and
  GameModeReplay names exist in the PS2 map but have no tw06-names.md row.
SHOULD-FIX:
- GameMode11 "scenarios"/Scenario_RequiredShape: no evidence (our name, d428ef1). Rename or revert to fn_8010069C.
- GameMode11.c:920 `case 80:` (asm cmpwi 0x50 at 8010185C): lesson 6 never judged -> EA bug label.
- GameMode11.c:440 fn_801008F8 comment ("demonstration again") wrong: it's the player's retry (step 7).
- fn_800A7720 declared u8 and tested (GM11, GM8) but sweep defines void (passes fn_800AD618's r3). Fix sweep def.
- GameMode11 (ShotAnim*)nShotHandle local partial view (same Character problem as C).
- Raw offsets / copies past declared size: GameTargets.c:54 +0xB0; GameEffects.c:419 ball+0x40 (= Ball.vStart);
  BestBall/AltShot/FourBall ball+0/+8; GameRound.c:351; Mem_cpy 0xBC into u8 ball[0x64] (Replay:131, GameRound:944);
  GameModeReplay.c:116-117 copies 0x1B8 through nStrokes[18] and 0x48 through unk30C[2]; &fBallX/&fTargetX as vec4.
- Unlabelled fake matches: GM11 (s8)GOLFERSTATE (+raw 20), (u16) casts, redundant re-tests 561/809, Hint() shape;
  BestBall/FourBall/AltShot `h` reuse; Earnings f32 locals for whole dollars; GameEffects.c:525 permuter note
  without `fake match:` prefix.
- unk fields read/written: gSession.unk11[1] (index into padding), unk14, unk5B38, SESSION_OPTIONS->unk0[4]/unk84/unkC.
- Local copies: GameModeReplay 10 dup prototypes, GameRound 30, GameEffects 16, BestBall/FourBall/AltShot/GM11 some;
  commit messages claimed removal. Multi-file prototypes in no header (fn_800E5200, fn_800A6DCC, fn_800A7720, memcpy x4,
  ...). Shared externs in .c: gNumPlayersSetUp 15+, gReplayData 8, lbl_80211D38/lbl_80282360, lbl_80282278.
  Duplicate typedefs: Vec4 (5 files), HoleTees 4, MiniPrize/PrizeTable (a7cd498 claimed centralised), UIQueueItem, TeeOrder.
- Comments: game.h:273 fn_800F1D34 "current target" (it's nearest to aim point); fn_80062D38/6C last param named
  nPlayer but passed counts/flags; GameModeReplay.c:78 "nF07" vs writes unk5B38; Earnings.c:26/611/3/31 wording;
  GameTargets.c:365 tee-set names unsupported.
- Stale GameMode10.c refs: style.md:6, golfer.h:359, GameMode12.c:15, decomp-notes.md:276/424/548/624.
NITS: GameTargets arg0 (m2c leftover), GameMessages p0/v; raw constants where LIE_/ATTR_/SW_/GS_ exist; two lie enum
  sets (golfer.h LIE_* vs physics.h LIE_*_e); file order GM11/GameTargets/GameRound; game.h:337-341 duplicate decls;
  GameTargets multiplier tables [13]/[15] indexed by target up to 40 (check course data).
CHECKED OK: GM11:358 EA bug (asm), labelled fake matches have reasons, GameUI fn_80063B98 fix, fn_80101AA8 comment.
PATTERNS: per-file partial views naming the same bytes differently (worst: holeOrder/pin, rating/golfer, n5000);
  claimed header cleanups incomplete; casts/unk fields instead of fixing declarations; match shapes unlabelled;
  "when it runs" comments inferred from address order, not callers; TW06 names not recorded, two nonexistent.

## E: paused branches (prephysics, modes23-24, speedgolf)
prephysics MUST-FIX (verified):
- E1 drift/UB: callers' local prototypes disagree with the new GTC definitions: Ball.c:22 Ter_CheckForDropLocation
  `void (CourseInfo*, Ball*, int, u8*, u8*, int)` vs def `f32 (CourseInfo*, f32*, u8, u8*, u8*, SurfaceType**)`;
  Ball.c:30 Ter_GetSupportingGroundData Ball* vs f32*; Ball.c:80/87/89/90 (s32* vs TerObject**, int vs s32);
  GameManager.c:41/43/44 (u8* vs f32*, int vs u8; Ter_CheckObjectAndHazardObstruction param order); GameRound local
  Ter_Use3DCupGeometry. Fix: all in ball.h with GTC's types; rebuild Ball.c/GameManager.c.
- E2 name/comment: Ter_LieIsPreferred / Ter_IsValidDropSurface compare SurfaceType.nClass (surface class: 3 = green per
  Ball.c gGreenSpeedMul, 2 fairway, 5 rough) against Lie_t values (1..4, LIE_FRINGE_e=10). Accepts class 1-4 incl.
  the green; comment "fairways and roughs" wrong. Fix: surface-class enum or raw numbers + comment.
- E3 comment: GTC:1193 pin "one inch across" — PIN_RADIUS_SQ = (1/36 yd)^2 -> one inch RADIUS.
prephysics SHOULD-FIX: +10 externs +6 typedefs in GTC (typeaudit 524/109; merge gate blocks): terrain types
  (TerCell/TerObject/TerPolyRef referenced by golfer.h CourseInfo) and lbl_801D5888/58C8 [4][4] (declared in Ball.c,
  GameMode8.c, GTC; TW06 gLastAcceptableDropLocation / gLastPreferredDropLocation) -> ball.h. goto at :447 IS needed
  (asm b .L_8004C4B0 skips outer compare) -> fix label text. Ter_CheckObjectAndHazardObstruction missing w/o note;
  0x8004C134 name Ter_SearchAreaForDropLocation doubtful (TW06 Ter_SearchForDropLocation / Ter_FindDropLocation have
  our signature). 4 TW06 names not in tw06-names.md (exist in PS2 map / Xbox PDB). :492 always-false
  `bOnDropSurface ||` inside `if (!bOnDropSurface)` + dead store :500 unlabelled. BALL(p) cast macro (Player.ball u8[]).
  Nits: fn_800509D8 pTri typing, raw view [0x275], raw +0x24.
  Verdict: merge after E1-E3 + terrain types/externs into ball.h + goto label.
modes23-24 MUST-FIX (verified):
- E4 drift: gpSaveData declared `Profile*` (PGATour.c:88) and `u8*` (RTE.c:71) within the branch; `Profile` name
  collides with Earnings' different Profile; RTESave another partial view. All save views fit one SaveProfile (0x10600).
  RTEChallenge == speedgolf's Challenge (0x80, reward at 0x64): keep Challenge. PlayerProfile (golfer.h) is the session
  block, a different object.
- E5 comment: fn_800EE5B4 "trails the leader by more than one" — test is strokes+1 < other; TW06 order puts
  IsPuttForLead before IsPuttForWin (fn_800EE6A0 calls it). Rename/comment as IsPuttForLead (check fn_800EE778 sign).
- E6 comment (verified): RTE fn_800F08A8 "every event done (0)" on `if (!fn_800F0820())`, which counts WON awards ->
  true when none won yet: message 0 = first win.
modes23-24 SHOULD-FIX: raw ((u8*)gpSaveData)[0xC8+i*8] (= Earnings Profile.aC8[i].b); (Challenge*) cast; fn_800F0CEC
  aDate[nYear-2003] unchecked (EA bug label); fn_800EFE90 aTourEvent[n-1] with n==0 possible (unsure); file order;
  fn_800F05DC declared twice; fn_800F0E20(u8* p0) writes s32; m2c param names; fn_8011E020 declared void returns 1.
  Names backed (PS2 map). Date-parameter reorders verified against asm.
  Verdict: merge after one SaveProfile type + E5/E6; RTEChallenge -> Challenge after speedgolf.
speedgolf MUST-FIX (verified where noted):
- E7 merge hazard: 92 commits behind main; main's game.h already has `typedef struct Challenge Challenge;` +
  fn_800EC544/fn_800EAC7C prototypes; branch adds full typedef elsewhere + local prototypes; still has GameMode24.c
  (renamed by modes23-24); GameModeMatch.c local ListEntry. Needs `git merge main` + cleanup by its agent.
- E8 UB (verified): fn_800EC558 reads nSum uninitialised under rule 7 when nNumPlayers <= 1; nPen uninitialised under
  scoring 1 rule 6. If exact -> `EA bug:` labels.
- E9 comment: 'PLYs' attributed to fn_800EADDC in 3 comments, but fn_800EAE74 registers fn_800EAF18 for 'PLYs' and
  fn_800EADDC is registered nowhere in the file.
speedgolf SHOULD-FIX: Challenge fields bMedal0-2 are rule numbers (not flags), reversed numbering; n0/n4 text offsets;
  missing offset comments; uClubs s32 with u prefix. New fields nMode/nTeeSet/nOpponents/aOpponent/nWind/n20 backed;
  PlayerProfile.n0 s8 backed. GameMode5 u8* gpSaveData raw, ChallengeSave view, PROFILE_* cast macros, lbl_80200538 u8[]
  with PRIZE_AT; unlabelled oddities (return n; return n; (u8) on u8; reused locals).
  Nits: lbl_80203554 Challenge[83] 0x2980 vs symbols 0x2984, literal 10624; m2c names; unk5B38; fn_800EAF7C no note.
  Verdict: merge main + cleanup first; land before modes23-24's Challenge change.
PATTERNS: private views of shared data (save profile 5 views, prize table 3, Challenge 3; typeaudit rises on all three
  branches); prototypes left behind when functions get defined; TW06 names/enums stretched past evidence; comments
  contradicting code; missing EA bug / fake match labels; file order; m2c names.

## D: shared headers + schema inventory
Offset comments in all header structs verified correct by a layout checker (Player 0xEF8, Replay 0xF28, Session 0x5BD0,
Ball 0xBC, SurfaceType 0x44). Local .c types match their own offset comments.
MUST-FIX (new ones verified; others duplicate A/B/C):
- D1 = B1 pin set (holeOrder/Game_CurrentHole/AITarget.nHole/unk5B38/Replay.nF07; pin[4]+tee[4]; CourseInfo is the
  current hole's terrain data = TW06 pTerrainData). No codegen change.
- D2 = C1 Swing hidden args (asm confirmed independently).
- D3 Player.ball u8[0x64]/ballBefore u8[0x94]: reads past the array, Mem_cpy 0xBC into 0x64, Swing.c:3733
  ballBefore[0x98] write into [0x94]; ~125 raw ball offsets/casts in 21 files. agent/headers fixes (Ball ball; Ball
  ballBefore;), but it moves Ball into golfer.h with ball.h including golfer.h (backwards) — better self-contained ball.h.
- D4 prize views indexed past size (=A2) + GameManager.c:1304 `SavedRound round[1]` indexed by nSaveCourse (verified).
- D5 callback slots (=B4). pfn1E0: Stroke assigns s32 fn(void), six others u8 fn(u8) via casts: find the caller.
- D6 prototypes: fn_800D7770 (=B7); fn_800588F4 int (GameManager.c:139, typed SaveProfile*) vs u8 (GameRound.c:13, u8*)
  (verified); fn_800E1CA8 (=C3); fn_8003A950/fn_8003DC78 GameEffects wrong, GoGolfCam right (asm derefs arg 1);
  fn_80008370 u8* vs void*; fn_80076158 pointer types differ.
SHOULD-FIX:
- All 8 "identifier redeclared" warnings: forward `typedef struct X X;` in a header + full `typedef struct X {..} X;`
  elsewhere (verified engine.h:12-13 vs golfer.h:403/608). Also GameEffects (game.h:76/GameEffects.c:48), Challenge
  (game.h:249/GameMode5.c:42 on branch), View (engine.h:10, GameManager.c:11, GoGolfCam.c:117, Swing.c:101), AnimLib
  (engine.h:104/skalib.c:35). Fix: full definition as `struct X {...};` or single typedef.
- Session: profiles 0xD38 (PlayerProfile[5]), options 0xE78 (GameOptions 0x88), course records 0xF00 hidden inside
  unk80[]; SESSION_OPTIONS/SESSION_PROFILE macros (83 uses) + raw (u8*)&gSession+0xF00. Declare the fields (rebuild).
- Player 0x314..0x354 is a CourseMoneyTracking (asm fn_800D3548); declare `money` in Player.
- fBallX..W / fTargetX..W / nView0-1 used as arrays -> vBall[4], vTarget[4], nView[2] (TW06 viewControllerID[2]).
- Character object: ShotObj (Swing), ShotAnim (GM11; 0x1628 "progress" vs headers branch "blend"), CharObj (skalib),
  u8* (CharAnim), int handles, s32 nShotHandle -> one Character struct, `Character* pChar` at 0xC18.
- engine.h fabsf declared double at 0x8000AE94 = really fabs; 0x8000AD9C (fabs+frsp) = fabsf. Rename in symbols.txt.
- fn_80072ACC declared (void) but takes r3,r4,f1; passed as void* callback.
- Duplicate constants golfer.h LIE_/SHOT_/NUM_CLUBS/CLUB_ vs physics.h enums; nShotKind comment vs ShotType_t.
- Header ownership vs style.md (golfer.h holds Session/GameState/Replay etc.; terrain types; math macros; stray GM11
  prototypes; game.h declares Swing.c functions; ball.h includes golfer.h backwards). Stale header comments (golfer.h:1-3,
  :427, :402, :603, :300; game.h:14-15, :52; engine.h:33 fn_80009744 is min(dot, FLT_MAX); engine.h:10; nOutfit; ball.h
  has no header comment). game.h:337-341 duplicates. gSurfaceTypes[156]; void* vs View* views; fn_800951A0 u16 param;
  Lie_AllowsFullSwing overclaims.
Branches: headers change supported by code (Ball offsets exact; Ter_PointInOOBNetwork f32*; fn_80050F88/510EC Ball*) but:
  Ball in golfer.h backwards; its ShotObj lacks n2C (main's Swing uses ->n2C, branch reverts to raw +0x2C); keeps
  lbl_80281F78 u8* raw; Ter_PointInFreeDropNetwork should be f32*; 58 commits behind; conflicts in Earnings/Golfer/Swing.
  nearmiss nBallStartSurface supported (= ball.nStartSurface); conflicts with headers (resolve to ball.nStartSurface).
  speedgolf: new Challenge fields supported; PlayerProfile.n0 s8 supported; delete main's forward typedef on merge.
7 CONFLICTING GLOBALS — right declaration / home:
  gReplayData: Replay (0xF28) / game.h. gpSaveData: SaveProfile* (0x10600) / include/game/save.h (GameMode23 s32 wrong;
  NOTE GameManager.c already has its own `SaveProfile` name). lbl_80200538: EarningsTable 0x22F0 / include/game/earnings.h.
  lbl_80202898: GameEffects 0x58 / game.h. lbl_80281664: Challenge* (GameModeMatch ListEntry wrong; GameMode24 raw
  +0x1604 = Challenge[n-1].n64) / modes/challenge.h. lbl_80281F78: CamTuning* (EA file list has GoCamTuningVars; size
  >= 0x1C4) / camera.h. lbl_802823FC: s32 (Swing int; int vs long incompatible in CW) / modes/lessons.h.
  Unified SaveProfile field list recorded in D's report (b0, szName 0x1, 0x3A unlocks, b70, n74.., aC8[31], a1C0[16],
  Award 0x20C[75], 0x338[25], 0x39C[39], aReplay[5][0xF28] 0x438, n5000 TOUR card level, medals 0x516C[29],
  stamps 0x51E4[29], saved rounds 0x5244 (0x70 each), created golfer ~0x54F8, tour season 0xB634..0xBA9C).
  symaudit: lbl_802822DC "too-big" = symbols.txt wrong (GameUI uses [0..2], next symbol at +3) -> fix symbols.txt.
  Plausibly real partials: gSimClubTries, lbl_80203554 (83*0x80+4), lbl_802124B8 (25*0x44+4), lbl_802030BC.
12 DUPLICATED TYPES: PrizeTable/MiniPrize/GolferPrize -> EarningsTable (neutral MiniPrize column names); HoleTees ->
  terrain struct; Profile + GM11 LessonSave -> SaveProfile; GM23 Lesson/LessonSave are different data (rename, keep
  local); SwingStateDef -> game.h; UIQueueItem -> game.h UI; View -> camera.h; Vec4 -> engine.h; TeeOrder -> match.h.
  Vec4/TeeOrder mostly type .rodata constants that look like compiler-generated local initialisers: try local
  initialisers first (then extern + type vanish).
PROPOSED LAYOUT (each includes only below it): game_types.h < engine.h (+Vec4, math macros, fabs/fabsf) < camera.h (new:
  View, CamShot, CamTuning, view prototypes typed View*) < character.h (new: Character, AnimLib, Character_*) < physics.h
  (single enum set) < ball.h (Ball, SurfaceType + gSurfaceTypes[156], PinPos, hole terrain struct, Ter_*, Ball.c/Wind;
  NOT including golfer.h) < golfer.h (Player with Ball x2, money, nView[2], vBall/vTarget, Character*; golfer/AI/swing)
  < game.h (Session w/ aProfile/options/course records, GameState w/ correct callbacks + nPinSet[18], Replay, golfer
  states, GM/round/UI/messages/GameEffects, mode setup table) + include/game/save.h, include/game/earnings.h,
  include/game/modes/{match,stroke,targets,challenge,lessons}.h. Modes with no shared data keep prototypes local.
PATTERNS: partial private views; local prototypes written from one call site (136 dup-of-header; 23 asm functions
  prototyped in 2+ files and no header); same data named differently; forward-typedef + .c typedef (all 8 warnings);
  globals extern'd in many .c (gNumPlayersSetUp 25, gpSaveData 20, lbl_80282278 15, lbl_80200538 12, gReplayData 9 ...);
  unk names on read fields.
Side effect: auditor D ran `git merge-tree --write-tree` -> unreferenced objects in .git (harmless; gc removes).
