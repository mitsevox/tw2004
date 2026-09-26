# SD_vShaderObject_Grass_Static_Render (GoShaderObject_Grass_Gc.c, 0x80120F74)

Status: SOLVED 2026-09-26 (r5-render): no pAxis local at all; the loop reads `pVert[nAxis].f`
and the compiler's own strength reduction makes the `nAxis*4 + pVert` induction pointer in the
inner loop's preheader (after GXBegin), which is EA's `add r16,r25,r17`. Commit: "GoShaderObject_Grass_Gc.c:
SD_vShaderObject_Grass_Static_Render exact" on agent/r5-render.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)
- 2026-09-26 r5-render: the diff (EA's pAxis add sits in the inner for loop's preheader, after
  the loop-invariant constant loads, and steps by 0x10 with pVert) read as a compiler-made
  induction variable, not a source local. Removed pAxis (declaration, assignment, `pAxis += 4`)
  and wrote `pVert[nAxis].f` in fShade: aligned 2 -> 0, real 99.30 -> 100 (also 0 with
  `(pVert + nAxis)->f`). mwccdbg not needed (no dump in the batch yet).
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
- 2026-09-26 r4-render, statement orders: fShade and nAlpha before fTexS/fTexT 41 -> 20, real
  97.63 -> 98.17; `pAxis = pVert + nAxis;` before GXBegin 17, real 98.89 (both kept). No gain on
  that: the fFade block among the four (all orders), pAxis in the for-init / after GXBegin /
  at the while head / `&pVert[nAxis]`, 32 fFade spellings, nDone's position, the setup
  statements (vAt..nDone) in 200 s of random moves, the setup's nAxis..nDone orders, the three
  per-point steps' orders, nWind/fBladeT swapped, a 10-min random/climb of the 21 declarations.
  Left: nVerts r22 / nDone r20 (EA the other way), the pAxis add's operand order and slot, and
  fFade's temp registers.
- 2026-09-26 r4-render, later: all initialisers split off (plain declarations + assignments)
  and a random/climb search over the 26 declarations: nDone declared first and nVerts after
  nAxis (the others keep their initialisers) 17 -> 11, real 99.04 (kept); `nDone++` in the for
  header (`nInRow++, nDone++`) 9, real 99.11 (kept); fFade as two statements (`fFade = fDist +
  (..);` then `fFade = fFade * lbl_80260900[1][3] + lbl_80260900[0][3];`) 2, real 99.30 (kept).
  Left only EA's `add r16,r25,r17` for pAxis (nAxis*4 + pVert) placed after GXBegin's constant
  loads; ours `add r16,r17,r25` before GXBegin. No gain: 9 + 7 pAxis spellings (index-first,
  &pVert[nAxis], u8*/uptr offsets, &pVert->f + nAxis, two statements), each before or after
  GXBegin (after: 5, the f30/f31 constants swap), pAxis in the for init (5), pVert/pAxis steps
  in the for header (4-13), the initial assignments' orders (17 at the time), an 8-min
  declaration search on the after-GXBegin form (5).

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
- 2026-09-26 PC declsearch (run 36221888505, iterated local search over the declaration order): queued but given no time (a scheduling bug, fixed); not searched.
- 2026-09-26 PC declsearch run 2 (36235241920, fair time slices): best 2 aligned (base), 9455 trials.
