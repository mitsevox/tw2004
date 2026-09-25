# Stm_Tick (hlaudtrackstm.c, 0x800ABDB0)

Status: OPEN, 99.06% on 2026-09-25.

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

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-10-notes_w6.txt

```
  Stm_Tick: decl climb 94 -> 88 only.
```
