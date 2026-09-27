# fn_8016AEEC (UISScreen.c, 0x8016AEEC)

Status: SOLVED 2026-09-27 (b12): j-block `int b = bLast;` stored as `b | bLast` (EA's mr is an or), loop-2 pEntry at function level, nNode as the loop counters.

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
- 2026-09-26 r7-uis, mwcc-debugger (mechanism in agents/findings/2026-09-26-r7-uis-copy-chains.md): the hoisted (short) conversion of
  bLast in loop 2's preheader is `rlwinm r50,r56,0,24,31` until constant propagation (pass 06)
  proves the mask redundant and rewrites it to `mr r50,r56`; the copy propagation after load
  deletion (08) deletes it. EA's `mr r30,r27` is that copy kept: in EA's IR r50 still fed a copy
  (or had a second definition) at pass 08 and 12. The inner preheader copy `mr r52,r50` is gone
  already at pass 03 in ours. No source change tried beyond reading the dumps.
- 2026-09-26 e-uisscreen (endgame; aligned counts on the WHOLE preprocessed unit, a scratch perm dir, same as the build): base 25. do { } while (0) / for (;;) { break; } / while (1) { break; }
  around the store (1-3 deep), around the inner loop (1-3 deep) or the outer loop: 25 (folded).
  A second store local reassigned in the loop (the lever that closed fn_8016B4D4): one local
  (u8/s32/int/u32, set before loop 2 or in a block, reassigned before/after the store or at the
  group start) 25 or worse; two locals (bX = bLast, bY = bX, bY reassigned after the store) 25+
  (u8 conversions appear). Per-function pragmas (19 sets) and scheduling models: 25.
  mwcc-debugger: the frontend hoists the store's conversion to loop 2's preheader
  (`rlwinm r50,r56,0,24,31`) and copies it into the inner preheader (`mr r52,r50`); pass 03 deletes
  r52, pass 06 turns r50 into `mr r50,r56`, pass 08 deletes it. EA's kept `mr r30,r27` needs that
  copy still feeding another copy (or a second definition) at pass 08 and 12. nNode (dead after
  pNode) reused as the counter of the start loop and the inner entry loop: 22 (start only 28, inner
  only 36, outer j 33); with every order of the declarations 22; plus the store-local lever 22.
  Left at 22: EA's `mr r30,r27` and a register rotation of the loop pointers.
- 2026-09-27 b12 (quicktrial aligned on base.c, base 25; scripts in scratch b12/aeec/t*.py):
  REGISTERS SOLVED, 1 instruction left. nNode as both entry-loop counters (22) + a j-block
  `u8 bStore = (u8)(u64)bLast;` stored as `pEntry->n2 = bStore` (18: the (u64) keeps a hoisted
  `clrlwi r30,r27,24` at EA's `mr r30,r27` spot, so bLast and the store value are two variables)
  + rasim search on that dump (`r56=r27 r40=r25 r42=r25 r38=r24 r37=r27 ...`: 0 wrong) -> the
  loop pointers at function level, declared `u8 bLast; UISEntry* pEntry2; UISNode* pNode; u32 j;
  UISEntry* pEntry1; UISGroup* pGroup;` (loop-2 entry first, loop-1 entry and group last): 1 diff,
  only `clrlwi r30,r27,24` for EA's `mr r30,r27` (scratch b12/aeec/best1.c). Any u8/u16/(s64)/(u64)
  conversion there gives clrlwi/extsh; any plain copy (u8/s16/int/u32 bStore, fn-level or j-block,
  or no bStore) is propagated or coalesced (9: registers move too). Not the mr:
  `(s32)((s64)((u64)x << 32) >> 32)` on bLast / bStore (before loop 2, in the j block, in the
  store, 1-2 deep, x 6 types: 18-20 on the old order, 1-5 with clrlwi on best1), s64/u64 bStore
  (pair-low vreg; still propagated, 1-9), nested 64-bit casts 1-3 deep (2480 variants, none with
  the mr), `x | x`, `x & x`, `x > x ? x : x` (folded), bStore = 1 in / before loop 1 then = bLast
  (9), `nMsg == -1` recomputed for bStore (21-24), bLast and pEntry2 as one u32 variable (frontend
  propagates it, 9), a shared u32/void* identity inline for both (9-13), per-function pragmas
  (opt level 3, loop invariants, propagation, dead code/assignments, peephole, unroll, lifetimes,
  cse; 14 sets x 8 forms): none gives the mr.
  Mechanism: a surviving `mr` is coalesced unless its two sides interfere; EA's bLast (r27) is dead
  in loop 2 (r27 = pEntry2 there), so EA's r30 must interfere with it before the copy.
  More, all without the mr: dead defs of the store local (`= 0`, `= nMsg`, `= 1`, self) before or in
  loop 1 with opt_dead_assignments/opt_dead_code off (frontend drops or splits them into a new
  temp: 9); a dead srawi into it (`(s32)((s64)(s32)bLast >> 32)` then `= bLast`; the frontend
  renames the second web, 9); `bLast op j` with j = 0 (+ | ^ - << >>, folded, 9); 1-7 nested
  64-bit shift chains, as one expression or statements (propagated, 5-13); loop 2 as a static
  inline taking bLast (48); register/volatile locals; bLast as int/u32/long/s16/u16/void* (5-9).
  COMMITTED (b12): the 1-diff form with the original names (loop-2 pEntry at function level,
  loop-1 pEntry block-scoped and shadowing it, pGroup block-scoped, j-block `u8 b = (u8)(u64)bLast`):
  report 97.64 -> 99.44, only `clrlwi r30,r27,24` for `mr r30,r27`. Next lane: find what keeps
  that copy (anything that makes r30 and r27 interfere, or a post-RA source of `or r30,r27,r27`).
- 2026-09-27 b12, SOLVED: EA's `mr r30,r27` is `or r30,r27,r27` (7F7EDB78 is both). Read from
  mwcceppc.exe (GC/2.6, coalescing at 0x57b9b0): the allocator coalesces only PCode whose opcode
  flags have 0x10 (IsMove: PC_MR yes, PC_OR no). So an OR whose two operands become one vreg
  survives everything. `(s64)bLast | bLast` (and ~1100 similar) gives `mr r29,r27` (store temp
  created after the j-offset temp, so r29/r30 swap; rasim --key confirms); a j-block
  `int b = bLast;` stored as `pEntry->n2 = b | bLast;` makes the OR an inner-loop hoisted temp
  (lower @ number, higher priority): r30, exact (310 of 3000 variants of that shape exact).
  Fix: 99.44 -> 100 (labelled fake: `b | bLast` is bLast).

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
