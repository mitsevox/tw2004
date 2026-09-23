// engine.h (our name): the services under the game code that more than one file calls: memory
// and strings, math, the random streams, the file streamer (UStream.c), controller input, events,
// sound and effects. Views and cameras are in camera.h, the golfer's character in character.h, the
// game itself in game.h.

#ifndef ENGINE_H
#define ENGINE_H

#include "game_types.h"

// ---- memory and strings ----------------------------------------------------------------------

void* Mem_cpy(void* pDst, const void* pSrc, u32 uLen);    // returns pDst
void* fn_80005AE8(void* pDst, int nValue, u32 uLen);      // memset; returns pDst
void* fn_80009B34(u32 uSize, u32 uFlags, u32 uAlign, const char* pFile, int nLine);  // alloc
void  fn_80009E70(void* p);             // free
void* fn_800951A0(u32 uSize, int nAlign, int a);
void  fn_800953C8(int a);
// The C library's (MSL) functions the game calls directly.
void* memcpy(void* pDst, const void* pSrc, u32 uLen);
void* memset(void* pDst, int nValue, u32 uLen);
u32   strlen(const char* s);
char* strcpy(char* pDst, const char* pSrc);
int  strcmp(const char* a, const char* b);
int  sprintf(char* pBuf, const char* pFmt, ...);

// ---- time ------------------------------------------------------------------------------------

#define FRAME_RATE 59.94f               // frames a second (NTSC)
#define FRAME_TIME (1.0f / FRAME_RATE)  // one frame, in seconds

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
void fn_8000C5D4(f32* pA, f32* pB, f32 f, f32* pOut);   // out = a + f x b
f32  fn_8000C5FC(f32* pA, f32* pB);     // dot product
double fn_8015F824(double x, double y); // pow
f32  fn_800BB028(f32* pA, f32* pB);     // squared distance
void vec4flt_CrossProduct(f32* pA, f32* pB, f32* pOut);   // cross product

// ---- the renderer ----------------------------------------------------------------------------

// The renderer's state (lbl_801B8980, 0x118 bytes); only what the game code writes.
typedef struct RenderState {
    u8   unk0[0x100];
    s32  n100;                  // 0x100  } the two textures of the next draw (fn_8005CC64: the swing
    s32  n104;                  // 0x104  } trail's)
    u8   unk108[0x114 - 0x108];
    u32  uFlags;                // 0x114  bit 1: n100/n104 are set
} RenderState;

extern RenderState lbl_801B8980;

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
    u32   uId;                    // 0x20  chunk+0x1C: the object's id within its type (fn_8000B70C
                                  //       finds objects by type and id); for skalib's SAL and BNK
                                  //       objects, the animation slot
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

// ---- events, sound, effects ------------------------------------------------------------------

void fn_8001C804(int nPlayer, int a, int b);
void fn_8001D8DC(int nPlayer);
void fn_8001EF34(f32* pIn, f32 f, f32* pOut);   // scale a vector (paired singles)

void fn_80045494(u8 bOn, int nPlayer);
void fn_80045558(u8 bOn, int nPlayer);
u8   fn_8004560C(void);
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
u8   fn_80095430(int a);
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
void BreakLine_Start(int nView);
int  fn_8011937C(int nPlayer, int a, u8 b);
void fn_80125854(int a);
void fn_80125910(u8 b);

#endif
