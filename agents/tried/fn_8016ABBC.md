# fn_8016ABBC (UISScreen.c, 0x8016ABBC)

Status: OPEN, 96.23% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-25 n-uisscreen: (scores are aligned diff counts from the WHOLE unit compiled: base.c from perm_setup drops the auto-inlined callees, e.g. fn_8016C6C4 inside fn_8016B4D4). EA keeps p in r7 and copies it per case (not coalesced, same as fn_8016AD54, fn_8016AEEC's bLast, fn_8016B4D4's nScreens, fn_8016A2D4's uEvent: one cause suspected). Tried: explicit casts, `void* const p`, split declaration/assignment, case 8 body as a static inline helper (118: the recursion inlines), if/else chain (22), nKind int: all 16 or worse. GC 1.3.2-2.7: identical; 1.2.5n/3.0: worse; -O4/-O3/-O2/-O4,s: worse; -opt nocse/nopeephole/nopropagation: worse.

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
