# SW_vImpact (Swing.c, 0x8005AEE0)

Status: OPEN, 99.81% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-25, ChatGPT: fast-snapshot identity helpers and `register` pointer declarations, plus
  `&p->vLaunchB[0]`: 6 differing instructions -> 6 (identity helpers worsened to 27).
- 2026-09-25, ChatGPT: all 720 permutations of the six declaration lines and a 6-minute
  combination lever sweep: 6 -> 6. No source change.
- 2026-09-25, ChatGPT subagent: fast-snapshot `nClub`/`nTrajectory`/`nKind` declaration permutations
  (combined and split lines, `int`/`s32`/`long`), and shot-power store expression variants
  (`p->swing`, redundant float cast): 6 -> 6. Local pointers to shot power at five declaration
  positions, accessed through either `gPlayers` or `p`, including use in the final physics call:
  6 -> 6.
- 2026-09-25, ChatGPT subagent: combined six integer declaration orders with six earlier
  `pLaunchB` assignment locations (before face-vector, mishit, power, forgiveness, putt reset,
  or its final store): 6 -> 6. Identity inline reads at the final shot-power/launch-vector
  arguments, singly and together: 6 -> 6. No source change.
- 2026-09-25, ChatGPT subagent (second pass): exact diff is only the `r27`/`r29` allocation swap:
  target uses `r27` for the saved shot-power address (`stfsu` then final `lfs`), `r29` for
  `pLaunchB` (`addi`, shape-vector arg, final physics arg); ours reverses them. New tests:
  pointer-to-array launch-B local with three equivalent call-site forms, redundant launch-B
  self-assignment/comma forms at six live-range points, late scalar shot-power read, nested
  block scope for launch-B at eight starts, explicit shot-power result temporary at seven
  declaration positions, and inline field/value helpers: best still 6. Nested scope worsened
  to 17; assigning launch-B again at final call worsened to 41. No source change.
- 2026-09-25, ChatGPT subagent (second pass continued): 128 combinations of seven individually
  neutral levers (shot-power result/final-read temps, launch-B self-assignment/comma argument,
  split integer declarations, player-identity helper, redundant float cast) all remained at 6.
  Crossing the launch-B declaration position with either shot-power temp (84 variants), then
  testing 168 explicit shot-power pointer variants (normal/`register`/`__restrict`, `p` or
  `gPlayers` address, store/final read through pointer, assignment timing) also remained at 6.
- 2026-09-25, ChatGPT subagent (second pass continued): `nPlayer` copy (`int` or `s32`) at
  three liveness points substituted into seven call/index sites; `Player* pCopy` at six points
  substituted into launch-B address or shot-power store: all 6 -> 6. No source change.
- 2026-09-25, ChatGPT subagent (third pass): TW07's `SW_vImpact` debug locals describe a
  substantially expanded shot path (974 PS3 bytes, `pCoreShotInfo`, `pSwingInfo`, trajectory/
  stance and network/estimated-ball locals), not a clear declaration-for-declaration analogue
  of this GameCube tail. Target GameCube bytes load shot power *after* the intervening calls;
  caching the value across them would change semantics, so that is not a valid lever. Safe
  two-stage launch-B aliases (flat or pointer-to-array, five capture points and three later
  uses) stayed at 6. Static inline structural splits of the putt launch reset, hook store,
  shot-power store, power-plus-forgiveness, and shape-plus-hook regions all stayed at 6.
  No source change.
- 2026-09-25, ChatGPT subagent (third pass continued): member-derived byte-pointer arithmetic
  rooted in the full `Player` object for launch-B and shot-power (nine combinations) stayed at
  6. Tiny pointer-to-pointer launch-B assignment helpers and shot-power store helpers, with
  void/value returns, also stayed at 6. The optimizer emits the same bytes for these safe
  structural forms; no source change.
- 2026-09-25, ChatGPT subagent (fourth pass, semantic audit): the real-unit diff is 157 vs 157
  instructions and exactly the same five mismatch sites as the fast snapshot, all the
  `r27`/`r29` swap. Type hypotheses in a fresh snapshot: `PlayerNumber_t` for the parameter
  worsened 6 -> 42; `Club_t` for `nClub` and/or `ShotType_t` for `nKind` stayed at 6. No
  assembly evidence supports changing their types. In particular, this project's
  `ShotType_t` enum stops at 5 while live shot-kind fields also use 6 and 7, so forcing an
  enum local would not be a safe general correction. No source change.
- 2026-09-25, ChatGPT subagent (fifth pass): final shot-power read in a new inner block with
  `f32` or `const f32` local: 6 -> 6; assignment expression into a local in the power argument:
  6 -> 6; `static inline` memory-read accessor (`const f32*` or `const Player*`): 6 -> 6.
  Five sequenced comma-expression forms around the final power read worsened 6 -> 44. All
  retain a separate memory read after the intervening calls; none changed the r27/r29 choice.
  No source change.
- 2026-09-25, n-const: registers only (r27/r29 swap, as the ledger says). Compilers 1.3.2 / 2.0 / 2.0p1 / 2.6 / 2.7: all 6.

## Safe-match constraints from the third pass

- The only mismatches are register numbers for two simultaneously live pointers, not a wrong
  vector coordinate or value. Swapping x/z writes would change the currently matching putt
  reset and the game's launch vector semantics.
- The target stores shot power, calls forgiveness/shape/controller/aim helpers, then reloads
  shot power from memory at the final physics call. Caching the float across those calls may
  change observable game behavior if a callee mutates player state, so a match that relies on
  that cached value is not acceptable even if its register allocation improves.
- Deriving launch-B with `pLaunchA + 4` crosses from one array member into another and is not
  portable C pointer arithmetic. The tested full-`Player` byte-pointer derivation is safe but
  left the same six differences.

## Semantic audit of the mismatch region

- The target's `stfsu f1, 0x414(r27)` stores the `SW_vCalculateShotPower` float at
  `Player.swing.fShotPower` (Player 0x3D4 + SwingData 0x40). Its final `lfs f1, 0(r27)` reloads
  that same field. The header's `f32` agrees with the load/store width and TW06's
  `SW_sSwingData::fShotPower` (`float` at its shifted offset 0x50).
- The target's `addi r29, r31, 0x398` is `Player.vLaunchB`, a four-float array. `r29` is passed
  to `fn_8005B8C8_ShapeVector(int, f32*)` for modification and later as `r8` to
  `Physics_ShotImpact`; both call sites and the array type agree. TW06's analogous
  `AIshot_t::strokeDirection[4]` is also float data, but its later layout was not copied here.
- The final call's registers match the current prototype: `r3` ball, `r4` club, `r5` kind,
  `f1` power, `f2` aim, `r6` trajectory, `r7` launch-A, `r8` launch-B. The other saved values
  (`r26`, `r25`, `r23`, `r24`, `r28`) already match the target. Ball's
  `Physics_ShotImpact` and Swing's `fn_8005B8C8_ShapeVector` are exact functions in the
  current real-unit report. No field offset, argument order or width mismatch was found.

## Next PC-only experiment

When the owner's PC is idle and its decomp-permuter checkout is available, run from a current
branch containing this source:

```
python tools/match/permute.py Swing SW_vImpact --minutes 120 -j 18 --max-jobs 18
```

The previous PC run was 14 minutes at two workers and found no improvement; a two-hour run at
18 workers explores a much larger set. Inspect any candidate for unchanged behavior and verify
it in the real unit; a score-0 permuter result alone is not acceptance. This Mac checkout has
no `TW_PERMUTER` setting or `tools/decomp-permuter/permuter.py` at either path `permute.py`
checks, so its local `quicktrial.py`/`leversweep.py` cannot perform that search.

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
Swing SW_vImpact
base 6, 37 levers, 2483 variants (37 singles) in 25 s; best 6

6 [safe]
  - move `f32 fAim;` to line 1 of the declarations

6 [safe]
  - move `f32 fAim;` to line 2 of the declarations

6 [safe]
  - move `f32 fAim;` to line 3 of the declarations

6 [safe]
  - move `f32* pLaunchB;` to line 5 of the declarations
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/cloud-2026-09-24-round1.txt

```
- Swing SW_vImpact (6): pLaunchB assignment at 15 positions 7; SwingData* pSw local over 127 mixes of
  the 7 swing.* sites 7; f32* pPower 23; physics argument through p-> 44; no pLaunchB 41.
```

### agents/notes/map-03-notes_w8.txt

```
Swing SW_vImpact permuter 14 min -j2 (at 99.81): no improvement found.
Swing SW_vImpact 99.2 -> 99.81: `pLaunchA = gPlayers[nPlayer].vLaunchA;` (not p->), decl pLaunchA
  before pLaunchB (w8/sites.py per-site p->/gPlayers[]/PLAYER() sweep). Left: pLaunchB r27 vs r29 and
  the fShotPower stfsu temp r29 vs r27. Tried: Physics arg via p-> (98.3), pLaunchB via gPlayers[]
  (same), via PLAYER() (95.9).
Swing SW_vImpact: all-gPlayers / all-p / PLAYER() / p = gPlayers; p += n / p set late: all 23 (no).
```

### agents/notes/map-06-notes_w7.txt

```
- SW_vImpact (23, EA p = r31 and the gPlayers offset r30): decl climb, p initialiser in 3 places,
  gPlayers + nPlayer: none.
```
- 2026-09-26 PC declsearch (run 36221888505, iterated local search over the declaration order): best 6 (no better order than the current one), 4792663213 trials.
