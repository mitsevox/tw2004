# PictInt_Decode (LLPictInt.c, 0x8005620C)

Status: OPEN, 93.26363% on 2026-09-26.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-26, r2-ll: MAD_decodemacroblock takes 7 arguments (TW06 PDB: src_y, src_cb, src_cr,
  dest_y, dest_cb, dest_cr, width, flags; our exact rcmp_mad_codec.c uses the first 7). The
  target's loop width in r9 is the 7th argument: passing `pFile->nWidth` (and NULL refs) fixed
  the whole loop. 93.12727 -> 93.26363% real, quicktrial 14 -> 11 aligned. Kept.
  After it: sibling width at the first swap 16, stwbrx moved after the height swap 28,
  `__stwbrx(pFile->uC, pFile, 0xC)` 13. uC field as s32/int/f32 (with a u32 read) 11; value
  `(int)` cast, `(void*)` address 11; `(u32*)pFile + 3` / `(u8*)pFile + 0xC` / both through a
  `(u8*)pFile + 0xC` pointer 68. 32-bit swap as a C shift expression 13 (not turned into stwbrx),
  `pFile->uC = __lwbrx(&pFile->uC, 0)` 15, with the sibling width 14. The three swaps put in a
  comma expression inside the allocator's first/last argument or before the call: 11 / 16 with
  the sibling width (no change at all). Left: the 32-bit value takes r4 (not r3) at BOTH swaps,
  as if r3 were live there (the allocator's r3 argument at the first, the returned pPict at the
  last); the first width mask needs the sibling spelling but that schedules it too early.
- 2026-09-25, n-ll: the whole header swap (32-bit, width and height, sibling fn_800B965C's
  spellings) as one static inline on the PictFile, used at both swaps / the first only / returning
  the pointer / with its own parameter name: 14 -> 19 aligned (same as the sibling width expression
  at the first swap: correct mask, too-early scheduling). No change kept.
- 2026-09-25, ChatGPT round 3: baseline real unit 93.08182% (LLPictInt 2/3 exact), quicktrial 15
  differing instructions. Target's 32-bit header store loads `uC` into r4 and delays `stwbrx`
  until after allocator-argument setup; ours uses r3 and stores earlier. Both width swaps differ
  at `rlwimi` mask (target 16..23, ours 0..23); the inner loop's width load/use is r9 in target
  versus r0 in ours. These are the only instruction-stream differences in the aligned snapshot.
- Width combined-form variants with u8/u16/u32 casts, low-byte masking and both operand orders
  (8 additional forms, including one that would avoid signed left-shift UB): 15 -> 19-23 diffs,
  most recognized as `sthbrx`. Earlier ledger already covered a broader width-expression sweep.
- Reordered the independent width, height and uC header swaps in all five alternative orders:
  15 -> 30-47 diffs. Identity inline on uC, `+ 0u`, `^ 0u`, and u32 cast: 15 -> 15.
- uC local assigned before the store, with/without a separate allocator-size local; allocator
  size, alignment or count in locals; declaration-order climb over seven locals: all 15 -> 15.
  Initializing uC at declaration worsened to 58 diffs. No source change retained.
- 2026-09-25, ChatGPT round-3 second pass: coupling the first allocator call and null check as
  `if ((pPict = fn_80009B34(...)) == NULL)`, negation, or inverted `!= NULL`/`else` all stayed
  quicktrial 15 aligned / 16 positional. The preceding 32-bit swap's load/store order was
  unchanged; no source change.
- 2026-09-25, ChatGPT round-3 second pass: held the `uC` field address in a `u32*` local before
  each byte-swap, initialized at declaration, or for the first swap only. Quicktrial 15 ->
  17 / 58 / 16 aligned, respectively. A separate second-swap-only local at that late C89
  statement position did not compile; not a candidate. None reproduced target's r4 load.
- 2026-09-25, ChatGPT round-3 second pass: loaded `nWidth` into a separate `s16`, `u16`, or
  `int` local immediately before each width swap. Quicktrial 15 -> 15 / 20 / 15 aligned;
  no `rlwimi` mask improvement. No source edit retained; real-unit baseline remains 93.08182%.

The width `rlwimi` masks 16..23 in the target versus 0..23 in our output; both put the same
low 16 bits into the following `sth`, so this is a compiler lowering choice, not evidence of
different decoded width. The inner-loop `mullw` uses the live width in r9 versus our r0;
the width load and comparison remain at the same points. Changing width to a cached value
across the decode call would alter alias-visible behavior and is not a safe matching tactic.

## Third pass: exact sibling MAD header swap (2026-09-25, ChatGPT round 3)

- `Code800B90F4.c`'s already exact `fn_800B965C` swaps the same `PictFile` header. Its width
  source is `((u16)w >> 8) | (((u16)w & 0xFF) << 8)`, and its width assembly has exactly the
  target `extrwi` plus `rlwimi ...,8,16,23`. This gives direct local evidence for the width
  expression, not just another speculative spelling.
- In PictInt_Decode's isolated snapshot, that expression at both width swaps scored 15 -> 19
  aligned diffs: both masks were fixed, but CodeWarrior scheduled the first width computation
  ahead of allocator-argument setup. At the first swap only it scored 20; at the final swap
  only it scored 14. Reordering the first three independent header swaps with the sibling
  expression scored 21/27, so did not recover the target call boundary.
- Kept the final-swap-only change in `src/LLPictInt.c`. Real unit score for PictInt_Decode
  improved **93.08182 -> 93.12727%**; LLPictInt unit 93.20536 -> 93.25%, still 2/3 exact.
  The final mask now matches while the first mask, the 32-bit swap scheduling/registers, and
  the loop width register remain different. This expression also avoids the signed left shift
  in the old final swap.
- Combined the first sibling width expression with `s16`/`u16`/`int` width locals assigned
  before it or initialized in the declaration: isolated score 19-23, no improvement over 14.
  Moving allocator size, alignment, line, or all scalar arguments through local variables,
  with and without the first sibling expression: score 14/19, no improvement. No other source
  change retained.
- Further first-swap packing tests with a cast on the sibling expression's result, a local
  `int`/`u16`/`s16` swap value, separated high/low terms in both orders, or a copied source
  width all scored 19 (base 14): correct mask but too-early scheduling. Three inline wrappers
  around the 32-bit store (file pointer, value plus pointer, and pointer-return helper) stayed
  at 14; paired with the first sibling width expression they stayed at 19. No structural win.
- Real-unit build and checks: `ninja build/GW4E69/report.json`, `lint.py src/LLPictInt.c`
  (0 findings), typeaudit `0 0 63 10`, symaudit `1 0 18`, and forced `ninja` DOL hash
  (`main.dol: OK`).

## Independent dynamic-width/control-flow pass (2026-09-25, ChatGPT round 3)

- Retained the current sibling-form final width swap as baseline (real 93.12727%,
  isolated 14 aligned differences). Tested assigning a freshly loaded width in the
  inner loop condition to an `int`, `s16`, or `s32` local, either using the local
  for all three decode-call arguments or re-reading the field there: all six 14.
  An ordinary `while` loop scored 14; a guard-break `for` scored 18. The loop
  width still compiles into r0 rather than target r9.
- Re-expressed the outer MADk guard as a positive test with `else`, a positive
  test with trailing return, or a negative `goto` to the failure return. All
  three worsened 14 -> 19 and did not delay the first stwbrx.
- Safely carried a dynamically reloaded width across the inner loop via a
  `for` initializer and increment expression, with width reload first/middle/
  last among the two counter increments, `int`/`s16`/`s32` local, and body using
  the local or field. Scores were 16-19 across 18 variants. This still performs
  a fresh width read after every decode call but does not improve register choice.
  No tracked source edit or real-unit candidate from this pass.

## Isolated compiler-version/flag diagnostic (2026-09-25, ChatGPT round 3)

- Kept the current source and tracked build configuration unchanged. In isolated
  `build/perm/PictInt-compiler-probe/` scratch, substituted available GC compiler
  executables into the existing LLPictInt compile command and compared each
  `PictInt_Decode` output against the same target with aligned quicktrial scoring.
  GC/1.3.2: 14 differences; GC/1.3.2r: 14; GC/2.0: 14; GC/2.0p1: 19;
  configured GC/2.5: 14; GC/2.6: 14; GC/2.7: 14; GC/3.0a3: 66.
- GC/2.0 and GC/2.5 have the **same aligned difference list**, including the
  first width `rlwimi` mask (`8,0,23` compiled versus target `8,16,23`), the
  32-bit header-swap load/store register and scheduling, and the inner-loop
  width register. GC/2.0p1 retains these mismatches and introduces extra
  floating-load/allocator-argument scheduling differences. Thus this is not
  explained by using GC/2.5 instead of GC/2.0 for the byte-swap lowering.
- With GC/2.5, alternative flags also failed to improve the 14-difference
  baseline: `-O4,s` 18; `-O3` 68; `-O2` 62; `-inline auto` 14;
  `-inline off` 14; `-fp_contract off` 14. The no-change variants still
  retained the width mask mismatch. No tracked source/config change, and no
  real-unit candidate: changing the compiler would not improve this function
  and would risk disturbing LLPictInt's two already-exact functions.

## Collected from the notes and docs (2026-09-25)

### agents/notes/cloud-2026-09-24-round1.txt

```
- LLPictInt PictInt_Decode 92.54 -> 93.08 (merged): height swap `(((u16)h >> 8) & 0xFF) |
  (((u16)h & 0xFF) << 8)` gives EA's clrlslwi 24,8 + rlwimi. Width (EA: extrwi then rlwimi 8,16,23):
  5 x 6 term spellings x both orders x none/(s16)/(u16); static inline Swap16 (u16/s16/int/u32 x 6
  bodies); u8/u32 casts: no. stwbrx order: u32 local, __stwbrx(v, pFile, 0xC), (u8*)pFile + 0xC: no.
  Loop mullw r9 vs r0: pointer-add args, operand orders, (y*w)/4, y/4*w: no.
```

### agents/notes/map-10-notes_w4.txt

```
- PictInt_Decode: orig byte swaps are NOT the sthbrx idiom (masked rlwimi 16-23 / clrlslwi); 60+ forms tried.
```
