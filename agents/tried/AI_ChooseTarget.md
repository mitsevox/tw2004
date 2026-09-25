# AI_ChooseTarget (Golfer.c, 0x8002C2DC)

Status: OPEN, 98.20% on 2026-09-25.

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
- NEXT (plan, 2026-09-25): learn CodeWarrior's callee-saved numbering rule by controlled one-change
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
