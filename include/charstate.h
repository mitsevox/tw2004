// charstate.h (our name): what char.c keeps beyond the character object in character.h: its
// globals and the small helpers it defines that other code also calls (the bit-array helpers, the
// vector copies).

#ifndef CHARSTATE_H
#define CHARSTATE_H

#include "character.h"
#include "game/save.h"              // SkinChoice, SkinChoices

// ---- a skin's parts (SkinPart.c) ----------------------------------------------------------------
// Parts, variants and the other entries are found by a 64-bit name code: fn_800CB700 packs a name
// ("Glove", "GloveOn") into one. The description comes from a file: fn_800CEF04 turns its offsets
// into pointers.

// A part of a skin, such as the glove.
typedef struct SkinPartDef {
    u64  uId;                   // 0x0  its name code
    s32  nVariants;             // 0x8
    s32  nFirst;                // 0xC  its first variant in SkinDesc.pVariants
} SkinPartDef;
LAYOUT_ASSERT(SkinPartDef, 0x10);

// A variant of a part, such as "GloveOn" or "GloveOff".
typedef struct SkinVariant {
    u64  uId;                   // 0x00  its name code
    s32  nOptions;              // 0x08  a part's option must be below it (fn_800CCC1C)
    s32  nFirstOption;          // 0x0C  its options: SkinDesc.p5C entries (fn_800CD664)
    s32  nLinks;                // 0x10  entries in SkinDesc.pLinks from nFirstLink
    s32  nFirstLink;            // 0x14
} SkinVariant;
LAYOUT_ASSERT(SkinVariant, 0x18);

// A variant's setting of another part's option (fn_800CDF80).
typedef struct SkinLink {
    u64  uPart;                 // 0x0  the part's name code
    s32  nOption;               // 0x8
    u8   unkC[4];
} SkinLink;
LAYOUT_ASSERT(SkinLink, 0x10);

// An entry of SkinDesc.p14, copied and patched by fn_800CE224.
typedef struct SkinDesc14 {
    u64  uId;                   // 0x00
    u32  u08;                   // 0x08  bit 2: take a4 from SkinDesc.pB8
    u8   unkC[0x16 - 0xC];
    s16  n16;                   // 0x16  entries of pB8 from n1C
    s32  n18;                   // 0x18  an entry of SkinDesc.p8C, -1 none
    s32  n1C;                   // 0x1C
    u32  a20[4];                // 0x20
} SkinDesc14;
LAYOUT_ASSERT(SkinDesc14, 0x30);

typedef struct SkinMeshBit {
    u8   unk0[6];
    s16  nBit;                  // 0x6  a bit of Skin.p10CC
} SkinMeshBit;
LAYOUT_ASSERT(SkinMeshBit, 8);

// An entry of the iterator fn_80113B34 walks and of SkinDesc.p34.
typedef struct SkinMesh {
    SkinMeshBit* pBits;         // 0x0  n8 of them (fn_800CD7D4)
    s32  nSize;                 // 0x4
    s32  n8;                    // 0x8
    u32  uFlags;                // 0xC  0x300000 both set: counted; 1 and 0x10: marks its bits
} SkinMesh;
LAYOUT_ASSERT(SkinMesh, 0x10);

typedef struct SkinDesc28 {
    s32  n0;                    // 0x00
    u8   unk4[0x18 - 4];
} SkinDesc28;
LAYOUT_ASSERT(SkinDesc28, 0x18);

typedef struct SkinDesc44 {
    s32  n0;                    // 0x00  its first entry in SkinDesc.p3C
    s32  n4;                    // 0x04  the first entry in SkinDesc.p20
    s32  n8;                    // 0x08  entries in SkinDesc.p3C from n0; 0: skipped
    s32  nC;                    // 0x0C  its SkinDesc.p28 entry
    s32  n10;                   // 0x10  the p44 entries of its morph targets start here
    s32  n14;                   // 0x14  its morph targets
    s32  n18;                   // 0x18  the first one's number (fn_8011C850: n14 + n18 needed)
    u8   unk1C[0x24 - 0x1C];
    u32  u24;                   // 0x24  bit 1: has morph targets
    u8   unk28[0x30 - 0x28];
} SkinDesc44;
LAYOUT_ASSERT(SkinDesc44, 0x30);

typedef struct SkinDesc5C {
    s32  n0;                    // 0x0  entries in SkinDesc.p6C from n4
    s32  n4;                    // 0x4
} SkinDesc5C;
LAYOUT_ASSERT(SkinDesc5C, 8);

// A set of the second kind of choice (Skin.aSets): each holds n08 entries of p7C.
typedef struct SkinDesc74 {
    u64  uId;                   // 0x00  its name code
    s32  n08;                   // 0x08  entries in SkinDesc.p7C from n10
    s32  n0C;                   // 0x0C  ids in SkinDesc.p84 per entry, from n14
    s32  n10;                   // 0x10
    s32  n14;                   // 0x14
} SkinDesc74;
LAYOUT_ASSERT(SkinDesc74, 0x18);

typedef struct SkinDesc7C {
    u64  uId;                   // 0x00  its name code
    s32  n08;                   // 0x08  entries in SkinDesc.p8C from n0C
    s32  n0C;                   // 0x0C
    s32  n10;                   // 0x10  given by fn_800CD124
    u8   unk14[4];
} SkinDesc7C;
LAYOUT_ASSERT(SkinDesc7C, 0x18);

typedef struct SkinDesc8C {
    u64  uId;                   // 0x00  its name code
    u8   a08[0x2C - 8];         // 0x08  fn_800CD248 gives its address
    s32  n2C;                   // 0x2C
} SkinDesc8C;
LAYOUT_ASSERT(SkinDesc8C, 0x30);

typedef struct SkinDescB8 {
    u32  a[4];                  // 0x0
} SkinDescB8;
LAYOUT_ASSERT(SkinDescB8, 0x10);

// A skin's description (SkinModel.pDesc), as loaded from its file.
typedef struct SkinDesc {
    s32  nVersion;              // 0x000  8
    s32  n04;                   // 0x004
    u8   unk08[4];
    u32  uFlags;                // 0x00C  bit 1: its offsets are pointers now
    s32  n10;                   // 0x010  entries in p14 and p18
    SkinDesc14* p14;            // 0x014
    u8*  p18;                   // 0x018  0x1C bytes each
    u8   unk1C[4];
    s32* p20;                   // 0x020
    u8   unk24[4];
    SkinDesc28* p28;            // 0x028
    u8   unk2C[4];
    s32  n30;                   // 0x030  entries in p34
    SkinMesh* p34;              // 0x034
    u8   unk38[4];
    s32* p3C;                   // 0x03C  SkinDesc.p34 entries, -1 none
    s32  n40;                   // 0x040  entries in p44
    SkinDesc44* p44;            // 0x044
    s32  nParts;                // 0x048
    SkinPartDef* pParts;        // 0x04C
    u8   unk50[4];
    SkinVariant* pVariants;     // 0x054
    s32  n58;                   // 0x058  entries in p5C
    SkinDesc5C* p5C;            // 0x05C
    u8   unk60[4];
    SkinLink* pLinks;           // 0x064
    u8   unk68[4];
    s32* p6C;                   // 0x06C  entries of p44, -1 none
    s32  n70;                   // 0x070  entries in p74
    SkinDesc74* p74;            // 0x074
    s32  n78;                   // 0x078  entries in p7C
    SkinDesc7C* p7C;            // 0x07C
    u8   unk80[4];
    u64* p84;                   // 0x084  name codes
    s32  n88;                   // 0x088  entries in p8C
    SkinDesc8C* p8C;            // 0x08C
    u8   unk90[0x9C - 0x90];
    u8*  p9C;                   // 0x09C
    u8   unkA0[4];
    u8*  pA4;                   // 0x0A4
    u8   unkA8[4];
    u8*  pAC;                   // 0x0AC
    u8   unkB0[0xB8 - 0xB0];
    SkinDescB8* pB8;            // 0x0B8
    u8   unkBC[0x120 - 0xBC];
} SkinDesc;
LAYOUT_ASSERT(SkinDesc, 0x120);

// An entry of SkinModel.p44: its first word is an index SkinBurn.c renumbers (fn_80127140).
typedef struct SkinModel44 {
    s32  n0;                    // 0x0
    s32* p4;                    // 0x4  n8 of them; fn_801272B4 numbers them 0, 1, 2...
    s16  n8;                    // 0x8
    u8   unkA[0x10 - 0xA];
} SkinModel44;
LAYOUT_ASSERT(SkinModel44, 0x10);

// An entry of SkinModel.p54, one per bit of Skin.p10CC; SkinBurn.c moves them (fn_801272B4).
typedef struct SkinModel54 {
    u8   unk0[0x14];
} SkinModel54;
LAYOUT_ASSERT(SkinModel54, 0x14);

// What Skin.pModel points at; only what SkinPart.c and SkinBurn.c read.
typedef struct SkinModel {
    u8   unk0[8];
    s32  n08;                   // 0x08  its size with all its arrays once burnt (fn_801276E4)
    s32  n0C;                   // 0x0C  entries in p3C
    u8   unk10[4];
    s32  n14;                   // 0x14  how many matrices Skin.p108C holds (fn_80018710); also
                                //       the 0x20-byte entries in p34
    u8   unk18[0x34 - 0x18];
    void* p34;                  // 0x34  handed to the character's model (fn_80029A74)
    void* p38;                  // 0x38  one 0x50-byte block
    void* p3C;                  // 0x3C  n0C 0x50-byte blocks
    s32  n40;                   // 0x40  bits in Skin.p10D0; also the entries in p44
    SkinModel44* p44;           // 0x44
    SkinDesc* pDesc;            // 0x48
    u8   unk4C[4];
    s32  n50;                   // 0x50  bits in Skin.p10CC; also the entries in p54
    SkinModel54* p54;           // 0x54  one per bit
    u8   unk58[0x140 - 0x58];
} SkinModel;
LAYOUT_ASSERT(SkinModel, 0x140);  // fn_801276E4 copies it whole

// What Skin.pMorph points at (SkinMorph.c; our name): the weights of the skin's morph targets.
typedef struct SkinMorphState {
    s32  nMorphs;               // 0x0
    f32* afWeights;             // 0x4  one per morph target (fn_8011CADC)
    u32* aChanged[2];           // 0x8  bit arrays of nMorphs bits, indexed like Skin.a10A0:
                                //      fn_8011CADC sets a changed target's bit in both,
                                //      fn_8011CE58 sets every bit, fn_8011CB5C clears one
} SkinMorphState;

// SkinMorph.c's work area (lbl_80281880; our name): blends a mesh's morph targets into one of two
// vertex buffers. Only what the code reads.
typedef struct SkinMorphVert {
    s32  aPos[4];               // 0x00  16.16 fixed point
    s32  aNrm[4];               // 0x10  the same
} SkinMorphVert;
LAYOUT_ASSERT(SkinMorphVert, 0x20);

typedef struct SkinMorphWork {
    SkinMorphVert aVerts[0x800];    // 0x00000  the mesh being blended (fn_8011C068 unpacks it)
    u8   unk10000[0x18];
    u16  n10018;               // 0x10018  vertices in the mesh being blended (fn_8011C49C)
    u8   pad1001A[0x10020 - 0x1001A];
    void* p10020;               // 0x10020  } the two buffers; fn_8011C46C swaps them
    void* p10024;               // 0x10024  }
    SkinMesh* apTargets[64];    // 0x10028  the target meshes fn_8011C5B4 picks
    f32  afTargets[64];         // 0x10128  and their weights
    SkinDesc* pDesc;            // 0x10228
    s32  nMorphs;               // 0x1022C  } SkinMorphState's (fn_8011C58C)
    f32* afWeights;             // 0x10230  }
    struct HwsMemBlock* pBlock;         // 0x10234  (fn_8011C5A8)
    struct HwsOverrideTable* pTable;    // 0x10238  (fn_8011C59C)
} SkinMorphWork;

extern SkinMorphWork* lbl_80281880;

// A skin (Skin.c): a character's body or one of its attachments; only what the code reads.
typedef struct Skin {
    SkinModel* pModel;          // 0x0000
    SkelPose pose;              // 0x0004  (fn_80018710 hands it to SKEL_UpdateState)
    u8   b1044;                 // 0x1044  set once fn_800184E4 has filled a1048
    u8   pad1045[3];
    f32  a1048[4][4];           // 0x1048  four leg points, each through fn_8000AB40 of its bone's
                                //         matrix (fn_800184E4: bones 0x3A, 0x48, 0x39, 0x47)
    f32  (*p1088)[4][4];        // 0x1088  } matrices fn_80018710 hands the model (fn_80029A88,
    f32  (*p108C)[4][4];        // 0x108C  } fn_80029A7C)
    u8   unk1090[0x1098 - 0x1090];
    struct HwsMemBlock* a1098[2];   // 0x1098  indexed like a10A0 (fn_8011CB5C)
    void* a10A0[2];             // 0x10A0  indexed by fn_800CE02C's argument; Skin.c sets [0]
    SkinChoice* aParts[4];      // 0x10A8  a choice per part, four copies (fn_800CEE04 copies one
                                //         over another); [3] is set while lbl_80282238 is clear
    SkinChoice* aSets[4];       // 0x10B8  the same per SkinDesc.p74 set
    SkinMorphState* pMorph;     // 0x10C8
    u32* p10CC;                 // 0x10CC  } bit arrays
    u32* p10D0;                 // 0x10D0  }
    u32  u10D4;                 // 0x10D4  bit 1 set by fn_80019CEC and when the choices change; bit 2
                                //         tested by fn_80037708
    f32  f10D8;                 // 0x10D8  from the CHR object's header (fn_8001A9F4)
    f32  f10DC;                 // 0x10DC  1 when loaded
} Skin;

// hwsOverride_Gc.c (our names): a block of memory handed out in pieces (fn_80112938), sized for
// a SkinDesc's meshes of flag 0x100000 (fn_80112848).
typedef struct HwsMemBlock {
    s32  nSize;                 // 0x0
    s32  nUsed;                 // 0x4
    u8*  pData;                 // 0x8  the memory, right after this header
    u8   unkC[4];
} HwsMemBlock;

// A pointer per mesh of a SkinDesc (fn_8011296C), filled from an HwsMemBlock (fn_80112A80).
// Skin.a10A0 holds them; fn_801138D8 makes one the renderer's current table.
typedef struct HwsOverrideTable {
    SkinDesc* pDesc;            // 0x0
    s32  nMeshes;               // 0x4
    void** apMesh;              // 0x8  nMeshes of them, right after this header
} HwsOverrideTable;

// What fn_801104AC (hwsBurn.c) makes when SkinBurn.c burns a skin (our name); only what SkinBurn.c
// reads.
typedef struct HwsBurn {
    u8   unk0[0x30];
    s32* p30;                   // 0x30  the new index of each old one, -1: dropped (fn_80127140)
} HwsBurn;

// Part of the renderer's state that fn_80112B34 sets up.
typedef struct HwsRender10 {
    u32  u0;                    // 0x00  cleared by fn_80112B34
    u8   unk4[0x24 - 0x4];
    u8   a24[0x48 - 0x24];      // 0x24
    u32* p48;                   // 0x48  fn_80112B34 points it at u0 (fn_80113764 gives it)
    u8*  p4C;                   // 0x4C  and this at a24
    s32  n50;                   // 0x50  cleared by fn_8011389C
} HwsRender10;

// The GameCube renderer's state (lbl_80223BB0; hwsRender_Gc.c, hwsOverride_Gc.c).
typedef struct HwsRenderState {
    void* p0;                   // 0x00  fn_801138CC sets it
    s32  n4;                    // 0x04  } cleared by fn_80113844
    s32  n8;                    // 0x08  }
    HwsOverrideTable* pOverride;    // 0x0C  fn_801138D8
    HwsRender10 s10;            // 0x10
} HwsRenderState;
LAYOUT_ASSERT(HwsRenderState, 0x64);

extern HwsRenderState lbl_80223BB0;

// A character's body sliders (Character.p17AC, made by CharSlider_CreateDefinitionsFromMem; our
// names). fn_8010E4DC sets each slider's value, lets the sliders push on each other, then moves
// the model's bones and the skin's morph targets by them.

// A slider that moves another's range when this one is between fFrom and fTo (fn_8010DF8C).
typedef struct CharSliderLink {
    s32  nSlider;               // 0x00  the slider it moves
    u32  uFlags;                // 0x04  1: moves the other's low end, 2: its high end
    f32  fFrom;                 // 0x08
    f32  fTo;                   // 0x0C
    u8   unk10[0x18 - 0x10];
} CharSliderLink;

// A slider this one shares a length with: the two values, as a vector, are cut to fLength
// (fn_8010DE60).
typedef struct CharSliderLimit {
    s32  nSlider;               // 0x0
    f32  fLength;               // 0x4
} CharSliderLimit;

// A bone a slider scales (fn_800298F4 finds it by uId; fn_80028A70 scales it on uAxes).
typedef struct CharSliderBone {
    u64  uId;                   // 0x00
    f32  fFrom;                 // 0x08  the scale at the range's start
    f32  fTo;                   // 0x0C  and at its end
    u32  uAxes;                 // 0x10
    u8   unk14[4];
} CharSliderBone;

// A morph target a slider weights (matched against CharSliderDefs.aMorphIds).
typedef struct CharSliderMorph {
    u64  uId;                   // 0x00
    f32  fFrom;                 // 0x08
    f32  fTo;                   // 0x0C
} CharSliderMorph;

// A part of a slider's travel, fStart to fEnd, over which its bones (or morph targets) change.
typedef struct CharSliderRange {
    f32  fStart;                // 0x0
    f32  fEnd;                  // 0x4
    s32  nItems;                // 0x8
    union {
        CharSliderBone* pBones;     // in CharSliderDef.pBoneRanges
        CharSliderMorph* pMorphs;   // in CharSliderDef.pMorphRanges
    } items;                    // 0xC
} CharSliderRange;

typedef struct CharSliderDef {
    s32  nId;                   // 0x00
    s32  nLinks;                // 0x04
    CharSliderLink* pLinks;     // 0x08
    s32  nLimits;               // 0x0C
    CharSliderLimit* pLimits;   // 0x10
    s32  nBoneRanges;           // 0x14
    CharSliderRange* pBoneRanges;   // 0x18
    s32  nMorphRanges;          // 0x1C
    CharSliderRange* pMorphRanges;  // 0x20
    u8   unk24[4];
} CharSliderDef;

// A slider's state: its range and its value in it.
typedef struct CharSliderValue {
    f32  fLow;                  // 0x0
    f32  fHigh;                 // 0x4
    f32  fValue;                // 0x8
    u8   bFixed;                // 0xC  not cut by a CharSliderLimit
    u8   unkD[3];
} CharSliderValue;

typedef struct CharSliderDefs {
    s32  nSliders;              // 0x00
    CharSliderDef* pDefs;       // 0x04
    CharSliderValue* pValues;   // 0x08
    s32  nMorphs;               // 0x0C
    u64* aMorphIds;             // 0x10  the skin's morph targets, in fn_8011CADC's order
} CharSliderDefs;

// The iterator fn_80113B34 builds in a buffer: its first word points at its next function.
typedef struct SkinIter {
    void (**ppfnNext)(struct SkinIter* pIter);  // 0x0
    u8   bValid;                // 0x4
    u8   pad5[3];
    SkinMesh* pCur;             // 0x8
    s32  nCur;                  // 0xC
} SkinIter;

// The whole iterator fn_80113A9C and fn_80113B34 build (our name): the meshes of one SkinDesc.p5C
// entry.
typedef struct SkinDescIter {
    SkinIter iter;              // 0x00
    SkinDesc* pDesc;            // 0x10
    SkinDesc5C* pEntry;         // 0x14
    s32  n18;                   // 0x18  -1 before the first step
    s32  n1C;                   // 0x1C
} SkinDescIter;

// What fn_80113B34 walks: a description and a SkinDesc.p5C entry (or, from fn_800CD5D0, a
// SkinVariant.nC index).
typedef struct SkinIterArgs {
    SkinDesc* pDesc;            // 0x0
    s32  n;                     // 0x4
} SkinIterArgs;

// An entry of the lists fn_800CE660 and fn_800CE8C0 build: each name code once.
typedef struct SkinListEntry {
    u64  uId;                   // 0x0
    u8*  p8;                    // 0x8  what fn_800CE224 gave for it
    s32  nC;                    // 0xC
} SkinListEntry;
LAYOUT_ASSERT(SkinListEntry, 0x10);

// What fn_800CE170 is given (by char.c and FEgolferanim.c); only what it reads.
typedef struct SkinTarget {
    u8   unk0[4];
    s32  n4;                    // 0x4  passed on to fn_80112614
} SkinTarget;

// What Character.p16D8 points at; only what SkinPart.c reads.
typedef struct CharSkinSet {
    u8   unk0[0xC];
    f32  afC[6];                // 0x0C  per club class: the club head bone's height (fn_8001C5B4)
    Skin* apSkins[6];           // 0x24
    u8   unk3C[0x9C - 0x3C];
    void* a9C[6];               // 0x9C  freed with fn_8001B1E8 (fn_8001B58C)
} CharSkinSet;

// A pool of seven entries characters take (fn_8001A418) and give back (fn_8001A3B0).
typedef struct CharPoolEntry {
    void* p;                    // 0x0
    u8    bUsed;                // 0x4
    u8    pad5[3];
} CharPoolEntry;

typedef struct CharPool {
    CharPoolEntry a[7];         // 0x00
    s32   nEntries;             // 0x38
} CharPool;
LAYOUT_ASSERT(CharPool, 0x3C);

extern CharPool   lbl_801B95E8;
extern Character* lbl_801B9624[5];     // the characters made so far (fn_8001C21C)
extern s32        lbl_80281CA8;         // how many
extern s32        lbl_80281CAC;         // the player fn_8001D8DC last marked (-1 at start)
extern s32        lbl_80187164[8];      // the clip key for each shot kind (fn_8001C724)
extern s32        lbl_80280E20;         // set to 6 (4 in split screen) by fn_8001C254, 3 by fn_8001C304
extern CharSkinSet* lbl_80280E24[2];   // what fn_8001B208 makes of the 'CLB ' object: one, or one per
                                        // view in split screen (Character.p16D8; fn_8001B58C frees them)

void  fn_80037CD8(void* pSkin);         // Skin.c: frees a skin

// SkinPart.c, as FE_CrAPDB.c uses it: find a part (or set) by id, a variant by id or name, and
// pick a part's (or set's) variant.
void  fn_800CC9D8(Character* pChar, int nSet, int nVariant, int nOption);
void  fn_800CCB08(Skin* pSkin, int nPart, int nVariant);
void  fn_800CCF90(Skin* pSkin, int nSet, int nVariant, int nOption);
u8*   fn_800CD248(Skin* pSkin, int nSet, int nVariant, int nOption);    // SkinDesc8C.a08, or NULL
s32   fn_800CDAFC(Skin* pSkin, u64 uId);
s32   fn_800CDBB0(Skin* pSkin, int nPart, u64 uId);
s32   fn_800CDC2C(Skin* pSkin, u64 uId);
s32   fn_800CDCA0(Skin* pSkin, const char* pName);
s32   fn_800CDCE0(Skin* pSkin, int nSet, u64 uId);
s32   fn_800CDD5C(Skin* pSkin, int nSet, const char* pName);
s32   fn_800CDDB0(Skin* pSkin, int nSet, int nVariant, u64 uId);
void  fn_8001D4A4(Character* pChar, int nSlot);   // dresses the character (its skins and clubs)
void  fn_8001EE98(Character* pChar, u8 b);    // sets the model's bEE
void  fn_8010E4DC(CharSliderDefs* pDefs, CharModel* pModel, Skin* pSkin, int nSliders, u8* aValues,
                  u8* pNode);
                                        // applies slider values (Character.p17AC's definitions)
void  fn_8010D454(CharSliderDefs* pDefs);   // CharSliders.c: frees slider definitions
void  fn_800CE170(Skin* pSkin, SkinTarget* pTarget);
void  fn_800CC1EC(Character* pChar, SkinChoices* pChoices);
void  fn_800CC658(Character* pChar, char* pSet, char* pVariant, char* pOption);
void  fn_800CC408(Character* pChar, SkinChoices* pChoices);
void  fn_800CC710(Character* pChar, int nSkin, u64 uPart, u64 uVariant);
void  fn_800CC7DC(Character* pChar, int nSkin, u64 uSet, u64 uVariant, u64 uOption);
void  fn_800CC8BC(Character* pChar, u8 bOn);
void  fn_800CEB1C(Skin** apSkins, int nSkins, u8* p);
void  fn_800CEBE8(Skin** apSkins, int nSkins, u8* p, u64* aIds, int nIds);
void  fn_800CECE0(Skin* pSkin, int nSet, int nVariant, int nOption, u8* p);
u8    fn_800CEE90(void);

// SkinPart.c, as SkinBurn.c uses it: the mesh iterator and an entry's copy.
void  fn_800CD9EC(Skin* pSkin);
s32   fn_800CE224(Skin* pSkin, SkinDesc14* pEntry, u8** ppOut, s32* pnOut, int nCopy);
u8    fn_800CEEC0(SkinIter* pIter);
void  fn_800CEEC8(SkinIter* pIter);
SkinMesh* fn_800CEEF4(SkinIter* pIter);
SkinIter* fn_80113B34(u8* pBuf, SkinIterArgs* pArgs);
void  fn_80113BAC(SkinIter* pIter);

// hwsOverride_Gc.c: a mesh table and a memory block for a skin description's morphed meshes.
HwsMemBlock* fn_801128EC(SkinDesc* pDesc, s32 nSize);
HwsOverrideTable* fn_80112A34(SkinDesc* pDesc, s32 nMeshes);

// SkinBurn.c: burns a skin (aParts and aList each end with -1).
void  fn_80127B98(Skin* pSkin, s32* aParts, s32* aList);
extern s32* lbl_802825A8;               // the new number of each mesh bit (fn_801271E0)

// Bit n of a bit array of 32-bit words: test, set, clear.
void  fn_8001E8A4(u32* aBits, u32 nBits);  // sets every bit of a bit array
void  fn_8001E938(u32* aBits, u32 nBits);  // clears a bit array
u8    fn_8001E9CC(u32* aBits, u32 n);
void  fn_8001EA34(u32* aBits, u32 n);
void  fn_8001EA54(u32* aA, u32* aB, u32* aOut, u32 nBits);  // the bits set in both
void  fn_8001EB6C(u32* aBits, u32 n);
u8    fn_8001E9F4(u32* aA, u32* aB, u32 nBits);  // two bit arrays share a set bit

// 4-vectors (the quaternion copy, fn_8001E85C, is in character.h).
void  fn_8001E880(f32* pSrc, f32* pDst);  // a point to a 4-vector with w = 1
f32   fn_8001EEA4(f32* pA, f32* pB);      // dot product

#endif
