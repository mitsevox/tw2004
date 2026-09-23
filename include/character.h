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
    u8        unk3C[0xEE - 0x3C];   // 0x3C  bone indices (fn_8001EED8), ...
    u8        bEE;              // 0xEE  fn_8001EDF4
} CharModel;

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
    u8    unk0[4];
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
    u8    unk1648[0x1698 - 0x1648];
    s32   n1698;                // 0x1698
    s32   nClubClass;           // 0x169C  the club class for clip lookups (Char_SetClip; 1 looks up as 0)
    u8    unk16A0[0x16D4 - 0x16A0];
    s32   n16D4;                // 0x16D4  the key for clip lookups (Char_SetClip)
    u8    unk16D8[0x16DC - 0x16D8];
    s32   n16DC;                // 0x16DC  twice the players set up so far, in split screen 2
                                //         (Player_SetGolfer)
    s32   nStyle;               // 0x16E0  the animation style (fn_8001C7FC); at -1
                                //         CharacterState_AddSKABlendData does nothing
    u8    unk16E4[0x1788 - 0x16E4];
    void* pCurClip;             // 0x1788  the clip Char_SetClip picked
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
void  fn_8001EEE4(CharModel* pModel, int nBone);
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

// The skeletal animation library (skalib.c) and the clip choice (CharClip.c).
void* AnimLib_Pick(int nPlayer, AnimLib* pLib, int nGroup, int nStyle, int nClub, int nKey, u32* pFlags,
                   const char* pName);
void* Char_SetClip(Character* pChar, int nGroup, int nStyle, const char* pName);

// Swing.c
f32   fn_8005CB78(Character* pChar, u64 uEvent);    // the time of an animation event
void  ShotObj_Set162C(Character* pChar, f32 f);
f32   ShotObj_GetBlend(Character* pChar);           // fBackswing
void  ShotObj_Set1630(Character* pChar, f32 f);
void  ShotObj_Set1634(Character* pChar, f32 f);
int   fn_80062B98(Character* pChar, u64 uEvent);
int   fn_80062BB0(Character* pChar, u64 uEvent);   // event uEvent is set and its time has come
void  fn_80062BE8(Character* pChar);
void  fn_80062BFC(Character* pChar);
int   fn_80062C10(Character* pChar);    // uFlags bit 4
int   fn_80062C1C(Character* pChar);    // uFlags bit 1
f32   fn_80062C28(Character* pChar);    // how far the animation still has to run

#endif
