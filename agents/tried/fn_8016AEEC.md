# fn_8016AEEC (UISScreen.c, 0x8016AEEC)

Status: OPEN, 97.27% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-25 n-uisscreen: (scores are aligned diff counts from the WHOLE unit compiled: base.c from perm_setup drops the auto-inlined callees, e.g. fn_8016C6C4 inside fn_8016B4D4). u8 copy of bLast before loop 2, int/s16 bLast, init at the declaration, bLast before pNode, the start loop or loop 2 or one group as a static inline helper (with -inline auto,deferred the recursion inlines into the helper: 107-144), a block-scoped or separate counter for the start loop (35), nMsg int (51): none below 25. The first loop's counter shares r24 with loop 2's pGroup in EA: the start loop's i is not loop 2's i, but a separate variable made it worse.

- 2026-09-25 nm-c: the sweep's best applied (fn_8016AEEC_Read identity inline on `nMsg == -1`,
  `u32 j` first): 97.27% -> 97.64% (40 -> 25 diffs). New leversweep 7 min: 25. bLast assigned just
  before the second loop: 31; through a second u8 (bMsg early, `bLast = bMsg` before the loop, EA
  has a `mr r30,r27` there): 44.
- 2026-09-25 n-uisscreen, more: `pEntry->n2 = <expr>` in the loop with no bLast (plain/ternary/s16 70, (u8)/(int)/read 31), int/s16/s32 bLast without the read 44-49, loop 2's inner counter as a new `u32 k` in the group block 39, j in its own block 32.
- 2026-09-26 r2-uisscreen (whole-unit aligned, base 25): the start loop, loop 2 (taking bLast as a u8
  parameter, for EA's `mr r30,r27`), or both as static inline helpers with fn_8016AEEC itself under
  `#pragma auto_inline off` so the recursion cannot inline into them: 41 / 52 / 52 (110 / 144 / 144
  without the pragma). `register u8 bLast` 25. See fn_8016AD54 for the shared copy problem.
- 2026-09-26 r5-uisscreen, mwcc-debugger: `r56 -> r31 !EA r27 49 nb @853` = bLast (the identity
  inline's value, skipped to level 2 by its 49 neighbours). EA computes nMsg == -1 into r27 before loop 1
  and copies it (`mr r30,r27`) at loop 2's start, stores r30: the same kept-copy shape as fn_8016B4D4 /
  fn_8016AD54 (see there); our build folds the copy.

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
- 2026-09-26 r4-uisscreen (base.c aligned, base 25): every order of the 4 declarations (24):
  25 (12 orders) or 32. EA's `mr r30,r27` sits where a loop-invariant conversion of bLast is
  hoisted to loop 2's preheader: `pEntry->n2 = (s8)bLast;` puts an `extsb r31,r24` exactly there,
  and a j-block `u16 b = bLast;` stored as `pEntry->n2 = b;` gives `extsh r30,r24` there (EA's r30)
  and fixes the inner loop's `mr r31,r28`: 25 -> 23 (any bLast type; logic unchanged, but an
  extsh instead of EA's mr: not kept). No conversion found that gives a plain mr: bLast u8/int/s32/
  u32/s16/u16/unsigned int x a j-block, i-block or store-scoped copy of 13 types (507 combos), store
  casts and cast pairs over 10 types (777), `+bLast`, `bLast | 0`, `!!bLast`, `bLast != 0`, the
  expression recomputed in the loop, etc.: 25 or worse. Standalone (store to a field / pass to a
  call inside nested loops, 10 x 10 types): never a hoisted mr.

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
- 2026-09-26 PC declsearch (run 36221888505, iterated local search over the declaration order): best 25 (no better order than the current one), 24 trials.
