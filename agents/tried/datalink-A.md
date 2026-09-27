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

## auto_05_801871D0_data (528 B) + auto_07_80280E78_sdata (504 B, lane D's; orchestrator agreed): bone names -> mtalib.c

- Contents: lbl_80187278, char*[90] (the bone names, last NULL; Skeleton.c reads it), its nine
  9-byte names in .data just before it (0x801871D0..) and its 81 short names in .sdata
  (0x80280E78..0x80281070). One definition with string literals makes all three.
- Owner: not char.c (the char lane: its "IGdriver" and "" are separate copies despite -str
  reuse). .data order is char.c | table | "mtalib.c" (0x801873E0), .sdata order char.c | names |
  skalib.c: the only file between them in both is mtalib.c, whose "mtalib.c" string follows the
  table (a file-scope definition precedes the literals of later functions, as seen in UMemPool.c).
- Done: defined at the top of mtalib.c; mtalib .data 0x801871D0..0x801873F0, .sdata
  0x80280E78..0x80281070. main.dol: OK on the first try.

## auto_05_801894D0_data (144 B): the unlock lists -> FE_Manager.c

- Contents: lbl_801894D0 (s32[6], courses), lbl_801894E8 (s32[16], golfers), lbl_80189528
  (s32[14], golfers); users GameManager, PasswordManager, FE_MessageTable (externs in fe.h and
  game/save.h).
- Position: after GoShaderObjectCommon_TexAnimManager_Gc.c's .data (its file-name string, then
  8-alignment), right before FE_Manager.c's "FE_Manager.c" at 0x80189560; the two units are
  adjacent in .text too, so it is one of them. Front-end unlock lists: FE_Manager.c, defined at
  file scope ahead of its functions, so they lead its .data.
- Result: FE_Manager .data 0x801894D0..0x801896F0. main.dol: OK on the first try.

## auto_05_80189CB0_data (24 B): lbl_80189CB0 -> goballfx.c

- u8[6][4] (lighting.h), only user goballfx (BFX marker colours). goballfx.c lies between
  uiTransform.c and GoShaderObject_Particle_Gc.c in .text, and the block between their .data.
- Result: goballfx .data 0x80189CB0..0x80189CC8. main.dol: OK.

## auto_05_80189DA8_data (192 B): lbl_80189DA8 -> SunFlr_Gc.c

- f32[6][8] (glows.h), only user SunFlr_Gc; sits right before SunFlr_Gc.c's "SunFlr_Gc.c" string
  (0x80189E68), after UFstPart.c's .data (BootCourse.c, between them in .text, has no .data).
  Defined ahead of the functions so it leads the file's .data.
- Result: SunFlr_Gc .data 0x80189DA8..0x80189E78. main.dol: OK.

## auto_05_8018C7C8_data (96 B): lbl_8018C7C8 + lbl_8018C7D8 -> MC.c

- lbl_8018C7D8: MCOpSet[4] (memcard.h), the memory-card operations: 15 of its 20 entries are
  MC.c functions (plus four EASportsBio.c ones and a NULL); users FE_MessageTable, startUp.
  lbl_8018C7C8: u32[4] GX primitive kinds (engine.h), only user DynamicRenderingBuffer.
- Position: after MC_Gc.c's .data ("BASLUS-20572", ends 0x8018C7C5), before MC.c's
  ("../BASLUS-20757" at 0x8018C828); 0x8018C7C8 is 8-aligned, so both can open MC.c's .data.
  lbl_8018C7C8's owner is not proven (MC_Gc.c after its strings would also fit); it goes with
  its neighbour into MC.c.
- Done: both defined at the top of MC.c (with prototypes of the table's functions, and core/easb.h
  for two EASportsBio ones); entries whose definitions have other types are cast to MCOp, with a
  port: note. main.dol: OK.

## auto_05_8018FFC8_data (4064 B): DiscError.c's .data (font generated at build time)

- Contents: 24 zero bytes (unreferenced), the font lbl_8018FFE0 (u32[107 * 8]), the colours
  lbl_80190D40 (DiscColor[9]), 4 zero bytes, the 15 English messages and their table
  lbl_80190F6C; DiscError.c's .data (its switch table) followed at 0x80190FA8. All users DiscError.
- Orchestrator decision: the font bitmap is game art, so its bytes never enter git.
  tools/build/gendata.py now also writes initializer fragments (FRAGMENTS) into
  build/GW4E69/gen from the user's main.dol; DiscError.c #includes DiscError_font.inc between the
  braces; the game cflags (configure.py cflags_base) and lint.py's compile get -i build/GW4E69/gen;
  the fragments are outputs of the existing pre-compile gendata step.
- Tries: (1) the 24 zero bytes as an explicit_zero_data u32[6]: the linker strips it (nothing
  references it; everything after shifted by 0x18). config.yml force_active: [lbl_8018FFC8] keeps
  it. (2) the 4-byte gap before the messages: a 4-byte zero object goes to .sdata; instead the
  first message is a named char array with aligned(8) (labelled fake match), the rest literals.
- Result: DiscError .data 0x8018FFC8..0x80190FE0. main.dol: OK; `git ls-files` lists no .inc.

## auto_05_8018F040_data (3672 B): the boot sounds' ADPCM data -> startUp.c (generated)

- lbl_8018F040 (0x600) and lbl_8018F640 (0x858), the two BootSounds' samples; only user startUp
  (its lbl_8018FE98 table points at them and follows them directly). Game data: defined in
  startUp.c with the initializers #included from gendata.py fragments (startUp_sound0/1.inc).
- Result: startUp .data 0x8018F040..0x8018FF68. main.dol: OK.
- For the audit: startup.h's comment on these externs ("split before startUp.c's") is now stale.

## auto_05_8018A028_data (9888 B): Code8009AA28's raw blobs -> Code8009AA28.c (generated)

- lbl_8018A028 (u8[0x4B0]) and lbl_8018A4D8 (0x21F0), stored by Code8009AA28 (fn at line ~108:
  p1928/p192C); directly after its lbl_80189E78, up to GoGreenGrid.c's .data. Parked in state.md as
  probable textures; per the orchestrator's decision defined in Code8009AA28.c with gendata.py
  fragments (Code8009AA28_tex0/1.inc).
- Result: Code8009AA28 .data 0x80189E78..0x8018C6C8. main.dol: OK.
