# CameraScript_LagAimMarker (gocamscripts.c, 0x80041A24)

Status: OPEN, 99.78% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

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
