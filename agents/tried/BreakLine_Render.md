# BreakLine_Render (GoBreakLine.c, 0x800C844C)

Status: OPEN, 99.89% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

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
