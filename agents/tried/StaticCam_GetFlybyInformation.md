# StaticCam_GetFlybyInformation (GoStaticCam.c, 0x80065488)

Status: OPEN, 99.95% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-25, gemini: Analyzed block 2 coalescing (`fmadds f31` vs `f3`, `fmr f3, f31` vs `fmr f31, f3`). With `fEnd` as temporary, MWCC coalesces `fEnd` with call argument register `f3`, copying `f3` to `f31` (`fmr f31, f3`). Writing `fT += ...` or `fT = ... + fT` eliminates the coalescing and emits `fmadds f31` and `fmr f3, f31`, but causes MWCC's latency scheduler to delay `addi r6, r10, 0x20` after `addi r4, r31, 0x20` and reorder `lfs f1/f2` (~7 diffs). Tested TW07 DWARF locals `percentDiff`, `distDiff`, `testPercent`, chained assignments `fT = fEnd = ...`, `fEnd = fT += ...`, `register f32 fEnd`, and inline identity reads; all either preserve score 2 or trigger the 7-diff argument reschedule.
- 2026-09-25, n-const: block 2 with the ratio or the difference through each dead local (fEnd, fLoDist, fLoT, fStep, fFrom, fLastT, fHiDist, fHiT) and fT or fEnd as the result, `X = fT; fT = X + (fHiT - X) * ..`, and ratio+difference through all 56 ordered pairs with `fT = fT + d * r`: best 2 (base). The fT-form diff is scheduling only: EA sets r6 before r4 and loads pNext->f78 (f2) before pShot->f78 (f1) as our fEnd form does, but with fmadds into f31. Compilers 1.3.2 / 2.0 / 2.6 / 2.7: 2, 2.0p1: 12.
- 2026-09-25, n-const (3): with `#pragma scheduling off` the base emits `fmadds f3; fmr f31,f3` then the arguments in order; the fT form emits `fmadds f31` and `fmr f3,f31` last, just before the call, so the scheduler orders r4/r5/r6 and the two f78 loads differently (EA: the base order with fmadds into f31). `fT = E; X = fT; call(.., X)`, `fT += ..; X = fT`, `X = fT = E` for X in fEnd, fFrom, fLoT, fStep, fLastT, fLoDist: all 12 (the copy is propagated into the call). Assignment inside the argument (`fT = fEnd`, `fT = E`, `fT = fEnd = E`, `fEnd = fT = E`): 12; call(fEnd) then `fT = fEnd`: 17.

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
GoStaticCam StaticCam_GetFlybyInformation
base 2, 385 levers, 36390 variants (385 singles) in 360 s; best 2

2 [safe]
  - move `CamShot* pAfter;` to line 8 of the declarations

2 [safe]
  - move `CamShot* pAfter;` to line 9 of the declarations

2 [safe]
  - move `CamShot* pAfter;` to line 10 of the declarations

2 [safe]
  - move `f32 vLast[4];` to line 1 of the declarations
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/cloud-2026-09-24-round1.txt

```
- GoStaticCam StaticCam_GetFlybyInformation (2: block 2 computes into f3 then `fmr f31,f3`; EA into f31
  then `fmr f3,f31`): `fT +=`, `fT = fT + ...`, chained `fT = fEnd = ...`, `fEnd = fT += ...`, operands
  swapped: 12 (registers right, argument setup r3,r4,r5,r6 vs EA r3,r6,r4,r5: EA likely had a separate
  temp allocated to fT's register). Fresh block-2 local, fresh locals in both blocks, `fEnd = fT; fEnd
  += ...`, any dead float local as the temp: 2. Call takes fEnd with `fT = fEnd` after: 17. TW07's
  percentDiff/distDiff/testPercent as locals (every subset/order x 9 final forms): 2. `fHiT -= fT` first
  4/13; ratio in fEnd first 6/16.
```

### agents/notes/map-02-notes_w8.txt

```
- StaticCam_GetFlybyInformation 99.94 -> 99.95: the path-end branch is a `break` (EA jumps past the loop
  test; ours `continue` went to it). Left: block 2's fmadds into f3 (EA f31).
- StaticCam_GetFlybyInformation block 2: chained fT = fEnd = ..., ratio/diff/product locals, fT +=: 7-17.
```

### agents/notes/map-03-notes_w10.txt

```
  GoStaticCam StaticCam_GetFlybyInformation, LLFileIO_Gc File_ReadAsyncEx, hlaudmovie fn_800A8AD4:
GoStaticCam StaticCam_GetFlybyInformation (2): block 2 through a static inline lerp (return / r = a; r +=)
  into fT or fEnd: 6; `fEnd = fHiT - fT; fEnd *= ratio; fT += fEnd` 49; ratio local first 6-11;
  `fEnd = fT; fT = fEnd + ...` 7; `fEnd = fT; fEnd += ...; fT = fEnd` 2; product first 7; 12 block-2
  forms x 3 block-1 forms (fEnd / direct fT / fT = fEnd =): best stays 2 (base). The `fT +=` form's diff
  is only the scheduler placing fmadds after `stw r30,0x10(r1)` and the two lfs f78 swapped.
```

### agents/notes/map-06-notes_w6.txt

```
- GoStaticCam StaticCam_GetFlybyInformation 97.9 -> 99.94: map-10's fEnd form (TW07 endPercent) in the ternary
  arms and both interpolation blocks. Left: block 2 computes into f3 (fEnd) where EA has f31 (fT); with
  `fT += ...` in block 2 the registers are right but the call's argument setup is scheduled differently (12).
  Permuter 12 min -j2 from the 99.94 form: no improvement.
```

### agents/notes/map-09-notes_w9.txt

```
GoStaticCam StaticCam_GetFlybyInformation (block 2 fmadds into f3, EA f31 = fT): writing fT directly
  (`fT = fT + ..`, `fT += ..`, `(..) + fT`, ratio in fStep, `fEnd = fHiT - fT; fT += fEnd * ..`) puts the
  fmadds in f31 as EA does but reschedules the call's arguments (EA sets r6 before r4 and f2 before f1): 99.20.
  With fEnd = fT after it and the call passing fEnd, or `fEnd = fT` inside the call: 99.20. `fEnd = fT;
  fEnd += ..`: 99.95. So EA's fmadds target is fT, but something else keeps the fEnd-form scheduling.
  Also: `fT = fEnd = ..` 99.20; fHiT/fStep/fFrom/fLoT/fHiDist as the temp then fT = it: 99.95 (same as fEnd);
  call(fEnd) with `fT = fEnd` after the call / after fn_80065B20 / after fDist: 98.85 (fEnd goes to f24).
```

### agents/notes/map-10-notes_w4.txt

```
- GoStaticCam fn_80065488: `if (fT >= fLastT) {fFrom=fLastT; fEnd=fT;} else {fFrom=0; fEnd=fT;} fT=fEnd;`
  + block1 `fEnd = X; fT = fEnd;` + block2 same -> 2 instrs left (block 2 computes into f3 not f31). Not kept.
```

### agents/notes/map-10-notes_w6.txt

```
- GoStaticCam fn_80065488: fEnd in the ternary arms + both interpolation blocks -> 2 left (block 2 computes
  into f3 not f31); fEnd only in T+B1 -> 12 (block 2 scheduling); fCur for the whole homing block, B1/B2
  expression forms, a second temp: none better than 2.
```

### agents/state.md

```
BreakLine_Render, StaticCam_GetFlybyInformation, UObject fn_800488B4, uiText fn_800922A8,
```
- 2026-09-26 PC declsearch (run 36221888505, iterated local search over the declaration order): best 2 (no better order than the current one), 11102 trials.
- 2026-09-26 r5-world (mwcc-debugger): summary has no '!' (fT f31, fEnd f3 as EA's). The
  block-2 value is a frontend temp (@368, f46; block 1's is @369, f45), not fEnd: before regalloc
  `fmadds f46; fmr f44(fT),f46; fmr f3,f46`, and f46 is coalesced into f3 (flags fCoalesced),
  the same as block 1. For EA's `fmadds f31; fmr f3,f31` the temp must go to fT instead, i.e.
  the copy into f3 must not be coalescable (f46 live past the f3 copy, e.g. the arg copy ahead of
  the fT copy in the pre-regalloc order). Tried: a fresh-local form `X = E; call(.., X); fT = X`
  with X in fStep/fFrom/fLastT/fLoDist/fHiDist and `fT = X` after the call, after fn_80065B20 or
  after the fDist line: all 98.85 (X is live across the call and goes to a saved register).
