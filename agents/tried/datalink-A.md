# Data linking, lane A (b1, 2026-09-27)

Blocks from agents/assign/2026-09-27-datalink.md "Lane A". One entry per block: owner, evidence,
result.

## auto_04_8017E9B8_rodata (10608 B): gSurfaceTypes -> new data-only unit MaterialTypes.c

- Contents: one object, gSurfaceTypes, 156 rows of SurfaceType (0x44 bytes each; 156 * 0x44 =
  0x2970, the whole block).
- Users: Ball, GoTerrainCollision (44 references), GameManager, GameEffects, target, stateFunc,
  SitDevStateVector, GoGolfCam, GoGreenGrid, HoleScore, SwingTips, GameMode12 and more.
- Position: after GoTerrainCollision.c's code, right before Ball.c's .rodata (0x80181328).
- Why not GoTerrainCollision.c: the table is const (.rodata), but every user, GoTerrainCollision.c
  included (`return &gSurfaceTypes[...]` into a SurfaceType*), reads it as non-const. Making the
  extern const is an error in those files (mwcc: const to non-const pointer). So EA's defining file
  did not see the users' extern: a data-only file. TW06 has golf/physics/materialtypes.c and TW07
  Golf/Physics/MaterialTypes.c (TGD_MaterialInfo, TW06's name for this row); TW06's physics files
  in link order are materialtypes, physics (= Ball.c), wind (= Wind.c), which puts it exactly
  between GoTerrainCollision.c and Ball.c.
- Done: src/MaterialTypes.c (const table, values from the DOL), SurfaceType moved verbatim from
  ball.h to a new include/materialtypes.h (ball.h includes it) so MaterialTypes.c need not see
  ball.h's non-const extern; unit added to configure.py after GoTerrainCollision.c and to
  splits.txt (.rodata 0x8017E9B8..0x80181328). main.dol: OK.

## auto_04_80183578_rodata (104 B): gClubCurve -> Swing.c

- Contents: gClubCurve, s32[26] (0x68, the whole block). Only user: Swing (Swing.c declared it
  extern). Position: between PasswordManager.c's and stateFunc.c's .rodata, as Swing.c's .data
  lies between fe_craputils.c and stateFunc.c.
- Done: the extern in Swing.c became the const definition; .rodata 0x80183578..0x801835E0 added
  to Swing.c. main.dol: OK.
