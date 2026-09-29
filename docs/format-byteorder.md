Byte order of the game's data formats
=====================================

Every format the game reads from disc or the memory card, with its byte order and whether the code
lays a struct straight over the bytes. This is the list a PC port (little-endian, maybe 64-bit) has
to handle: each "none seen" format is read as the GameCube's native big-endian, and each struct
overlay needs a byte-swap (and, for pointers, a layout change) at load time on such a machine.

How the columns were filled (2026-09-23):

- **Handler**: every `UStream_RegisterHandler` call in the DOL (63; tag and function recovered from
  the call's `r3`/`r4` in the assembly, C or asm alike).
- **Swap**: *swapped* = the handler reaches one of the two byte-swap routines, `fn_80076158` (swap a
  run of values) or `ByteSwap_Records` (swap by a field-format table), within four calls; the path is
  given; "little-endian on disc" below is inferred from that swap. *none seen* = no such call within four calls and no `lwbrx`/`lhbrx`. A "none seen" format
  is used as big-endian as far as the code shows; a deferred swap further away (as for `BNK `) is
  possible, so "none seen" is not proof.
- **Overlay**: *yes* = the C copies the bytes into a struct or reads them through a struct pointer
  (a port must swap each field); *bytes* = text or byte data, order-free; *asm* = the handler is not
  in C yet, not checked. Only formats with C handlers are marked yes/bytes.

The stream container
--------------------

`.hog` / `.gcb` / `.ngc` files, read by UStream.c (format: [formats/ctrl-container.md](formats/ctrl-container.md)).

| Part | Where | Byte order | Overlay |
|---|---|---|---|
| chunk header (tag, length, sub-tag, SHDR fields) | `UStream_ParseChunks`, `UStream_BeginObject` (`UStreamChunk`, include/ustream.h) | big-endian, read through the struct (a `port:` note in `UStream_ParseChunks` marks where a port converts the 0x40 bytes) | yes: `UStreamChunk`; the SHDR fields from 0x14 are copied into `UStreamObject` (engine.h) |
| `SDAT` payload | `UStream_ParseChunks` | the object's own format (below) | copied unchanged |
| `Rdat` payload | `UStream_Decompress` | piece size: big-endian u32, read with `BE32` (include/endian.h); command words read a byte at a time, order-free | no |
| `SONO` sound (`shdr` / `samp`) | `UStream_ParseChunks` | header fields big-endian, through `UStreamChunk`; sample data handed to the audio code as-is | yes: `UStreamChunk` |
| `SWVR`, `MPG2`, `DSPM`/`VAGM`/`XADP` | `UStream_ParseChunks` | passed on to the movie/audio code (asm, not checked); the `SWVR` test on a read buffer uses `BE32`, the `RPNS` base value too | asm |

Objects delivered by UStream
----------------------------

| Tag | Handler | Unit | Swap | Overlay / notes |
|---|---|---|---|---|
| `txf2` | fn_8000BCA0 | asm | none seen | asm |
| `load` | fn_8000BA94 | asm | none seen | asm |
| `Cnet` | Network_DownloadDataPNB | asm | none seen | asm; dispatches the course's sub-chunks to the loaders registered with `Network_RegisterLoadNetworkCallback` (below) |
| `txf ` | fn_80010180 | asm | none seen | asm; `TXG ` texture groups, [formats/txg-textures.md](formats/txg-textures.md) |
| `sfn ` | fn_800125BC | sweep | none seen | asm |
| `SAC ` | Character_LoadSacFromStream | char.c (sweep block) | swapped: AnimLib_MergeOverlay > SKA_SwapClip > fn_80076158 | little-endian on disc; `port:` notes at the handler and at the swap in AnimLib_MergeOverlay (a little-endian port does not swap) |
| `CLB ` (2) | Character_ClubStreamCallbackIG, Character_ClubStreamCallbackFE | char.c (asm) | swapped: Character_CreateClubSkinSet > fn_80076158 | little-endian on disc |
| `CHR ` (2) | Character_GolferStreamCallbackIG, Character_GolferStreamCallbackFE | char.c (asm) | swapped: Character_LoadTextures > fn_80076158 | little-endian on disc |
| `SKLO` | SkeletalObject_StreamCallback | char.c (sweep block) | swapped: Character_CreateFromMem > fn_80076158 | little-endian on disc; a `port:` note at the handler |
| `MAL ` | MtaLib_OnLoaded | sweep | swapped: MtaLib_LoadBank > ByteSwap_Records | little-endian on disc |
| `SAL ` | AnimLib_OnLoaded | skalib.c | swapped: AnimLib_Load > ByteSwap_Records | little-endian on disc; swapped by field tables (`SwapField`); yes: `AnimLib`, `ClipRecord` and `Clip` are then read in place, their offsets turned into 32-bit pointers. `port:` notes at every swap call (header, clip numbers, index, records, tree nodes): a little-endian port does not swap there |
| `BNK ` | ClipBank_OnLoaded | skalib.c | swapped later | the handler only stashes the file; `ClipBank_Install` > `ClipBank_Load` swaps it (`ClipBank_SwapHeader`, SKA_SwapClip per clip); yes: `ClipBank` is used in place, its clip offsets turned into 32-bit pointers. `port:` notes at the swap calls |
| `stat` | Golfer_OnStatsLoaded | Golfer.c | swapped: Golfer_TableByteSwap > fn_80076158 | yes: copied over `gGolferTable[34]` (`GolferRecord`); only 0x98..0x140 of each record is swapped, in 8-byte units; the u32 at 0x90 is not. A `port:` note at the swap call |
| `rcrd` | Session_OnRecordsLoaded | Golfer.c | none seen | yes: copied straight over `gSession.aCourseRecord`, big-endian; a `port:` note there (a little-endian port converts the records field by field) |
| `ter ` | Ter_HoleDataLoadCallback | GoTerrain.c | none seen | asm |
| `tgd ` | Ter_CourseLoadCallback | GoTerrain.c | none seen | yes: the course's collision data; `Ter_InitTGD` (Ter_InitTGD, GoTerrainCollision.c) lays `CourseInfo` (ball.h) over it and turns its offsets into pointers in place (`TER_RELOCATE`, 32-bit); `TerCell`, `TerPolyRef`, `TerObject` and the vertex list are read in place |
| `tLOD` | Ter_LODLoadCallback | GoTerrain.c | none seen | asm |
| `CAMS` | DynamicCam_LoadCAMSfromStream | GoDynamicCam.c | swapped: fn_80076158 | little-endian on disc |
| `CAMV` (2) | DynamicCam_LoadCAMVfromStream, DynamicCam_LoadCAMVfromStreamFE | GoDynamicCam.c | swapped: DynamicCam_CopyScriptData > ByteSwap_Records | little-endian on disc |
| `CAMA` | DynamicCam_LoadCAMAfromStream | GoDynamicCam.c | swapped: DynamicCam_CopyAnimPairData > ByteSwap_Records | little-endian on disc |
| `TEO ` | DynObj_LoadTeoModel | sweep | none seen | asm |
| `BALL` | DynObj_LoadBallLogos | asm | none seen | asm |
| `Cact` | Kernel_DownloadActors | UKernel.c | none seen | asm; its type-10 objects go to PlayNow_LoadBallSpot (PlayNowMode.c), which reads a challenge's ball spot in place through `ChallengeSpotRecord` (`port:` note there) |
| `CAMC` | StaticCam_LoadCAMCfromStream | GoStaticCam.c | swapped: ByteSwap_Records | little-endian on disc |
| `sscr` | SitDev_LoadScripts | SitDevFile.c | swapped: SitDev_SwapTables > ByteSwap_Records | little-endian on disc |
| `BIO ` | FE_CharBios_LoadBIOfromStream | FE_Manager.c | none seen | asm |
| `LITE` | FE_lite_vStreamCallback | FEgolferanim.c | swapped: ByteSwap_Records | little-endian on disc |
| `DATS`, `TXFS`, `FONS`, `GRPS`, `MPCS` | UI_StreamLoadFile, UI_StreamLoadTextures, UI_StreamLoadFonts, UI_StreamLoadPictures (two tags) | uiLoadFile.c | none seen | asm |
| `MCI `, `MCB ` | fn_8009EB30, fn_8009EB38 | MC_Gc.c | none seen | the handlers only keep the object |
| `eagm` | fn_800A1D4C | MC.c | none seen | asm |
| `sfxd` | fn_800A29B4 | asm | none seen | asm |
| `LEGL` | Startup_LoadLegalPicture | startUp.c | none seen | asm |
| `TEO ` | FE_CrAPBall_LoadTEO | sweep | none seen | asm (a second `TEO ` handler) |
| `BALF` | FE_CrAPBall_LoadBALF | sweep | none seen | asm |
| `TRAX`, `TRXT` | UI_vEATraxLoadfromStream, UI_vEATraxLoadLogoFromStream | sweep | none seen | asm |
| `CRI `, `CMPS` | GM_CourseInfo_LoadCRIfromStream, GM_CourseInfo_LoadCMPSfromStream | sweep | none seen | asm |
| `ERN ` | EarningsInfo_LoadERNFromStream | Earnings.c | none seen | yes: copied over the prize table `lbl_80200538` (`EarningsTable`, include/game/earnings.h); a `port:` note in the handler marks where a port converts it |
| `PLY ` | PlayNow_LoadPLYFromStream | PlayNowMode.c | none seen | yes: copied over `gPlayNowChallenges` (`Challenge[83]`, include/game/modes/challenge.h); `port:` note in the handler |
| `PLYs` | PlayNow_LoadPLYsFromStream | PlayNowMode.c | none seen | bytes: a string block, copied |
| `PGAc`, `PGAt`, `PGAp` | fn_800EDF34, fn_800EDF60, fn_800EDF90 | GameModeDriverPGATour.c | none seen | yes: copied over `gPgaData.aTournament`, `.aTourEvent`, `.aTriple` (`Tournament`, `TourEvent`, `PgaTriple`, include/game/modes/pgatour.h); a `port:` note in each handler |
| `PGAn` | fn_800EDFC0 | GameModeDriverPGATour.c | none seen | bytes: names, copied |
| `RTEc`, `RTEs` | fn_800F05B0, fn_800F05DC | GameModeDriverRTE.c | none seen | yes: copied over `gRTEs.aEvent`, `.aChallenge` (`RTEvent`, `Challenge`, include/game/modes/rte.h); a `port:` note in each handler |
| `RTEn` | fn_800F060C | GameModeDriverRTE.c | none seen | bytes: names, copied |
| `TCM ` | fn_8010237C | LadderedMode.c | none seen | yes: copied over `lbl_802124B8` (`LadderEvent[25]`, LadderedMode.c); `port:` note in the handler |
| `TCMS` | fn_801023A8 | LadderedMode.c | none seen | bytes: text, copied |
| `CR_A` | FE_CrAP_LoadAssetsFromStream | FE_CrAPDB.c | swapped: CrAPAssetsByteSwap > ByteSwap_Records | little-endian on disc |
| `CR_S` | FE_CrAP_LoadStringsFromStream | FE_CrAPDB.c | none seen | asm |
| `PGST` | PGATourSimulation_LoadPGSTFromStream | sweep | none seen | asm |
| `gras` | Grass_LoadStreamFile | GoGrass.c | none seen | asm; GoGrass.c has a swapping function (Grass_LoadNetworkData) the handler does not reach within four calls: probably swapped later (inferred) |
| `EASI` | fn_80124B10 | sweep | none seen | asm |

The course file's sub-chunks (`Cnet`)
-------------------------------------

`Network_RegisterLoadNetworkCallback(n, fn)` adds a loader to the table (`lbl_801A2A00`) that the `Cnet` handler
walks; each loader gets its sub-chunk's bytes.

| Sub-chunk | Loader | Unit | Swap | Overlay |
|---|---|---|---|---|
| 0: AI targets | AI_TargetsLoad | Golfer.c | none seen | yes: `AITargetDef` (golfer.h) laid over the chunk, count an s16 at +2 (read with `BES16`, include/endian.h); the loader also writes into the chunk (a self-link becomes -1); `gAITargets[i].pDef` points into it. A `port:` note at the overlay |
| 1: out-of-bounds outlines | Ter_OOBNetworkLoadCallback | GoTerrainCollision.c | none seen | yes: `TNetwork` (ball.h) pointers into the chunk |
| 4: free-drop outlines | Ter_FreeDropNetworkLoadCallback | GoTerrainCollision.c | none seen | yes: `TNetwork` pointers into the chunk |

The memory card
---------------

| Data | Code | Swap | Overlay |
|---|---|---|---|
| the save profile (`SaveProfile`, include/game/save.h, 0x10600 bytes, with raw replays) | MC_Gc.c: `CARDReadAsync` (MC_LoadFile), `CARDWriteAsync` (fn_8009E130), `CARDRead` (MC_ReadFile) | none seen: nothing in MC_Gc.c or MC.c calls a swap routine | yes: read and written as the struct's bytes (`gpSaveData`). The load and store are in MC_Gc.c (asm, lane C); `include/game/save.h` says a port reads and writes the profile field by field |

Other byte-order facts
----------------------

- SharedFileIO.c:101: `gSFIODefaultDescriptor` is a big-endian BMP header (128x128, 8-bit).
- `stwbrx` (a byte-reversed store) appears at 0x80056248 and 0x8005638C (the picture decoder after
  Ball.c) and 0x800B96C8 (rcmp_mad_codec.c): those write little-endian data (asm, not looked into).
- Other callers of the swap routines, not reached from a handler above: AnimStream.c (SKA_UnpackSwappedName),
  Skeleton.c (fn_80028564), Skin*.c (SKN_SwapMeshEntries ..), CharSliders.c
  (CharSlider_CreateDefinitionsFromMem), GoGrass.c (Grass_LoadNetworkData).
