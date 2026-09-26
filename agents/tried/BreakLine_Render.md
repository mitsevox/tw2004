# BreakLine_Render (GoBreakLine.c, 0x800C844C)

Status: SOLVED 2026-09-26 (r6-args): fZ holds the NEW z, computed before the x store, and both
expressions read the old z from aVert[..][2] (`fZ = fX * fCos + v[2] * fSin; v[0] = fX * -fSin + v[2] * fCos;
v[2] = fZ;`). GoBreakLine linked in the same commit.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-26, r6-args: mwcc-debugger: the rotate temps are colored in reverse creation order (new z f61 -> f0 first,
  then the x chain f59/f58 -> f2); EA's x chain gets f0, so its temps had to be created after z's. Computing new z
  into a local before the x statement: fDist/fAngle/fAlpha 7, fX 5, into fZ itself (old z re-read from the array) 0.
  Kept `fZ = fX * fCos + v[2] * fSin` (both reads from the array also 0). 99.89 -> 100.

- 2026-09-25, n-const: registers only (fZ f0 vs EA f2; the first product f2 vs EA f0). `-fSin` into fAlpha before/after the fCos call: 10/9; loads z first with commuted sums 8; while loop 7; `-fSin * fX + fCos * fZ` operand order 7; fCos call before fSin 10; products / new x / new z / loads through the dead locals fDist, fAngle, fAlpha (each singly, both products through a pair in both orders, and an exhaustive sweep of X/Z/P1/P2 over {fX, fZ, fDist, fAngle, fAlpha}): none below 7. Compilers 1.3.2 / 2.0 / 2.6 / 2.7: 7, 2.0p1: 28.

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
GoBreakLine BreakLine_Render
base 7, 233 levers, 33183 variants (233 singles) in 361 s; best 7

7 [review]
  - move `int nPlayer = fn_8001707C(nView);` to line 13 of the declarations

7 [review]
  - move `int nPlayer = fn_8001707C(nView);` to line 14 of the declarations

7 [review]
  - move `int nPlayer = fn_8001707C(nView);` to line 15 of the declarations

7 [review]
  - move `int nPlayer = fn_8001707C(nView);` to line 16 of the declarations
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/cloud-2026-09-24-round1.txt

```
- GoBreakLine BreakLine_Render 99.89 (fZ in f0, EA f2, rotate loop): 128 operand/load orders, store
  order, fX-/fZ-only locals, pointer local (16-23), a temp at every position (4 forms), TW07's
  `direction` local (3 forms x every position), declaration climb (20), inline rotate/get/set/mul 7-17.
```

### agents/notes/map-03-notes_w10.txt

```
  Skin fn_800368FC, UObject fn_800488B4, TerrainData fn_8000C278, GoBreakLine BreakLine_Render,
GoBreakLine BreakLine_Render: permuter 20 min -j 4 (16.5k iterations, base 55): no improvement.
GoBreakLine BreakLine_Render (fZ f0, EA f2): fX/fZ replaced by every ordered pair of the function's
  float locals (fX, fZ, fDist, fAngle, fAlpha; 19 pairs): all 7 (quicktrial), no change. Block-scoped
  fresh fX2/fZ2 (either order) 7; a local f32 v[4] 9; new x into a temp stored after z 8; `t = fZ*fCos;
  t += fX*-fSin` 8.
```

### agents/notes/map-10-notes_w6.txt

```
- GoBreakLine BreakLine_Render (f0/f2 in the rotate loop): expression orders, temps, block locals, decl positions,
  GC versions: no. BreakLine_Reset (pPlayer r31): decl perms, nPlayer + gPlayers[], init in decl, stmt orders: no.
```

### agents/state.md

```
BreakLine_Render, StaticCam_GetFlybyInformation, UObject fn_800488B4, uiText fn_800922A8,
hlaudmovie, ska_shared, PsMgr, GoStaticCam, BreakLine_Render, Rain, uiText, MC, GameModeBestBall,
```
- 2026-09-26 PC declsearch (run 36221888505, iterated local search over the declaration order): best 7 (no better order than the current one), 4827098366 trials.
- 2026-09-26 r5-world (mwcc-debugger): the rotate loop's temps are assigned in reverse creation
  order: f61 (new z) f0, f60 (z product) f0, f59 (new x) f2, f58 (x product) f2, then fZ f0. EA's
  registers (x temps f0, z temps and fZ f2) need the new-x temps assigned before the new-z ones,
  i.e. the x expression's temps numbered after the z expression's, while the x store stays
  first. Not tried further this round.
