# GR_BuildGridRenderData (GoGreenGrid.c, 0x8009C0BC)

Status: OPEN, 98.81% on 2026-09-26 (r2-render).

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)
- 2026-09-25, n-shaders: first loop's fAlong/fHeight/fAcross orders (6) x nRow/nCol order:
  fAcross, fAlong, fHeight (as the second loop has it) 156 -> 140 (nRow/nCol order: no effect).
  Then the fn_8009C914 fix (an s32 nViewCopy for the GOLFERSTATE call's fn_8001707C): declared
  last 140 -> 117, declared first 131. Real 97.42 -> 97.67 (kept). fDirX..fCornerZ declared
  before fPrev/fHole: no change; decl climb: none. Left: the four dir/corner floats get
  f20-f23 (orig f28-f31), nView is kept in r29 in orig, gSession base vs gSession+0x24 hoisted.
- 2026-09-26, r2-render: `Session* pSession = &gSession;` for both nFrameCount reads (fake
  match): aligned 117 -> 80, real 97.67 -> 98.27 (kept; fixes the hoisted base). Without the
  nView copy then: 140. No gain: fHole as a literal in the compares / set once (117); plain
  s32 nFrameCount (121); fresh locals for the second loop's fX/fY/fZ (116), + fAcross/fAlong/
  fHeight (117-120), fU/fV (122), the ints (120). Left: dir/corner floats f20-f23 vs orig
  f28-f31 (orig gives the two hoisted constants f27/f26), and the int registers that follow.
  TW07 (GR_BuildGridRenderData, drifted a lot: bool showDots, unsigned row/col) declares
  playerNum, nx, ny, cx, cy first as consts.
- 2026-09-26, r2-render: const-initialised dir/corner locals (at the top or in place): 88;
  the dir/corner loads moved among the opening statements (7 positions + 150 random orders):
  80 at best; fX/fZ statements swapped: 80. A move climb of the 25 declarations: 80 -> 72;
  random declaration orders (300) then a move/swap climb: 80 -> 44, real 98.27 -> 98.81 (kept;
  the dir/corner floats now get f31-f28 as in the original). A second 400-order random run and
  a further 8-minute climb from 44: no better. With that order, dropping the nView copy or
  pSession: 116 / 84. Left: the preamble keeps nView in r3 (orig `mr r29,r3`), and the fX/fY/fZ
  / fU float registers.

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
GoGreenGrid GR_BuildGridRenderData
base 156, 638 levers, 30493 variants (638 singles) in 361 s; best 135

135 [review]
  - identity inline on `nCol = n / lbl_802813C0->anRows[nView];`
  - move `int n;` to line 24 of the declarations
  - do/while loop #1 as a while loop
  fake match: EA did not write an identity wrapper. Look in TW07 (docs/reference-builds/tw07-ps3/cu/) for a real helper or macro at this spot and use it; otherwise name it fn_<caller address>_Read with a `// fake match:` comment.

135 [review]
  - identity inline on `nCol = n / lbl_802813C0->anRows[nView];`
  - move `int n;` to line 24 of the declarations
  - do/while loop #2 as a while loop
  fake match: EA did not write an identity wrapper. Look in TW07 (docs/reference-builds/tw07-ps3/cu/) for a real helper or macro at this spot and use it; otherwise name it fn_<caller address>_Read with a `// fake match:` comment.
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-09-notes_w6.txt

```
GoGreenGrid GR_BuildGridRenderData 97.4: aligned climb 24 went 156 -> 138 with a scrambled order; stopped at
  the checkpoint, not applied (log w6_gr_climb.log).
```
