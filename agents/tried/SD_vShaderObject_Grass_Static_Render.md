# SD_vShaderObject_Grass_Static_Render (GoShaderObject_Grass_Gc.c, 0x80120F74)

Status: OPEN, 96.78% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
GoShaderObject_Grass_Gc SD_vShaderObject_Grass_Static_Render
base 74, 699 levers, 42608 variants (699 singles) in 360 s; best 55

55 [safe]
  - move `GrassWord* pVert = pData->apVerts[nSet][nRun];` to line 26 of the declarations
  - move `int nPass;` to line 7 of the declarations
  - move `f32 fPerX;` to line 17 of the declarations

55 [safe]
  - move `GrassWord* pVert = pData->apVerts[nSet][nRun];` to line 26 of the declarations
  - move `int nPass;` to line 7 of the declarations
  - move `f32 fPerX;` to line 18 of the declarations

55 [safe]
```

## Collected from the notes and docs (2026-09-25)

Nothing recorded.
