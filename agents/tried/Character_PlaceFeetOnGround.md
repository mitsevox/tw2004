# Character_PlaceFeetOnGround (char.c, 0x800187CC)

Status: OPEN, 99.05% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
char Character_PlaceFeetOnGround
base 7, 146 levers, 19362 variants (146 singles) in 170 s; best 3

3 [safe]
  - identity inline on `pPos = pChar->pModel->pBones[0].v1C;`
  fake match: EA did not write an identity wrapper. Look in TW07 (docs/reference-builds/tw07-ps3/cu/) for a real helper or macro at this spot and use it; otherwise name it fn_<caller address>_Read with a `// fake match:` comment.

3 [safe]
  - identity inline on `pPos = pChar->pModel->pBones[0].v1C;`
  - move `CourseInfo* pCourse;` to line 6 of the declarations
  fake match: EA did not write an identity wrapper. Look in TW07 (docs/reference-builds/tw07-ps3/cu/) for a real helper or macro at this spot and use it; otherwise name it fn_<caller address>_Read with a `// fake match:` comment.

3 [safe]
  - identity inline on `pPos = pChar->pModel->pBones[0].v1C;`
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-10-notes_w8.txt

```
- char Character_PlaceFeetOnGround (orig loads fLow into f1 and copies it into fY with fmr, then tests
  fLow; ours tests fY): if-forms, else-forms, negated ternaries, whole-block rewrites: 33 or worse; 200
  random decl orders: none better.
```

### agents/notes/nm-a-notes_cloud2.md

```
## char Character_PlaceFeetOnGround: 7 -> 3 (identity inline on pPos, fn_800187CC_Read)
```
