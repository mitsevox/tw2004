// engine.h (our name): the services under the game code that more than one file calls: memory
// and strings, math, the random streams, the file streamer (UStream.c), controller input, events,
// sound and effects. Views and cameras are in camera.h, the golfer's character in character.h, the
// game itself in game.h.

#ifndef ENGINE_H
#define ENGINE_H

#include "game_types.h"
#include "platform.h"

// ---- memory and strings ----------------------------------------------------------------------

void* Mem_cpy(void* pDst, const void* pSrc, u32 uLen);    // returns pDst
void* fn_80005AE8(void* pDst, int nValue, u32 uLen);      // memset; returns pDst
void* fn_80009B34(u32 uSize, u32 uFlags, u32 uAlign, const char* pFile, int nLine);  // alloc
void  fn_80009E70(void* p);             // free
void  fn_8000A0AC(s32 v);               // } a value callers pass on as fn_80009B34's uFlags
s32   fn_8000A0B4(void);                // } (EASportsBio.c sets 0 while the Bio starts, then 2)
void* fn_800951A0(u32 uSize, int nAlign, int a);
void  fn_800953C8(int a);
// Sorts nCount items of nSize bytes with pfnCompare (the C library's qsort, by its arguments).
void  fn_8015929C(void* pBase, u32 nCount, u32 nSize, s32 (*pfnCompare)(const void* pA, const void* pB));

// ---- time ------------------------------------------------------------------------------------

#define FRAME_RATE 59.94f               // frames a second (NTSC)
#define FRAME_TIME (1.0f / FRAME_RATE)  // one frame, in seconds

// ---- math and random numbers -----------------------------------------------------------------

void Vec3Copy(const f32* pSrc, f32* pDst);   // 0x80008304 (const: see code_800082F8.c)
f32  fn_800095F0(f32 fAngle);           // sin
f32  fn_80009638(f32 fAngle);           // cos
double fn_80009680(double x);           // sqrt
f32  fn_80009744(f32* pVec);            // dot with itself
void Vec_Copy(f32* pSrc, f32* pDst);    // 0x8000AD10
f32  fn_8000AD78(f32 y, f32 x);         // atan2f
f32  fabsf(f32 x);                      // 0x8000AD9C: fabs (0x8000AE94, platform.h) rounded to a float
f32  fn_8000AF7C(f32 x);                // natural logarithm
u32  Rand_Next(int nStream);            // 0x8000B130  EA's lagged-Fibonacci generator
f32  fn_8000B318(int nStream);          // a normally distributed random number (mean 0, deviation 1):
                                        // Box-Muller on two Rand_Floats, the second value kept
void fn_8000B1D4(int nStream, u32 uSeed);   // seed a random stream
u32  fn_8000B244(void);                 // a random seed from the clock
void fn_8000B2B8(u32 uSeed);            // seed all three random streams
void fn_8000B30C(void);                 // drop the kept normal value (fn_8000B318)
f32  Rand_Float(int nStream);           // 0x8000B428  [0, 1)
void fn_8000883C(f32* pA, f32* pB, f32 fT);   // quaternion slerp from a to b by fT, into b
f32  fn_80029B64(f32 x);                // square root (Skeleton.c); x itself when x <= 0
void fn_8000C5D4(f32* pA, f32* pB, f32 f, f32* pOut);   // out = a + f x b
f32  fn_8000C5FC(f32* pA, f32* pB);     // dot product
double fn_8015F824(double x, double y); // pow
f32  powf(f32 x, f32 y);                // 0x8002C8D0 (Golfer.c): pow rounded to a float
f32  fn_800BB028(f32* pA, f32* pB);     // squared distance
void vec4flt_CrossProduct(f32* pA, f32* pB, f32* pOut);   // cross product

// ---- textures --------------------------------------------------------------------------------

// A texture in a bank (0x50 bytes; the bank's p8 is an array of them). Only what the game code reads.
typedef struct TexEntry {
    u8   unk0[8];
    u32  uPixels;               // 0x08  where its pixels start in the bank's p18
    u8   unkC[0x3C - 0xC];
    s16  nPalette;              // 0x3C  its row in the bank's pC
    u8   unk3E[0x50 - 0x3E];
} TexEntry;
LAYOUT_ASSERT(TexEntry, 0x50);

// A row of a bank's palette table (12 bytes).
typedef struct TexPalette {
    u32  uColors;               // 0x0  where its colours start in the bank's p20
    u8   unk4[0xC - 0x4];
} TexPalette;
LAYOUT_ASSERT(TexPalette, 0xC);

// A loaded texture bank (up to 200, listed at lbl_801A26DC). Only what the game code reads.
typedef struct TexBank {
    u8   unk0[8];
    TexEntry*   p8;             // 0x08  its textures
    TexPalette* pC;             // 0x0C  its palettes
    u8   unk10[0x18 - 0x10];
    u8*  p18;                   // 0x18  the pixel data
    u8   unk1C[0x20 - 0x1C];
    u8*  p20;                   // 0x20  the palette data
    u32  u24;                   // 0x24  the size of one palette (FE_LogoDesign copies this much)
} TexBank;

u64  fn_8000BEE4(char* pName);          // a name's 64-bit hash
// Find a loaded texture by its name's hash: its bank and entry (both NULL if none).
int  fn_800102DC(u64 uHash, TexBank** ppBank, TexEntry** ppTex);

// ---- the renderer ----------------------------------------------------------------------------

// The renderer's state (lbl_801B8980, 0x118 bytes); only what the game code writes.
// GoTerrain.c's setters write one group of fields each and set that group's bit in u110.
typedef struct RenderState {
    u8   unk0[0x10];
    s32  n10;                   // 0x010  } set together, bit 0x10
    s32  n14;                   // 0x014  }
    u8   unk18[0x1C - 0x18];
    u8   b1C;                   // 0x01C  bit 0x80
    u8   b1D;                   // 0x01D  bit 0x80
    u8   unk1E[0x20 - 0x1E];
    u32  u20;                   // 0x020  bits cleared and set by fn_80035170, bit 0x20
    u8   unk24[0x28 - 0x24];
    f32  f28;                   // 0x028  } bit 0x8, with a30. fn_80035398 sets all three from
    f32  f2C;                   // 0x02C  } lbl_802811E0
    u8   a30[4];                // 0x030  a colour: three bytes given, the fourth always 0x80
    u8   unk34[0xFC - 0x34];
    s32  nFC;                   // 0x0FC  bit 0x400
    TexBank*  p100;             // 0x100  } the texture of the next draw (fn_8005CC64: the swing
    TexEntry* p104;             // 0x104  } trail's, the logo editor's)
    u8   unk108[0x110 - 0x108];
    u32  u110;                  // 0x110  which of the groups above changed
    u32  uFlags;                // 0x114  bit 1: p100/p104 are set
} RenderState;
LAYOUT_ASSERT(RenderState, 0x118);

extern RenderState lbl_801B8980;

void fn_8005CC64(TexBank* pBank, TexEntry* pTex);  // set the texture of the next draw

// A render surface (GoRenderSurface.c; our name, after the file): one of five 0x2C-byte slots at
// lbl_801D3950. A slot whose n0 is not 1 owns a buffer of nSize bytes. Only what the code reads.
typedef struct RenderSurface {
    s32   n0;                   // 0x00  given when it is made; 0: the slot is free
    s32   nWidth;               // 0x04  in pixels (512 x 448 for surface 0)
    s32   nHeight;              // 0x08
    s32   nC;                   // 0x0C  } fn_8002F38C's four values; made as the width, the
    s32   n10;                  // 0x10  }   height, 0 and the pixel kind
    s32   n14;                  // 0x14  }
    s32   n18;                  // 0x18  }
    s32   n1C;                  // 0x1C  the pixel kind: 1 or 2 is 4 bytes a pixel, 4 is 2, 8 or 16 is 1
    s32   n20;                  // 0x20
    void* pBuffer;              // 0x24
    s32   nSize;                // 0x28  bytes: width x height x bytes a pixel
} RenderSurface;
LAYOUT_ASSERT(RenderSurface, 0x2C);

extern RenderSurface lbl_801D3950[5];
extern s32 lbl_80281D50;        // the surface fn_8002F38C selected last

// GoRenderSurface.c
int  fn_8002F260(s32 n0, s32 nWidth, s32 nHeight, s32 nKind, s32 n20, s32 nSurface);   // 0: no memory
void fn_8002F38C(s32 nSurface, s32 nC, s32 n10, s32 n14, u32 uFlags, s32 n18);
s32  fn_8002F454(s32 nSurface);     // the surface's buffer size, 0 if the slot is free

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

int  UStream_RegisterHandler(int nType, void (*pfnHandler)(UStreamObject*));
int  UStream_UnregisterHandler(int nType);
u32  fn_8000E790(UStreamObject* pObject, u32 uMax, void* pDst);   // copy the data out, free the object
u32  fn_8000E81C(UStreamObject* pObject, void** ppData);          // the data and its size

// Files on disc: a handle from open, -1 for none.
int  fn_800060E0(const char* pName);    // file open
int  fn_8000633C(int hFile);            // file close
// Reads uLen bytes at uOffset into pDst without waiting; pfnDone is called when it is done. Below
// 0: the read could not be queued.
int  fn_80006444(int hFile, void* pDst, u32 uLen, u32 uOffset, void (*pfnDone)(int nBytes, int nError));
u32  fn_800065B0(int hFile);            // file size

// ---- controller input ------------------------------------------------------------------------

void fn_80012EF8(void);
void fn_80012F18(int a);
void fn_80012F34(int a);
void fn_80012F50(int a, int b, int c);
void fn_80013130(int nController, int nStrength);   // rumble strength
u8*  fn_800136C4(int nController);      // the pad's state: stick bytes at +0, +2, +3
u32  fn_800136DC(int nController);      // buttons: held << 16 | pressed this frame
void fn_80014118(int a);
u32  fn_800142AC(int nButton, int a);   // a button's mask
u8   fn_80014300(u32 uMask);            // any pad pressed these buttons

// ---- events, sound, effects ------------------------------------------------------------------

void fn_8001C804(int nPlayer, u8 a, u8 b);  // char.c: sets bits of the player's character's u10
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
void fn_800BD83C(int nSound, int a);      // SitDevFile.c: fn_800A7664(0, nSound, a)
void BreakLine_Start(int nView);
int  fn_8011937C(int nPlayer, int a, u8 b);
// The EA Sports Bio, game side (EASportsBio.c; TW06's names)
void EASBio_SetGamePlayState(u8 bFlag);
void EASBio_IncrementGamesWon(u32 uCount);
void EASBio_SetCurrentGameWon(u8 bWon);
u8   EASBio_IsCurrentGameWon(void);

#endif
