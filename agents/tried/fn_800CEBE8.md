# fn_800CEBE8 (SkinPart.c, 0x800CEBE8)

Status: OPEN, 96.21% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
SkinPart fn_800CEBE8
base 9, 43 levers, 3023 variants (43 singles) in 27 s; best 9

9 [safe]
  - move `int i;` to line 3 of the declarations

9 [safe]
  - type of nList: s32 -> int

9 [review]
  - type of nList: s32 -> u32

9 [safe]
  - identity inline on `nList = fn_800CE660(apSkins, nSkins, &pList, aIds, nIds, 2);`
```

## Collected from the notes and docs (2026-09-25)

Nothing recorded.
