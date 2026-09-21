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
| `0x8016C718` - `0x80175F54` | EA shared file library, 124 functions, **GCC unoptimized** |
| `0x8017DB60` | `.rodata` (assert strings, jump tables) |
| `0x801866E0` `.data`, `0x8019D540` `.bss`, `0x80280DA0` `.sdata`, `0x80281B80` `.sbss`, `0x80282A60` `.sdata2` (float constants) | |

The boundary between EA code and the SDK is approximate: it is where dtk's first automatically
named SDK function appears.

Leaked source file names
------------------------

Found in assert strings. Each assert sits inside a function, so each name is pinned to a real
address (not yet mapped). Meanings of the prefixes are guesses.

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
| EA shared library (GCC block) | | `../../../Source/Common/Checksum/ChecksumCRC32.c` `../../../Source/Common/SharedFileIO/SharedFileIO.c` `../../../Source/Common/TagFile/TagFile.c` `../../../Source/NGC/SharedFileIO/llSharedFileIO.c` |

Camera and visibility code (`0x80007BC4` - `0x800083A4`)
--------------------------------------------------------

Probably `GoCamera.c` or `GoViewport.c` (guess; both names are in the binary). Types are in
`include/unsorted/cull.h`.

| Address | Name | What it does | Status |
|---------|------|--------------|--------|
| `0x80007BC4` | `fn_80007BC4` | Move an object's bounding sphere into camera space, optionally report its depth, scale its radius, cull it | matched |
| `0x80007C80` | `fn_80007C80` | Move a point into camera space (`viewMtx * point`) | matched |
| `0x80007CE8` | `fn_80007CE8` | Same as `fn_80007BC4` without the depth output | matched |
| `0x80007D74` | `fn_80007D74` | **Sphere vs view frustum test.** Returns 1 inside, 2 outside, 4 crossing an edge. Reads all ten camera floats | next |
| `0x800082F8` | `fn_800082F8` | Get a drawable object's bounding sphere (`obj->data + 0x58`) | matched |
| `0x80008304` | `Vec3Copy` | Copy three floats. **Called 596 times from 110 functions** | matched |
| `0x80008320` - `0x80008368` | ten getters | Return camera floats `0x220` down to `0x1F4` | matched |
| `0x80008370`, `0x80008378` | getters | `cam->unk10`, then field 0 of that object; the cull test takes a different path when it is non-zero | matched |
| `0x80008380` | `fn_80008380` | Calls `fn_800070DC`. Called from 21 functions | matched |
| `0x800BAD60` | `fn_800BAD60` | 4x4 matrix times 4-float vector, uses paired-single math (hand-written assembly?) | not started |

Camera fields (guesses from how the cull test uses them): `0x11C` view matrix (4x4 floats);
`0x1F4` / `0x1F8` near and far clip distances; `0x204`-`0x210` and `0x214`-`0x220` two sets of four
frustum edge-plane numbers, chosen by a mode argument (0 or 1). Why there are two sets is unknown
(two aspect ratios? a tight and a loose view?). `fn_80013EA0` also reads the two clip distances.

Float constants for this file sit together in `.sdata2`: `1.0` at `0x80282A80` (owned by the three
matched functions), `0.5` at `0x80282A84` (used by the cull test).

Leads and loose ends
--------------------

- `fn_800AACBC` (15 instructions, C bit-fields): best candidate for settling GC/2.0 vs GC/2.5.
  Two larger bit-packing functions: `fn_800B8984`, `fn_800B965C`.
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

Project conventions
-------------------

- `src/unsorted/code_ADDRESS.c` for files whose real name is unknown; the address is the first function.
- `fn_ADDRESS` for functions and `unkOFFSET` for struct fields until there is evidence for a name.
- Shared types for a group of files go in `include/unsorted/`.
- Compiler: GC/2.5, flags in `configure.py` (`cflags_base`). Confirmed and unconfirmed flags are
  listed in `compiler.md`.

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
