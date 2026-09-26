# Physics_HandleCollision (Ball.c, 0x80052598)

Status: SOLVED 2026-09-26 (r4-ball, commit "Ball.c: Physics_HandleCollision exact (99.30 -> 100)";
Ball.c linked in the next commit). Fix: start from round 3's "every web its own local" template at
its 28 order, then (1) the end-of-function turns compute the cosine into the angle's own variable
(`a = -head; s = sin(a); a = cos(a);`): 28 -> 22; (2) greedy merges of webs whose line spans do not
overlap (logic unchanged), alternating with declaration climbs: 22 -> 6; (3) merges that do not
raise the score (to shrink the local count) found the last ones: all three turns back share one
angle/cosine local and one sine local, and that sine local is also the bounce factor: 6 -> 0.
Rule seen: a call result assigned to a local with other assignments goes straight into its register;
into a local assigned once it goes through f0 (`fmr f0,f1 ... fmr fN,f0`). 26 float locals (was 16).

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-26 r4-ball (quicktrial aligned, base 29): round 3's dot copy through fT (32) with a
  declaration climb + random restarts: no gain (32). Second turn's sine/cosine/vDir[2] in every
  pairing of fS/fRest/fCo/fSi/fT/fLen, nested E1C in E10 or not: all 29 (or 32 with the dot copy).
  Split template at the 28 order: end turns `a = cos(a)` 22; greedy safe web merges + climbs: 6
  (w_t, w_y2 -> bent length; x -> dot; y -> surface +0x28 temp); merging the end sines with fC/head
  or the angles with fD: 24-114, not kept; score-neutral merges: end angles into one local, end sines
  into one, then into the bounce factor: 0 (exact, objdiff 100).

- 2026-09-26 r2-modes (quicktrial aligned, base 92): greedy "reuse an existing local that is dead over
  this live range" search (scratch hc4.py: every web x every other float local, only non-overlapping
  ranges): 92 -> 39, kept 9 renames (spin clamp fT -> fD; vDir[2] fRest -> fS; second turn's sin fCo
  -> fRest; third turn's sin/cos fSi/fCo -> fBounce/fGrip; rough scale fSi -> fBite; random pass fD ->
  fBounce; soil fLen -> fRest; end fC -> fRest), then declaration moves: fSpeed after fImpact 39 -> 31.
  Objdiff 98.74 -> 99.28 (committed, labelled fake match). Adding 4 fresh locals to the same greedy: 31.
  Identity inline wrappers on each sin/cos/atan2 call (15 sites, greedy): none better. GC/2.0 92,
  2.0p1 94, 2.6/2.7 92.
- 2026-09-26 r2-modes, finding: EA's float registers are ONE register per ORIGINAL variable: fA f31 (soil
  factor and bank angle), fSpeed f30, fImpact f29, fRest f28 (vDir[2] and the restitution), fB f27,
  fC f26 (all three webs), fD f25, fCo f24 (first cos, second sin, third cos), fSi f23 (spin clamp,
  second cos, third sin). That is what `#pragma opt_lifetimes off` produces (no live-range splitting):
  on the original C (before the renames) it gives 171; with declarations fA, fSpeed, fImpact, fRest, fB,
  fC, fD, fCo, fSi: 78; plus spin-clamp fSi -> fScale, second turn's fT -> fS, spin-clamp fS -> fresh
  local: 22 aligned, every register right. What is left under the pragma: EA moves the call results of
  fC/fD/fT (both turns' fn_80055E1C/E10, the atan2 of the heading, the three end-of-function sin/cos)
  straight into their register, ours goes through f0 (`fmr f0,f1 ... fmr f26,f0`); sin/cos into
  fB/fCo/fSi go through f0 in both. `register` on any subset: no change. Objdiff of the 25 version
  98.60 (worse than 99.28 because of the f0 copies), not kept. Without the pragma the end block's
  direct moves match, so EA is probably not simply "lifetimes off"; but whatever EA wrote colours
  every variable into one register.
- 2026-09-26 r2-modes: randomised restarts of the rename + declaration-move search from the original C
  (22 min, 13 restarts, no pragma): basins 29-71, best 29 aligned with 13 renames (not kept: 2 fewer than
  the committed 31, much less readable). The rename space alone does not reach 0.

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
- 2026-09-26 round 3 (r3-ball; written by the orchestrator from the lane report, the disk was full): kept (ed333ee): fGrip = pSurface->f10 * fabsf(vSlip[1]); fGrip *= 0.3f; (and the same for the other grip) fixes both fmuls operand swaps: 31 -> 29 aligned, 99.28 -> 99.30. EA's fmr f4,f1 ... fmr f25,f4 at the dot product is reproduced without the pragma by a copy through a second local (fT = fn_8000C5FC(...); fD = fT;): 32, fD lands in f23; its declaration climb was cut off. No gain: an inline sin/cos helper (fn_80055E28 shape, TW07 mySinCos) 31; an identity inline on the dot, fImpact through an inline square/length or vSlip 31-52; other spellings of the 0.3f multiply 30-48. Every web as its own local (43 locals, scratch split_tmpl.c): 163 in appearance order, one climb reached 28 (scratch split1.out; not checked in objdiff). Pragma lead: on round 2's 22 variant, nesting the second turn's E1C call inside E10 gives 20, but under the pragma every call result assigned to a local still goes through f0, and it scores the same on GC 1.3.2-2.7: EA probably did not simply use opt_lifetimes off; not recommended.
