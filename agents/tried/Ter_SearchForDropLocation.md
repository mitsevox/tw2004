# Ter_SearchForDropLocation (GoTerrainCollision.c, 0x8004C134)

Status: OPEN, 99.89% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-26 r2-terrain (6; EA's pin/preferred distances share f26 with fHeading, the ring
  distance is f22, fLift f25): separate fPin/fPref locals (5x5 declaration positions): 12; the ring
  distance kept in fDropDist: 6, in fHeading/fAngle/fSin/fCos/fTurn: 59-75; fLift kept in
  fDropDist: 14, fDist/fHeading/fSin/fCos: 57-73; nPlayer int/s32 copies: 6.

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
GoTerrainCollision Ter_SearchForDropLocation
base 6, 327 levers, 36991 variants (325 singles) in 360 s; best 6

6 [safe]
  - move `u8 bPreferredLie;` to line 11 of the declarations

6 [safe]
  - move `u8 bPreferredLie;` to line 12 of the declarations

6 [safe]
  - move `u8 bPreferredLie;` to line 13 of the declarations

6 [safe]
  - move `u8 bPreferredLie;` to line 14 of the declarations
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/cloud-2026-09-24-round1.txt

```
- GoTerrainCollision Ter_SearchForDropLocation (6, fDist/fLift f22/f25): separate locals for pin and
  preferred distance (3 positions), pin distance inline: 6. 2.0p1 gives 277.
```

### agents/notes/map-02-notes_w8.txt

```
- Ter_SearchForDropLocation (6, f22/f25 swap): fLift block-scoped / declared in the ring loop, turn locals
  block-scoped, ring search as a static inline (fDist a parameter; vPos own or passed): worse; fLift/fAngle/
  vPos[1] accumulator forms; permuter 14 min -j2: nothing.
```

### agents/notes/map-10-notes_w4.txt

```
  Session_SetupProfiles, Ter_SearchForDropLocation, Ball_SetLie, Ball_FlightStep, fn_8003D414 (fnmsubs order),
```

### agents/notes/map-10-notes_w6.txt

```
- Ter_SearchForDropLocation: greedy decl climb over all 18 decls: none.
```

### agents/findings/cleanup-audit-2026-09-23.md

```
  0x8004C134 name Ter_SearchAreaForDropLocation doubtful (TW06 Ter_SearchForDropLocation / Ter_FindDropLocation have
```
