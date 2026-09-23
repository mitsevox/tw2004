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
  run of values) or `fn_8001F08C` (swap by a field-format table), within four calls; the path is
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
| `Cnet` | fn_8000BF9C | asm | none seen | asm; dispatches the course's sub-chunks to the loaders registered with `Course_RegisterLoader` (below) |
| `txf ` | fn_80010180 | asm | none seen | asm; `TXG ` texture groups, [formats/txg-textures.md](formats/txg-textures.md) |
| `sfn ` | fn_800125BC | sweep | none seen | asm |
| `SAC ` | fn_8001A75C | sweep | swapped: AnimLib_MergeOverlay > fn_80020BC8 > fn_80076158 | little-endian on disc |
| `CLB ` (2) | fn_8001CCF8, fn_8001CD80 | asm | swapped: fn_8001B208 > fn_80076158 | little-endian on disc |
| `CHR ` (2) | fn_8001CE5C, fn_8001D020 | asm | swapped: fn_80019798 > fn_80076158 | little-endian on disc |
| `SKLO` | fn_8001D3EC | sweep | swapped: fn_8001A9F4 > fn_80076158 | little-endian on disc |
| `MAL ` | fn_8001FA3C | sweep | swapped: fn_8001F804 > fn_8001F08C | little-endian on disc |
| `SAL ` | AnimLib_OnLoaded | skalib.c | swapped: AnimLib_Load > fn_8001F08C | little-endian on disc; swapped by field tables (`SwapField`) |
| `BNK ` | ClipBank_OnLoaded | skalib.c | swapped later | the handler only stashes the file; `ClipBank_Install` > `ClipBank_Load` swaps it (`ClipBank_SwapHeader`, fn_80020BC8 per clip) |
| `stat` | Golfer_OnStatsLoaded | Golfer.c | swapped: Golfer_TableByteSwap > fn_80076158 | yes: copied over `gGolferTable[34]` (`GolferRecord`); only 0x98..0x140 of each record is swapped, in 8-byte units; the u32 at 0x90 is not |
| `rcrd` | Session_OnRecordsLoaded | Golfer.c | none seen | yes: copied straight over `gSession.aCourseRecord` |
| `ter ` | fn_800342B4 | GoTerrain.c | none seen | asm |
| `tgd ` | fn_800342F0 | GoTerrain.c | none seen | yes: the course's collision data; `Ter_InitTGD` (fn_8004B1EC, GoTerrainCollision.c) lays `CourseInfo` (ball.h) over it and turns its offsets into pointers in place (`TER_RELOCATE`, 32-bit); `TerCell`, `TerPolyRef`, `TerObject` and the vertex list are read in place |
| `tLOD` | fn_800341A4 | GoTerrain.c | none seen | asm |
| `CAMS` | fn_80039554 | GoDynamicCam.c | swapped: fn_80076158 | little-endian on disc |
| `CAMV` (2) | fn_80039690, fn_80039754 | GoDynamicCam.c | swapped: fn_800399E0 > fn_8001F08C | little-endian on disc |
| `CAMA` | fn_800397EC | GoDynamicCam.c | swapped: fn_80039A48 > fn_8001F08C | little-endian on disc |
| `TEO ` | fn_80045F74 | sweep | none seen | asm |
| `BALL` | fn_80045FC8 | asm | none seen | asm |
| `Cact` | fn_80048BDC | UKernel.c | none seen | asm |
| `CAMC` | fn_800644F4 | GoStaticCam.c | swapped: fn_8001F08C | little-endian on disc |
| `sscr` | SitDev_LoadScripts | SitDevFile.c | swapped: fn_800BB52C > fn_8001F08C | little-endian on disc |
| `BIO ` | fn_80076F80 | FE_Manager.c | none seen | asm |
| `LITE` | fn_8008D9DC | FEgolferanim.c | swapped: fn_8001F08C | little-endian on disc |
| `DATS`, `TXFS`, `FONS`, `GRPS`, `MPCS` | fn_8008ED80, fn_8008EE1C, fn_8008EFFC, fn_8008EEB8 (two tags) | uiLoadFile.c | none seen | asm |
| `MCI `, `MCB ` | fn_8009EB30, fn_8009EB38 | MC_Gc.c | none seen | the handlers only keep the object |
| `eagm` | fn_800A1D4C | MC.c | none seen | asm |
| `sfxd` | fn_800A29B4 | asm | none seen | asm |
| `LEGL` | fn_800B166C | startUp.c | none seen | asm |
| `TEO ` | fn_800B99FC | sweep | none seen | asm (a second `TEO ` handler) |
| `BALF` | fn_800B99BC | sweep | none seen | asm |
| `TRAX`, `TRXT` | fn_800BA118, fn_800BA15C | sweep | none seen | asm |
| `CRI `, `CMPS` | fn_800D2A64, fn_800D2A90 | sweep | none seen | asm |
| `ERN ` | fn_800D344C | Earnings.c | none seen | yes: copied over the prize table `lbl_80200538` (`EarningsTable`) |
| `PLY ` | fn_800EAEEC | GameMode5.c | none seen | yes: copied over `lbl_80203554` |
| `PLYs` | fn_800EAF18 | GameMode5.c | none seen | bytes: a string block, copied |
| `PGAc`, `PGAt`, `PGAp` | fn_800EDF34, fn_800EDF60, fn_800EDF90 | GameModeDriverPGATour.c | none seen | yes: copied over `gPgaData.aTournament`, `.aTourEvent`, `.aTriple` |
| `PGAn` | fn_800EDFC0 | GameModeDriverPGATour.c | none seen | bytes: names, copied |
| `RTEc`, `RTEs` | fn_800F05B0, fn_800F05DC | GameModeDriverRTE.c | none seen | yes: copied over `gRTEs.aEvent`, `.aChallenge` |
| `RTEn` | fn_800F060C | GameModeDriverRTE.c | none seen | bytes: names, copied |
| `TCM ` | fn_8010237C | GameMode4.c | none seen | yes: copied over `lbl_802124B8` |
| `TCMS` | fn_801023A8 | GameMode4.c | none seen | bytes: text, copied |
| `CR_A` | fn_80105188 | FE_CrAPDB.c | swapped: fn_80105DAC > fn_8001F08C | little-endian on disc |
| `CR_S` | fn_801051F4 | FE_CrAPDB.c | none seen | asm |
| `PGST` | fn_80117694 | sweep | none seen | asm |
| `gras` | fn_8011E584 | GoGrass.c | none seen | asm; GoGrass.c has a swapping function (fn_8011E4D8) the handler does not reach within four calls: probably swapped later (inferred) |
| `EASI` | fn_80124B10 | sweep | none seen | asm |

The course file's sub-chunks (`Cnet`)
-------------------------------------

`Course_RegisterLoader(n, fn)` adds a loader to the table (`lbl_801A2A00`) that the `Cnet` handler
walks; each loader gets its sub-chunk's bytes.

| Sub-chunk | Loader | Unit | Swap | Overlay |
|---|---|---|---|---|
| 0: AI targets | AI_TargetsLoad | Golfer.c | none seen | yes: `AITargetDef` (golfer.h) laid over the chunk, count an s16 at +2; the loader also writes into the chunk (a self-link becomes -1); `gAITargets[i].pDef` points into it |
| 1: out-of-bounds outlines | fn_8004B63C | GoTerrainCollision.c | none seen | yes: `TNetwork` (ball.h) pointers into the chunk |
| 4: free-drop outlines | fn_8004B588 | GoTerrainCollision.c | none seen | yes: `TNetwork` pointers into the chunk |

The memory card
---------------

| Data | Code | Swap | Overlay |
|---|---|---|---|
| the save profile (`SaveProfile`, include/game/save.h, 0x10600 bytes, with raw replays) | MC_Gc.c: `CARDReadAsync` (fn_8009DD94), `CARDWriteAsync` (fn_8009E130), `CARDRead` (fn_8009F208) | none seen: nothing in MC_Gc.c or MC.c calls a swap routine | yes: read and written as the struct's bytes (`gpSaveData`) |

Other byte-order facts
----------------------

- SharedFileIO.c:101: `gSFIODefaultDescriptor` is a big-endian BMP header (128x128, 8-bit).
- `stwbrx` (a byte-reversed store) appears at 0x80056248 and 0x8005638C (the picture decoder after
  Ball.c) and 0x800B96C8 (rcmp_mad_codec.c): those write little-endian data (asm, not looked into).
- Other callers of the swap routines, not reached from a handler above: AnimStream.c (fn_800CB8F0),
  Skeleton.c (fn_80028564), Skin*.c (fn_80036278 ..), CharSliders.c
  (CharSlider_CreateDefinitionsFromMem), GoGrass.c (fn_8011E4D8).
