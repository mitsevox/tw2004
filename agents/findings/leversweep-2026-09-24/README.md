# Lever sweep, owner's PC, 2026-09-24 night (first 53 of 80 functions)

`python tools/match/leversweep.py --from-report --min 95 -j 20` on the PC's 20 cores, from main at
7e3e9549, so with the lever set from BEFORE 543bf7bc: no ternary-temp, x/2.0f-as-multiply or
field-address levers yet. A re-run with the new levers is worth doing on the functions below.
`summary.tsv` has one row per function (unit, function, base score, best score, EXACT, lever class,
levers); `<fn>.txt` has each function's best variants and the changed lines. Scores are quicktrial's
differing instructions. These are candidates: apply the change in src/, confirm it in the real build,
label fake matches (agents/brief.md). The rest of the 80 comes in a later branch.

Exact, applied on this branch: SkinPart fn_800CEB1C (`int i;` declared first, nList `s32` -> `int`;
both safe, not a fake match).

Improved, not exact (closest first; most use an identity inline, so they would be fake matches):

| unit | function | base -> best |
|---|---|---|
| hlaudtrackseq | fn_800AA9EC | 8 -> 2 |
| hwsBurn | fn_8011172C | 10 -> 2 |
| char | Character_PlaceFeetOnGround | 7 -> 3 |
| hwsBurn | fn_80110FB4 | 6 -> 5 |
| SkinPart | fn_800CE52C | 14 -> 5 |
| LLFont | fn_80011D0C | 9 -> 7 |
| UISApi | fn_80168FC8 | 15 -> 7 |
| SkinPart | fn_800CE8C0 | 26 -> 10 |
| UISScreen | fn_8016B4D4 | 46 -> 26 |
| GoTerrainCollision | Ter_CheckObjectAndHazardObstruction | 41 -> 31 |
| GoTerrain | fn_80031154 | 59 -> 31 |
| GoGreenGrid | fn_8009B970 | 43 -> 37 |
| GoGreenGrid | fn_8009BE08 | 43 -> 40 |
| Ball | Physics_HandleCollision | 92 -> 82 |

The other rows: no lever combination beat the base score. For those, read the row as "every single
lever and pair tried" and add it to the function's tried notes.
