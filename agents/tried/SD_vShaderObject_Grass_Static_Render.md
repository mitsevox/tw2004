# SD_vShaderObject_Grass_Static_Render (GoShaderObject_Grass_Gc.c, 0x80120F74)

Status: OPEN, 97.63% on 2026-09-26 (r2-render).

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)
- 2026-09-26, r2-render: random orders of the 21 declarations after nVerts (up to 300 in ~4
  minutes), then a move/swap climb: aligned 55 -> 41, real 97.21 -> 97.63 (kept; order in the
  source). Not exhausted: a longer random search may go further.
- 2026-09-25, n-shaders: the PC sweep's three moves searched over nearby positions (pVert 20-27,
  nPass 5-9, fPerX 15-19): best pVert 22nd, nPass 6th, fPerX 15th, aligned 74 -> 55, real
  96.78 -> 97.21 (kept). Decl climb after it: none. const on pObject / the pointer locals: 74.
  Left: register allocation (nSteps-like counters r18/r22, f22/f27) and two address adds
  scheduled differently. Permuter candidate.
- 2026-09-26 r4-render (aligned, base 41; left: nVerts r22 / nDone r20 swapped vs EA, the
  `pVert + nAxis` add's operand order, fFade's temp registers): `nAxis + pVert`, `&pVert[nAxis]`,
  an f32* or u8* pAxis: 41. nDone's declaration at every position: 41. nVerts declared plain and
  assigned before the `nVerts <= 0` test, every declaration position: 41-45. `nVerts > nDone`,
  a for loop: 41. int/s32 on nVerts/nDone/nSet/nRun/nRow/nInRow/nAxis/nWind: 41. 32 fFade
  expression orders: 41. The skip loop on nRow itself 91, counting up 55, statements swapped 43;
  the draw in an else 41, `nRow >= 2` first 116-117.

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
GoShaderObject_Grass_Gc SD_vShaderObject_Grass_Static_Render
base 74, 699 levers, 42608 variants (699 singles) in 360 s; best 55

55 [safe]
  - move `GrassWord* pVert = pData->apVerts[nSet][nRun];` to line 26 of the declarations
  - move `int nPass;` to line 7 of the declarations
  - move `f32 fPerX;` to line 17 of the declarations

55 [safe]
  - move `GrassWord* pVert = pData->apVerts[nSet][nRun];` to line 26 of the declarations
  - move `int nPass;` to line 7 of the declarations
  - move `f32 fPerX;` to line 18 of the declarations

55 [safe]
```

## Collected from the notes and docs (2026-09-25)

Nothing recorded.
- 2026-09-26 round 3 (r3-render; written by the orchestrator from the lane report, the disk was full): 1500 random declaration orders + climb: no gain (41).
