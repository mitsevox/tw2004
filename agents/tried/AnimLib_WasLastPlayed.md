# AnimLib_WasLastPlayed (skalib.c, 0x80025808)

Status: OPEN, 89.07% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-03-notes_w7.txt

```
- skalib AnimLib_WasLastPlayed (11): pointer local, offset sums in orig order (club first / explicit
  0x600.. sum), *ppSlot reuse, (*(lbl + n)), &[..][0], !strcmp, nKind u32/s32/uint/u8/char/short. The
  orig loads the table base (lwz) before the kind multiply, keeping nKind out of r0.
```

### agents/notes/map-10-notes_w5.txt

```
- skalib AnimLib_WasLastPlayed (nKind in r10, orig; ours r0): types, !strcmp, *ppSlot reuse, &[0], if-return.
  AnimLib_WasLastPlayed. GoDynObj fn_8004731C: decl climb and the spin-constant expression forms, no gain.
```
