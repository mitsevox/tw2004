# fn_800B6FCC (DiscError.c, 0x800B6FCC)

Status: OPEN, 99.44% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
DiscError fn_800B6FCC
base 7, 13 levers, 518 variants (13 singles) in 6 s; best 7

7 [safe]
  - for loop #1 (`for (i = 0; i < nLines; i++) {`) as a while loop

7 [safe]
  - type of nY: int -> s32

7 [review]
  - type of nY: int -> u32

7 [safe]
  - identity inline on `nY = (28 - nLines) / 2;`
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-05-split-finder-notes.txt

```
  but fn_800B6FCC (99.4%, r30/r31 swap of &szLine[1] and nY) sits on the cut. Tried: decl swap i/nY
```

### agents/notes/map-10-notes_w6.txt

```
- DiscError fn_800B6FCC: s16/s32/u32 nY, decl orders, pointer local, nY++ forms, stmt orders: no.
  fn_800B6CD8: c = *szText first (45 -> 10 aligned, param copy order still off), types, pointer: not kept.
```

### agents/state.md

```
DiscError needs fn_800B6FCC. Orphan data still parked: Code8009AA28's raw blobs lbl_8018A028
```
