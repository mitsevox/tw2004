# AnimLib_WasLastPlayed (skalib.c, 0x80025808)

Status: OPEN, 89.07% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-26 r2-terrain (12 aligned): inline getter for the slot (whole index, player+kind
  row): 12; s32 copy of nClub, kind kept in nGroup: 12; strcmp on *ppSlot: 11 (not EA's: it
  re-reads the table); if/else chain: 18; case 1 first: 12; `register` nKind: 12; slot / row /
  player pointer locals: 14; a second kind variable: 12; `if (strcmp..) return 1; return 0;`: 12;
  `nPlayer > 3`: 14; unsigned player test: 15; parameter copies (pcopy): 12.

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
- 2026-09-26 round 3 (r3-terrain; written by the orchestrator from the lane report, the disk was full): nKind initialised at its declaration (0 or -1), an extra slot local, u32 nKind, a (u8) cast on the return, an if-return form: all stay at 11.

- 2026-09-26 r4-terrain (12 aligned): `[nClub]` spelled apart, `&..[nClub][0]`, `!strcmp`, if-return 1 / return 0, a pName copy: 12; the switch as an if / else-if chain: 18; `default: nKind = -1` then `if (nKind < 0) return 0`: 15.

- 2026-09-26 r4-terrain (12 aligned): the slot as byte arithmetic on (char*)lbl_80281D14 (every order and bracketing of the four products and the base, 1680 variants): 8 at best, with nKind in EA's r10 only when its product is added last (EA adds it second); `[0]` for the kind plus `+ nKind * 0x300`: 11; kind / player / style / club through an identity inline, a copy of nKind before or after the player test: 12-13; the player test as `if (nPlayer >= 0 && nPlayer < 4) {..}`: 17, as two ifs: 15, before the switch: 25; GC/1.3-2.7: 12 (1.x: 32, 3.0: 13).
