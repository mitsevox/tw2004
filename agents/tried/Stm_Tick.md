# Stm_Tick (hlaudtrackstm.c, 0x800ABDB0)

Status: SOLVED 2026-09-27 (b3, fake match): cap computed first into `uCap`, then volatile reads
of pStream and uReadPos (`((volatile AudTrack*)pTrack)->u.stm...`) and of the stream's uOffset
(`((volatile AudStream*)pStream)->uOffset`), same values; hlaudtrackstm linked (.data, .bss,
.sbss) in the same commit (agent/b3, "hlaudtrackstm.c: Stm_Tick exact, unit linked").

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-27, b3 second attempt (quicktrial aligned, base 2; mwccdbg on full-unit variants).
  **Solved by a load-ordering fake (volatile), not by the flag:** with `uCap = nChannels << 15`
  first, then `pStream`, `uReadPos`, `uOffset = pStream->uOffset + uReadPos`, pBuffer, uRemaining,
  volatile on exactly {pStream, uReadPos, pStream->uOffset}: 0. Volatile loads keep their order in
  both schedulers (edges pStream -> uReadPos -> uOffset), which gives the uReadPos load the longer
  path and keeps the shift after it. Sweep of all dependency-legal statement orders x 9 volatile
  subsets of at most two fields: best 2; three volatiles without the cap first: 14; adding
  volatile on uLength: 6; on pBuffer too: 3; ((volatile AudTrackStm*) view, volatile AudTrack*
  local, or casts on pTrack: all 0 (casts on pTrack used). Other findings on the way:
  - A real conditional between the call and the loads (e.g. `if ((u8)hFile <= 0xFF) {...}`,
    `if (hFile == 0) hFile = 0;`, `if (bFed)`) gives EA's load order (the loads' block keeps its
    pre-RA schedule) but costs its compare+branch: 3. Nothing zero-cost found: the frontend folds
    every constant/self condition (x==x, x-x, x^x, x&0, inline constants, empty inlines,
    `switch` with only break/return arms, `&& (h = f(), 1)`, if/else with identical arms -> a
    select through a temp whose copy lands in the join block); the backend never folds a compare
    (no pass removes cmp/branch; a branch to the next block goes only at emission, leaving the
    compare: `hFile = hFile ? hFile : hFile` -> extra `cmpwi r3,0`, order still wrong because the
    join block got the deleted copy). The compare is not CSE'd across the call (nState kept in a
    local and retested: 5).
  - Pre-RA scheduling (pass 14) merges a block ending in `b` into its predecessor; the post-RA
    peephole (19) merges straight fallthrough pairs too (B0+B1), keeping the predecessor's flags.
  - Nested call forms: complex arguments are evaluated right to left (a ternary 3rd argument
    before a ternary 1st argument): 80/81/26.

- 2026-09-27, b3 (mwcc-debugger with a patched scratch copy that relinks / edits PCode before a
  pass; quicktrial aligned, base 2). **Why the swap happens (verified):**
  1. The final (post-RA) scheduler's output for B58 does not depend on its input order: 16
     valid input orders of the block's 12 instructions (relinked in memory just before it) all
     give `lbz, lwz 0x64, slwi, lwz 0x70`. What lifts the slwi is its WAR on r0 into
     `lwz r0,0x80`: renaming that load's r0 to r8 in memory gives EA's order. EA has the same
     registers, so EA's block cannot have gone through the final scheduler at all.
  2. The scheduler (0x507c70 in GC/2.6) skips a block whose flag 0x8 ("scheduled") is set; the
     pre-RA pass sets it; the generic PCode insert/remove routines (0x4dcf30/0x4dcf70/0x4dcfb0)
     clear it. Our B58 loses it at register allocation because the coalesced copies of the call
     result (`mr r128,r3; mr r35,r128`, hFile) are deleted there. Setting flag 0x8 on B58 in memory
     before the final scheduler gives EA's block exactly. Blocks without deleted copies keep it
     (B20, B66 ...), blocks with them lose it (B18, B23, B28, B58, B60 ...).
  3. So EA's B58 had no copy to delete: the call's result reached fn_800AB4C0's r3 with no `mr` in
     the block after the call. Check: passing an uninitialised hFile (call result discarded, UB,
     not usable) gives EA's instruction order (only registers differ, hFile then sits in r27): 34.
     A single precoloured copy (`mr r128,r3`, used in B60; edited in memory before copy
     propagation) still clears the flag: copy propagation is local only (it removes a call-result
     copy only when the use is in the same block, as in B66), so any named or temp variable for
     the result that crosses the min's branch loses the match.
  Tried for a copy-free form:
  - nested `fn_800AB4C0(fn_800AC328(), ...)` with the min as a ternary (<, <=, >, >=): 80. The
    frontend evaluates a ternary argument before the call, inline-call arguments before a plain
    call (left to right among inline calls), plain calls next, simple arguments last. Min as an
    inline function 80, `uLen = ternary` argument 73, comma form 81.
  - inline wrappers Get() (returns fn_800AC328()) + inline Min/Buf/Off, all 24 combinations: best 7
    (Get's return value is a copy again).
  - `if (x) {} else {}` (6 conditions incl. `(hFile & 1) == 0`), `while (x) { break; }`,
    `for (;;) { .. break; }`, `for (hFile = fn_800AC328(); ;)`, `do {} while (0)`,
    `switch (0|hFile) { default: }`, `if (1)`, `goto L; L:`, right after the call or after
    pStream/uReadPos/nChannels<<15 locals (all 6 orders): all 2. Every empty construct is gone
    before backend-00 (checked in the dumps); no block boundary survives.
  - volatile reads of uReadPos / uLength / nChannels / pStream / pBuffer: 2 (10 when uReadPos is
    read twice).
  - `#pragma scheduling once` on the function: 45; `off` 64; `601` 37, `603` 26, `604` 7, `7450`
    26, `505`/`8240` 26, `750`/`7400`/`altivec`/`twice` 2; `peephole off` 35. Compilers GC/1.3 to
    2.7 on the unchanged source: 2 (1.2.5n 58, 3.0a* 122).
  Open question for whoever continues: what C gives a call result that reaches the next call's
  r3 across a branch with no copy in the block after the call (or a block boundary between that
  copy and the loads). Scratch tools (not in the repo): /home/user/scratch/tw/agents/b3/
  dbgperm.py (env PERM_ORDER / PERM_REG / PERM_FLAGS / PRE_EDIT), runperm.py, sched.py.

- 2026-09-26, e-link1 (quicktrial aligned, base 2): one empty test `if (e) {} else {}` for e in
  nChannels << 15, nChannels, uReadPos, uLength, uLength - uReadPos, uRemaining, uLen, uOffset,
  pBuffer, the cap compare, at every legal position in the read block: 2. Ordered 1-3 empty tests
  from {nChannels, uReadPos, uLength, pStream, pStream->uOffset, pBuffer, nReadId, pList,
  nChannels << 15} after the declarations: 2. Permuter 20 min -j4 (6370 iterations): nothing
  below base. mwccdbg: the bl is its own block (B57); B58 after regalloc is EA's final order
  (lbz r0; lwz r4,0x64; lwz r6,0x70; rlwinm r7; lwz r0,0x80 ...), the final scheduler lifts the
  rlwinm over lwz r6 (its WAR on r0 into lwz r0,0x80 gives it the longer path); pre-RA B58 also
  holds `mr r128,r3; mr r35,r128` (hFile), coalesced away.

- 2026-09-26, r6-args (aligned, base 2): an empty `if (pTrack) { } else { }` before uRemaining or before the offset/buffer statements: 2. A block-local copy of pTrack (plain 2, through void* 5) or of pList through void* (2) for the read block; the whole function's pTrack parameter through a void* copy 5.

- 2026-09-26, r6-args (aligned, base 2): an unreferenced label before the offset/buffer statements or before
  uRemaining (testing whether EA's post-RA block split there): 2 (labels dropped). fn_800AB4C0 takes only
  int arguments, so the parameter-order lever cannot apply without changing registers.

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
- 2026-09-26 r5-uisscreen, mwcc-debugger: the pre-final-schedule order of the read-length block
  (backend-19) is EA's final order exactly (lbz nChannels, lwz 0x64, lwz 0x70, rlwinm, lwz 0x80, ...);
  our post-RA scheduler then moves the rlwinm above lwz 0x70 (it carries the WAR on r0 into lwz r0,0x80 ->
  subf -> cmpl), EA's did not. Same registers on both sides, so the DAG we give the last scheduler matches
  EA's except for something invisible here. Unit flags (-proc 750/603e/generic, -O2/-O3/-O4,s,
  -opt noschedule/nopeephole, -inline auto, -fp_contract off): 2 or much worse.
- 2026-09-26 r5-uisscreen: permuter 15 min -j2 (8991 iterations): nothing below the base.

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-10-notes_w6.txt

```
  Stm_Tick: decl climb 94 -> 88 only.
```
- 2026-09-26 PC declsearch (run 36221888505, iterated local search over the declaration order): queued but given no time (a scheduling bug, fixed); not searched.
- 2026-09-26 PC declsearch run 2 (36235241920, fair time slices): best 2 aligned (base), 7381 trials.
