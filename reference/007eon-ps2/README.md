# 007 Everything or Nothing, PS2 USA (SLUS-20751, 2003-12-12 build)

Surveyed 2026-09-25 on the owner's PC. The build (from debugging.games) stays in `C:/dev/ext/`; only
this note and `shared_functions.tsv` are in the repo.

## What the build has

`ACTION.ELF` (92 MB, MIPS, not stripped) and a linker map, `BOND3_TF.MAP`. The debug information is
**not DWARF**: it is GCC 2.95 (ee-gcc) stabs inside an ECOFF `.mdebug` section, so every function,
parameter, local and struct (with field offsets) is there. 845 source files, 890 object files, 15,459
map symbols. Code: `c:/bond3/...` (EA Redwood Shores' Bond 3 engine), EA's shared "Real" libraries
(`c:/eac/realps2/lib`: sound `libsndps2`, `librealfile` with `hlafile.o`, `librealmem`,
`librealsystem`, `librealcodec` with RefPack `refdecode.o`, `librealmemcard`, `libpath`), EA's
DirtySDK (online), and the console maker's runtime.

## Does it share code with TW2004?

Mostly **no**. None of TW2004's own engine libraries are in it: no `LL*` (LLFont, LLTex, LLPict*,
LLDisp), no UIS menu system (UISApi/UISScreen/UISEvent/UIStudio), no `hlaud*` audio, no `rcmp`/MAD.
Function names shared with TW2004's named symbols: `main` only. `GameManager` and `shadow` share
file names with TW2004 units but not code (Bond's game manager and shadows).

The one shared family is **SKA**, EA's skeletal-animation format. 007 has `src/ChrCntl/SKA.cpp` and
`src/ChrCntl/SKA_Util.cpp`. TW2005's source paths put `SKA.c`, `SKA_util.c` and `skalib.c` together in
`Golf/Animation/`, and TW2004's `ska_shared.c` holds the matching functions:

| 007 (EA name) | TW2004 | how sure |
|---|---|---|
| `SKAUtil_EulerAnglesToQTs8(uint8* spAniAngle8_p, Quat* xpQuats_p, int32 iNumberOfElements_p, uint64* uiAxisMask_p, uint16* spBaseAniAngle16_p)` | `fn_8002148C(u8*, f32*, s32, u32*, u16*)` | strong: same five parameters |
| `SKA_LoadFromMem(char* pData, int32* iSize, uint32 align)` returns the animation | `fn_80020DD4(u8*, u32*, u32 nAlign)` returns `Clip*` | strong |
| `SKAUtil_EulerAnglesToQTs16(uint16*, Quat*, int32, uint64*, uint16*)` | `fn_80021134(u16*, f32*, s32, u32*)` | medium: TW2004 has no base-angle parameter |
| `SKA_PatchMemory(TSkeletonAnimation*, char*)` returns the animation | `fn_80020F60(Clip*, u32)` returns `Clip*` | medium |
| `SKA_DistributePointers(TSkeletonAnimation*, char* pRotData)` | `fn_800206C8(Clip*, u32)` | medium |

These are candidates by signature and file, not names yet: they need the usual evidence check
(docs/style.md) before any rename. Notes for whoever checks them:
- On the PS2, `unsigned long` is 64 bits: 007's `uint64*` axis masks are TW2004's `u32*` bit arrays.
- 007's `TSkeletonAnimation` (0xE0 bytes: a 0x90-byte stats block, then data pointers
  `pMemory` 0x90, `pCompressedAnimData` 0x94, `pAnimData` 0x98, `boneAnimInfo` 0x9C,
  `p16BITYPRFrames` 0xA0 ... `bone16BitTranslationMask` 0xD8) is related to TW2004's `Clip` (0x100
  bytes) but not the same layout: a different version of the format. `TBoneAnimInfo` is 8 bytes
  (`flags` u32, `origID` u16, `numAxis` u8, `numTAxis` u8).
- `SKAUtil_EulerAnglesRPY(ex, ey, ez, Quat* Q)` is inlined into the expanders (its locals ss, sc, cs,
  cc, sh, sj, si, ch, cj, ci, th, tj repeat in them): the Euler-to-quaternion step.

`shared_functions.tsv`: those two files' 16 functions: size (from the map), return type, parameters
and locals (from the stabs), and the TW2004 candidate. Nothing else from the build is in the repo.
