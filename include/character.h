// character.h (our name): the golfer's character object, the animated golfer on screen (TW06's
// Character_* and CharacterState_* functions take it), its skeleton and animation state, and the
// functions that take one. Player.pChar points at the player's.

#ifndef CHARACTER_H
#define CHARACTER_H

#include "engine.h"

typedef struct AnimLib AnimLib;         // skalib.c

// A character's skeleton data (CharModel.pSkel; the SKEL_ functions take it); only what the game
// code reads.
typedef struct Skeleton {
    u8   unk0[0x10E4];
    s32  n10E4;                 // 0x10E4  set to 4 as a swing starts
} Skeleton;

// A character's model: its bones; only what the game code reads.
typedef struct CharModel {
    u8        unk0[8];
    f32     (*pMatrices)[4][4]; // 0x08  one per bone (fn_8001EED8 gives a bone's index); row 3 is its
                                //       position
    u8        unkC[0x38 - 0xC];
    Skeleton* pSkel;            // 0x38
    u8        aBone[0x59];      // 0x3C  each bone id's index (fn_8001EED8)
    u8        aBone2[0x59];     // 0x95  the index fn_8001EEE4 gives while bEE is set, by bone index
    u8        bEE;              // 0xEE  fn_8001EDF4
} CharModel;

// A clip's header (the fields used here). In a file, pD0 marks the end of the header and
// uAram points at the end of the key data; once a clip's frames are streamed out, uAram is
// their ARAM address and flag 4 is set.
typedef struct Clip {
    u32    uFlags;              // 0x00  4: its frame data is in ARAM
    s32    n04;                 // 0x04  bytes of the second frame stream
    u8     unk08[4];
    s16    nFrames;             // 0x0C
    u8     unk0E[0xE];
    s32    n1C;                 // 0x1C
    u8     unk20[0xC];
    s32    n2C;                 // 0x2C
    u8     unk30[8];
    s32    n38;                 // 0x38  bytes of the first frame stream
    s32    n3C;                 // 0x3C
    s32    n40;                 // 0x40
    u8     unk44[8];
    s32    n4C;                 // 0x4C
    s32    n50;                 // 0x50
    u8     unk54[0x10];
    s32    n64;                 // 0x64
    u8     unk68[0x24];
    s16    n8C;                 // 0x8C  halfwords per frame, first stream
    s16    n8E;                 // 0x8E  bytes per frame, second stream
    u8     unk90[0x10];
    char   name[0x30];          // 0xA0
    u8*    pD0;                 // 0xD0
    u8     unkD4[8];
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

// A node of a character's SKA blend tree (the root is at Character + 0x40C): its kind at +4 and two
// children at +0x24 / +0x28 (fn_80072CB8 walks them); only what the game code reads.
typedef struct SKABlendNode {
    u8   unk0[0x2C];
    s32  nGroup;                // 0x2C  the clip group CharacterState_AddSKABlendData last added
} SKABlendNode;

// A clip as the swing reads it through a ClipBlend; only what the swing reads.
typedef struct BlendClip {
    u8   unk0[8];
    f32  f08;                   // 0x08  added to the time fn_800204A0 samples the clip at
    u8   unkC[0x24 - 0xC];
    f32  f24;                   // 0x24  the swing measures the ball-hit time from it
} BlendClip;

// What Character.pBlend points at: two clips and how far along the blend is; only what the swing
// reads.
typedef struct ClipBlend {
    u8   unk0[0xCC];
    f32  fCC;                   // 0xCC  how far along it is, 0..1 (Character.fBackswing copies it)
    u8   unkD0[4];
    BlendClip* pD4;             // 0xD4
    BlendClip* pD8;             // 0xD8  fn_800204A0 samples it
} ClipBlend;

// The golfer's character object (0x1798 bytes or more); only the fields read so far. Anim_SetRate,
// Anim_SetTime and fn_8007326C take the address of its animation player at 0x164, whose fields
// from 0x168 on are named here directly.
typedef struct Character {
    s32   nIndex;               // 0x000  its entry in lbl_801B9624 (fn_8001C21C)
    s32   nPlayer;              // 0x004  the player it belongs to (Player_SetGolfer); 1000 for the
                                //        characters fn_8001D324 finds by id
    s32   nId;                  // 0x008  (fn_8001D324)
    u8    unkC[0x10 - 0xC];
    u32   u10;                  // 0x010  bit 0x40 tested by the game manager and the swing; bit 0x8000
                                //        cleared by CharacterState_AddSKABlendData
    u8    unk14[0x1C - 0x14];
    s32   nAnim;                // 0x01C  the playing animation (6 backswing, 7 downswing)
    s32   n20;                  // 0x020
    u8    unk24[0x2C - 0x24];
    s32   n2C;                  // 0x02C  tested for 0 (PreShotInit) and for 4 or 5 (ShotSetupInit)
    u8    unk30[0x34 - 0x30];
    s32   nSlot;                // 0x034  the animation slot it uses (skalib); the CrAP camera's shot names
                                //        get an 'f' in front when it is 1
    CharModel* pModel;          // 0x038
    u8    unk3C[0x164 - 0x3C];
    u8    anim[4];              // 0x164  the animation player (+0x14 is its playback rate)
    s32   uFlags;               // 0x168  bit 0x40: the backswing is being backed down; 0x200 / 0x400: the
                                //        clip lookup fell back (Char_SetClip). Signed: the original tests
                                //        it with cmpwi
    u8    unk16C[0x17C - 0x16C];
    f32   fAnimTime;            // 0x17C
    u8    unk180[0x184 - 0x180];
    f32   fAnimEnd;             // 0x184  the animation's end time
    u8    unk188[0x3D8 - 0x188];
    AnimLib* pLib;              // 0x3D8  its animation library
    struct ClipRecord* pRecords;    // 0x3DC  records for its merged library (skalib)
    u8    unk3E0[0x40C - 0x3E0];
    SKABlendNode blend;         // 0x40C  the root of its blend tree
    u8    unk43C[0x4AC - 0x43C];
    struct { u32 bSet; f32 fTime; u8 unk8[8]; } events[18];   // 0x4AC  animation events, by 64-bit id
    s32   n5CC;                 // 0x5CC
    u8    unk5D0[0x1624 - 0x5D0];
    ClipBlend* pBlend;          // 0x1624
    f32   fBackswing;           // 0x1628  how far along the backswing is, 0..1 (pBlend's fCC, copied every
                                //         frame of the backswing; the swing's power is its square root)
    f32   f162C;                // 0x162C
    f32   f1630;                // 0x1630
    f32   f1634;                // 0x1634
    f32   v1638[3];             // 0x1638
    f32   f1644;                // 0x1644
    u8    unk1648[0x1654 - 0x1648];
    s32   n1654;                // 0x1654  (fn_8001EE90)
    s32   n1658;                // 0x1658
    f32   f165C;                // 0x165C  } scaled by the view's lens (fn_8001EE00, fn_8001ED44)
    f32   f1660;                // 0x1660  }
    u8    unk1664[0x1698 - 0x1664];
    s32   n1698;                // 0x1698
    s32   nClubClass;           // 0x169C  the club class for clip lookups (Char_SetClip; 1 looks up as 0)
    s32   nClubHeadBone;        // 0x16A0  bone 0x53's index: the club head (the swing trail's end)
    s32   nGripBone;            // 0x16A4  bone 0x52's index: the grip (the trail's other end)
    s32   n16A8;                // 0x16A8  fn_8001EEE4's answer for bone 0x15
    u8    unk16AC[0x16D4 - 0x16AC];
    s32   n16D4;                // 0x16D4  the key for clip lookups (Char_SetClip)
    u8    unk16D8[0x16DC - 0x16D8];
    s32   n16DC;                // 0x16DC  twice the players set up so far, in split screen 2
                                //         (Player_SetGolfer)
    s32   nStyle;               // 0x16E0  the animation style (fn_8001C7FC); at -1
                                //         CharacterState_AddSKABlendData does nothing
    u8    unk16E4[0x1788 - 0x16E4];
    Clip* pCurClip;             // 0x1788  the clip Char_SetClip picked
    u8    unk178C[0x1790 - 0x178C];
    void* p1790;                // 0x1790  cleared by fn_80062BFC; CharacterState_AddSKABlendData plays it for
                                //         groups 5, 6 and 10
    void* p1794;                // 0x1794  cleared by fn_80062BE8; the same for group 9
} Character;

// The players' characters (gViewSlots, 0x80187124): Player_SetGolfer takes the player's from here.
typedef struct ViewSlot {
    void*      pUnk;            // 0x0
    Character* pChar;           // 0x4
} ViewSlot;

extern ViewSlot gViewSlots[5];          // 0x80187124  per player

// The blend callback CharacterState_AddSKABlendData attaches (fn_80072ACC is one).
typedef void (*SKABlendFn)(SKABlendNode* pNode, int* pn, f32 fTime);

void  Character_SetPosition(Character* pChar, f32* pPos, int a);
void  fn_8001C724(Character* pChar, int nKind);
void  fn_8001C774(Character* pChar, int nClub);
void  fn_8001C7FC(Character* pChar, int nStyle);   // the animation style (nStyle)
void  fn_8001D7A4(Character* pChar);
void  fn_8001DA04(Character* pChar, u8* pA, u8* pB);
void  fn_8001DB04(Character* pChar, f32* pOut);    // the golfer's position
u8    fn_8001DBF4(Character* pChar);    // the ball is in the golfer's hand
void  Character_GetBallOnFingerPosition(Character* pChar, f32* pPos);
f32 (*fn_8001ED08(Character* pChar, int nBone))[4];  // a bone's matrix
u8    fn_8001EDF4(Character* pChar);    // the model's bEE
int   fn_8001EE90(Character* pChar);
int   fn_8001EED8(CharModel* pModel, int nBone);    // a bone's index
int   fn_8001EEE4(CharModel* pModel, int nBone);
void  Anim_SetRate(u8* pAnim, f32 fRate);           // 0x8001F084
void  SKEL_SetIKSolutionWeight(Skeleton* pSkel, f32 f);
void  fn_80027808(CharModel* pModel, f32* pRot);
void  fn_8002792C(Skeleton* pSkel);
int   fn_80048574(Character* pChar, u64 uEvent);    // the character's animation has event uEvent
void  fn_80072ACC(SKABlendNode* pNode, int* pn, f32 fTime);
f32   fn_80072CB8(SKABlendNode* pNode, u64 uEvent); // an event's time in a blend tree
void  fn_8007326C(u8* pAnim);
void  Anim_SetTime(u8* pAnim, f32 fTime);           // 0x8007327C
u8    fn_800734A0(SKABlendNode* pNode);
void  fn_80095744(Character* pChar, int nAnim);     // play an animation
int   fn_80095780(Character* pChar);    // the animation playing
int   fn_80095798(Character* pChar);
void  fn_800957B0(Character* pChar, int a);
void  fn_800957D8(Character* pChar);
void  fn_800957FC(Character* pChar, int a);
void  CharacterState_AddSKABlendData(Character* pChar, int a, int nGroup, SKABlendFn pfnBlend, int c, int d,
                                     f32 f1, f32 f2, f32 f3, f32 f4, f32 f5);
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

// One field of a byte-swap description: nBytes bytes made of nSize-byte values (negative: not
// swapped).
typedef struct SwapField {
    s32 nBytes;                 // 0x0
    s32 nSize;                  // 0x4
} SwapField;

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

#endif
