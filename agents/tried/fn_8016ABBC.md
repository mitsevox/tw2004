# fn_8016ABBC (UISScreen.c, 0x8016ABBC)

Status: OPEN, 96.23% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-25 n-uisscreen: (scores are aligned diff counts from the WHOLE unit compiled: base.c from perm_setup drops the auto-inlined callees, e.g. fn_8016C6C4 inside fn_8016B4D4). EA keeps p in r7 and copies it per case (not coalesced, same as fn_8016AD54, fn_8016AEEC's bLast, fn_8016B4D4's nScreens, fn_8016A2D4's uEvent: one cause suspected). Tried: explicit casts, `void* const p`, split declaration/assignment, case 8 body as a static inline helper (118: the recursion inlines), if/else chain (22), nKind int: all 16 or worse. GC 1.3.2-2.7: identical; 1.2.5n/3.0: worse; -O4/-O3/-O2/-O4,s: worse; -opt nocse/nopeephole/nopropagation: worse.
- 2026-09-25 n-uisscreen, more: p typed u32/s32/int/unsigned int/char*/u8*/UISNode* (header, casts at the calls and in the cases): all 16.
- 2026-09-26 r2-uisscreen (whole-unit aligned, base 16): identity inline on the p copies (node, group, both) 16; the first pInfo read through p (EA's `lwz r3,0(r7)` after `mr r29,r7`), once or in both reads, 16; function-level pNode/pGroup assigned in both cases (pGroup = ppGroups[i] in case 8, pNode = &pNodes[..] in case 7), all 6 declaration orders, 16; `register` / `const` pNode and pGroup, `register void* p` 16; each kind as a static inline helper with fn_8016ABBC under `#pragma auto_inline off` 31 (135 without the pragma). See fn_8016AD54 for the shared copy problem.
- 2026-09-26 r5-uisscreen, mwcc-debugger: `r35 -> r28 !EA r7 41 nb p`: EA never moves p out of r7 (its
  NULL test reads r7) and copies it per case (`mr r29,r7` / `mr r28,r7`) with the case's first load on
  r7; ours keeps p in r28 from the entry and folds the case copies. Same kept-copy shape as
  fn_8016AD54 (see there); no new C tried.

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
- 2026-09-26 r4-uisscreen: the round 3 copy rules tested on fn_8016AD54 (same shape, see there):
  integer-typed copies of p, every p type x cast chain, p reassigned as the call argument, all
  merge; not repeated here.

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-03-notes_w6.txt

```
- UISScreen fn_8016ABBC (orig keeps p in r7 and copies it per case: mr r29/r28,r7): pNode/pGroup at
  function top (4 orders), casts, split null tests: no.
```
- 2026-09-26 round 3 (r3-uisscreen; written by the orchestrator from the lane report, the disk was full): void* copies in the case ((UISNode*)(void*)p, also via u8*/char*/const void*), with the first pInfo read through p, copies at the function top, the parameter typed UISNode* in the header plus void* copies; per-function pragmas opt_lifetimes/opt_dead_assignments/opt_loop_invariants/peephole/opt_findoptimalunrollfactor off and optimization_level 3: all 16 (base). A case-helper inline 26. Observed in standalone tests: a copy q = p keeps its own register when q is passed to a call or the copy is long/unsigned; int->int and pointer->pointer copies merge; a copy only dereferenced in a loop merges.
