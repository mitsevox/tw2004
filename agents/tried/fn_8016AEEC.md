# fn_8016AEEC (UISScreen.c, 0x8016AEEC)

Status: OPEN, 97.27% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-25 nm-c: the sweep's best applied (fn_8016AEEC_Read identity inline on `nMsg == -1`,
  `u32 j` first): 97.27% -> 97.64% (40 -> 25 diffs). New leversweep 7 min: 25. bLast assigned just
  before the second loop: 31; through a second u8 (bMsg early, `bLast = bMsg` before the loop, EA
  has a `mr r30,r27` there): 44.

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
UISScreen fn_8016AEEC
base 40, 24 levers, 1323 variants (24 singles) in 13 s; best 25

25 [safe]
  - identity inline on `bLast = (nMsg == -1);`
  - move `u32 j;` to line 1 of the declarations
  fake match: EA did not write an identity wrapper. Look in TW07 (docs/reference-builds/tw07-ps3/cu/) for a real helper or macro at this spot and use it; otherwise name it fn_<caller address>_Read with a `// fake match:` comment.

25 [safe]
  - identity inline on `bLast = (nMsg == -1);`
  - move `u32 j;` to line 2 of the declarations
  fake match: EA did not write an identity wrapper. Look in TW07 (docs/reference-builds/tw07-ps3/cu/) for a real helper or macro at this spot and use it; otherwise name it fn_<caller address>_Read with a `// fake match:` comment.

25 [safe]
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-01-notes_w9.txt

```
- UISScreen fn_8016AEEC (97.27): function-level pEntry/pGroup/pfnHandler, 7 orders: best 97.59 (not kept).
```

### agents/notes/map-03-notes_w6.txt

```
- UISScreen fn_8016AEEC (orig: bLast computed early, `mr r30,r27` copy before the 2nd loop): u16 bLast
  with `(u8)(nMsg == -1)` 40 -> 28 raw; not applied (register web left).
```
