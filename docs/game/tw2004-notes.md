Tiger Woods PGA Tour 2004: the binary
=====================================

What is known about this game's executable as a whole: how it's laid out, what EA built it from,
and a few systems worth knowing before reading the code. General CodeWarrior lessons are in
[`decomp-notes.md`](../compiler/decomp-notes.md), the compiler evidence in
[`compiler.md`](../compiler/compiler.md), and the data formats in [`formats/`](formats/README.md).

The game
--------

- Game ID `GW4E69`, USA, revision 0, two discs. Shipped September 2003 by EA Redwood Shores.
- `main.dol` is byte-identical on both discs (SHA-1 `bbbc55485e51973931dee169e7bf87bc7379223f`),
  1.62 MiB: 7,647 functions, 1.54 MB of code, 1.08 MB of data. No REL modules and no symbol map.
- Written in C. Every source name EA left in the binary ends in `.c`, and there are no C++
  exception tables.
- Dolphin SDK build of Sep 5 2002, with the CARD (memory card) library patched Apr 2 2003. Metroid
  Prime revision 0 uses the same SDK build.

Memory map
----------

| Range | Contents |
|-------|----------|
| `0x80003100` | `.init`: start-up, `memcpy`, `memset`, the MetroTRK vector table |
| `0x80005520` - `0x8012E950` | EA's game and engine code (CodeWarrior GC/2.x, `-O4`) |
| `0x8012E950` - `0x801654D0` | Dolphin SDK, MSL C library, MetroTRK v2.0 |
| `0x801654D0` - `0x8016C718` | EA Tiburon's UI Studio library (`UIStudio.c`, `UISEvent.c`...: the menu script engine), CodeWarrior `-O4` |
| `0x8016C718` - `0x80175F54` | EA's shared save-file library, built **unoptimised with GCC** (ProDG) |
| `0x8017DB60` | `.rodata` (assert strings, jump tables) |
| `0x801866E0` | `.data`; then `.bss` at `0x8019D540`, `.sdata` at `0x80280DA0`, `.sbss` at `0x80281B80`, `.sdata2` (float constants) at `0x80282A60` |

The SDK and runtime
-------------------

Nintendo's SDK, the MSL C library and MetroTRK are built from other public GameCube decompilations
(`extern/`, each with its credits): Metroid Prime's SDK and MSL, Pikmin 2's MetroTRK, and pieces
from Final Fantasy Crystal Chronicles, The Wind Waker, Twilight Princess, Sonic Heroes and
Gauntlet: Dark Legacy. Their functions were first named by matching them byte for byte against
those projects' split objects (`tools/research/match_sdk_names.py`: a name is taken only when the
match is unique both ways).

Two differences from Prime's SDK are visible in the code:

- **The April 2003 CARD patch.** `__CARDIsWritable` and `__CARDIsReadable` take the control block
  (`card, ent`) and replace the `__CARDAccess` + `__CARDIsWritable(ent)` pair; `CARDWriteAsync`,
  `CARDFastDeleteAsync` and `CARDRenameAsync` call `__CARDIsWritable`, and `CARDReadAsync` calls
  `__CARDIsReadable`. Prime's source has these spots under `CARD_PATCH_2003`.
- **MSL's `printf`** is a different revision (`vprintf`, `float2str`, `double2hex` differ).

Source file names
-----------------

EA's asserts name 94 source files; [`filemap.md`](../evidence/filemap.md) pins each one to the
functions that name it, in link order. The prefixes group them:

| Prefix | What | Files |
|--------|------|-------|
| `LL` | the low-level platform layer | `LLDisp_Gc` `LLDynTex` `LLFileIO_Gc` `LLFont` `LLObj_Gc` `LLPict_Gc` `LLPictInt` `LLTex` `LLTexGrp` `LLVideo` |
| `U` | utilities and the kernel | `UAudMemStack` `UFont` `UFstPart` `UKernel` `UMemPool` `UObject` `UObject3D` `UStream` `UISEvent` `UIStudio` |
| `Go` | the golf engine: cameras, renderer, game objects | `GoARAM` `GoBreakLine` `GoCamera` `GoCamTuningVars` `GoComicCam` `GoDynamicCam` `GoDynObj` `GoEntry` `GoFrameBuf` `GoGolfCam` `GoGrass` `GoGreenGrid` `GoLighting` `GoPostFx` `GoRenderCtx_Gc` `GoRenderSurface` `GoStaticCam` `GoTerrain` `GoViewport` |
| `GoShaderObject` | shader objects | `_Glows_Gc` `_Particle_Gc` `_PrelitUVAnimation_Gc` `_Rain_Gc` `Common_DynamicRenderingBuffer_Gc.h` `Common_MorphAnimManager_Gc` `Common_ShaderObjectsData_Gc` `Common_TexAnimManager_Gc` `Container_OBFData_Gc` |
| `hws` | materials and hardware shading | `hwsBurn` `hwsMaterial_Gc` `hwsOverride_Gc` `hwsRender_Gc` |
| skinning | characters and animation | `Skeleton` `Skin` `SkinBurn` `SkinMorph` `SkinPart` `AnimStream` `DynChain` `CharSliders` `char` `mtalib` `skalib` |
| `FE_` / `ui` | the front end and menus | `FE_CrAPDB` `FE_LogoDesign` `FE_Manager` `FE_PGATourMessages` `FEgolferanim` `uiLoadFile` `uiProcessInterface` `uiTransform` `EASportsBio` |
| gameplay | | `YhSwing` `PsBallFx` `PsMgr` `Replay` `shadow` `SunFlr_Gc` `TibExt` `WPvi` `user` `4startUp` |
| storage, audio, other | | `MC_Gc` (memory card) `dvdfs` `SitDevFile` `crcmp_mad_codec` `GCN_Mem_Alloc` `gbacable` `GBAXfer` `GBA` `dvd` `OSThread` |
| the save library (GCC) | | `ChecksumCRC32.c` `SharedFileIO.c` `TagFile.c` `llSharedFileIO.c` (under `../../../Source/Common/` and `Source/NGC/`), plus an XOR cipher module with no asserts |

`_Gc` marks EA's GameCube version of a file; the TW2003 Xbox build has the same files as `_Xbox`
(see [`reference/README.md`](../../reference/README.md) for EA's folder layout).

Culling and the widescreen fix
------------------------------

`fn_80007D74` is the sphere-against-view-frustum test every drawable goes through, course scenery
included. It returns 1 (fully visible), 2 (outside), 4 (touching a side) or 8 (touching the near
limit), with a separate path for a flat, non-perspective camera that only answers 2 or 4. Its
callers only ask "was it 2?"; `LLObj_GetMeshClipMethod` retries a 4 or 8 against a second, looser
set of planes.

`RC_vUpdateRenderCtxScreenMatricesAndInfo` builds the planes whenever the view changes: the
tangent of half the field of view and its reciprocal at `cam+0x224`/`0x228`, the half-extents of
the view at `0x1FC` (horizontal) and `0x200` (vertical), one pair of edge planes from those
(`0x204`-`0x210`), and a second pair from the extents doubled (`0x214`-`0x220`, the loose set).
The near and far limits are at `0x1F4`/`0x1F8`, the projection matrix at `0x5C` and the combined
matrix at `0xDC`.

Because the planes are built for 4:3, Dolphin's widescreen hack shows objects popping in and out in
the outer eighth of the screen on each side. This Gecko code treats "outside" on the left and right
planes as "touching", and was confirmed in play (normal play, flyovers, replays, both discs):

```
$Widescreen Culling Fix          left / right "outside" (2) becomes "touching the edge" (4)
04007EC0 3BE00004                was 3BE00002 (li r31, 2)
04007EF0 3BE00004                was 3BE00002
```

A native 16:9 fix would scale the horizontal extent at `0x1FC` by 4/3 in
`RC_vUpdateRenderCtxScreenMatricesAndInfo`.

The save-file library
---------------------

The GCC-built block is EA's cross-platform memory-card save library, not an asset loader:

- `SharedFileIO.c` (`SFIO`): device enumeration (the two memory-card slots), save names, icons,
  directories; `llSharedFileIO.c` is its GameCube CARD layer.
- `TagFile.c`: the save container (below).
- `ChecksumCRC32.c` guards the data and the XOR cipher scrambles it.
- The game calls in through `TibExt.c`: an assert stub, and the allocator pair `TibExtMemAlloc` /
  `TibExtMemFree`.

The library's own conventions show in its asserts: Hungarian names (`u` unsigned, `e` enum, `p`
pointer, `b` BOOL, `s` short), `BOOL` is a byte, and `SFIO_ASSERT(cond)` expands to
`{ u8 bSkip = 0; if (!((cond) | bSkip)) Assert(__FILE__, __LINE__, #cond, 0); }`.

### The save file format

A save is a sequence of records. Each is a 12-byte header and its payload, padded to the device's
block size (8 KiB on the memory card):

    u32 uTag        record identifier
    u32 uSize       payload size in bytes
    u32 uChecksum   CRC32 of the payload

An unused header slot holds the 12-byte string `"TAG SENTINEL"`. Headers and payloads are
XOR-ciphered with a key the game passes to `TagFile_Init`.

In memory the library keeps a map, an array of `{tag, offset, size, checksum}` built as records are
written or, on load, by walking the file header by header. `TagFile_End` stores the CRC32 of the
map; on load it is recomputed and a mismatch is error 9. Records with the same tag are addressed by
index (`TagFile_Read(pSession, uTag, uIndex, ...)`).

Error codes: 1 already initialised, 2 not initialised, 4 bad parameter, 5 map full, 6 out of
memory, 7 tag not found, 8 busy, 9 bad map checksum, 0xA bad payload checksum, 0xB bad header,
0x6E size mismatch. Errors from the layers below are offset: SFIO + 100, cipher + 200, checksum + 300.

### The save profile

`gpSaveData` points to five profiles, one per player slot, 0x10600 bytes each (`SaveProfile` in
`include/game/save.h`):

| Offset | Field | What it is |
|---|---|---|
| 0x0 | `bActive` | 1 when the slot holds a profile |
| 0x1 | `szName` | the profile's name (compared with the all-time record holders) |
| 0x1C | `aGolferUnlocked[30]` | per golfer |
| 0x3A | `aCourseUnlocked[23]` | per course |
| 0x51 | `aRewardUnlocked` | per reward, up to 0x70 |
| 0x70 | `b70` | set when an award is won, a round counted or a challenge started; cleared at round setup |
| 0xC8 | `aC8[31]` | one per PGA TOUR tournament, 8 bytes each; byte 0 is 1 once it is won |
| 0x20C | `aRTEAward[75]` | the real-time events' awards (4 bytes: won flag, the day) |
| 0x338 | `aLadderAward[25]` | the ladder events' awards; the earnings rating counts the won ones |
| 0x39C | `aAward[39]` | the other awards |
| 0x438 | `aReplay[5]` | saved replays (0xF28 bytes each) |
| 0x5000 | `nTourCardLevel` | 0..6: level 1 from the lessons, the rest from `GM_Earnings_PayRoundGoals`; scales payouts |
| 0x5004, 0x504C | `a5004[71]`, `a504C[71]` | per marked hole (`UserInfo_GetPar5EagleStat`), continued at 0x10578 / 0x1057C for holes 71..74 |
| 0x516C | `aMedal[29]` | the best challenge medal per group (0 best, 3 none), and the day at 0x51E4 |
| 0x5230 | `aSavedRound[3]` | three saved custom rounds, 0x70 bytes each |
| 0x54C2 | | the created golfer kept in this slot: its names at 0x54C8, outfit 0x54F8, ball 0x54F9 |
| 0xB634 | `tour` | the PGA TOUR season (TW06's `PGATourSeason_t`: 31 tournaments of 0x24 bytes) |

A second block with the same layout, `gpPasswordUnlocks`, holds unlocks that apply to every profile.
Two cheat codes fill it: `"THEKITCHENSINK"` unlocks all 30 golfers, 23 courses and 18 rewards and
sets TOUR card level 1; `"ALLTHETRACKS"` unlocks the courses.

Loading data from disc
----------------------

Everything on the disc is read through `UStream.c`, which unpacks EA's chunked `CTRL` container
(`.hog`, `.gcb`) and hands each object to the handler registered for its tag. The container and
its `Rdat` compression are described in [`formats/ctrl-container.md`](formats/ctrl-container.md);
`tools/formats/ctrl_dump.py` extracts every object on the disc.
