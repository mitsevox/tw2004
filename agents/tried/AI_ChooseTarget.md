# AI_ChooseTarget (Golfer.c, 0x8002C2DC)

Status: OPEN, 98.20% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

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
