// charstate.h (our name): what char.c keeps beyond the character object in character.h: its
// globals and the small helpers it defines that other code also calls (the bit-array helpers, the
// vector copies).

#ifndef CHARSTATE_H
#define CHARSTATE_H

#include "character.h"

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
    u8   unk0[4];
    s32  n4;                    // 0x04  the first entry in SkinDesc.p20
    s32  n8;                    // 0x08  0: skipped
    s32  nC;                    // 0x0C  its SkinDesc.p28 entry
    u8   unk10[0x30 - 0x10];
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
    u8*  p3C;                   // 0x03C
    u8   unk40[4];
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

// What Skin.pModel points at; only what SkinPart.c reads.
typedef struct SkinModel {
    u8   unk0[0x40];
    s32  n40;                   // 0x40  bits in Skin.p10D0
    u8   unk44[4];
    SkinDesc* pDesc;            // 0x48
    u8   unk4C[4];
    s32  n50;                   // 0x50  bits in Skin.p10CC
} SkinModel;

// A part's (or a set's) choice: its variant and that variant's option. For a set, the variant is
// an entry of its p7C run and the option one of that entry's p8C run.
typedef struct SkinChoice {
    s32  nVariant;              // 0x0  -1 none
    s32  nOption;               // 0x4  -1 none
} SkinChoice;
LAYOUT_ASSERT(SkinChoice, 8);

// A skin (Skin.c): a character's body or one of its attachments; only what the code reads.
typedef struct Skin {
    SkinModel* pModel;          // 0x0000
    u8   unk4[0x10A0 - 4];
    void* a10A0[2];             // 0x10A0  indexed by fn_800CE02C's argument; Skin.c sets [0]
    SkinChoice* aParts[4];      // 0x10A8  a choice per part, four copies (fn_800CEE04 copies one
                                //         over another); [3] is set while lbl_80282238 is clear
    SkinChoice* aSets[4];       // 0x10B8  the same per SkinDesc.p74 set
    u8   unk10C8[4];
    u32* p10CC;                 // 0x10CC  } bit arrays
    u32* p10D0;                 // 0x10D0  }
    u32  u10D4;                 // 0x10D4  bit 1 set by fn_80019CEC and when the choices change; bit 2
                                //         tested by fn_80037708
} Skin;

// The iterator fn_80113B34 builds in a buffer: its first word points at its next function.
typedef struct SkinIter {
    void (**ppfnNext)(struct SkinIter* pIter);  // 0x0
    u8   bValid;                // 0x4
    u8   pad5[3];
    SkinMesh* pCur;             // 0x8
    s32  nCur;                  // 0xC
} SkinIter;

// What fn_80113B34 walks: a description and a SkinDesc.p5C entry (or, from fn_800CD5D0, a
// SkinVariant.nC index).
typedef struct SkinIterArgs {
    SkinDesc* pDesc;            // 0x0
    s32  n;                     // 0x4
} SkinIterArgs;

// A golfer's skin choices as kept outside the skins (fn_800CC1EC fills it from the body's skin or
// the skin from it; fn_800CC408 gives the other six skins theirs).
typedef struct SkinChoices {
    u8   unk0[0x114];
    SkinChoice aParts[40];      // 0x114  the body's, per part (-1 -1 throughout: not set yet)
    SkinChoice aSets[116];      // 0x254  the body's, per set
    SkinChoice aSkinParts[6][10];   // 0x5F4  the six skins' of CharSkinSet
    SkinChoice aSkinSets[6][10];    // 0x7D4
} SkinChoices;

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
extern void*      lbl_80280E24[2];      // what fn_8001B208 makes of the 'CLB ' object: one, or one per view
                                        // in split screen
extern Character* lbl_80281EE8;   // a character fn_8001C37C clears and fn_8001C518 frees

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
void  fn_800CC408(Character* pChar, SkinChoices* pChoices);
void  fn_800CC710(Character* pChar, int nSkin, u64 uPart, u64 uVariant);
void  fn_800CC7DC(Character* pChar, int nSkin, u64 uSet, u64 uVariant, u64 uOption);
void  fn_800CC8BC(Character* pChar, u8 bOn);
void  fn_800CEB1C(Skin** apSkins, int nSkins, u8* p);
void  fn_800CEBE8(Skin** apSkins, int nSkins, u8* p, u64* aIds, int nIds);
void  fn_800CECE0(Skin* pSkin, int nSet, int nVariant, int nOption, u8* p);
u8    fn_800CEE90(void);

// Bit n of a bit array of 32-bit words: test, set, clear.
u8    fn_8001E9CC(u32* aBits, u32 n);
void  fn_8001EA34(u32* aBits, u32 n);
void  fn_8001EB6C(u32* aBits, u32 n);
u8    fn_8001E9F4(u32* aA, u32* aB, u32 nBits);  // two bit arrays share a set bit

// 4-vectors (the quaternion copy, fn_8001E85C, is in character.h).
void  fn_8001E880(f32* pSrc, f32* pDst);  // a point to a 4-vector with w = 1
f32   fn_8001EEA4(f32* pA, f32* pB);      // dot product

#endif
