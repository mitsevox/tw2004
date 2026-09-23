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
void* fn_80005884(void* pDst, const void* pSrc, u32 uLen); // a copy the ranges may overlap in
void* fn_80005AE8(void* pDst, int nValue, u32 uLen);      // memset; returns pDst
int   fn_80005BC8(const void* pA, const void* pB, u32 uLen);   // memcmp
// Allocates from the static heap (StaticMemory.c); nMode picks where (see there).
void* fn_80009B34(int nSize, int nMode, int nAlign, const char* pFile, int nLine);
void  fn_80009E70(void* p);             // free
void  fn_8000A0AC(s32 v);               // } a value callers pass on as fn_80009B34's uFlags
s32   fn_8000A0B4(void);                // } (EASportsBio.c sets 0 while the Bio starts, then 2)
void* fn_800951A0(u32 uSize, int nAlign, int a);
void  fn_8009527C(void* p);             // frees what fn_800951A0 allocated
void  fn_800953C8(int a);

// A pool of fixed-size nodes carved from one allocation (UMemPool.c): the header, then the nodes.
// A free node holds the next free one in its first word.
typedef struct UMemPoolNode {
    struct UMemPoolNode* pNext; // 0x0  the next free node
} UMemPoolNode;

typedef struct UMemPool {
    u32  uNodeSize;             // 0x0  one node, rounded up to the alignment
    u8*  pEnd;                  // 0x4  the end of the pool's memory
    u16  nNodes;                // 0x8
    u16  nFree;                 // 0xA
    UMemPoolNode* pFree;        // 0xC  the free list
} UMemPool;
LAYOUT_ASSERT(UMemPool, 0x10);

UMemPool* fn_8000AFA0(int nNodes, u32 uNodeSize, u32 uFlags, u32 uAlign);   // create
void  fn_8000B058(UMemPool* pPool);                     // destroy
void* fn_8000B078(UMemPool* pPool);                     // take a node (NULL when none is free)
void  fn_8000B0D4(UMemPool* pPool, void* pNode);        // give a node back
// Sorts nCount items of nSize bytes with pfnCompare (the C library's qsort, by its arguments).
void  fn_8015929C(void* pBase, u32 nCount, u32 nSize, s32 (*pfnCompare)(const void* pA, const void* pB));

// ---- time ------------------------------------------------------------------------------------

#define FRAME_RATE 59.94f               // frames a second (NTSC)
#define FRAME_TIME (1.0f / FRAME_RATE)  // one frame, in seconds

// llrtclock.c: the real-time clock as a date: month 1-12, day, year, hour 0-23, minute, second,
// millisecond. Always TRUE.
int  fn_8011E020(s32* pnMonth, s32* pnDay, s32* pnYear, s32* pnHour, s32* pnMinute, s32* pnSecond,
                 s32* pnMsec);
void RTClock_GetDateTimeString(char* szOut);   // "M/D/YYYY H:MM AM"

// ---- math and random numbers -----------------------------------------------------------------

void Vec3Copy(const f32* pSrc, f32* pDst);   // 0x80008304 (const: see code_800082F8.c)
f32  fn_800095F0(f32 fAngle);           // sin
f32  fn_80009638(f32 fAngle);           // cos
double fn_80009680(double x);           // sqrt
f32  fn_80009744(f32* pVec);            // dot with itself (at most FLT_MAX)
extern f32 lbl_80281B40[];              // FLT_MAX (MSL's)
void Vec_Copy(f32* pSrc, f32* pDst);    // 0x8000AD10
f32  fn_8000AD78(f32 y, f32 x);         // atan2f
f32  fabsf(f32 x);                      // 0x8000AD9C: fabs (0x8000AE94, platform.h) rounded to a float
f32  fn_8000AF7C(f32 x);                // natural logarithm
void fn_8000AF20(void);                 // make the log2 table (lbl_80281BD8)
void fn_8000AF58(void);                 // free the log2 table
double fn_8015F784(double x);           // acos
double fn_8015F7A4(double x);           // asin
double fn_8015F7C4(double y, double x); // atan2
double fn_8015F804(double x);           // log
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
    u64  u0;                    // 0x00  its name's hash (fn_8000BEE4; fn_8001005C finds a texture by it)
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

// A loaded texture bank (0x30 bytes, followed by its tables; up to 200, listed at lbl_801A26DC).
typedef struct TexBank {
    u8   unk0[2];
    s16  n2;                    // 0x02  how many textures p8 holds
    s16  n4;                    // 0x04  how many rows pC holds
    u8   unk6[2];
    TexEntry*   p8;             // 0x08  its textures
    TexPalette* pC;             // 0x0C  its palettes
    void* p10;                  // 0x10
    void* p14;                  // 0x14
    u8*  p18;                   // 0x18  the pixel data
    u8   unk1C[0x20 - 0x1C];
    u8*  p20;                   // 0x20  the palette data
    u32  u24;                   // 0x24  the size of one palette (FE_LogoDesign copies this much)
    u8   unk28[0x2D - 0x28];
    u8   b2D;                   // 0x2D  1: p18 and p20 are not the bank's own (never freed)
    u8   unk2E[2];
} TexBank;
LAYOUT_ASSERT(TexBank, 0x30);

u64  fn_8000BEE4(char* pName);          // a name's 64-bit hash
// Find a loaded texture by its name's hash: its bank and entry (both NULL if none).
int  fn_800102DC(u64 uHash, TexBank** ppBank, TexEntry** ppTex);
// Makes a texture bank from a 'txf ' stream object's data (LLTex.c), in pBank or, when it is NULL,
// a new allocation.
TexBank* fn_8000FB88(struct UStreamObject* pObject, TexBank* pBank, int n);

// The texture bank list (LLTexGrp.c): the banks loaded from 'txf ' stream objects, searched by
// fn_800102DC.
typedef struct TexGrpSlot {
    TexBank* pBank;             // 0x0  NULL: a free slot
    int  n4;                    // 0x4  the list's n8 when the bank came in (fn_80010608 frees by it)
    int  n8;                    // 0x8  -1, or the list's n10 when the bank came in
} TexGrpSlot;
LAYOUT_ASSERT(TexGrpSlot, 0xC);

// How to load a 'txf ' object whose id (modulo 100000) is 20000 or more (0x18 bytes).
typedef struct TexGrpRec {
    u8   b0;                    // 0x00  set: the object's bank is not loaded
    u8   unk1[3];
    u32  uId;                   // 0x04  the object id (modulo 100000) it is for
    int  n8;                    // 0x08  goes to the slot's n8
    int  nC;                    // 0x0C  passed on to fn_8000FB88
    int  nSlot;                 // 0x10  the slot the bank went into
    u8   bUsed;                 // 0x14  set once an object took it
    u8   unk15[3];
} TexGrpRec;
LAYOUT_ASSERT(TexGrpRec, 0x18);

typedef struct TexGrpList {
    TexGrpSlot* pSlots;         // 0x00
    int  nNumSlots;             // 0x04  100
    int  n8;                    // 0x08
    u8   bUseRecs;              // 0x0C  set: objects are loaded as pRecs says
    u8   unkD[3];
    int  n10;                   // 0x10
    u8   b14;                   // 0x14  set: fn_800102DC only looks in slots whose n8 is n10
    u8   unk15[3];
    int  nNumRecs;              // 0x18
    TexGrpRec* pRecs;           // 0x1C
} TexGrpList;
LAYOUT_ASSERT(TexGrpList, 0x20);

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

// One row of lbl_80188E88 (our name; 20 rows of 0x44 bytes): a module's hooks. The main loop
// (gomainloop.c) calls each row's pfnC..pfn20 at six points of a frame (fn_8006DDA8 and its
// neighbours), skipping NULL ones; fn_8003519C calls a row's pfn8 with data. Rows 0 and 1 hold
// functions of 0x8006FED4-0x80070FB0 from +0x24 on.
typedef struct ModuleHooks {
    u8    unk0[8];
    void  (*pfn8)(void* pData);   // 0x08
    void  (*pfnC)(void);          // 0x0C  fn_8006E068
    void  (*pfn10)(void);         // 0x10  fn_8006DDE8
    void  (*pfn14)(void);         // 0x14  fn_8006DFE8
    void  (*pfn18)(void);         // 0x18  fn_8006DE68
    void  (*pfn1C)(void);         // 0x1C  fn_8006DF68
    void  (*pfn20)(void);         // 0x20  fn_8006DEE8
    u8    unk24[0x44 - 0x24];
} ModuleHooks;
LAYOUT_ASSERT(ModuleHooks, 0x44);

extern ModuleHooks lbl_80188E88[20];

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

// The graphics helpers at 0x80029FC8 (file name unknown)
void* fn_8002A624(void);            // the screen-copy texture's pixels (lbl_80281100's first word)

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
void fn_8006ACE0(int nPlayer, int nResult);
void fn_8006ACF8(int nPlayer, int a);
void Emotion_UpdatePlayerEmotion(int nPlayer);
void fn_8006B2C4(int nPlayer, u8 bBefore);   // the shot's outcome from the ball (bBefore: ballBefore)
void fn_8006BAA8(int nPlayer);
void fn_8006BF60(int nPlayer);          // the replay recorder
void fn_8006C300(int nPlayer);
void fn_8006C4A0(void);                 // clears gSession.bReplay: a saved replay's playback ends
void fn_8006F4B4(void);
u8   fn_80095430(int a);
void fn_8009B970(int nView);
void fn_8009EF98(void);
void fn_800A6278(void);
void fn_800A6FE0(void);
void fn_800A707C(void);
void fn_800A62A4(void);
void fn_800A62E0(void);
void fn_800A6358(void);
void fn_800A63D0(void);
void fn_800A6DCC(int nMusic, int a);
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
