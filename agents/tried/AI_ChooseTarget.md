# AI_ChooseTarget (Golfer.c, 0x8002C2DC)

Status: OPEN, 98.53% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-25 orchestrator (quicktrial aligned, base 112; all 374 instructions equal, only ~20
  callee-saved register numbers differ): parameter type PlayerNumber_t / s32 / u32 111, s8 / u8 /
  short / u16 150-155. Parameter copied to a local (`int nPlayer = nPlayerArg;` or a statement) at
  every declaration position x {int, PlayerNumber_t}: best 103 (PlayerNumber_t, position 9 of 19),
  but nPlayer lands in r19/r16, not EA's r28, and p stays r22 (EA r31): register order is NOT
  declaration order here. Clue: ours gives the hoisted (s8) extsb temps r31/r14, EA r21/r20.
- Register map, EA vs ours: nPlayer r28/r15, nPinSet r23/r26, p r31/r22, nAggr r24/r27, nIQ r25/r28,
  zone offset r26/r29, gAITargets base r27/r30, pin row r19/r23, pin+0x58 r14/r24, (s8) temps
  r21,r20/r31,r14, k r22/r25, nCand r16/r19, t r30/r21.
- 2026-09-25 orchestrator, register-order research (regmap: full EA->ours map per variant).
  Corrected roles: r25 is nPower (nIQ is dead after fDumb), r22 = k*2 (the nLinks offset), r18 = k,
  r14 = &gSession.nTeeSet[nPlayer] (hoisted), r19 = the pin row. Colouring order (first = r31):
  EA   p t nKind nPlayer | AIbase zoneoff nPower nAggr nPinSet k*2 s8Aggr s8Pow pinrow | k nBest nCand nSkill | tee58
  ours s8Aggr AIbase zoneoff nPower nAggr nPinSet k*2 tee58 pinrow | p t nKind nCand k nBest nSkill nPlayer | s8Pow
  Rules seen: (1) the user locals keep declaration order among themselves, block scope included
  (declaring k/nCand/nSkill inside the if or the for is the same as declaring them last), then the
  parameter; reversing the declarations reverses that group only. (2) nPower/nAggr/nPinSet (one
  call-result assignment each) and the hoisted temps are a separate group in code order (swapping
  the nPower/nAggr calls swaps them). (3) EA spills pCourse to 0x10(r1): all 18 callee-saved
  registers are in use, so this is Chaitin under pressure; which locals come before the temp group
  depends on degrees, not a simple rule. Dropping one `(s8)` on an `__abs` compare (code changes)
  rotates the order to nCand k nBest nPlayer | temps | p t nKind ...: the casts steer it.
  `(u32)k` + nKind declared first gives EA's top (p r31, t r30, 97) but changes the loop (no k*2
  strength reduction, pCourse no longer spilled): not the answer. Moving p's statement first (118),
  fDumb into the loop (107), parameter copies (coalesced away: no change), EA-like declaration
  order p t nKind nPower nAggr nPinSet k nBest nCand nSkill (120): no.
  Running: permuter (base 675; only float-register wins so far, e.g. the final fDZ inlined: 107
  quicktrial) and a random-restart declaration-order search (swaps + moves over all 20 lines).
- 2026-09-25 orchestrator, results: (a) random-restart search over all 20 declaration lines
  (swaps + moves, 74,479 orders, 90 min): floor 102 (t p nSkill nKind k nBest nCand after the temp
  group); the temp group comes first in EVERY order, so declarations alone cannot give EA's shape.
  (b) decomp-permuter, 2 h, -j3, ~95k iterations: base 675 -> 610, float registers only (final
  fDZ inlined, `fDX = t->pDef->x; fDX = pin.x - fDX;`); no integer register moved. (c) same-code
  rewrites, no effect on the integer order: pDef truthy test, nCand without cast / (int)k / (s32)k /
  pointer form, t = gAITargets + nCand, (signed char)/(char) casts, split if, ++k / k += 1,
  k initialised earlier, (int) on the tee compare, operand swaps; worse: -x for __abs, t after the
  -1 test, fDumb via int. (d) only changes that remove the k*2 strength reduction flip the groups
  (`(u32)k`, `k = k + 1`, a while with k++): the locals then go first (nCand k nBest nPlayer | temps
  | p t nKind ...), but the loop code changes and pCourse is no longer spilled. (e) compilers GC
  1.3.2, 2.0, 2.0p1, 2.6, 2.7 give 2.5's exact order; 1.2.5n and 3.0 change the code. Not the build.
  Conclusion: EA's C differs in some expression shape we have not found, under full register
  pressure (chaotic: small graph changes reorder everything). Best next step is a long permuter run
  on a many-core machine (free), not hand work.
- 2026-09-25 PC permuter (18 cores): in all three distance blocks compute fDX before fDZ (EA's order
  is X then Z; no logic change): permuter 675 -> 550 (block 3 alone 620); objdiff 98.20 -> 98.53%,
  merged. The sweep's `(u32)k` + nKind-first scores 1940 in the permuter: left out. The PC permuter
  continues from this version.
- 2026-09-25 PC permuter from the 520 version (until ~15:35 CDT): best raw 415 needs a
  half-initialised nPlayer copy (reads it before the copy: not usable); a clean
  `int nPlayer = nPlayerArg;` copy scores 520, no gain. (That run died once after ~258k iterations
  on a stdout OSError in perm_objdump.py: fixed in 01fd1a7.)
- DONE (was NEXT, 2026-09-25): learn CodeWarrior's callee-saved numbering rule by controlled one-change
  experiments on this function, recording the full register map each time (first definition, use
  count, loop vs straight code, parameter vs local, how the (s8) casts are written, extra uses).

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
Golfer AI_ChooseTarget
base 112, 396 levers, 33365 variants (395 singles) in 360 s; best 97

97 [review]
  - (u32) cast on every [k] index
  - move `int nKind;` to line 1 of the declarations

97 [review]
  - (u32) cast on every [k] index
  - move `int nKind;` to line 2 of the declarations

97 [review]
  - (u32) cast on every [k] index
  - type of nKind: int -> s32
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/cloud-2026-09-24-round1.txt

```
- Golfer AI_ChooseTarget 97.89 -> 98.20 (merged): no pBest local, every use gAITargets[nBest].field
  (gives EA's addi r4,r3,0x7; lbz for the skill compare; 374/374 instructions, registers only). EA's
  order looks like declaration order (p r31, t r30, nKind r29, nPlayer r28); ours puts the hoisted
  (s8)nAggr temp at r31. Tried after: declaration climb (19); per-site p->/gPlayers[] and
  t->/gAITargets[] (only Golfer_GetAttribute(&gPlayers[nPlayer], ...) lowers aligned 112 -> 98, adds a
  spill); s8/int/s32 on nCand/k/nBest/nZone (81) and nSkill/nAggr/nPower (27); fDumb placement;
  t after the -1 test; k = 0 outside the for; no (s8) on nCand; pDef != 0.
```

### agents/notes/map-03-notes_w8.txt

```
  AI_ChooseTarget (Golfer.c) does NOT want it (115 -> 130/160 quicktrial).
AI_ChooseTarget: quicktrial decl climb (20 decls) and per-site p->/gPlayers[]/PLAYER() sweep: no move helps.
```

### agents/notes/map-10-notes_w6.txt

```
  AI_ApplyError, AI_ChooseTarget, Swing_ApplySpin, Swing_ApplyPowerBoost. s8/int/s32/long nAttr,
```

### agents/notes/map-10-notes_w7.txt

```
Golfer AI_ChooseTarget 92.8 -> 97.0 (committed):
```

### docs/decomp-notes.md

```
  score higher but fuse the other multiply into `fmadds` and round differently (AI_ChooseTarget): check
  `AI_ChooseTarget`** (0x8002C2DC, 20 live variables, a loop with eight calls): no ordering tried
```

### agents/state.md

```
UStream_Update exact). The other 13 "matches" (Earnings x2, Golfer AI_ChooseTarget, TerrainData,
```
- 2026-09-26 PC declsearch (run 36221888505, iterated local search over the declaration order): queued but given no time (a scheduling bug, fixed); not searched.
- 2026-09-26 PC declsearch run 2 (36235241920, fair time slices): best 91 aligned (new order), 6588 trials. Applied: real 98.53 -> 98.66%.
