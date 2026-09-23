// engine.h (our name): the services under the game code that more than one file calls: memory
// and strings, math, the random streams, the file streamer (UStream.c), views and cameras,
// events, sound and effects, and character animation. The game itself is in game.h.

#ifndef ENGINE_H
#define ENGINE_H

#include "game_types.h"

typedef struct View View;                   // GoGolfCam.c, Swing.c

// ---- memory and strings ----------------------------------------------------------------------

void* Mem_cpy(void* pDst, const void* pSrc, u32 uLen);    // returns pDst
void* fn_80005AE8(void* pDst, int nValue, u32 uLen);      // memset; returns pDst
void* fn_80009B34(u32 uSize, u32 uFlags, u32 uAlign, const char* pFile, int nLine);  // alloc
void  fn_80009E70(void* p);             // free
void* fn_800951A0(u32 uSize, int nAlign, int a);
void  fn_800953C8(int a);
char* strcpy(char* pDst, const char* pSrc);
int  strcmp(const char* a, const char* b);
int  sprintf(char* pBuf, const char* pFmt, ...);

// ---- math and random numbers -----------------------------------------------------------------

void Vec3Copy(f32* pSrc, f32* pDst);    // 0x80008304
f32  fn_800095F0(f32 fAngle);           // sin
f32  fn_80009638(f32 fAngle);           // cos
double fn_80009680(double x);           // sqrt
f32  fn_80009744(f32* pVec);            // dot with itself
void Vec_Copy(f32* pSrc, f32* pDst);    // 0x8000AD10
f32  fn_8000AD78(f32 y, f32 x);         // atan2f
f32  fn_8000AD9C(f32 x);                // fabsf
double fabsf(double x);                 // 0x8000AE94; double: Swing.c needs it to match
u32  Rand_Next(int nStream);            // 0x8000B130  EA's lagged-Fibonacci generator
void fn_8000B1D4(int nStream, u32 uSeed);   // seed a random stream
f32  Rand_Float(int nStream);           // 0x8000B428  [0, 1)
double fn_8015F824(double x, double y); // pow

// ---- the file streamer (UStream.c) -----------------------------------------------------------

// An object built from SHOC chunks. The header is 0x34 bytes, then the copied chunk header
// (from SHDR chunk offset 0x14) and, 0x80-aligned, the data.
typedef struct UStreamObject {
    u8*   pData;                  // 0x00
    u32   uUnk4;                  // 0x04
    u32   uUnk8;                  // 0x08
    struct UStreamObject* pPrev;  // 0x0C  finished-object queue
    struct UStreamObject* pNext;  // 0x10
    int   nUnk14;                 // 0x14
    u32   uFlags;                 // 0x18  chunk+0x14; set to 1 for txf / Cpyr / Cact / txf2
    u32   uType;                  // 0x1C  chunk+0x18, e.g. 'ter '
    u32   uHash;                  // 0x20  chunk+0x1C
    u32   uSize;                  // 0x24  chunk+0x20 decompressed size
    u32   uRef28;                 // 0x28  chunk+0x24 } rebased by the RPNS value when the
    u32   uRef2C;                 // 0x2C  chunk+0x28 } object is delivered
    u32   uRef30;                 // 0x30  chunk+0x2C }
    u32   uUnk34;                 // 0x34  chunk+0x30
    u32   uNameLen;               // 0x38  chunk+0x34
    u32   uUnk3C;                 // 0x3C  chunk+0x38
    char  szName[4];              // 0x40  chunk+0x3C
} UStreamObject;

int  UStream_RegisterHandler(u32 uType, void (*pfnHandler)(UStreamObject*));
int  UStream_UnregisterHandler(u32 uType);
u32  fn_8000E790(UStreamObject* pObject, u32 uMax, void* pDst);   // copy the data out, free the object
u32  fn_8000E81C(UStreamObject* pObject, void** ppData);          // the data and its size

// ---- controller input ------------------------------------------------------------------------

void fn_80012EF8(void);
void fn_80012F34(int a);
void fn_80012F50(int a, int b, int c);
void fn_80013130(int nController, int nStrength);   // rumble strength
u8*  fn_800136C4(int nController);      // the pad's state: stick bytes at +0, +2, +3
u32  fn_800136DC(int nController);      // buttons: held << 16 | pressed this frame
void fn_80014118(int a);
u32  fn_800142AC(int nButton, int a);   // a button's mask
u8   fn_80014300(u32 uMask);            // any pad pressed these buttons

// ---- views, events, sound, animation ---------------------------------------------------------

// The golfer's character object at Player.nShotHandle; only the fields read so far.
typedef struct ShotObj {
    u8    unk0[0x1C];
    s32   nAnim;                // 0x01C  the playing animation (6 backswing, 7 downswing)
    u8    unk20[0x2C - 0x20];
    s32   n2C;                  // 0x02C  tested for 0 (PreShotInit) and for 4 or 5 (ShotSetupInit)
    u8    unk30[0x38 - 0x30];
    u8*   pView;                // 0x038  -> +0x38 -> a struct with +0x10E4
    u8    unk3C[0x164 - 0x3C];
    u8    anim[4];              // 0x164  the animation: +0x14 is its playback rate
    s32   uFlags;               // 0x168  bit 0x40: the backswing is being backed down (signed: the original tests it with cmpwi)
    u8    unk16C[0x17C - 0x16C];
    f32   fAnimTime;            // 0x17C
    u8    unk180[0x184 - 0x180];
    f32   fAnimEnd;             // 0x184  the animation's end time
    u8    unk188[0x438 - 0x188];
    s32   n438;                 // 0x438  compared with 11 (GM_ShowPostShotAnimation)
    u8    unk43C[0x4AC - 0x43C];
    struct { u32 bSet; f32 fTime; u8 unk8[8]; } events[18];   // 0x4AC  animation events, by 64-bit id
    s32   n5CC;                 // 0x5CC
    u8    unk5D0[0x1624 - 0x5D0];
    u8*   pClip;                // 0x1624 -> +0xCC blend, +0xD4/+0xD8 clips
    f32   f1628;
    f32   f162C;
    f32   f1630;
    f32   f1634;
    f32   v1638[3];             // 0x1638
    f32   f1644;
    u8    unk1648[0x1698 - 0x1648];
    s32   n1698;                // 0x1698
} ShotObj;

// The camera tuning values (GoGolfCam.c); only the fields read so far.
typedef struct CamTuning {
    u8   unk0[0x4C];
    f32  f4C;                   // 0x04C  camera 5's height over the ball
    f32  f50;                   // 0x050  camera 5: the ball-to-pin distance of a full swing out
    f32  f54;                   // 0x054  camera 5: how long the swing out lasts
    f32  f58;                   // 0x058  how long camera 13's closing shot lasts
    u8   unk5C[4];
    f32  f60;                   // 0x060  camera 13's slow-motion step length (0: every frame)
    f32  f64;                   // 0x064
    f32  v68[4];                // 0x068  camera 13's fn_80038010 vector; [3] shrinks as the camera's time runs
    f32  f78;                   // 0x078  ... over this many seconds
    f32  f7C;                   // 0x07C  camera 13's fallback shots' f78/f7C: from this value ...
    f32  f80;                   // 0x080  ... to this one
    u8   unk84[4];
    f32  f88;                   // 0x088
    f32  f8C;                   // 0x08C
    u8   unk90[4];
    f32  f94;                   // 0x094  the elevator camera's first blend value
    u8   unk98[0xB8 - 0x98];
    f32  fB8;                   // 0x0B8  camera 15 waits this long on a ball near the green
    s32  nBeats;                // 0x0BC  the heartbeat camera's beats
    s32  nBeatFrames;           // 0x0C0
    f32  fC4;                   // 0x0C4
    f32  fC8;                   // 0x0C8
    f32  fCC;                   // 0x0CC
    u8   unkD0[0x168 - 0xD0];
    f32  f168;                  // 0x168  the ground clearance for CamScript_KeepAboveGround
    f32  f16C;                  // 0x16C  the obstruction radius around the ball for the pre-shot routine
    f32  f170;                  // 0x170  a blend for fn_80063B98 / fn_80063BF4
    u8   unk174[0x178 - 0x174];
    f32  f178;                  // 0x178
    f32  v17C[4];               // 0x17C
    u8   unk18C[0x1C0 - 0x18C];
    s32  n1C0;                  // 0x1C0  nonzero enables camera 19
    u8   unk1C4[0x1C8 - 0x1C4];
    s32  bCheckSlope;           // 0x1C8  fn_800C4650 tests the slope to the target (fn_800C4520)
    s32  bCheckTerrain;         // 0x1CC  and the ground in between (fn_800C4604)
    u8   unk1D0[0x1E0 - 0x1D0];
    f32  fSlopeUp;              // 0x1E0
    f32  fSlopeDown;            // 0x1E4
} CamTuning;

extern CamTuning* lbl_80281F78;

u8*  fn_80016CFC(int nView);
void* fn_80017004(int nView);
void* fn_80017028(int nView);           // the view
void fn_8001704C(int nView, int nPlayer);   // attach a player
void fn_800170C4(int nView, int a);
void Character_SetPosition(int nHandle, f32* pPos, int a);
void fn_8001C724(int nHandle, int nKind);
void fn_8001C774(int nHandle, int nClub);
void fn_8001C804(int nPlayer, int a, int b);
void fn_8001D8DC(int nPlayer);
void fn_8001EF34(f32* pIn, f32 f, f32* pOut);   // scale a vector (paired singles)

typedef struct AnimLib AnimLib;        // skalib.c
void* AnimLib_Pick(int nPlayer, AnimLib* pLib, int nGroup, int nStyle, int nClub, int nKey, u32* pFlags,
                   const char* pName);

void fn_80045494(u8 bOn, int nPlayer);
void fn_80045558(u8 bOn, int nPlayer);
u8   fn_8004560C(void);
void View_SetCamera(void* pView, int nCamera, int nPlayer, int nView);
void fn_80063B98(void* pView, f32 f, f32* pVec);
void fn_80062F1C(View* pView);
void fn_80063BF4(void* pView, f32 f, f32* pVec);
u8   fn_80063C7C(View* pView);
u8   fn_80063C90(void* pView);          // the camera is still moving
void fn_80063CF0(View* pView, int nCamera, int nPlayer);
void fn_8006434C(void* pView, f32* pPos, f32* pX, f32* pY, int a);   // a world position on screen (0..1)
void fn_8006A8D4(void* pView, f32* pX, f32* pY);
void EVENT_Trigger(int nPlayer, int nEvent, void* pData, int b);   // through the event table at
                                        // lbl_80188628; pData: the ball, a position, or NULL
void fn_800689D4(int nPlayer);
u8   fn_80068AC8(int nPlayer);
void fn_80069330(int nPlayer, f32* pPos);
void PlaceBall_UpdateMomentums(int nPlayer, f32 f);
void fn_8006A6C4(int nPlayer);
int  fn_8006AA9C(int nPlayer);          // how the shot turned out (0..4, 8+)
void fn_8006AAB4(int nPlayer, int a);
void fn_8006ACF8(int nPlayer, int a);
void Emotion_UpdatePlayerEmotion(int nPlayer);
void fn_8006B2C4(int nPlayer, int a);
void fn_8006BAA8(int nPlayer);
void fn_8006BF60(int nPlayer);          // the replay recorder
void fn_8006C300(int nPlayer);
void fn_8006C4A0(void);                 // take the shot back (a mulligan)
void fn_8006F4B4(void);
void fn_80072ACC(void);
u8   fn_80095430(int a);
void fn_80095744(int nHandle, int nAnim);   // play an animation
int  fn_80095780(int nHandle);          // the animation playing
int  fn_80095798(int nHandle);
void fn_800957D8(int nHandle);
void fn_800957FC(int nHandle, int a);
void CharacterState_AddSKABlendData(u8* pChar, int a, int nGroup, void* pfn, int c, int d, f32 f1, f32 f2,
                                    f32 f3, f32 f4, f32 f5);
void CharAnim_StartTapIn(u8* pChar);
void fn_8009B970(int nView);
void fn_8009EF98(void);
void fn_800A6278(void);
void fn_800A62A4(void);
void fn_800A62E0(void);
void fn_800A6358(void);
void fn_800A63D0(void);
void fn_800A72EC(u8 a, u8 b);
void fn_800A7664(int nKind, int nMsg, int a);
void fn_800A76E4(void);
void Vec_Normalize(f32* pSrc, f32* pDst);
void fn_800BAF04(f32* pSrc, f32* pDst);   // normalise
f32  Vec_Distance(f32* pA, f32* pB);
u8   fn_800C6CB0(void);
void fn_800C70F8(View* pView, int a);
void BreakLine_Start(int nView);
int  fn_8011937C(int nPlayer, int a, u8 b);
void fn_80125854(int a);
void fn_80125910(u8 b);

#endif
