# Stm_Tick (hlaudtrackstm.c, 0x800ABDB0)

Status: OPEN, 99.10% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-25, ChatGPT: reuse the `ppVoice` array pointer in the two pause loops, move
  `fn_800AC328()` to an explicit handle local before read sizing, precompute stream offset and
  buffer, preserve the read-length intermediate, and express the first clamp as a ternary:
  91.30% -> 99.06% in the real unit. The remaining target differences are `divwu r3,r3,r0`
  versus `divwu r0,r3,r0`, and a read-position load / shift scheduling swap near the async read.
- 2026-09-25, ChatGPT: tried declaration climb, local/operand/condition orders, quotient and
  threshold temporaries, `int` quotient, identity inline, and register/type variants on the
  remaining spots, plus a 6-minute combination lever sweep: 99.06% -> 99.06%. The new C remains
  a portable partial match.
- 2026-09-25, ChatGPT follow-up: varied quotient helper bodies and the declaration/assignment
  orders of the quotient, channel count, buffer, stream offset, and read position; also tried
  unsigned-equivalent subtraction forms and GC/2.0 with unchanged source. Fast-snapshot diff
  stayed at 4 instructions (the same two spots); real-unit score remains 99.06%. No source edit.
- 2026-09-25, ChatGPT follow-up: split the unsigned threshold calculation into a shift and
  `/= pList->nChannels`. The real-unit score rose 99.06% -> 99.10% and the `divwu` destination
  now matches. One ordering swap remains: original loads `uReadPos` before shifting the channel
  count; our compiler schedules those two independent instructions in the opposite order.
  Reordered declarations, read-size/buffer/offset calculations, compound arithmetic, cap
  expressions, and GC/2.0 on the new source: 2 differing instructions -> 2.
- 2026-09-25, ChatGPT follow-up: ran a six-minute combination lever sweep from the new
  99.10% snapshot. Its best candidate merely moved `bFed` in the declarations and still
  differed by the same two instructions, so no sweep candidate was applied.
- 2026-09-25, ChatGPT follow-up: repeated a focused four-minute sweep after main added
  the adjacent-assignment-swap lever. Best result remained two differing instructions;
  no candidate improved the real source.
- 2026-09-25 n-misc (quicktrial aligned, base 2): cap in a `uMax = nChannels << 15` temp
  (before or after uRemaining, or before uOffset), uReadPos in a local used by offset and remaining,
  `nChannels * sizeof(StreamChunk)`, `sizeof(StreamChunk) * nChannels`, `(u32)nChannels << 15`,
  `* 0x8000`: all 2; `uLen >= cap` 4; the read length as a ternary argument of fn_800AB4C0 81, with
  offset/buffer as call arguments 19. Permuter candidate (pure scheduling of two loads/shift).

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-10-notes_w6.txt

```
  Stm_Tick: decl climb 94 -> 88 only.
```
- 2026-09-26 PC declsearch (run 36221888505, iterated local search over the declaration order): queued but given no time (a scheduling bug, fixed); not searched.
- 2026-09-26 PC declsearch run 2 (36235241920, fair time slices): best 2 aligned (base), 7381 trials.
