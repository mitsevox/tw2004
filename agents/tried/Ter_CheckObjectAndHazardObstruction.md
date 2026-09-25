# Ter_CheckObjectAndHazardObstruction (GoTerrainCollision.c, 0x8004B89C)

Status: OPEN, 99.32% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
GoTerrainCollision Ter_CheckObjectAndHazardObstruction
base 41, 1393 levers, 31235 variants (1393 singles) in 360 s; best 31

31 [safe]
  - move `TerCell* pCell;` to line 1 of the declarations
  - identity inline on `fWide = 0.5f + fRadius;`
  fake match: EA did not write an identity wrapper. Look in TW07 (docs/reference-builds/tw07-ps3/cu/) for a real helper or macro at this spot and use it; otherwise name it fn_<caller address>_Read with a `// fake match:` comment.

31 [safe]
  - identity inline on `pCell = &pCourse->pGrid[nX + nZ * pCourse->nGridWidth];`
  - identity inline on `fWide = 0.5f + fRadius;`
  fake match: EA did not write an identity wrapper. Look in TW07 (docs/reference-builds/tw07-ps3/cu/) for a real helper or macro at this spot and use it; otherwise name it fn_<caller address>_Read with a `// fake match:` comment.

31 [safe]
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-02-notes_w8.txt

```
- Ter_CheckObjectAndHazardObstruction: fWide/fTop accumulators, nX computed later, for (nX = nX;...): 41-100.
```

### agents/notes/map-06-notes_w7.txt

```
- Ter_CheckObjectAndHazardObstruction (41, int regs + spill slots: bObstructed at 0x88 / nX at 0xa0 in EA):
  every position of bObstructed, nX, bFirst in the decl list: none below 41.
```

### agents/findings/cleanup-audit-2026-09-23.md

```
  GameManager.c:41/43/44 (u8* vs f32*, int vs u8; Ter_CheckObjectAndHazardObstruction param order); GameRound local
  (asm b .L_8004C4B0 skips outer compare) -> fix label text. Ter_CheckObjectAndHazardObstruction missing w/o note;
```
