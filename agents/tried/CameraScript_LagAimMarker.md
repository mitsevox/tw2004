# CameraScript_LagAimMarker (gocamscripts.c, 0x80041A24)

Status: SOLVED 2026-09-26 (r7-cam): the reciprocal `fB = 1.0f / fn_8001EFFC(...)` and the first
length `fA = fn_80009680(...)` as locals declared after fMin (vregs below it, few neighbours, so
the allocator removes them before it: fMin 31 remaining neighbours at its first visit), with the
float locals in the order fFrames, fDiv, fDist, fOldY, fAimY, fAngle, fMin, fA, fB. Exact.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-26, r7-cam: rasim replay + what-if study (scratch lag_study.py). EA's colouring needs fMin
  coloured LAST (after fAimY and fAngle): fMin must leave the graph in the first sweep, declared
  after fAimY/fAngle (lower vreg), with <= 31 remaining neighbours when visited (FPR K = 31). It had
  33, all 14 temps inherent to the final code (none removable), and no declaration order alone
  works (0 of 5040). Only a local with a lower vreg that leaves the graph first counts less: one
  length local fA gave 32; fA plus a second local (the second length, or the reciprocal fB) gave
  1008 exact orders in the replay. Built: second length as local 4 diffs (its temps' registers);
  reciprocal as local: exact. One-expression `fMin = fF0 * (1/x) * -vDir[1]` puts the load after
  the calls (5 diffs); `fMin = fMin * (1/x) * -vDir[1]` splits fMin's web (10). heightDiff local
  `fH = vGoal[1] - fAimY` is copy-propagated away by the frontend.

- 2026-09-26, r6-args: the late parameter copy through void* (`T p = (T)(void*)pArg;`, the fn_800AB860 fix) on each pointer parameter, declared first or last: pShot 11, pSub 23, pCam 32. Permuter 15 min -j2 (12.9k iterations, base 65): nothing better.

- 2026-09-26, r6-args: fMin's 33 neighbours = 14 volatile + fMinDist/fYShare/fAimY/fOldY/fDiv + 14 temps
  (f55-f59 the `*=` chain and vDir[1] = 0, f62 the first length's frsp, f63-f68 the zero tests, f71/f73/f74/f76
  the vDir[1] expression). One length local for both length reads (new fLen: fMin still 33 nb, 17; into fDist
  or fAngle 19; fFrames 12), plus a heightDiff local `fH = vGoal[1] - fAimY`: same scores. Base 11.

- 2026-09-25, n-const: merged every pair of locals/params with disjoint lives (fFrames, fRate, fMin, fAimY, fMinDist into fMin/fAimY/fDist/fAngle/fFrames; 15 single merges and all 4-distinct pairs of them): none below 11 (base 11).

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
gocamscripts CameraScript_LagAimMarker
base 11, 223 levers, 37830 variants (223 singles) in 352 s; best 11

11 [safe]
  - move `f32 vGoal[4];` to line 1 of the declarations

11 [safe]
  - move `f32 vAim[4];` to line 11 of the declarations

11 [safe]
  - move `f32 vAim[4];` to line 12 of the declarations

11 [safe]
  - move `f32 fFrames;` to line 5 of the declarations
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/cloud-2026-09-24-round1.txt

```
- gocamscripts CameraScript_LagAimMarker (11): fMinDist reused as fDist 15; fAngle/fAimY one variable 11;
  both 15; fMin/fAimY/fAngle/fDist reusing fFrames/fRate/fMinDist 11-48; declaration climb: nothing.
```

### agents/notes/map-02-notes_w10.txt

```
  fn_8003F2E0 (8), CameraScript_LagAimMarker (11): no version changes anything (3.0a3 far worse).
- CameraScript_LagAimMarker (11): EA's colour webs {fMinDist, fDist} f30, {fAimY, fAngle} f27, fMin f26;
  ours {fMin, fDist} f30, {fMinDist, fAngle} f27, fAimY f26.
```

### agents/notes/map-02-notes_w8.txt

```
- CameraScript_LagAimMarker (11, fMinDist/fMin/fAimY rotate f30/f26/f27): fAimY/fMin block-scoped (inner if,
  outer if), fMin first/last, fMinDist copied to a local, fMin split/one-expression, f64 fAimY: none.
```

### agents/notes/map-06-notes_w7.txt

```
- CameraScript_LagAimMarker: not retried (TW07 params: lagAmount before the two bools; tried by map-10).
```

### agents/notes/map-10-notes_w4.txt

```
- CameraScript_LagAimMarker: TW07 local order (arrays match EA order), param order (fRate first), fLen/fDY
  locals, decl climb: none below 11.
```

### agents/notes/map-10-notes_w6.txt

```
  the compare's 0.0f register). CameraScript_LagAimMarker: no fAimY, fMin one-liners, f64 temps: worse.
```

### agents/state.md

```
fn_800D477C/fn_800D4F14, gocamscripts CameraScript_LagAimMarker/fn_8003F2E0.
```
- 2026-09-26 PC declsearch (run 36221888505, iterated local search over the declaration order): best 11 (no better order than the current one), 11468 trials.
- 2026-09-26 r5-world (mwcc-debugger): the float variables are all above 28 neighbours; priority
  fDiv, fMin (33 nb), fOldY, fDist, fYShare, fMinDist. fMin goes second and takes f30, so
  fMinDist falls to f27 and fAimY to f26. EA's colouring (fMin f26 after fAimY f27, fMinDist f30)
  needs fMin at a low priority: 28 or fewer neighbours (5 fewer: 14 volatile regs + fMinDist,
  fYShare, fAimY, fOldY, fDiv + 14 temps f55-f76 of its `*=` chain, the three zero tests and
  the vDir[1] expression). Not tried further this round.
