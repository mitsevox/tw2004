Tiger Woods PGA Tour 2004: what we know
=======================================

Everything learned about this specific game. General CodeWarrior / GameCube lessons live in
[`decomp-notes.md`](decomp-notes.md), compiler evidence in [`compiler.md`](compiler.md), and
file formats in [`formats/`](formats/README.md).

Guesses are labelled as guesses. Rename and correct freely as evidence comes in.

The game
--------

- Game ID `GW4E69`, USA, Revision 0, two discs. Shipped September 2003.
- `main.dol` is byte-identical on both discs (SHA-1 `bbbc55485e51973931dee169e7bf87bc7379223f`),
  1.62 MiB, 7,646 functions, 1.54 MB code, 1.08 MB data. No REL modules. No symbol map on either disc.
- Written in **C**. Every leaked source name ends in `.c`, and there are no C++ exception tables.
- Dolphin SDK build Sep 5 2002 (CARD library patched Apr 2 2003). This is the same SDK build
  Metroid Prime Rev 0 uses; 76% of that project's SDK functions are byte-identical here, so its
  SDK source is reusable. The `GBA` library is present but none of its functions matched (different
  version); `thp` (video) and `dtk` (disk track player) were not found.

Memory map of `main.dol`
------------------------

| Range | Contents |
|-------|----------|
| `0x80003100` | `.init`: startup, `memcpy`, `memset`, TRK vector table |
| `0x80005520` - ~`0x8012FFFF` | EA game and engine code (CodeWarrior GC/2.x, `-O4`) |
| ~`0x80130000` - `0x8016C717` | Dolphin SDK, MSL C library, MetroTRK v2.0 |
| `0x8016C718` - `0x80175F54` | EA shared file library, 124 functions, **GCC unoptimized** (`src/Common/...`, ProDG) |
| `0x8017DB60` | `.rodata` (assert strings, jump tables) |
| `0x801866E0` `.data`, `0x8019D540` `.bss`, `0x80280DA0` `.sdata`, `0x80281B80` `.sbss`, `0x80282A60` `.sdata2` (float constants) | |

The boundary between EA code and the SDK is approximate: it is where dtk's first automatically
named SDK function appears.

SDK and runtime names
---------------------

423 SDK / MSL functions were named by matching them byte for byte against Metroid Prime Rev 0's
split objects (`tools/research/match_sdk_names.py`). Only linker-filled bytes are masked, a name is
used only if the match is unique both ways, and two self-checks passed: all 180 functions dtk had
already named agreed, and all 1,906 calls between matched functions pointed at the expected names.
12 were skipped because the name is shared by several static functions (`OnReset`, `AlarmHandler`,
`WriteCallback`, `EraseCallback`). By library: gx 128, card 72, dvd 47, runtime 42, os 32, pad 20,
si 19, exi 13, dsp 11, ar 9, mtx 6, other 36.

The SDK starts earlier than first thought: `VIWaitForRetrace` is at `0x8012F314`.

**SDK source (Level 0).** `extern/sdk` is the Metroid Prime project's SDK / MSL source. **57 units
(162 functions, 51,376 bytes) are linked from it and match.** Compilers: GC/1.2.5n for Dolphin
libraries, GC/1.3 for MSL, flags in `cflags_sdk` / `cflags_runtime`. The pipeline is in
`tools/research/sdk/` (run in the order given in each script's header). What is left of Prime's 154 units:

- 4 fully matching on code but blocked on data with no anchor (`fstload`, `EXIBios`, `GXPixel`,
  `OSError`): their `.data` is referenced only from other data. Would need a byte-pattern search.
- 27 partially matching: a few functions per unit differ from Prime's source (this game's SDK patch
  level or MSL revision), e.g. `OSCache` 12/22, `ai` 17/23, `runtime.c` 11/14, `mem_funcs`, `printf`,
  the `__ieee754_*` math. Each needs per-function work; not automated.
- The rest are libraries this game does not link or SDK parts Prime never decompiled.

Beyond that, Level 0 is done as far as borrowed source goes.

Leaked source file names
------------------------

Found in assert strings. **All 94 are now pinned to addresses in [`filemap.md`](filemap.md)**, in
link order. Meanings of the prefixes are guesses.

| Prefix | Guess | Files |
|--------|-------|-------|
| `LL` | low-level platform layer | `LLDisp_Gc` `LLDynTex` `LLFileIO_Gc` `LLFont` `LLObj_Gc` `LLPict_Gc` `LLPictInt` `LLTex` `LLTexGrp` `LLVideo` |
| `U` | utility / kernel | `UAudMemStack` `UFont` `UFstPart` `UKernel` `UMemPool` `UObject` `UObject3D` `UStream` `UISEvent` `UIStudio` |
| `Go` | game objects, cameras, renderer | `GoARAM` `GoBreakLine` `GoCamera` `GoCamTuningVars` `GoComicCam` `GoDynamicCam` `GoDynObj` `GoEntry` `GoFrameBuf` `GoGolfCam` `GoGrass` `GoGreenGrid` `GoLighting` `GoPostFx` `GoRenderCtx_Gc` `GoRenderSurface` `GoStaticCam` `GoTerrain` `GoViewport` |
| `GoShaderObject` | shader objects | `_Glows_Gc` `_Particle_Gc` `_PrelitUVAnimation_Gc` `_Rain_Gc` `Common_DynamicRenderingBuffer_Gc.h` `Common_MorphAnimManager_Gc` `Common_ShaderObjectsData_Gc` `Common_TexAnimManager_Gc` `Container_OBFData_Gc` |
| `hws` | materials / hardware shading | `hwsBurn` `hwsMaterial_Gc` `hwsOverride_Gc` `hwsRender_Gc` |
| `Skin` / skeleton | character skinning, animation | `Skeleton` `Skin` `SkinBurn` `SkinMorph` `SkinPart` `AnimStream` `DynChain` `CharSliders` `char` `mtalib` `skalib` |
| `FE_` / `ui` | front end, menus | `FE_CrAPDB` `FE_LogoDesign` `FE_Manager` `FE_PGATourMessages` `FEgolferanim` `uiLoadFile` `uiProcessInterface` `uiTransform` `EASportsBio` |
| gameplay | | `YhSwing` `PsBallFx` `PsMgr` `Replay` `shadow` `SunFlr_Gc` `TibExt` `WPvi` `user` `4startUp` |
| storage, audio, misc | | `MC_Gc` (memory card) `dvdfs` `SitDevFile` `crcmp_mad_codec` `GCN_Mem_Alloc` (MSL) `gbacable` `GBAXfer` `GBA` `dvd` `OSThread` (SDK) |
| EA shared library (GCC block) | | (+ an XOR cipher module with no asserts, `src/Common/Cipher/CipherXOR.c`) `../../../Source/Common/Checksum/ChecksumCRC32.c` `../../../Source/Common/SharedFileIO/SharedFileIO.c` `../../../Source/Common/TagFile/TagFile.c` `../../../Source/NGC/SharedFileIO/llSharedFileIO.c` |

Camera and visibility code (`0x80007BC4` - `0x800083A4`, fully matched)
--------------------------------------------------------

File unknown. It is **not** `GoCamera.c` (that is at `0x80076408`, see `filemap.md`); it sits
between `LLObj_Gc.c` and `GoEntry.c`. Types are in `include/unsorted/cull.h`.

| Address | Name | What it does | Status |
|---------|------|--------------|--------|
| `0x80007BC4` | `fn_80007BC4` | Move an object's bounding sphere into camera space, optionally report its depth, scale its radius, cull it | matched |
| `0x80007C80` | `fn_80007C80` | Move a point into camera space (`viewMtx * point`) | matched |
| `0x80007CE8` | `fn_80007CE8` | Same as `fn_80007BC4` without the depth output | matched |
| `0x80007D74` | `fn_80007D74` | **Sphere vs view frustum test.** Returns 1 fully visible, 2 not visible, 4 touching a side edge, 8 touching the near limit. Separate path for a flat (non-perspective) camera, which only returns 2 or 4 | matched |
| `0x800080D0`, `0x800081C4` | empty functions | Do nothing. Called from `fn_8006D8E8` and `fn_8006C854` | matched |
| `0x800080D4` | `fn_800080D4` | Clears two fields of the global at `0x80280DA0`, then eight `GXSetVtxAttrFmt` calls: vertex formats 0 and 4 (position, color, two texture coords, normal) | matched |
| `0x800081C8`, `0x80008214` | mode dispatch | Store a mode index, look up a function table (16-byte entries at `0x80188E78`, functions at +8), call its first function; second one calls the optional second function | matched |
| `0x8000827C`, `0x80008248`, `0x800082CC` | type dispatch | Same idea for a type index: 0x44-byte entries at `0x80188E88`, functions at +0x24 (init, optional, third) | matched |
| `0x800082F8` | `fn_800082F8` | Get a drawable object's bounding sphere (`obj->data + 0x58`) | matched |
| `0x80008304` | `Vec3Copy` | Copy three floats. **Called 596 times from 110 functions** | matched |
| `0x80008320` - `0x80008368` | ten getters | Return camera floats `0x220` down to `0x1F4` | matched |
| `0x80008370`, `0x80008378` | getters | `cam->unk10`, then field 0 of that object; the cull test takes a different path when it is non-zero | matched |
| `0x80008380` | `fn_80008380` | Calls `fn_800070DC`. Called from 21 functions | matched |
| `0x800BAD60` | `fn_800BAD60` | 4x4 matrix times 4-float vector, uses paired-single math (hand-written assembly?) | not started |

The dispatch object stores its type index at offset `0x0`. The cull test reads offset `0x0` of
`cam->unk10` to choose perspective or flat, so these are probably the same object (unconfirmed;
the two structs are still declared separately).

`fn_80140590` is `GXSetVtxAttrFmt`: byte-identical to the one in Metroid Prime's SDK. Renamed in `symbols.txt`.

Camera fields (guesses from how the cull test uses them): `0x11C` view matrix (4x4 floats);
`0x1F4` / `0x1F8` near and far clip distances; `0x204`-`0x210` and `0x214`-`0x220` two sets of four
frustum edge-plane numbers, chosen by a mode argument (0 or 1). Why there are two sets is unknown
(two aspect ratios? a tight and a loose view?). `fn_80013EA0` also reads the two clip distances.

The camera's sub-object (`cam->unk10`): field `0x0` is 0 for a perspective camera and non-zero for a
flat one; `0xB4` / `0xB8` are the flat view's width and height (guess - the cull test halves them to
get the box edges).

Float constants for this file sit together in `.sdata2`: `1.0` at `0x80282A80` (owned by the three
matched functions), `0.5` at `0x80282A84` (the cull test; in the source it is `/ 2.0f`). Both now belong to `code_80007BC4.c`.

Frustum setup and widescreen codes
----------------------------------

`fn_80013950` (223 instructions, not yet decompiled) builds the camera's view volume each time it
changes. Read from the disassembly, not yet verified by matching:

- `cam+0x224` = result of `fn_80014280(0.5 * fov)` (looks like a tangent); `cam+0x228` = its reciprocal.
- `cam+0x1FC` and `cam+0x200` = half-extents of the view, built from `0x224` and viewport values.
  `0x1FC` feeds the planes the cull test uses with `x`, `0x200` the ones used with `y`.
- Each edge plane is made by normalizing `(1, extent, 0)` with `fn_800BAF04`; the two results are
  stored as a pair (set A: `0x204`/`0x20C` and `0x208`/`0x210`).
- **Set B (`0x214` - `0x220`) is the same thing with the extents multiplied by 2.0** (constant at
  `0x80282B7C`). So mode 1 of the cull test is a view twice as wide: a loose second-chance test.
- `cam+0x1F4` / `0x1F8` = near / far limits. Then it builds the projection matrix at `cam+0x5C`
  (`fn_8000ABE8` for perspective, `fn_8000AC5C` for flat), and a combined matrix at `cam+0xDC`.
- `fn_800977F8` (424 instructions) also writes both plane sets. Unexamined - maybe another camera type.

How callers use the cull result: all five call sites only ask "was it 2?". `fn_80007B2C` maps
2 -> 3, 1 -> 2, and for 4 / 8 runs the test again in mode 1.

**Widescreen Gecko codes (GW4E69 Rev 0, both discs). `$Widescreen Culling Fix` is CONFIRMED by an
A/B test on 2026-09-21** (Dolphin 2606a, Vulkan, widescreen hack on, stretch to 16:9):

- **Code off (hack only):** obvious pop-in. Trees, patches of grass and other objects cut out in the
  outer ~12.5% of the screen on each side - exactly the strip that lies outside the 4:3 view
  (4:3 covers the middle 75% of a 16:9 frame). Happens on any camera movement, not only flyovers.
- **Code on:** full 16:9 view, no pop-in, no glitches, in normal play, hole flyovers, replays, and
  across a disc swap. Both discs used.
- So the sphere test at `fn_80007D74` is the culling path for course scenery too (trees, grass), not
  just small objects, and `x` is the horizontal axis as assumed. No second culling path has shown up.
- Menus and HUD stay stretched, as with any widescreen-hack setup.
- All seven call sites pass mode 0 (the tight 4:3 planes) first; only `fn_80007B2C` retries in mode 1.
- `$Disable Object Culling` has not been tested; it was not needed.

```
$Widescreen Culling Fix          left / right "outside" (2) becomes "touching the edge" (4)
04007EC0 3BE00004                was 3BE00002 (li r31, 2)
04007EF0 3BE00004                was 3BE00002

$Disable Object Culling          whole test returns 4 without testing (enable only one code)
04007D74 38600004                li r3, 4
04007D78 4E800020                blr
```

Assumes `x` is the horizontal axis in camera space. The top / bottom pair is at `0x80007F2C` and
`0x80007F5C` if that turns out to be wrong. This test has only five call sites and works on bounding
spheres, so course scenery may be culled by a different path. If pop-in remains with a code on,
look at `fn_800977F8` and at code using the combined matrix at `cam+0xDC`. The cleaner long-term fix
is in `fn_80013950`: scale the horizontal extent (`0x1FC`) by 4/3, or change the aspect it feeds
to the projection matrix for a native 16:9 code that needs no widescreen hack.

What the GCC library actually is
--------------------------------

The assert strings settle it: the four `../../../Source/...` files are EA's cross-platform
**memory-card save library**, not the course/asset loader.

- `SFIO` = Shared File I/O: device enumeration (`SFIO_DEVICE_FIRST/LAST`, two memory-card slots),
  save names, icons, directories, `CARD_RESULT_READY` from the GameCube CARD SDK.
- `TagFile.c` (`src/Common/TagFile/TagFile.c`, 41 functions, done): the save-data container.
  See "Save file format" below.
- `ChecksumCRC32.c` guards the save data; the XOR cipher scrambles it.
- The host game calls in through an assert stub `fn_8012214C` (4 bytes in retail) and an allocator
  `fn_801220D4(pAllocator, size, align, __FILE__, __LINE__)` / `fn_80122128(...)`.

Save file format (from TagFile.c)
---------------------------------

A save is a sequence of **records**. Each record is a 12-byte header followed by the payload,
and the whole record is padded to the device block size (8 KiB on the memory card, 512 bytes
for buffer type 1, 1 byte for type 4 = memory):

    u32 uTag        record identifier (the game's tag values are not yet known)
    u32 uSize       payload size in bytes
    u32 uChecksum   CRC32 of the payload (ChecksumCRC32.c)

An unused header slot holds the 12-byte string `"TAG SENTINEL"` (`TAG_BUFFERSIZE` = 12; the
constant in the binary is the string three times over, 37 bytes, so the assert
`sizeof(TAG_SENTINEL) >= TAG_BUFFERSIZE` holds). Header and payload are XOR-ciphered
(CipherXOR.c) with a key the game passes to `TagFile_Init`; the key itself is copied out of the
init parameters, so it will be found in whichever CodeWarrior file calls `TagFile_Init`.

In memory the library keeps a **map**: an array of `{tag, offset, size, checksum}` (16 bytes
each, `_TagFile_pData->Map.pList`) built as records are written or, on load, as the file is
walked header by header (`TagFile_Update`, operation 4/5 = "delete/read map"). `TagFile_End`
stores the CRC32 of the map into the session block at +0x44; on the read side the library
recomputes it and returns error 9 if it differs. Records with the same tag are addressed by
index (`TagFile_Read(pSession, uTag, uIndex, ...)`).

Error codes: 1 already initialised, 2 not initialised, 4 bad parameter, 5 map full, 6 out of
memory, 7 tag not found, 8 busy, 9 bad map checksum, 0xA bad payload checksum, 0xB bad header,
0x6E size mismatch. Errors from the layers below are re-based: SFIO + 100, cipher + 200,
checksum + 300.

Boundary note: `llSharedFileIO.c` is `0x80171308`-`0x801730C8` (17 functions) and `TagFile.c`
`0x801730C8`-`0x80175F54` (41). The "40 / 50" in filemap.md were assert-site counts.

So decompiling it documents the **save-file format**, which is useful (save editors, understanding
`MC_Gc.c`), but the `.hog` / `.gcb` course formats live elsewhere: look at `LLFileIO_Gc.c`,
`dvdfs.c` callers, `UStream.c` and the `GoShaderObjectContainer_OBFData_Gc.c` loader instead.

Conventions learned from the asserts: Hungarian notation (`u` unsigned, `e` enum, `p` pointer,
`b` BOOL, `s` short), `BOOL` is a byte, macro `SFIO_ASSERT(cond)` expands to
`{ u8 bSkip = 0; if (!((cond) | bSkip)) Assert(__FILE__, __LINE__, #cond, 0); }`, and
`SFIO_DEVICE_MASK(e)` is `(u16)(1 << (u16)(e))` (the cast is what stops GCC folding it).

In-progress files are kept `NonMatching` (original code is linked) until every function matches,
because GCC emits the shared assert strings as private labels that the not-yet-written functions
still reference from the auto units. Per-function progress comes from `report.json`.

Leads and loose ends
--------------------

- `fn_800AACBC` (15 instructions, C bit-fields): **does not settle GC/2.0 vs GC/2.5.** The C is known
  (read the index byte into a local, store into a byte table, set two 1-bit fields from `index == 0`
  and `index == 1`) and matches exactly, but GC/2.0, 2.5 and 2.6 all produce the same bytes for it.
  Not yet added to the project. Two larger bit-packing functions remain untested: `fn_800B8984`, `fn_800B965C`.
- `fn_8000B508`, `fn_8000B54C`, `fn_8000B70C`: linked-list search family (head pointer at
  `0x80281BFC`, `next` at `0xC`, two ID fields at `0x1C` and `0x20`). C for `fn_8000B70C` already
  produces matching bytes; not yet added to the project.
- `fn_8001E85C`: copies four floats one at a time, unlike `Vec3Copy` which interleaves. Different
  source form (struct assignment?) or different file flags. Unexplained.
- Eleven functions in the EA region use the old-style function opening. Unexamined; may be a
  library built with an older compiler.
- The GCC block is the file-reading layer (`TagFile`, `SharedFileIO`). Unoptimized code is the
  easiest to decompile, and it is the bridge to the asset formats.
- 79 small functions contain real paired-single math (e.g. `fn_800BAD60`, `fn_800BADF8`,
  `fn_8001EF78`). Probably hand-written assembly; would be matched as assembly, not C.

Suggested next steps
--------------------

1. ~~Map the leaked file names to addresses.~~ Done: `docs/filemap.md`.
2. **Follow the named SDK calls into EA code.** Callers of `PADRead` lead to input and the swing
   (`YhSwing.c`); callers of `CARD*` to saves (`MC_Gc.c`); callers of `DVD*` to file loading and
   the asset formats.
3. **The GCC file library** (`0x8016C718`): compiler wired up (`ProDG/3.5`, `EASharedFileLib` in
   configure.py); `ChecksumCRC32.c`, the XOR cipher module, `SharedFileIO.c` (50 functions) and
   `TagFile.c` (41) done. Left: `llSharedFileIO.c` (17 functions, `0x80171308`-`0x801730C8`, the
   GameCube CARD platform layer). Each module exposes a 7-entry function-pointer table via
   `<Module>_GetInterface()`; shared error codes 2 = bad argument, 3 = wrong state,
   6 = not initialised.
4. **Add the already-solved small functions** to the project: the linked-list family at
   `0x8000B508` and `fn_800AACBC`.

Project conventions
-------------------

- `src/unsorted/code_ADDRESS.c` for files whose real name is unknown; the address is the first function.
- `fn_ADDRESS` for functions and `unkOFFSET` for struct fields until there is evidence for a name.
- Shared types for a group of files go in `include/unsorted/`.
- Compiler: GC/2.5, flags in `configure.py` (`cflags_base`). Confirmed and unconfirmed flags are
  listed in `compiler.md`.

Progress dashboard
------------------

`tools/dashboard/server.py` serves a read-only progress page on the LAN (port 8420). It reads
`build/GW4E69/report.json`, `symbols.txt` and git history; refreshes every 20 s. It runs as the
Windows scheduled task `tw2004-dashboard` (created with `schtasks`, no admin needed) so it outlives
the Claude session. It does not survive a reboot; restart it with:

    schtasks /run /tn tw2004-dashboard

Inbound port 8420 needs a Windows firewall allow rule (the PC's network is on the Public profile).
The server launches no programs (see decomp-notes.md); commit history comes from
`build/dashboard_history.json`, written by the post-commit hook. After cloning, run
`python tools/dashboard/install_hook.py` once. To stop or restart: kill `pythonw3.13.exe`
(not `pythonw`), then `schtasks /run /tn tw2004-dashboard`.

Adding a function: the steps
----------------------------

1. Read the disassembly in `build/GW4E69/asm/`. Write the plain-English logic first.
2. Write the C in `src/`. Function names must equal the names in `config/GW4E69/symbols.txt`.
3. In `config/GW4E69/splits.txt`, add or widen the file's `.text` range to cover the function.
   Ranges must be contiguous.
4. If the function uses float constants nobody else uses, add their `.sdata2` range too.
5. In `configure.py`, make sure the file is listed as `Object(Matching, ...)`.
6. `python configure.py`, then `ninja`. Success is `build/GW4E69/main.dol: OK`.
7. If it fails: `ninja build/GW4E69/report.json` for per-function percentages, then
   `objdiff-cli diff` for the instruction-level differences (left = original, right = ours).
