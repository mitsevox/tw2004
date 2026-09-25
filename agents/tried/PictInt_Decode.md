# PictInt_Decode (LLPictInt.c, 0x8005620C)

Status: OPEN, 93.08% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

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
