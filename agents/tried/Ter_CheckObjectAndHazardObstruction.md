# Ter_CheckObjectAndHazardObstruction (GoTerrainCollision.c, 0x8004B89C)

Status: OPEN, 99.32% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-26 r2-terrain (41; EA holds the first cell x in r14 and stores it to nX's slot 0x90 only
  at the loop, after fWide): a separate start variable (int nMinX, or reusing i/j/k/n/nCorner/nZ)
  copied in the for-init or before/after fWide: 100-136 (CodeWarrior sinks the (int) conversion to
  the copy, keeping the floor result in an extra float register); `nX = nMinX = ..`: 41; u32/s32
  start: 136; the floor kept as f32 and cast at the loop: 41-136; Ter_GridCell for any of the four
  cells (16 combos): 41-66; nMinX through Ter_GridCell: 134-136; parameter copies (pcopy): 63+.

- 2026-09-26 r2-terrain, structural lead (41 -> 28 in quicktrial, not applied): EA's delayed nX
  store is reproduced by a separate start variable fed through an inline with a local, which
  stops CodeWarrior sinking the (int) conversion into the loop's copy: `static inline int
  G(f32 x) { int n = (int)fn_80035074(x); return n; }`, `nMinX = G((fMinX - ..) / ..);`, `for (nX =
  nMinX; ..)` (40, shape now EA's: `lwz rN,0x6c(r1)` early, `stw rN,0x90(r1)` at the loop). Then a
  declaration climb (`int j;` then `TerCell* pCell;` first, `int nMinX;` after `int nX;`): 30;
  plus the identity inline on fWide (`fWide = TW(0.5f + fRadius);`): 28. Climbing every
  declaration again from there: 28. Left: bObstructed / n spill slots 0x88 / 0xa0 swapped, i /
  nCorner / j registers r19-r21 rotated, and the pFlags / pVert address adds. The same inline for
  nMinZ / nMaxX / nMaxZ too: 51.

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
