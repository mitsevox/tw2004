# Character_SetupForShot (char.c, 0x8001C860)

Status: OPEN, 99.90% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-25 n-charskin: bStance/bPlace/bClipTime as u32: 9, s32: 6, u16: 37; the three
  flags read before pPlayer/pModel: 6; pBallPos from gPlayers directly: 11; nPlayer in a local: 6.

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
char Character_SetupForShot
base 6, 199 levers, 31039 variants (199 singles) in 318 s; best 6

6 [safe]
  - move `f32 vDir[4];` to line 1 of the declarations

6 [safe]
  - move `f32 vPos[4];` to line 8 of the declarations

6 [safe]
  - move `f32 vPos[4];` to line 9 of the declarations

6 [safe]
  - move `f32 vPos[4];` to line 10 of the declarations
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-02-notes_w6.txt

```
- char Character_SetupForShot: all 360 orders of the 6 head statements: 16 stays (as map-09).
```

### agents/notes/map-06-notes_w6.txt

```
  fn_8000AE28 swap (45 calls): Character_SetupForShot 99.69 -> 99.90, nothing down.
```

### agents/notes/map-06-notes_w7.txt

```
  down; Ball_FlightStep 98.72 -> 100 (with if/else fDrag), Character_SetupForShot 99.69 -> 99.90.
```

### agents/notes/map-10-notes_w8.txt

```
- char Character_SetupForShot (6: nPlayer/u10 load regs swapped): gPlayers + n, (u32) index, pModel
  first, u10 through a u32 local: all 6.
```
