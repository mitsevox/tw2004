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

## auto_04_80183C78_rodata (3568 B): lbl_80183C78, lbl_80184268 -> rcmp_mad_codec.c

- Contents: lbl_80183C78 (MadCode[95], 0x5F0) and lbl_80184268 (MadCode[128], 0x800); the
  block ends where rcmp_mad_codec.c's .rodata (lbl_80184A68) begins. Only user: rcmp_mad_codec
  (the table builder); llpict.h already had the const externs.
- Done: both defined in rcmp_mad_codec.c ahead of lbl_80184A68 (values from the DOL); the unit's
  .rodata now starts at 0x80183C78. main.dol: OK.

## auto_05_80186838_data (16 B): lbl_80186838 (a zero f32[4]) -> urandom.c

- Users: Skeleton (fn_80029BC8 copies it; GoLighting calls that), nothing in the files around it.
- Position: after UMemPool.c's .data ("UMemPool.c" string, ends 0x80186833), before LoadData.c's.
- Tried in UMemPool.c: at the top it lands before the string; after UMemPool_Create it follows the
  string but at 0x80186834 (4-aligned inside the same section), so Skeleton's reference is off by
  4 (doldiff 0x80029BDF). The original starts a new 8-aligned section: another file. The files
  in between are urandom.c and ObjList.c (no other .data); nothing picks one, so urandom.c (the
  next in link order), with a section note. explicit_zero_data as in uiobject.c's zero vector.
- Result: main.dol: OK. Owner between urandom.c and ObjList.c is a guess (noted in the code).
- Later (see the next block): data 8-aligned after a file-name string in .data is a recurring
  pattern (TibExt.c, GoRenderCtx_Gc.c), so UMemPool.c + aligned(8) is an equal candidate; the
  section note in urandom.c says so.

## auto_04_80185158_rodata (8 B): dropped (lane b2 linked it into runtime/locale.c)

## auto_04_801861A0_rodata (108 B): skipped, UIStudio.c

- lbl_801861A0 (the "Attempting to activate screen ..." message) and lbl_80186200 ("UIStudio.c"),
  both used only by UIStudio. Not this lane's (UIStudio.c is still being matched).

## auto_05_801869B8_data (288 B): skipped, LLFont.c

- 8 zero bytes, then lbl_801869C0 (0xC0), lbl_80186A80 (0x4C) and lbl_80186ACC (a 9-byte string),
  all used only by LLFont. Not this lane's (LLFont.c is still being matched).

## auto_05_80186AF0_data (232 B): lbl_80186AF0 -> GoRenderCtx_Gc.c

- One object, u32[1][58] (engine.h's extern: [][0xE8 / 4]); only user GoRenderCtx_Gc
  (fn_800142AC). Follows GoRenderCtx_Gc.c's .data (its "GoRenderCtx_Gc.c" string), ends where
  streammanagerhole.c's begins.
- First try: plain definition before fn_800142A4 -> lands at 0x80186AEC (4-aligned after the
  string), doldiff 0x800142BF. EA's is 8-aligned: labelled aligned(8), the same fake as TibExt.c's
  lbl_80194758 (the functions from fn_800142A4 on were sweeps and may be another file).
- Result: GoRenderCtx_Gc .data 0x80186AD8..0x80186BD8. main.dol: OK.

## auto_05_80186CC8_data (1064 B): IK chain tables and club part names -> char.c

- Contents: lbl_80186CC8 / lbl_80186D54 / lbl_80186E1C (IKLinkDef[7], [10], [5]; 0x14 each, only
  referenced from lbl_80186E80/EA0), lbl_80186E80 / lbl_80186EA0 (IKChainDef[2]; char.c's
  lbl_80280E10/E18 point at them), and the seven char[6][13] club part tables lbl_80186EC0 ..
  lbl_801870A0 (80-byte stride), all used only by char. Ends where char.c's .data (0x801870F0)
  began; the units in between (Code80015470, Code80016198, ViewController) have no .data.
- First try: defined after the sweep declarations -> char_tex_manager.c's pooled strings
  ("_usrtextr", used by the first function) come first, doldiff 0x80017346 (70F0 -> 6CC8).
  Defined before the #include of char_tex_manager.c they lead the .data: main.dol OK. Data-order
  note in the code.
- Result: char.c .data 0x80186CC8..0x801871D0. main.dol: OK.
- For the audit: char.c's file comment still says the unity's .data starts at 0x801870F0 (now
  0x80186CC8); not changed here (matching lanes do not edit comments).
