// engine.h (our name): the services under the game code that more than one file calls: memory
// and strings, math, the random streams, the file streamer (UStream.c), views and cameras,
// events, sound and effects, and character animation. The game itself is in game.h.

#ifndef ENGINE_H
#define ENGINE_H

#include "game_types.h"

typedef struct CourseInfo CourseInfo;       // golfer.h
typedef struct SurfaceType SurfaceType;     // golfer.h

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

f32  fn_800095F0(f32 fAngle);           // sin
f32  fn_80009638(f32 fAngle);           // cos
double fn_80009680(double x);           // sqrt
f32  fn_80009744(f32* pVec);            // dot with itself
void Vec_Copy(f32* pSrc, f32* pDst);    // 0x8000AD10
f32  fn_8000AD78(f32 y, f32 x);         // atan2f
f32  fn_8000AD9C(f32 x);                // fabsf
u32  Rand_Next(int nStream);            // 0x8000B130  EA's lagged-Fibonacci generator
void fn_8000B1D4(int nStream, u32 uSeed);   // seed a random stream
f32  Rand_Float(int nStream);           // 0x8000B428  [0, 1)
double fn_8015F824(double x, double y); // pow

// ---- the course ------------------------------------------------------------------------------

CourseInfo* fn_8000C594(void);          // the current course
f32  Terrain_HeightAt(f32* pPos, SurfaceType** ppSurface);   // 0x800447DC

// ---- the file streamer (UStream.c) -----------------------------------------------------------

typedef struct UStreamObject UStreamObject;

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

u8*  fn_80016CFC(int nView);
void* fn_80017028(int nView);           // the view
void fn_8001704C(int nView, int nPlayer);   // attach a player
void fn_800170C4(int nView, int a);
void Character_SetPosition(int nHandle, f32* pPos, int a);
void fn_8001C724(int nHandle, int nKind);
void fn_8001C774(int nHandle, int nClub);
void fn_8001C804(int nPlayer, int a, int b);
void fn_8001D8DC(int nPlayer);

typedef struct AnimLib AnimLib;        // skalib.c
void* AnimLib_Pick(int nPlayer, AnimLib* pLib, int nGroup, int nStyle, int nClub, int nKey, u32* pFlags,
                   const char* pName);

void fn_80045494(u8 bOn, int nPlayer);
void fn_80045558(int a, int nPlayer);
u8   fn_8004560C(void);
void View_SetCamera(void* pView, int nCamera, int nPlayer, int nView);
void fn_80063B98(void* pView, f32 f, f32* pVec);
void fn_80063BF4(void* pView, f32 f, f32* pVec);
u8   fn_80063C90(void* pView);          // the camera is still moving
void EVENT_Trigger(int nPlayer, int nEvent, int a, int b);    // through the event table at lbl_80188628
void fn_800689D4(int nPlayer);
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
void fn_8009B970(int nView);
void fn_8009EF98(void);
void fn_800A30E4(int nKind, u8* pBall, int nPlayer, int a, f32 f);
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
void BreakLine_Start(int nView);
void fn_80125854(int a);
void fn_80125910(u8 b);

#endif
