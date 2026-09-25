# fn_8016ABBC (UISScreen.c, 0x8016ABBC)

Status: OPEN, 96.23% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
UISScreen fn_8016ABBC
base 16, 6 levers, 23 variants (6 singles) in 2 s; best 16

16 [safe]
  - for loop #1 (`for (i = 0; i < nGroups; i++) {`) as a while loop

16 [safe]
  - for loop #2 (`for (i = 0; i < nEntries; i++) {`) as a while loop

16 [safe]
  - move `u32 i;` to line 1 of the declarations

16 [safe]
  - type of i: u32 -> unsigned int
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-03-notes_w6.txt

```
- UISScreen fn_8016ABBC (orig keeps p in r7 and copies it per case: mr r29/r28,r7): pNode/pGroup at
  function top (4 orders), casts, split null tests: no.
```
