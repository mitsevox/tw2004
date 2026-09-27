# AnimLib_WasLastPlayed (skalib.c, 0x80025808)

Status: SOLVED 2026-09-27 (lane b6), 89.07 -> 100: the slot's byte offset accumulated in one
int local, `nOff = player + kind; nOff += style; nOff += club;` (sizeof strides), then
`(char*)lbl_80281D14 + nOff` for both the store and strcmp (labelled fake match, same element).
Commit: `git log --grep AnimLib_WasLastPlayed` on agent/b6.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-27 b6 (quicktrial aligned, base 12). mwccdbg: the sum's last add (r47) coalesces
  with strcmp's r4 and the base load takes r6 because the partial sums r45/r46 are coloured
  first and take r0; EA keeps one sum in r7 and the base in r0. `#pragma scheduling once` 24;
  dead `(s64)` round trips in strcmp's arguments: pName 10 (nKind to EA's r10), index nKind 10,
  nStyle 11, nPlayer 14, nClub 12 (with scheduling once 18-24); `&T[0]`, `!strcmp`, `(s64)` on the
  slot argument: 12. An offset local written in one expression: 13 (int/u32); offset inline in
  both arguments 16, operands reversed 17. **One local accumulated with `+=`**: every order and
  grouping of the four products (int/u32, declared first or last): `pk/s/c` (player+kind, then
  style, then club) 0; kp/s/c 1, spk/c 1, pk/cs 2, spkc 3. sizeof strides or hex constants,
  int/s32/u32: all 0.

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

- 2026-09-26 e-char (mwccdbg, read only): the frontend CSEs each index product into @1457 (kind
  *0x300) / @1458 (player) / @1459 (style) / @1460 (club<<4); every variable has at most 13
  neighbours, so colouring is plain reverse-vreg order. EA's registers (base r0, club r6, kind
  product r8, player r9, style r4, nKind r10) need the table-base load (r44) coloured before the
  backend temps r45/r48 that take r0 in ours: rasim finds such an order only by moving backend
  temps (r38, r48, r45, @1457, @1459, r43, r49, @1458, r46, @1460, ...), not by the locals alone.
  No source change.
