// character.h (our name): the golfer's character object, the animated golfer on screen (TW06's
// Character_* and CharacterState_* functions take it), its skeleton and animation state, and the
// functions that take one. Player.pChar points at the player's.

#ifndef CHARACTER_H
#define CHARACTER_H

#include "engine.h"

typedef struct AnimLib AnimLib;         // skalib.c

// skalib.c: the clips for an animation group, style, club class and key (AnimStream.c calls it too).
void** AnimLib_Find(AnimLib* pLib, int nGroup, int nStyle, int nClub, int nKey, s32* pCount,
                    u32* pFlags, u32** ppUsed, s32* pFirst);

// A link of an IK chain: one bone.
typedef struct IKLink {
    u8   b0;                    // 0x00  cleared by fn_80028208
    u8   nBone;                 // 0x01  the model's bone index (fn_8001EEE4)
    u8   pad2[2];
    f32  f4;                    // 0x04  only links above 0 are posed (fn_80026B4C, fn_80026F90)
    s32  n8;                    // 0x08
    f32  fC;                    // 0x0C
    f32  f10;                   // 0x10
    s8   nPrev;                 // 0x14  the link before it in the chain, -1 for the first (every
    u8   pad15[3];              //       access is a byte: fn_80026BF4, fn_80028208)
    f32  q18[4];                // 0x18  its rotation (quaternion) from the link before it
    f32  v28[4];                // 0x28  its offset from the link before it
    f32  q38[4];                // 0x38  } q18 and v28 kept for a link whose b0 bit 1 is set:
    f32  v48[4];                // 0x48  }   fn_80027478 poses it from these
    f32  v58[3];                // 0x58  its rotation as a vector (axis * angle)
    u8   unk64[0x78 - 0x64];
} IKLink;
LAYOUT_ASSERT(IKLink, 0x78);

// An IK chain of a skeleton (Skeleton.pChains).
typedef struct IKChain {
    s8   nLinks;                // 0x00
    u8   unk1[3];
    IKLink* pLinks;             // 0x04
    f32  v8[4];                 // 0x08  its last link's position (fn_800271A0)
    s32  n18;                   // 0x18
    f32  f1C;                   // 0x1C
} IKChain;
LAYOUT_ASSERT(IKChain, 0x20);

// An IK link's setup (our name): what fn_80028208 copies into an IKLink.
typedef struct IKLinkDef {
    s32  nBone;                 // 0x00  a bone id (fn_8001EEE4 gives its index)
    f32  f4;                    // 0x04  } IKLink's f4, n8, fC and f10
    s32  n8;                    // 0x08  }
    f32  fC;                    // 0x0C  }
    f32  f10;                   // 0x10  }
} IKLinkDef;
LAYOUT_ASSERT(IKLinkDef, 0x14);

// An IK chain's setup (our name): what fn_80028208 builds an IKChain from.
typedef struct IKChainDef {
    IKLinkDef* pLinks;          // 0x00
    s32  nLinks;                // 0x04
    s32  n8;                    // 0x08  IKChain's n18
    f32  fC;                    // 0x0C  IKChain's f1C
} IKChainDef;
LAYOUT_ASSERT(IKChainDef, 0x10);

// A bone's pose in a model (CharModel.p34).
typedef struct BonePose {
    f32  q0[4];                 // 0x00  its rotation (quaternion)
    f32  v10[4];                // 0x10  its position (SKEL_TranslateIKChainY moves its y)
} BonePose;
LAYOUT_ASSERT(BonePose, 0x20);

// A blend node's pose buffer (SKABlendNode.pPose; our name): bit arrays over the 128 bones
// (fn_800177A0 clears the first two with fn_8001E938 and the next two with fn_8001E8A4), then a pose
// per bone. Format 0 is exactly this (0x1040 bytes); format 1 has 0x10C bytes more.
typedef struct SkelPose {
    u32      a0[4];             // 0x000  bones the pose sets (fn_8001E9CC tests them)
    u32      a10[4];            // 0x010
    u32      a20[4];            // 0x020
    u32      a30[4];            // 0x030
    BonePose aBones[128];       // 0x040
} SkelPose;
LAYOUT_ASSERT(SkelPose, 0x1040);

// A format 1 pose buffer (0x114C bytes; animblender.c copies it whole): three blocks from 0x4, each
// starting with a bit per morph (20: fn_80072ACC clears them all; CharSliders.c clears morph
// m's in every block with fn_800736D8).
// fn_80071C28 sets each block's bits and its 20 floats, and clears the SkelPose's first bit arrays.
typedef struct SkelPoseBlock {
    u32  aBits[1];              // 0x00
    u8   unk4[4];
    f32  af8[20];               // 0x08  one per morph, 0 when the buffer is taken (fn_80071C28)
} SkelPoseBlock;
LAYOUT_ASSERT(SkelPoseBlock, 0x58);

typedef struct SkelPose1 {
    u8   unk0[4];
    SkelPoseBlock aBlocks[3];   // 0x004
    SkelPose pose;              // 0x10C  as a format 0 buffer
} SkelPose1;
LAYOUT_ASSERT(SkelPose1, 0x114C);

// A character's skeleton data (CharModel.pSkel; the SKEL_ functions take it): its IK chains and
// how strongly their solution is applied (the IK weight, 0..1); only what the code reads.
typedef struct Skeleton {
    s32  n0;                    // 0x0000  cleared by fn_80028314
    s32  nChains;               // 0x0004
    IKChainDef* pDefs;          // 0x0008  the chains' setups (CharModelDefs.pDefs)
    IKChain* pChains;           // 0x000C
    u32  a10[4];                // 0x0010  a bit per bone (128; fn_8001EB6C clears one)
    f32  (*p20)[4];             // 0x0020  a quaternion per bone
    f32  (*p24)[4];             // 0x0024  a quaternion per bone
    f32  (*p28)[4];             // 0x0028  p20 at an IK weight of 0 or 1, otherwise p24
    struct Clip* pClip;         // 0x002C  its clip (fn_8001C860); cleared before and after
                                //         fn_8001966C's animation update
    SkelPose pose;              // 0x0030  (fn_8001C860 passes it to SKEL_UpdateState)
    f32  fIKWeight;             // 0x1070  SKEL_SetIKSolutionWeight
    f32  f1074;                 // 0x1074  } set by fn_8002792C and SKEL_TransitionIK
    f32  f1078;                 // 0x1078  }
    f32  q107C[4];              // 0x107C  a rotation (quaternion) fn_800279C0 turns the grip's by
    f32  v108C[4];              // 0x108C  an offset from the grip, turned by its rotation: the IK
                                //         target of the second chain (fn_800279C0)
    f32  f109C;                 // 0x109C  } 0.025 and 0.15 from fn_800280E8
    f32  f10A0;                 // 0x10A0  }
    f32  v10A4[4];              // 0x10A4
    f32  v10B4[4];              // 0x10B4  v10A4 scaled by the IK weight
    f32  f10C4;                 // 0x10C4  the IK weight
    f32  f10C8;                 // 0x10C8  } set up by fn_800280E8: the first link's offset height,
    f32  f10CC;                 // 0x10CC  }   0.025 and 0.05
    f32  f10D0;                 // 0x10D0  }
    f32  q10D4[4];              // 0x10D4  a rotation (quaternion) given by fn_80027808
    s32  n10E4;                 // 0x10E4  set to 4 as a swing starts
    f32  a10E8[2][4];           // 0x10E8  per leg, the last good bend axis (Character_IKLegToGround,
                                //         legs 0 and 1)
    u8   a1108[4];              // 0x1108  the indexes of bones 0x24, 0x25, 0x11 and 0x12 (fn_80028314)
    u8   unk110C[0x112C - 0x110C];
    s32  n112C;                 // 0x112C  } the character's club class and n16D4 (fn_8001C860)
    s32  n1130;                 // 0x1130  }
} Skeleton;

// A bone of a character's model (CharModel.pBones).
typedef struct Bone {
    u64  uId;                   // 0x00  fn_800298F4 finds a bone by it
    s8   nParent;               // 0x08  its parent bone (fn_80114270 walks a chain down by it)
    u8   unk9[3];
    f32  q0C[4];                // 0x0C  a rotation (quaternion)
    f32  v1C[4];                // 0x1C  a position (the root bone's is the character's,
                                //       Character_SetPosition)
    u8   unk2C[0x30 - 0x2C];
} Bone;
LAYOUT_ASSERT(Bone, 0x30);

// A character's model: its bones; only what the game code reads.
typedef struct CharModel {
    s32       nBones;           // 0x000
    Bone*     pBones;           // 0x004
    f32     (*pMatrices)[4][4]; // 0x008  one per bone (fn_8001EED8 gives a bone's index); row 3 is its
                                //        position
    f32       fC;               // 0x00C  } lengths Character_UpdateTestPoints sets points 0-3 out by
    f32       f10;              // 0x010  } along the leg bones' axes when the skin has no a1048
    u32       a14[4];           // 0x014  } bits per bone: SKEL_TransformBones turns a bone set in a14
    u32       a24[4];           // 0x024  }   and moves one set in a24, then sets them all again
    BonePose* pPoses;           // 0x034  one per bone; freed with the model
    Skeleton* pSkel;            // 0x038
    u8        aBone[0x59];      // 0x03C  each bone id's index (fn_8001EED8), 0xFF none; fn_80029664
                                //        fills it in by name
    u8        aBone2[0x59];     // 0x095  the index fn_8001EEE4 gives while bEE is set, by bone index
                                //        (fn_80029804: itself, or the other bone of a pair)
    u8        bEE;              // 0x0EE  fn_8001EDF4
    u8        unkEF;
    struct DynChain* pF0;       // 0x0F0  } freed with the model (fn_80114398)
    struct DynChain* pF4;       // 0x0F4  }
    struct DynChain* pF8;       // 0x0F8  }
    struct DynChain* apFC[6];   // 0x0FC  }
    struct DynChain* ap114[6];  // 0x114  }
    f32       f12C;             // 0x12C  } five floats of the CHR object's header (fn_8001A9F4)
    f32       f130;             // 0x130  }
    f32       f134;             // 0x134  }
    f32       f138;             // 0x138  }
    f32       f13C;             // 0x13C  }
    f32       a140[128][3];     // 0x140  per bone, a factor for each axis: reset to 1 by fn_80028A3C,
                                //        multiplied by fn_80028A70 (the next field is at 0x740)
    f32       q740[4];          // 0x740  } rotations (quaternions) fn_80029968 fills in
    f32       q750[4];          // 0x750  }
    void*     p760;             // 0x760  } fn_80029A90 does nothing unless all three are set
    f32     (*p764)[4][4];      // 0x764  }   a matrix per bone, transformed into p768
    f32     (*p768)[4][4];      // 0x768  }
    s32       n76C;             // 0x76C  matrices in p768
} CharModel;

// DynChain.c (EA's name; our type names): a chain of bones that swings on its own, from a bone
// down through its children (fn_80114270).
typedef struct DynChainLink {
    f32  fLength;               // 0x00  to its parent bone in the rest pose (0.5 unless type 0)
    f32  v04[4];                // 0x04  its matrix's position when set up; the updates move it
    f32  v14[4];                // 0x14  v04 as the last update left it (fn_80114540)
    f32  v24[4];                // 0x24  set by fn_80029BC8; the updates keep a velocity in it
    f32  v34[4];                // 0x34  from its bone to the next one's position (fn_80114A84)
    f32  q44[4];                // 0x44  } its rest pose's rotation, twice
    f32  q54[4];                // 0x54  }
    f32  v64[4];                // 0x64  } and position, twice
    f32  v74[4];                // 0x74  }
    s32  nBone;                 // 0x84
    s32  nParent;               // 0x88  its bone's parent
    f32  f8C;                   // 0x8C  fn_80114540 keeps the link's stretch in it
} DynChainLink;
LAYOUT_ASSERT(DynChainLink, 0x90);

typedef struct DynChain {
    s32  nBone;                 // 0x00  the top bone; -1 or 0xFF: none
    s32  nLinks;                // 0x04
    DynChainLink* pLinks;       // 0x08
    s32  nType;                 // 0x0C  0..3: which update runs (fn_8011443C)
    s32  n10;                   // 0x10  its kind, 0..5 (fn_80115348 indexes the settings by it)
    s32  n14;                   // 0x14  } counters the updates advance
    s32  n18;                   // 0x18  }
    u8   bReset;                // 0x1C  set up the links again on the next update
    u8   pad1D[3];
} DynChain;
LAYOUT_ASSERT(DynChain, 0x20);

// Twelve values of DynChainSettings, twice (fn_80113E60 sets both the same way).
// Each is per link (fn_80115348 indexes them by the link; a chain of more than three links reads
// on into the next array).
typedef struct DynChainParams {
    f32  a0[3];                 // 0x00  the sway's size, in degrees
    f32  aC[3];                 // 0x0C  the sway's period, in minutes (60 * FRAME_RATE frames)
    f32  a18[3];                // 0x18  the sway's phase, in turns
    f32  a24[3];                // 0x24  an angle added to the sway, in degrees
} DynChainParams;

// The chains' settings (lbl_802824F8, made by fn_80113E60; DynChain.c's updates read them).
typedef struct DynChainSettings {
    DynChainParams aParams[2];  // 0x00
    f32  f60;                   // 0x60
    f32  f64;                   // 0x64
    f32  f68;                   // 0x68
    f32  f6C;                   // 0x6C
    f32  f70;                   // 0x70
    f32  f74;                   // 0x74
    f32  f78;                   // 0x78
    f32  f7C;                   // 0x7C
    f32  f80;                   // 0x80
    f32  f84;                   // 0x84
    f32  f88;                   // 0x88
    f32  f8C;                   // 0x8C
    f32  f90;                   // 0x90  how much fn_80116304's sway takes off 1
    f32  f94;                   // 0x94  } the base fn_80116304 adds, from f94 to f98 as its
    f32  f98;                   // 0x98  } fStrength goes from 0 to 35
    s32  an9C[6];               // 0x9C  per chain kind (DynChain.n10): fn_80115348 sways it
    s32  nB4;                   // 0xB4  fn_80115348's wind direction: 0 the wind's, else a fixed one
    s32  nB8;                   // 0xB8  the strength fn_80116468 gives; -1: fn_80055F80's
    s32  nBC;                   // 0xBC
} DynChainSettings;
LAYOUT_ASSERT(DynChainSettings, 0xC0);

extern DynChainSettings* lbl_802824F8;
extern f32 lbl_80193DE8[6][4];  // DynChain.c: a direction per chain kind, in the model's root space
extern f32 lbl_80193E48[6];     // DynChain.c: an angle per chain kind, in degrees (fn_80115B2C)

// Skeleton.c: a short string per bone id (the first is empty); the model loader (fn_80028564)
// copies bone 0x54's first 8 bytes into each bone it adds as the bone's uId.
extern char* lbl_80187278[90];
// Skeleton.c: the names of the club models' bones ("IGDriver", "IGputter", "IGiron3", "IGiron7",
// "IGwedge"); fn_80029664 gives a model bone with one of them bone id 0x52's index.
extern char* lbl_80187418[5];
extern struct Character* lbl_80281D20;   // Skeleton.c: the character fn_80027E8C moves with its root
extern u8 lbl_80281098[6];      // Skeleton.c: the bone ids of the model's kind 2 dynamic chains
extern u8 lbl_802810A0[6];      // Skeleton.c: the bone ids of its kind 3 dynamic chains

// A clip's header (the fields used here). In a file, pD0 marks the end of the header and
// uAram points at the end of the key data; once a clip's frames are streamed out, uAram is
// their ARAM address and flag 4 is set.
typedef struct Clip {
    u32    uFlags;              // 0x00  4: its frame data is in ARAM
    s32    n04;                 // 0x04  bytes of the second frame stream
    u8     unk08[4];
    s16    nFrames;             // 0x0C
    u8     unk0E[0xA];
    f32    f18;                 // 0x18  fn_8001BE88 blends up to it
    s32    n1C;                 // 0x1C
    u8     unk20[0xC];
    s32    n2C;                 // 0x2C
    u8     unk30[8];
    s32    n38;                 // 0x38  bytes of the first frame stream
    s32    n3C;                 // 0x3C
    s32    n40;                 // 0x40
    u8     unk44[4];
    s16    nEvents;             // 0x48  how many pEvents holds
    u8     unk4A[2];
    s32    n4C;                 // 0x4C
    s32    n50;                 // 0x50
    u8     unk54[0x10];
    s32    n64;                 // 0x64
    u8     unk68[0x80 - 0x68];
    f32    v80[3];              // 0x80  a point fn_8001DB04 puts through bone 0's matrix
    s16    n8C;                 // 0x8C  halfwords per frame, first stream
    s16    n8E;                 // 0x8E  bytes per frame, second stream
    u64    u90;                 // 0x90  looked up in lbl_801B9638 (FEgolferanim.c fn_8008D058)
    u8     unk98[8];
    char   name[0x2C];          // 0xA0
    f32    fCC;                 // 0xCC  how far along the swing is, 0..1 (Character.fBackswing copies it)
    u8*    pD0;                 // 0xD0
    struct ClipEvent* pEvents;  // 0xD4  its timed events (fn_8001F02C finds one by its id)
    struct BlendClip* pD8;      // 0xD8  fn_800204A0 samples it; set: FEgolferanim.c turns the
                                //       golfer round for the clip
    u32    uAram;               // 0xDC
    u8     unkE0[4];
    u8*    pE4;                 // 0xE4
    u8     unkE8[4];
    u8*    pEC;                 // 0xEC
    u8*    pF0;                 // 0xF0
    u8*    pF4;                 // 0xF4
    u8*    pF8;                 // 0xF8
    u8*    pFC;                 // 0xFC
} Clip;

// char.c: run on a clip just read from disc (skalib.c, AnimStream.c).
void* fn_80020DD4(void* pClip, void* pOut, int nAlign);

typedef struct SKABlendNode SKABlendNode;

void Skalib_Init(void);                 // skalib.c
void Skalib_Shutdown(void);             // skalib.c
void fn_8001F64C(void);                 // mtalib.c
void fn_80071AD0(void);                 // animblender.c
void AnimLib_Free(AnimLib* pLib);       // skalib.c
void ClipBank_Release(int nSlot);       // skalib.c
void fn_8001F66C(void);                 // mtalib.c
void fn_80071B94(void);                 // animblender.c

// animblender.c's pools (fn_80071AD0 creates them, fn_80071B94 destroys them): blend tree nodes by
// type (0x34, 0x2C and 0x20 bytes), then pose buffers of format 0 (0x1040) and format 1 (0x114C).
extern UMemPool* lbl_80281E98;
extern UMemPool* lbl_80281E94;
extern UMemPool* lbl_80281E90;
extern UMemPool* lbl_80281E8C;
extern UMemPool* lbl_80281E88;
// animblender.c: clear bit nBit in the three blocks of pNode's format 1 pose buffer, and its sources'.
void fn_800736D8(SKABlendNode* pNode, s32 nBit);
void fn_80071F58(struct SKABlendNode** ppNode, u8 bFreeSources);   // animblender.c: gives a blend
                                        // tree back (bFreeSources: the sources' clips too)

// animblender.c: whether a source under pNode plays pSrc (format 0, format 1).
u8 fn_80073554(SKABlendNode* pNode, void* pSrc);
u8 fn_80073610(SKABlendNode* pNode, void* pSrc);

// The blend callback CharacterState_AddSKABlendData attaches (fn_80072ACC is one).
typedef void (*SKABlendFn)(SKABlendNode* pNode, int* pn, f32 fTime);

// animblender.c: set up *ppNode (taken from nType's pool when NULL) as a node of nType with pose
// format nFormat, bC set from nC.
void fn_80071C28(SKABlendNode** ppNode, int nType, int nFormat, SKABlendFn pfnBlend, int nC);
// animblender.c: make pNode a blend node that mixes its children with pfnBlend at fWeight.
void fn_800725BC(SKABlendNode* pNode, SKABlendFn pfnBlend, f32 fWeight);
f32  fn_80072938(SKABlendNode* pNode);  // animblender.c: the latest end time under pNode
// animblender.c: pNew plays pClip (a Clip, or an MtaLib from a MAL bank for a format 1 node).
void fn_800724C0(SKABlendNode* pNode, SKABlendNode* pNew, void* pClip, f32 fWeight);
// animblender.c: blend pNew into *ppNode over the window pBlend (six floats, fn_800958F8).
void fn_800720C8(struct Character* pChar, SKABlendNode* pNew, SKABlendNode** ppNode, f32* pBlend,
                 SKABlendFn pfnBlend, int b);

// A node of a character's SKA blend tree (animblender.c; the root is at Character + 0x40C). A node
// of type 1 blends its two children into its pose with pfnBlend; a node of type 0 plays one source
// from fFrom to fTo. fn_80071C28 takes nodes from three pools by type (0x34, 0x2C and 0x20 bytes),
// so the blend fields end at 0x2C; nGroup follows the root in Character.
struct SKABlendNode {
    s32  bPooled;               // 0x00  taken from a pool, so fn_80071F58 gives it back
    s32  nType;                 // 0x04  0: plays a source, 1: blends apChild
    s32  nFormat;               // 0x08  its pose buffer's format, 0 (0x1040 bytes) or 1 (0x114C bytes)
    u8   bC;                    // 0x0C
    u8   padD[3];
    f32  fStart;                // 0x10  a blend's is its children's earliest (fn_800728D8)
    f32  fEnd;                  // 0x14  a blend's is its children's latest (fn_80072938)
    f32  fWeight;               // 0x18  its share of its parent's blend (fn_80072ACC)
    struct SkelPose* pPose;     // 0x1C  its pose buffer, taken from the pool for nFormat
    union {
        struct {
            SKABlendFn pfnBlend;            // 0x20
            SKABlendNode* apChild[2];       // 0x24
        } blend;                            // nType 1
        struct {
            void* pSrc;                     // 0x20  a Clip for nFormat 0 (fn_8001F02C)
            f32   fFrom;                    // 0x24
            f32   fTo;                      // 0x28
        } src;                              // nType 0
    } u;
    s32  nGroup;                // 0x2C  the clip group CharacterState_AddSKABlendData last added (the
                                //       root only: past the end of a pooled blend node)
};
LAYOUT_ASSERT(SKABlendNode, 0x30);

// What Clip.pD8 points at; only what the swing reads.
typedef struct BlendClip {
    u8   unk0[8];
    f32  f08;                   // 0x08  added to the time fn_800204A0 samples the clip at
    f32  f0C;                   // 0x0C
} BlendClip;

// One of a clip's timed events (Clip.pEvents; fn_8001F02C finds one by its id). Event 2's time is
// the ball-hit time the swing measures (fn_8001C860 starts the skeleton's clip at it).
typedef struct ClipEvent {
    u32  uId;                   // 0x0
    f32  fTime;                 // 0x4
    u8   unk8[8];
} ClipEvent;
LAYOUT_ASSERT(ClipEvent, 0x10);

// One of a character's four data buffers (Character.buffers): pBuf holds three runs of 16-byte
// entries, p0C..p18 mark where they start and end, their counts read from p04's +0x60, +0x58 and
// +0x5C (the code at 0x8001FE50 fills them; fn_8001DB98 empties them, fn_8001C0E0 frees pBuf).
typedef struct CharBuffer {
    s32   n00;                  // 0x00  -1 when empty
    void* p04;                  // 0x04  what the buffer was filled for
    u8*   pBuf;                 // 0x08
    u8*   p0C;                  // 0x0C
    u8*   p10;                  // 0x10
    u8*   p14;                  // 0x14
    u8*   p18;                  // 0x18
} CharBuffer;
LAYOUT_ASSERT(CharBuffer, 0x1C);

// One of an animation player's ten entries (our name; 0x18 bytes).
typedef struct AnimPlayerEntry {
    struct AnimPlayerEntry* pNext;  // 0x00  } fn_80072D90 chains the ten in order
    struct AnimPlayerEntry* pPrev;  // 0x04  }
    u8    unk8[0x18 - 0x8];
} AnimPlayerEntry;
LAYOUT_ASSERT(AnimPlayerEntry, 0x18);

// An animation player (0x138 bytes); only what is read. Character has two: the one at 0x164, whose
// fields are named in Character directly, and anim29C.
typedef struct AnimPlayer {
    s32   n00;                  // 0x00  } reset to 0 and -1 by fn_8001BE88
    s32   uFlags;               // 0x04  fn_8007325C sets bit 2, fn_8007326C clears bits 1 and 2
    s32   n08;                  // 0x08  }
    s32   nC;                   // 0x0C  } set together by fn_800958EC
    f32   f10;                  // 0x10  }
    f32   f14;                  // 0x14  1 after fn_80072D90; fn_800737B4 scales its time step by it
    f32   fTime;                // 0x18
    f32   fStart;               // 0x1C  } Anim_SetTime's -30000 and -10000 stand for these
    f32   fEnd;                 // 0x20  }
    f32   f24;                  // 0x24  } fn_800737B4: with uFlags bit 3, f28 climbs to f24; with
    f32   f28;                  // 0x28  } bit 4, it falls to f2C (at 0 bit 4 gives way to bit 0),
    f32   f2C;                  // 0x2C  } and the step is scaled by f28 / f24 on the way
    f32   f30;                  // 0x30  with uFlags bit 7, fn_80072ED8 counts it down to 0, then
                                //       clears bits 0 and 7
    f32   f34;                  // 0x34  fn_80073108: a clock that drives a sway of three cosines
    f32   f38;                  // 0x38  fn_80073108: the time the sway is centred on
    s32   n3C;                  // 0x3C  } cleared by fn_80072D90
    s32   n40;                  // 0x40  }
    struct AnimPlayerEntry* p44;    // 0x44  a48[0] after fn_80072D90
    AnimPlayerEntry a48[10];    // 0x48  chained both ways by fn_80072D90
} AnimPlayer;
LAYOUT_ASSERT(AnimPlayer, 0x138);

void fn_80072D90(AnimPlayer* pPlayer);  // animblender.c: reset a player

// An entry of Character.p44 (0x30 bytes), read from the CHR object by fn_8001A9F4.
typedef struct CharEntry44 {
    f32   v0[3];                // 0x00
    f32   fC;                   // 0x0C  1 when loaded
    u32   a10[6];               // 0x10
    u8    unk28[0x30 - 0x28];
} CharEntry44;
LAYOUT_ASSERT(CharEntry44, 0x30);

// What fn_8001A9F4 hands the skeleton loader (fn_80028564) for a golfer's model (our name):
// lbl_80280E10, or lbl_80280E18 in split screen; a table of 0x10-byte entries and their count.
typedef struct CharModelDefs {
    IKChainDef* pDefs;          // 0x0  (fn_80028314 builds a chain from each)
    s32   nDefs;                // 0x4
} CharModelDefs;

extern CharModelDefs lbl_80280E10;
extern CharModelDefs lbl_80280E18;

// Per club class, an offset (x, y, z) fn_8001C860 places the golfer by (0x4C bytes: one more
// float follows the six).
extern f32 lbl_80187184[6][3];

// The golfer's character object (0x1798 bytes or more); only the fields read so far. Anim_SetRate,
// Anim_SetTime and fn_8007326C take the address of its animation player at 0x164, whose fields
// from 0x168 on are named here directly.
typedef struct Character {
    s32   nIndex;               // 0x000  its entry in lbl_801B9624 (fn_8001C21C)
    s32   nPlayer;              // 0x004  the player it belongs to (Player_SetGolfer); 1000 for the
                                //        characters fn_8001D324 finds by id
    u32   uId;                  // 0x008  the id of the 'SKLO' object it was built from (fn_8001D3EC);
                                //        fn_8001D324 finds it by this
    s32   nC;                   // 0x00C  the golfer's id (FEgolferanim.c: 7 and 29 are special)
    u32   u10;                 // 0x010  bit 0x40 tested by the game manager and the swing; bit 0x8000
                                //        cleared by CharacterState_AddSKABlendData
    f32   f14;                  // 0x014  set to 2^30 (never) by Character_UpdateAnimation; for a
                                //        character that is not a golfer it is tested against
                                //        fn_8001ED44 first
    s32   n18;                  // 0x018  cleared with the animation state (fn_8001D7EC)
    s32   nAnim;            // 0x01C  the playing animation (6 backswing, 7 downswing)
    s32   n20;                  // 0x020
    s8    n24;                  // 0x024  } counters CharacterState's idle update (fn_80096398) runs down
    s8    n25;                  // 0x025  }
    s8    n26;                  // 0x026  set while that update's clip plays
    u8    unk27;
    s32   u28;                  // 0x028  bit 0: a state change is waiting (fn_80096F0C)
    s32   n2C;                  // 0x02C  tested for 0 (PreShotInit) and for 4 or 5 (ShotSetupInit)
    s32   n30;                  // 0x030
    s32   nSlot;                // 0x034  the animation slot it uses (skalib); the CrAP camera's shot names
                                //        get an 'f' in front when it is 1
    CharModel* pModel;          // 0x038
    struct Skin* pSkin;         // 0x03C  its body's skin (Skin.c), the first of apSkins
    s32   n40;                  // 0x040  how many entries p44 holds (fn_8001A9F4)
    struct CharEntry44* p44;    // 0x044  } freed with the character (fn_8001C0E0)
    s32   n48;                  // 0x048  a texture bank slot (LLTexGrp.c), freed with it when >= 0
    u8*   p4C;                  // 0x04C  where its CHR object's data ends (fn_8001A9F4)
    TexBank* p50;               // 0x050  its texture bank, bank78 (fn_80019798); LLDynTex.c is given
                                //        this field's address (fn_80019DE8)
    s32   hFile;                // 0x054  a file closed with it (fn_8001971C), -1 none
    s32   n58;                  // 0x058  } from its CHR object's texture header (fn_80019798)
    s32   n5C;                  // 0x05C  } 0 without palettes
    void* p60;                  // 0x060  the entry of a64 fn_80019DE8 set up
    void* a64[2];               // 0x064  } entries taken from lbl_801B95E8 (fn_8001A418), and their
    s8    a6C[2];               // 0x06C  } indices there (-1 once given back)
    u8    unk6E[2];
    s32   n70;                  // 0x070  how many of a64 it takes
    s32   n74;                  // 0x074  the one of a64 fn_80019E80 uses
    TexBank bank78;             // 0x078  its textures (fn_80019798)
    TexEntry* pA8;              // 0x0A8  } bank78's tables, freed by fn_8001971C
    s32   nAC;                  // 0x0AC  how many textures pA8 holds
    TexPalette* pB0;            // 0x0B0  }
    s32   nB4;                  // 0x0B4  how many palettes pB0 holds
    void* pB8;                  // 0x0B8  } 64 bytes per texture
    void* pBC;                  // 0x0BC  } a byte per palette
    struct Skin* apSkins[7];    // 0x0C0  its skins: the body's, then its attachments' (fn_8001CE5C)
    s32   nSkins;               // 0x0DC
    u8    bE0;                  // 0x0E0  cleared by fn_8001A3B0, set by fn_8001A20C
    char  szE1[0x164 - 0xE1];   // 0x0E1  its texture file's name, hFile (fn_8001A870); the size is
                                //        unknown (up to the next known field)
    u8    anim[4];              // 0x164  the animation player (+0x14 is its playback rate)
    s32   uFlags;               // 0x168  bit 0x40: the backswing is being backed down; 0x200 / 0x400: the
                                //        clip lookup fell back (Char_SetClip). Signed: the original tests
                                //        it with cmpwi
    s32   n16C;                 // 0x16C  set to -1 by fn_8001D020
    s32   n170;                 // 0x170  } the state queued for when fAnimTime reaches f174
    f32   f174;                 // 0x174  }   (CharacterState_UpdateSKAState; fn_800958EC sets both)
    u8    unk178[0x17C - 0x178];
    f32   fAnimTime;            // 0x17C
    f32   f180;                 // 0x180  fn_8001966C: fAnimTime = f180 + the blend's time - v1638[1]
    f32   fAnimEnd;             // 0x184  the animation's end time
    u8    unk188[0x198 - 0x188];
    f32   f198;                 // 0x198  } set to 0 and the animation time when state 8 starts
    f32   f19C;                 // 0x19C  }   (CharacterState_UpdateSKAState)
    u8    unk1A0[0x29C - 0x1A0];
    AnimPlayer anim29C;         // 0x29C  a second animation player
    s32   n3D4;                 // 0x3D4  the bytes of its CHR object before the animation library
    AnimLib* pLib;              // 0x3D8  its animation library
    struct ClipRecord* pRecords;    // 0x3DC  records for its merged library (skalib)
    u8    node3E0[0x40C - 0x3E0];   // 0x3E0  a blend node for anim29C (fn_800732F4 takes it as it takes blend)
    SKABlendNode blend;         // 0x40C  the root of its blend tree
    CharBuffer buffers[4];      // 0x43C
    struct { u32 bSet; f32 fTime; u8 unk8[8]; } events[18];   // 0x4AC  animation events, by 64-bit id
    s32   n5CC;                 // 0x5CC
    u8    unk5D0[0x1614 - 0x5D0];
    char  sz1614[16];           // 0x1614  a name the situation scripts test (fn_800BB7AC)
    Clip* pBlend;               // 0x1624
    f32   fBackswing;           // 0x1628  how far along the backswing is, 0..1 (pBlend's fCC, copied every
                                //         frame of the backswing; the swing's power is its square root)
    f32   f162C;                // 0x162C
    f32   f1630;                // 0x1630
    f32   f1634;                // 0x1634
    f32   v1638[3];             // 0x1638
    f32   f1644;                // 0x1644
    u8    unk1648[0x1650 - 0x1648];
    s32   n1650;                // 0x1650  cleared by fn_8001942C
    s32   n1654;                // 0x1654  (fn_8001EE90)
    s32   n1658;                // 0x1658
    f32   f165C;                // 0x165C  } scaled by the view's lens (fn_8001EE00, fn_8001ED44)
    f32   f1660;                // 0x1660  }
    f32   f1664;                // 0x1664  } fn_8001B878: 1 near the camera, fading to 0 from 6 to 15
    f32   v1668[3];             // 0x1668  } its bounding sphere, tested against the camera
    f32   f1674;                // 0x1674  } (fn_8001B878)
    f32   vMin[4];              // 0x1678  } the box around its bones (fn_8001B644), grown by 0.33;
    f32   vMax[3];              // 0x1688  } v1668 and f1674 are its centre and half its diagonal
    u8    unk1694[0x1698 - 0x1694];
    s32   n1698;                // 0x1698
    s32   nClubClass;           // 0x169C  the club class for clip lookups (Char_SetClip; 1 looks up as 0)
    s32   nClubHeadBone;        // 0x16A0  bone 0x53's index: the club head (the swing trail's end)
    s32   nGripBone;            // 0x16A4  bone 0x52's index: the grip (the trail's other end)
    s32   n16A8;                // 0x16A8  fn_8001EEE4's answer for bone 0x15
    f32   q16AC[4];             // 0x16AC  } the grip bone's rotation and offset from the root while
    f32   v16BC[4];             // 0x16BC  } flag 0x4000 holds it (fn_8001BD18)
    s32   nClub;                // 0x16CC  the club (fn_8001C774)
    s32   nShotKind;            // 0x16D0  the player's shot kind (fn_8001C724)
    s32   n16D4;              // 0x16D4  the key for clip lookups (Char_SetClip)
    struct CharSkinSet* p16D8;  // 0x16D8  six more skins (SkinPart.c)
    s32   n16DC;                // 0x16DC  twice the players set up so far, in split screen 2
                                //         (Player_SetGolfer)
    s32   nStyle;               // 0x16E0  the animation style (fn_8001C7FC); at -1
                                //         CharacterState_AddSKABlendData does nothing
    f32   aPoints[5][4];        // 0x16E4  points Character_PlaceFeetOnGround sets the heights of; the
                                //         skeleton code (0x80027FF8) moves them in x and z
    f32   aGroundNormal[4][4];  // 0x1734  } the ground under points 0-3 (Character_UpdateFeetTerrainInfo)
    f32   afGroundHeight[4];    // 0x1774  }
    s32   n1784;                // 0x1784  set to -1 by Character_SetPosition
    Clip* pCurClip;             // 0x1788  the clip Char_SetClip picked
    struct MtaLib* p178C;       // 0x178C  the MAL library the second player plays (fn_80095FD0);
                                //         cleared by fn_8001BE88
    Clip* p1790;                // 0x1790  cleared by fn_80062BFC; CharacterState_AddSKABlendData plays it for
                                //         groups 5, 6 and 10
    void* p1794;                // 0x1794  cleared by fn_80062BE8; the same for group 9
    Clip* p1798;                // 0x1798  cleared by fn_8001942C; with n2C 6, fn_8001C650 and
                                //         fn_8001C860 set n16D4 to 4 when it is 0
    f32   a179C[4];             // 0x179C  cleared by Character_PlaceFeetOnGround; fn_80017DDC acts only
                                //         while a179C[1] is above 0.9
    void* p17AC;                // 0x17AC  its slider definitions (CharSlider_CreateDefinitionsFromMem,
                                //         fn_8001A9F4); fn_8001DC64 applies them
    void (*pfn17B0)(void);      // 0x17B0  called by Character_UpdateAnimation before the bones are
                                //         transformed; cleared by fn_8001942C
    s8    n17B4;                // 0x17B4  cleared by fn_8001942C; Skin.c hands it to fn_800CE02C as
                                //         the a10A0 index
    u8    unk17B5[0x17B8 - 0x17B5];
    struct SkinChoices* pChoices;   // 0x17B8  its look (fn_8001D4A4 dresses it from this); fn_8001A20C
                                    //         puts its logos on the model (fn_8001744C)
} Character;

// The players' characters (gViewSlots, 0x80187124): Player_SetGolfer takes the player's from here.
typedef struct ViewSlot {
    void*      pUnk;            // 0x0
    Character* pChar;           // 0x4
} ViewSlot;

extern ViewSlot gViewSlots[5];          // 0x80187124  per player

// Club names as 64-bit ids ("IGdriver", [1] unset, "IGputter", "IGiron3", "IGiron7", "IGwedge"),
// set by fn_8001C37C; FEgolferanim compares ids against them.
extern u64 lbl_801B9638[6];

extern f32 lbl_80189A30[4];             // (0, 0, 0, 0): where fn_8001D020 places the menu's golfer

// char.c: the club skins' part and set names, one per club kind (0 drivers, 1 fairway woods,
// 2 putters, 3 and 4 the 3 and 7 irons, 5 wedges), for Character_SetClubStatesForCharacter
extern char lbl_80186EC0[6][13];        // the parts: "Drivers" ...
extern char lbl_80186F10[6][13];        // "fwd_shaft", "pwi_shaft" sets
extern char lbl_80186F60[6][13];        // "Defaults", their variants
extern char lbl_80186FB0[6][13];        // "EA_Driver" ... sets
extern char lbl_80187000[6][13];        // "Defaults"
extern char lbl_80187050[6][13];        // "fwd_grip", "pwi_grip" sets
extern char lbl_801870A0[6][13];        // "Defaults"

void  Character_SetClubStatesForCharacter(Character* pChar, int nSlot, struct SkinChoices* pChoices);

// Skeleton.c
extern f32 lbl_801C6498[4];             // the identity rotation (quaternion), set by fn_80029530
extern u8  lbl_802810A6;                // IK on (fn_80027738); off, the IK functions do nothing
extern u8  lbl_8018742C[42][2];         // pairs of standard bones (fn_80029804 reads the first 41)
extern f32 lbl_80186838[4];             // a zero vector (fn_80029BC8 copies it)

// AnimStream.c: the animation groups it streams clips for (groups 1 and 5, the reactions), and the
// index each has in its tables.
typedef struct AnimStreamGroup {
    u8   nGroup;                // 0x0
    s32  nIndex;                // 0x4
} AnimStreamGroup;
LAYOUT_ASSERT(AnimStreamGroup, 8);

extern AnimStreamGroup lbl_80191490[2];

// A buffer the stream reads clips into.
typedef struct AnimStreamBuf {
    void* pData;                // 0x0  64-aligned
    s32   nSize;                // 0x4
} AnimStreamBuf;
LAYOUT_ASSERT(AnimStreamBuf, 8);

// A player's streamed clips for one group, style and club class.
typedef struct AnimStreamClips {
    s32  nNext;                 // 0x0  the next of them to play (wraps around)
    s32  nMaxSize;              // 0x4  the largest of them, in bytes
    u8   b8;                    // 0x8  set by fn_800CA268
    u8   pad9[3];
} AnimStreamClips;
LAYOUT_ASSERT(AnimStreamClips, 0xC);

// A player's part of the stream.
typedef struct AnimStreamPlayer {
    s32  nId;                   // 0x000  -1 unused; fn_800CB568 finds a player's part by it
    AnimStreamClips clips[2][8][6];     // 0x004  [group index][style][club class]
} AnimStreamPlayer;
LAYOUT_ASSERT(AnimStreamPlayer, 0x484);

// The stream's state (lbl_80282230, allocated by fn_800C937C).
typedef struct AnimStream {
    AnimStreamBuf*   p0;        // 0x0000  the buffer the current read fills (a clip, fn_800C9F14)
    AnimStreamClips* p4;        // 0x0004  the clips it is for
    AnimStreamBuf bufs[2][2][8][6];     // 0x0008  [double buffer][group index][style][club class]
    AnimStreamPlayer players[5];        // 0x0608
    void* pRead;                // 0x1C9C  the read buffer
    s32   nReadSize;            // 0x1CA0
    s32   aPlayerBytes[5];      // 0x1CA4  bytes each player's clips need
    s32   nBytes;               // 0x1CB8  bytes the stream uses in all
    s32   hFile;                // 0x1CBC  the open file, -1 none
    s32   nState;               // 0x1CC0  0 idle, 1 reading, 2 read (fn_800C9EFC)
    s32   nResult;              // 0x1CC4  the bytes the last read got (its callback's nBytes)
    s32   n1CC8;                // 0x1CC8
    u8    bReadDone;            // 0x1CCC  set when a waited-for read finishes (fn_800CB550)
    u8    bOn;                  // 0x1CCD  streaming is on (off in split screen, multiplayer and some
                                //         modes)
    u8    pad1CCE[2];
} AnimStream;
LAYOUT_ASSERT(AnimStream, 0x1CD0);

extern AnimStream* lbl_80282230;

u8    fn_800C9828(int nGroup, int nStyle, int nClub, int nKey);   // the clips are streamed
void  fn_800CA9DC(int nSlot);

void  fn_800177A0(Character* pChar, SkelPose* pPose);   // a blend node's pose from the body skin
void  fn_80017864(Character* pChar, SkelPose* pPose);   // only its bit arrays
void  fn_80018484(Character* pChar, CharModel* pModel);
void  fn_8001C0E0(Character* pChar);    // frees the character
void  fn_8001C5B4(Character* pChar, int n);
void  fn_8001D238(void);
void  fn_8001D4A4(Character* pChar, int nSlot);   // dresses the character (its skins and clubs)
void  fn_8001DC64(Character* pChar, struct SkinChoices* pChoices);  // applies a look (char.c)
void  fn_8001EE98(Character* pChar, u8 b);    // sets the model's bEE
void  Character_SetPosition(Character* pChar, f32* pPos, u8 bPlace);
int   fn_8001C558(int nPlayer);          // the model id of the player's golfer
void  fn_8001C724(Character* pChar, int nKind);
void  fn_8001C774(Character* pChar, int nClub);
void  fn_8001C7FC(Character* pChar, int nStyle);   // the animation style (nStyle)
Character* fn_8001D324(int nId);        // the character with this id (100: the flag, by its clips), or NULL
void  fn_8001D624(int n);               // set gSession.aD2D[n]
void  fn_8001D7A4(Character* pChar);
void  fn_8001DA04(Character* pChar, f32* pPos, f32* pAngles);   // where the hand holds the ball
void  fn_8001DB04(Character* pChar, f32* pOut);    // the golfer's position
void  fn_8001DB98(Character* pChar);    // empty the character's four data buffers
u8    fn_8001DBF4(Character* pChar);    // the ball is in the golfer's hand
void  Character_GetBallOnFingerPosition(Character* pChar, f32* pPos);
f32 (*fn_8001ED08(Character* pChar, int nBone))[4];  // a bone's matrix
u8    fn_8001EDF4(Character* pChar);    // the model's bEE
int   fn_8001EE88(Character* pChar);    // n1658
int   fn_8001EE90(Character* pChar);
int   fn_8001EED8(CharModel* pModel, int nBone);    // a bone's index
int   fn_8001EEE4(CharModel* pModel, int nBone);
f32   fn_8001F02C(struct Clip* pClip, u64 uEvent);   // an event's time (by its 64-bit id)
void  Anim_SetRate(u8* pAnim, f32 fRate);           // 0x8001F084
// Plays a clip on the character: blended in from the current one, or (bNoBlend) from scratch.
void  fn_8001BE88(Character* pChar, Clip* pClip, int bNoBlend, f32 fTime);
void  fn_8001E85C(f32* pSrc, f32* pDst);            // copy a quaternion
void  SKEL_SetIKSolutionWeight(Skeleton* pSkel, f32 f);
void  fn_80027108(Skeleton* pSkel);                                         // Skeleton.c
void  SKEL_TranslateIKChainY(CharModel* pModel, IKChain* pChain, f32 f);   // Skeleton.c
void  fn_80027808(CharModel* pModel, f32* pRot);
void  fn_8002792C(Skeleton* pSkel);
void  SKEL_TransitionIK(Skeleton* pSkel, u8 b, f32 f);
void  fn_80028A3C(CharModel* pModel);
void  fn_80028A70(CharModel* pModel, int nBone, u32 uAxes, f32 f);
void  fn_80029530(void);
void  fn_8002955C(void);
void  fn_8002957C(CharModel* pModel);
int   fn_800298F4(CharModel* pModel, u64 uId);
void  fn_80029948(CharModel* pModel, struct DynChain* pChain, f32 f);
void  fn_80029A74(CharModel* pModel, void* p);
void  fn_80029A7C(CharModel* pModel, f32 (*pMatrices)[4][4], s32 nMatrices);
void  fn_80029A88(CharModel* pModel, f32 (*pMatrices)[4][4]);
void  fn_80029A90(CharModel* pModel, f32 (*pMtx)[4], int nBone);
void  fn_80029AF8(CharModel* pModel);
int   fn_80048574(Character* pChar, u64 uEvent);    // the character's animation has event uEvent
u8    fn_8009637C(Character* pChar);    // CharAnim.c: n26 is not 1 (both callers mask the result)
void  fn_80072ACC(SKABlendNode* pNode, int* pn, f32 fTime);
f32   fn_80072CB8(SKABlendNode* pNode, u64 uEvent); // an event's time in a blend tree
void  fn_80072ED8(AnimPlayer* pPlayer, SKABlendNode* pNode, f32 fT);    // advance a player
void  fn_80073108(Character* pChar, int nPlayer, AnimPlayer* pPlayer, SKABlendNode* pNode, f32 fT);
void  fn_8007326C(u8* pAnim);
void  Anim_SetTime(u8* pAnim, f32 fTime);           // 0x8007327C
u8    fn_800734A0(SKABlendNode* pNode);
void  fn_80095744(Character* pChar, int nAnim);     // play an animation
int   fn_80095780(Character* pChar);    // the animation playing
int   fn_80095798(Character* pChar);
void  fn_800957B0(Character* pChar, int a);
void  fn_800957D8(Character* pChar);
void  fn_800957FC(Character* pChar, u8 bReset);   // CharAnim.c: stop the second player
void  CharacterState_AddSKABlendData(Character* pChar, u8 bReset, int nGroup, SKABlendFn pfnBlend, int nC,
                                     int nAnim, f32 fStart, f32 fFrom, f32 fTo, f32 fOffset, f32 fTime);
void  CharAnim_StartTapIn(Character* pChar);
void  CharacterState_UpdateSKAState(Character* pChar);
void  fn_800CC5C0(Character* pChar, char* pA, char* pB);   // an attachment (the glove) on / off

// ---- the skeletal animation library (skalib.c) ------------------------------------------------

// An animation library (a SAL object: glbchar.gcb holds the shared male/female ones, and each
// character's CHR object embeds its own). On disc it is little-endian; the loader swaps it and
// fills in the pointers. Clips are found through a tree of s16 byte offsets into pTree:
//   group node:  [0] default leaf, [1 + style] style node (8 styles)
//   style node:  [club] club node (6 club classes)
//   club node:   [1] default leaf, [2 + key] leaf (11 keys)
//   leaf:        [0] clip count, [1] first entry in ppClips, then a u32 "played" mask (AnimLeaf)
struct AnimLib {
    s32    groups[64];          // 0x000  byte offset of each group's node, -1 none (21 used)
    u64    uId;                 // 0x100  must match the clip bank's when the clips live there
    s32    n108;                // 0x108
    s32    nDefault;            // 0x10C  the leaf used when a group has nothing
    s32    nTreeSize;           // 0x110
    s32    nClips;              // 0x114  entries in ppClips
    s32    nRecords;            // 0x118  clip records (ClipRecord)
    u8*    pTree;               // 0x11C
    void** ppClips;             // 0x120  clip pointers (Clip)
    s16*   pIndex;              // 0x124  record of each clip
    void*  pFile;               // 0x128  the loaded file this library sits in
    s32    n12C;                // 0x12C
    struct ClipRecord* pRecords;    // 0x130
    u8*    pClipData;           // 0x134  the library's own clips
    struct ClipBank* pBank;     // 0x138  set in the file: the clips are in a bank instead
    u32    uFlags;              // 0x13C  1: the library carries its own clips
    s32    n140;                // 0x140
    s16    n144;                // 0x144
    s16    nClips2;             // 0x146
};
LAYOUT_ASSERT(AnimLib, 0x148);

// A clip's record in a library (a 16-character name first).
typedef struct ClipRecord {
    char   name[16];            // 0x00
    s16    n10;                 // 0x10  leaves using the clip, while merging
    s16    n12;                 // 0x12  merge flags: 1 keep, 2 / 0x10 moved (pClip then points to the
                                //       record it went to)
    s32    n14;                 // 0x14
    s32    n18;                 // 0x18
    void*  pClip;               // 0x1C  offset into the clip data on disc, pointer once loaded
    s32    n20;                 // 0x20
} ClipRecord;
LAYOUT_ASSERT(ClipRecord, 0x24);

// A clip bank: clips shared by several libraries (a 0x20-byte header, the clip offsets, then the
// clips, each 16-aligned).
typedef struct ClipBank {
    u64    uId;                 // 0x00
    s32    nClips;              // 0x08
    s32    n0C;                 // 0x0C
    void** ppClips;             // 0x10
    void*  pFile;               // 0x14  the loaded file, while the bank owns it
    u8*    pRecords;            // 0x18  a planned bank: where its records go
    u32    n1C;                 // 0x1C
} ClipBank;
LAYOUT_ASSERT(ClipBank, 0x20);

// A library that can be layered over a slot's own.
typedef struct LibOverlay {
    AnimLib* pWork;             // 0x00  the loaded (swapped) copy
    void*  pCopy;               // 0x04  the file as it came off the disc
    u32    nSize;               // 0x08
    Character* pChar;           // 0x0C  the golfer it was loaded for
    s32    n10;                 // 0x10
    s32    n14;                 // 0x14
    u8     bActive;             // 0x18
    u8     pad19[3];
    s32    nTree;               // 0x1C  the tree size before a merge
} LibOverlay;
LAYOUT_ASSERT(LibOverlay, 0x20);

// One of the three animation slots.
typedef struct LibSlot {
    AnimLib*   pLib;            // 0x000  the loaded (swapped) library
    void*      pCopy;           // 0x004  the file as it came off the disc
    u32        nSize;           // 0x008
    LibOverlay overlays[10];    // 0x00C
    s32        nOverlays;       // 0x14C
    s32        n150;            // 0x150
    u8*        pEnd;            // 0x154  the end of the slot's clip bank records
} LibSlot;
LAYOUT_ASSERT(LibSlot, 0x158);

// A leaf of the clip tree (see AnimLib); while two trees are merged its mask holds flags
// instead: 1 keep this one, 2 replace it.
typedef struct AnimLeaf {
    s16 nCount;                 // 0x0
    s16 nFirst;                 // 0x2
    u32 uMask;                  // 0x4
} AnimLeaf;

// A club node of the clip tree (see AnimLib).
typedef struct AnimClubNode {
    s16 n0;                     // 0x00
    s16 nDefault;               // 0x02  the default leaf
    s16 aKeys[11];              // 0x04  the leaf of each key
    u8  unk1A[2];
    u32 uFlags;                 // 0x1C  1: AnimStream also takes the character's base clips here
} AnimClubNode;
LAYOUT_ASSERT(AnimClubNode, 0x20);

// What the merge adds up as it walks.
typedef struct MergeCtx {
    u32  n0;                    // 0x00
    s32* pCount;                // 0x04  clips still in use
    s32  nBytes;                // 0x08  bytes of clip data still in use
    s32  nTarget;               // 0x0C  bytes to get down to
    s32  nKeep;                 // 0x10  clips a leaf keeps
    s32  nMaxUsers;             // 0x14  clips shared by more leaves than this are not picked
} MergeCtx;

// The library a merge writes, and the records it copies clips into.
typedef struct BuildCtx {
    AnimLib*    pLib;           // 0x0
    ClipRecord* pRecords;       // 0x4
} BuildCtx;

// What was spent on each slot's clip bank, kept for reference.
typedef struct SlotStats {
    s32 nBudget;                // 0x00  bytes the bank may use
    s32 n04;                    // 0x04
    s32 nBytes;                 // 0x08  clip bytes wanted before trimming
    s32 nTrimmed;               // 0x0C  bytes the trim took off
    s32 nKeep;                  // 0x10  clips per leaf it settled on
    s32 nMaxUsers;              // 0x14
} SlotStats;
LAYOUT_ASSERT(SlotStats, 0x18);

// Called for each position of two clip trees walked side by side (AnimLib_WalkPair): level 0 the
// default leaves, 1 a group (its default leaves), 2 a style, 3 a club (its default leaves), 4 a
// key's leaves. nIndex is the group, style, club or key. A result above 0 stops the walk.
typedef int (*AnimLibWalkFn)(AnimLib* pA, AnimLib* pB, void* pLeafA, void* pLeafB, void* pCtx, int nLevel,
                             int nIndex);

extern u8          lbl_801B9730[0x6290];   // the staging buffers' space (see lbl_80281CC4)
extern u8          lbl_801BF9C0[0x6290];
extern u8          lbl_801C5C50[0x1DC];
extern u8          lbl_801C5E2C[0x1DC];
extern SlotStats   lbl_801C6008[3];
extern ClipBank*   lbl_801C6050[3];     // the clip bank of each slot
extern AnimLib*    lbl_801C605C[3];     // the library of each slot, when its clips are in the bank
extern LibSlot     lbl_801C6068[3];
extern u32         lbl_801C6470[3];     // ARAM copy of each slot's bank file
extern u32         lbl_801C647C[3];     // its size
extern UStreamObject* lbl_801C6488[3];  // each slot's bank file, while it is in main memory
extern u8          lbl_801D9908[0xC8];
extern s32         lbl_80281070;        // leaves this short are left alone by the drop pass
extern s32         lbl_80281074;        // clips a leaf may keep this round
extern u32         lbl_80281078;        // the current slot
extern u8*         lbl_80281CC4;        // staging buffers (32-aligned), see Skalib_Init
extern u8*         lbl_80281CC8;
extern u8*         lbl_80281CCC;
extern u8*         lbl_80281CD0;
extern u8          lbl_80281CD8;        // double buffering: libraries alternate between slots 0 and 1
extern u32         lbl_80281CDC;        // bytes of clips a slot may keep
extern UStreamObject* lbl_80281CE0;     // the buffer banks are brought back from ARAM into
extern u8          lbl_80281CE4;
extern s32         lbl_80281CE8;        // group, style, club and key being merged
extern s32         lbl_80281CEC;
extern s32         lbl_80281CF0;
extern s32         lbl_80281CF4;
extern s16*        lbl_80281CF8;        // the group, style and club node being built
extern s16*        lbl_80281CFC;
extern s16*        lbl_80281D00;
extern u32         lbl_80281D04[2];     // ARAM copy of each scratch area
extern u32         lbl_80281D0C[2];     // its size
extern char (*lbl_80281D14)[2][8][6][16];   // the last clip name played: [player][reaction kind][style][club]
extern u32         lbl_80281D18;
extern f32         lbl_80281D1C;

// The clip choice (skalib.c, char.c).
void* AnimLib_Pick(int nPlayer, AnimLib* pLib, int nGroup, int nStyle, int nClub, int nKey, u32* pFlags,
                   const char* pName);
void* Char_SetClip(Character* pChar, int nGroup, int nStyle, const char* pName);
void* fn_80017678(Character* pChar, int nGroup, int n);   // char.c: a random item of its 'MAL ' bank

// char.c: turning the character, and its dynamic textures (the menu golfer, FEgolferanim.c).
void  fn_800192D4(Character* pChar, f32 fAngle);void  fn_80019D64(Character* pChar, void (*pfnA)(Character* pChar), void (*pfnB)(Character* pChar));
void  fn_80019DE8(Character* pChar);
void  fn_80019E80(Character* pChar);
void  fn_80019EF4(Character* pChar);
void  fn_8001A0FC(Character* pChar);
s32  AnimLib_MergeOverlay(u8* pData, int nSlot);   // skalib.c; char.c's 'SAC ' handler
void  AnimLib_FreeWorkCopies(void);
void  AnimLib_ReloadSlot(void);
void* AnimLib_FindByName(AnimLib* pLib, const char* pName);   // a clip by name (NULL: none)

// Swing.c
f32   fn_8005CB78(Character* pChar, u64 uEvent);    // the time of an animation event
void  fn_8005CB88(Character* pChar, f32 f);       // sets f162C (none without a character)
f32   Char_GetBackswing(Character* pChar);       // fBackswing (0 without a character)
void  fn_8005CBB0(Character* pChar, f32 f);       // sets f1630
void  fn_8005CBC0(Character* pChar, f32 f);       // sets f1634
int   fn_80062B98(Character* pChar, u64 uEvent);
int   fn_80062BB0(Character* pChar, u64 uEvent);   // event uEvent is set and its time has come
void  fn_80062BE8(Character* pChar);
void  fn_80062BFC(Character* pChar);
int   fn_80062C10(Character* pChar);    // uFlags bit 4
int   fn_80062C1C(Character* pChar);    // uFlags bit 1
f32   fn_80062C28(Character* pChar);    // how far the animation still has to run

// ---- mtalib.c --------------------------------------------------------------------------------

// A group of items of a 'MAL ' bank.
typedef struct MalGroup {
    int    nNum;                // 0x0
    void** apItem;              // 0x4
} MalGroup;

// A bank loaded from a 'MAL ' stream object (0x1C bytes): up to three groups of items, one picked
// at random by fn_8001F79C.
typedef struct MalBank {
    int      nNumGroups;        // 0x00
    MalGroup aGroup[3];         // 0x04
} MalBank;
LAYOUT_ASSERT(MalBank, 0x1C);

// An animation library as fn_8001F110 byte-swaps and links it in place (MtaLib, MtaRecord and
// MtaEntry are our names): a 0x34-byte header, its records, each record's entries, then each
// entry's data (4-byte aligned). An entry is a track of one value per frame, a byte each: frame
// byte b gives fLo + (fHi - fLo) * b / 256 (fn_8001F32C).
typedef struct MtaEntry {
    u8     unk00[0x20];
    s32    nMorph;              // 0x20  the morph whose weight it drives (SkelPoseBlock.af8); < 0: none
    s32    nBytes;              // 0x24  the bytes of its data
    s32    nLastFrame;          // 0x28
    f32    fFrameTime;          // 0x2C  the time per frame
    u8     unk30[4];
    f32    fLo;                 // 0x34
    f32    fHi;                 // 0x38
    u8*    pData;               // 0x3C  a byte per frame
    u8     unk40[0x48 - 0x40];
} MtaEntry;
LAYOUT_ASSERT(MtaEntry, 0x48);

typedef struct MtaRecord {
    u8     unk00[0x20];
    s32    nBlock;              // 0x20  its block of the pose buffer (SkelPose1.aBlocks)
    s32    nEntries;            // 0x24
    MtaEntry* pEntries;         // 0x28
} MtaRecord;
LAYOUT_ASSERT(MtaRecord, 0x2C);

typedef struct MtaLib {
    u8     unk00[0x14];
    s32    nBytes;              // 0x14  the library's size (fn_8001F804 allocates it)
    u8     unk18[4];
    f32    f1C;                 // 0x1C  its end time (fn_80095FD0 plays it up to this)
    s32    nRecords;            // 0x20
    u8     unk24[0x30 - 0x24];
    MtaRecord* pRecords;        // 0x30
} MtaLib;
LAYOUT_ASSERT(MtaLib, 0x34);

MtaLib* fn_8001F110(MtaLib* pLib, s32* pnSize);    // char.c: swap and link a library; *pnSize: its bytes

MalBank* fn_8001F760(int nBank);
void*    fn_8001F79C(MalBank* pBank, int nGroup, int n);

#endif
