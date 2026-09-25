# GR_BuildGridRenderData (GoGreenGrid.c, 0x8009C0BC)

Status: OPEN, 97.42% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

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
