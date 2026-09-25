# Physics_HandleCollision (Ball.c, 0x80052598)

Status: OPEN, 98.74% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
Ball Physics_HandleCollision
base 92, 311 levers, 30754 variants (311 singles) in 360 s; best 82

82 [safe]
  - move `f32 fD;` to line 1 of the declarations
  - identity inline on `fCo = fn_80009638(fA);`
  - move `f32 fB;` to line 13 of the declarations
  fake match: EA did not write an identity wrapper. Look in TW07 (docs/reference-builds/tw07-ps3/cu/) for a real helper or macro at this spot and use it; otherwise name it fn_<caller address>_Read with a `// fake match:` comment.

82 [safe]
  - move `f32 fD;` to line 1 of the declarations
  - identity inline on `fCo = fn_80009638(fA);`
  - move `f32 fB;` to line 12 of the declarations
  fake match: EA did not write an identity wrapper. Look in TW07 (docs/reference-builds/tw07-ps3/cu/) for a real helper or macro at this spot and use it; otherwise name it fn_<caller address>_Read with a `// fake match:` comment.
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-02-notes_w8.txt

```
- Physics_HandleCollision: TW07's nine angle locals (bank/pitch/heading angle, sin, cos) at 5 decl places:
  182-209 (base 92), worse.
```

### agents/notes/map-06-notes_w7.txt

```
- Physics_HandleCollision 98.41 -> 98.74: `fD = sqrt(vSlip[0]*vSlip[0] + vSlip[2]*vSlip[2])` in one expression
  (124 -> 96 diff insns), `if (pBall->fSpinX || pBall->fSpinY)` (-4; EA loads the field before the 0.0f).
- Physics_HandleCollision (rest is float register allocation, fSpeed f30 / fA f31 in EA): decl climb over 21 lines
  after the fix (92, none); fGrip in fT/fS/fScale per block, `fGrip = fGrip * m`, `m * fGrip`, *(p+i), (int)
  index, f64: none (the fmuls operand swap at `fGrip *= gTurfSpeedMul[]` stays). Top spin clamp as one
  expression: worse (97). `fGrip = f10 * fabs; fGrip *= 0.3f;` 91 (one insn, not kept).
- Physics_HandleCollision web splits (w7_hc6.py: soil fA, first fD, cos-bank fCo, second fLen, soil fT, clamp
  fT/fSi/fLen, bounce fT as own locals, declared first or last): 92-134, none better.
- Physics_HandleCollision: permuter 14 min -j2: best (775 vs 855) copies cos(bank) through a temp; in quicktrial
  92 -> 93, not kept.
```

### agents/notes/map-10-notes_w6.txt

```
- Physics_HandleCollision: greedy float decl climb: none.
```

### agents/notes/money-01-notes_cloud1.txt

```
- Ball Physics_HandleCollision (92 aligned): `fGrip *= gTurfSpeedMul[]` as a*b, b*a, through a temp,
  with a cast, in pointer form, at both sites in every pairing: all 92.
```

### docs/journal.md

```
- **`Ball.c` checkpoint 6: the bounce, `fn_80052598` (98%). All 68 functions are now in C (unit
  99% fuzzy).** Soft ground bends the normal toward the incoming ball; restitution from surface
  +0x0C; negative restitution = branches the ball passes through, randomised with LUCK pulling
  toward a cleaner pass (the LUCK tooltip's "kinder bounce"); rain deadens bounces; an automatic
  backspin "check" for shots from over 63 yd on dry short grass, cut to a tenth when the spin
  stick was used. The original reuses nine float variables for many jobs; the C now mirrors
  that reuse.
  were rerun (same conclusions). Left: `fn_80052598` (bounce, saved-register choice across nine
```
