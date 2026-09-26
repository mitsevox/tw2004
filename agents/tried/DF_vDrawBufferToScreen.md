# DF_vDrawBufferToScreen (DepthField.c, 0x80045908)

Status: OPEN, 97.88% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-26, r6-args: permuter 15 min -j2 (13.5k iterations, base 550): best 330 and 395, both change behaviour (fOffset stored into fX0 / fY0, which the next pass reads for aUV); not usable.

- 2026-09-26, r6-args: an empty `if (n) { } else { }` (block boundary) after `aXY[2] = fZ` or before the first clamp: 79 (the CSE is not stopped).

- 2026-09-26, r6-args (aligned, base 79): the EA clamps' 1.0 is the same pooled symbol as fZ's hoisted 1.0
  (lbl_80283148 in both), so EA's first clamp simply was not CSE'd with fZ's load. fZ as f64 (79), f64 fZ with
  an f32-parameter clamp (79), clamping fZ instead of aXY[] (f32 or f64 fZ) 94.

- 2026-09-26 r5-render (aligned, base 79): the hi bound as a different constant object so CSE
  cannot merge it: DF_Clamp fLo/fHi f32/f64 x literals `0.0f`/`0.0`, `1.0f`/`1.0`: f32 lo + f64
  hi 58 (the hi loads become fresh `lfd`, but the ternary turns double: an frsp at the end), f64
  both 51, f64 lo only 82; with f64 hi, bodies casting fHi (`(f32)fHi` in the result and/or the
  compare) 79 (folded back to the float constant, CSE'd again), `(f32)(inner ternary)` 50, if/return
  and `f32 x = f` bodies 79-81.

- 2026-09-26 r5-render: mwccdbg (batch dump). backend-00: fZ's `1.0f -` loads @6 (1.0) in
  B12; the first clamp's hi compare is B14, whose only predecessor is B12, so pass 02 (CSE)
  merges its `lfs @6` into fZ's register (then hoisted to f30); the second clamp's hi load sits
  in B22 after the bOdd join (B20), so it stays a load, as in EA. EA has a fresh `lfs` at BOTH
  clamps, so in EA the first clamp's hi compare is not in the same extended block as fZ's 1.0
  (or its 1.0 is not the same load). The FPR list shows the rest (f105.. f0/f1 swaps) follows
  from that. No C found for it this round (no attempts beyond the reading).

(add yours here: date, lane, what, score)

- 2026-09-26 r2-modes (quicktrial aligned, base 79): fZ's 1.0 as `1`, `(f32)1.0`, `-(...) + 1.0f` x
  first clamp's bounds as `1`, `1.0`, `(f32)1.0`, `0`, `0.0`: all 79. Optimizer pragmas on the function
  (opt_lifetimes, propagation, dead_assignments, strength_reduction, unroll_loops off: 79; common_subs
  off 87; loop_invariants off 143). Observation: f30 (1.0) and f31 (0.0) are hoisted from fZ's `1.0f -`
  and the clamps' low bound, not from fn_800141F8's arguments (those are reloaded each pass).

- 2026-09-25 n-modes (quicktrial aligned, base 79): observation: only the FIRST clamp differs; ours
  compares with f30 (the loop's hoisted 1.0 of `1.0f - ...`, same basic block, so CSE'd), EA loads
  1.0 again into the result register and compares with it (as ours already does in the second clamp,
  which sits in a later block). Clamp bodies with a result local `r = fHi; if (!(x > r)) r = x;` (and
  if/else, ternary-into-r, early-return variants) 62-83: best 62 aligned but objdiff 97.88 -> 97.67
  (reverted); x f32/f64 on fLo/fHi: no effect (constants propagate). Clamp on fZ instead of aXY[] 92-93;
  fZ computed at the loop top / before the call 115-136; `fOne` local for the 1.0 of fZ: no effect
  (copy-propagated).

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
DepthField DF_vDrawBufferToScreen
base 79, 151 levers, 19655 variants (151 singles) in 176 s; best 79

79 [safe]
  - move `int bOdd;` to line 8 of the declarations

79 [safe]
  - move `int bOdd;` to line 9 of the declarations

79 [safe]
  - move `int bOdd;` to line 10 of the declarations

79 [safe]
  - move `int bOdd;` to line 11 of the declarations
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-02-notes_w6.txt

```
- DepthField DF_vDrawBufferToScreen (orig reloads the clamp's 1.0f with lfs instead of reusing f30; the
  result goes to f0): clamp arg literal forms (0/1/1.0/(f32)1.0), clamp bodies (<=, !(>), if/return,
  local r) x param types f32/f64 x fZ f32/f64 x arg aXY/fZ: best 51 aligned (all f64 params) vs 79 base,
  not applied (objdiff not checked). A separate depth temp (fDepth) before 1.0f - fDepth: 79. Permuter
  15 min: nothing real (its 550 output reuses fX0, a behaviour change).
```

### agents/notes/map-03-notes_w7.txt

```
- DepthField DF_vDrawBufferToScreen (orig: lfs 1.0 into the result register f0 at each clamp, 0.0 from
  f31): macro clamps on aXY/fZ (f32/f64 fZ), if/else clamps, inline bodies with if-returns / result
  local / f64 hi: none better; the f64-parameter ternary (all three f64) is 51 aligned (79 base) but
  144 positional (not applied). mac on fZ with f64 fZ gives the exact branch shape but compares in double.
```

### agents/notes/map-10-notes_w4.txt

```
- Skeleton-like brute force (types, loop vars, inlines) found nothing for: fn_8004731C, DF_vDrawBufferToScreen,
  Session_SetupProfiles, Ter_SearchForDropLocation, Ball_SetLie, Ball_FlightStep, fn_8003D414 (fnmsubs order),
  fn_80026844 (`pTree + nGroupOff + 2` is 4 aligned but objdiff drops 99.18->98.75), fn_80034CAC (orig keeps
  the `& 1` mask in a register: li r28,1; and.), fn_80031154 (inline nLast 59->41 only).
```

### agents/notes/map-10-notes_w6.txt

```
- DepthField DF_vDrawBufferToScreen: fZ forms (f64, split, reorder): CW regroups 1-(a+D) into (1-D)-a.
```

### agents/notes/money-01-notes_cloud1.txt

```
- DepthField DF_vDrawBufferToScreen: best 27 aligned (was 79, old best 51) with
  `DF_Clamp(f64 f, f64 fLo, f32 fHi)` { f32 x = f; if (x < fLo) x = fLo; else if (x > fHi) x = fHi;
  return x; } - not applied: it compares the low bound in double and reloads it, the mirror of the
  original (0.0 kept in f31, 1.0 reloaded with lfs into the result register). Operand orders x 3
  body forms x f32/f64 on every parameter, int/s32/u8/s16 fHi, nested Min(Max()) (143): none < 27.
```
- 2026-09-26 PC declsearch (run 36221888505, iterated local search over the declaration order): queued but given no time (a scheduling bug, fixed); not searched.
- 2026-09-26 PC declsearch run 2 (36235241920, fair time slices): best 79 aligned (base), 7198 trials.
