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
// The main-memory heap (GoShaderObject_Particle_Gc.c): fn_80095108 makes it from the arena.
// fn_800951A0 returns a block aligned to nAlign (0: 16) that remembers the heap block and its
// own padding just before and after it (every caller passes 1 as n, which it does not read).
typedef struct HeapBlockHead {
    void* pBlock;               // 0x0  the heap block it is in
    u32   uSize;                // 0x4  what was asked for
} HeapBlockHead;

void  fn_80095108(void);
void* fn_800951A0(u32 uSize, u16 nAlign, int n);
void  fn_8009527C(void* p);             // frees what fn_800951A0 allocated

// The stopwatches (GoShaderObject_Particle_Gc.c, lbl_802813B0): a 64-bit clock made from the
// 32-bit tick (fn_8000B3E8), and five watches read against it.
typedef union ProfTime {
    u64 u;
    struct {
        u32 nHi;                // port: the halves in the GameCube's (big-endian) order
        u32 nLo;
    } w;
} ProfTime;

typedef struct ProfWatch {
    ProfTime tBase;             // 0x00  a running watch reads the clock minus this
    ProfTime tStop;             // 0x08  when it was stopped
    u8   bRunning;              // 0x10
} ProfWatch;
LAYOUT_ASSERT(ProfWatch, 0x18);

typedef struct ProfClock {
    ProfTime tStart;            // 0x00  when fn_800952D8 set it up
    ProfTime tNow;              // 0x08  the last reading
    ProfWatch aWatches[5];      // 0x10
} ProfClock;

extern ProfClock* lbl_802813B0;

// The particles' buffers (GoShaderObject_Particle_Gc.c, lbl_802813A8).
typedef struct ParticleBuffers {
    void* apBuffers[4];         // 0x00  two of 90000 bytes, two of 10000 (fn_8009414C)
    u8    b10;                  // 0x10  flipped by fn_80094278
    s32   n14;                  // 0x14
} ParticleBuffers;

extern ParticleBuffers* lbl_802813A8;

u32  fn_8000B3E8(void);                 // the tick (urandom.c)
void fn_800952D8(void);                 // set up, every watch reset and stopped
u64  fn_80095368(void);                 // the clock, since the set-up
void fn_800953C8(int nWatch);           // start
u8   fn_80095430(int nWatch);           // running?
u64  fn_80095444(int nWatch);           // stop; returns the reading
u64  fn_800954A4(int nWatch);           // the reading
void fn_80095504(int nWatch);           // reset to 0
// Pack up to 12 characters of pName into a 64-bit code (base 40, table lbl_80191520).
int   fn_800CB700(u64* pId, const char* pName);
// And back: the 12 characters a code was made from (table lbl_80191720); szName takes 13 bytes.
void  fn_800CB868(u64* pId, char* szName);
void  fn_800CB8F0(u64* pId, char* szName);      // for a code stored with its bytes reversed
extern char lbl_80191720[40];                   // the 40 characters a name code is made of
extern s32  lbl_80191520[128];                  // each character's code, -1 for none

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
// Sorts nCount items of nSize bytes with pfnCompare (MSL, 0x8015929C).
void  qsort(void* pBase, u32 nCount, u32 nSize, s32 (*pfnCompare)(const void* pA, const void* pB));

// ---- time ------------------------------------------------------------------------------------

#define FRAME_RATE 59.94f               // frames a second (NTSC)
#define FRAME_TIME (1.0f / FRAME_RATE)  // one frame, in seconds

// llrtclock.c: the real-time clock as a date: month 1-12, day, year, hour 0-23, minute, second,
// millisecond. Always TRUE.
int  fn_8011E020(s32* pnMonth, s32* pnDay, s32* pnYear, s32* pnHour, s32* pnMinute, s32* pnSecond,
                 s32* pnMsec);
void RTClock_GetDateTimeString(char* szOut);   // "M/D/YYYY H:MM AM"

// ---- math and random numbers -----------------------------------------------------------------

#define PI    3.14159265f
#define TWOPI 6.28318531f
#define DEG(x) ((x) * (PI / 180.0f))

void Vec3Copy(const f32* pSrc, f32* pDst);   // 0x80008304 (const: see code_800082F8.c)
f32  fn_800095F0(f32 fAngle);           // sin
f32  fn_80009638(f32 fAngle);           // cos
f32  fn_8000965C(f32 x);                // asin
double fn_80009680(double x);           // sqrt
f32  fn_80009744(f32* pVec);            // dot with itself (at most FLT_MAX)
extern f32 lbl_80281B40[];              // FLT_MAX (MSL's)
void Vec_Copy(const f32* pSrc, f32* pDst);   // 0x8000AD10 (const: see Vec3Copy)
f32  fn_8000AD78(f32 y, f32 x);         // atan2f
f32  fabsf(f32 x);                      // 0x8000AD9C: fabs (0x8000AE94, platform.h) rounded to a float
f32  fn_8000AF7C(f32 x);                // natural logarithm
void fn_8000A4E0(f32 (*pMtx)[4], f32* pA, f32* pB, f32* pC);   // a rotation matrix's three angles
void fn_8000AF20(void);                 // make the log2 table (lbl_80281BD8)
void fn_8000AF58(void);                 // free the log2 table
double acos(double x);                  // 0x8015F784 (MSL)
double asin(double x);                  // 0x8015F7A4 (MSL)
double atan2(double y, double x);       // 0x8015F7C4 (MSL)
double log(double x);                   // 0x8015F804 (MSL)
u32  Rand_Next(int nStream);            // 0x8000B130  EA's lagged-Fibonacci generator
f32  fn_8000B318(int nStream);          // a normally distributed random number (mean 0, deviation 1):
                                        // Box-Muller on two Rand_Floats, the second value kept
void fn_8000B1D4(int nStream, u32 uSeed);   // seed a random stream
u32  fn_8000B244(void);                 // a random seed from the clock
void fn_8000B2B8(u32 uSeed);            // seed all three random streams
void fn_8000B30C(void);                 // drop the kept normal value (fn_8000B318)
f32  Rand_Float(int nStream);           // 0x8000B428  [0, 1)
void fn_8000883C(f32* pA, f32* pB, f32 fT);   // quaternion slerp from a to b by fT, into b
void fn_80008BB8(f32* pOut, f32 fA, f32 fB, f32 fC);   // the quaternion of three (negated) angles
void fn_8000923C(f32* pRot, f32* pOut); // a rotation vector (axis * angle) as a quaternion
void fn_80009710(f32* pQ);              // the identity quaternion (0, 0, 0, 1)
f32  fn_80029B64(f32 x);                // square root (Skeleton.c); x itself when x <= 0
void fn_8000C5D4(f32* pA, f32* pB, f32 f, f32* pOut);   // out = a + f x b
f32  fn_8000C5FC(f32* pA, f32* pB);     // dot product
f32  fn_80009614(f32 x);                // arc cosine
void fn_8000AE28(f32* pIn, f32 f, f32* pOut);   // scale a vector (four floats)
double pow(double x, double y);         // 0x8015F824 (MSL)
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
void fn_8000FFAC(TexBank* pBank);       // LLTex.c: free a bank's pixel and palette data

// The texture banks from 'txf2' stream objects (LoadData.c): a bank per object id (modulo
// 100000), searched by name with fn_8000BDF8.
#define NUM_TXF2_BANKS 200
#define TXF2_KEPT_SIZE 0x8000           // how much of the id-10000 'txf2' object's data is kept
extern TexBank* lbl_801A26DC[NUM_TXF2_BANKS];
void fn_8000B884(void);                 // clear the banks
void fn_8000B8F4(void);                 // free the banks
void fn_8000B984(void);                 // allocate the buffers of the 'load' and 'txf2' objects
void fn_8000B9E4(void);                 // register the 'txf2' handler
void fn_8000BA14(void);                 // unregister it
void fn_8000BAE0(void);                 // load the loading-screen stream file of the current mode/course
u8   fn_8000BD80(char* pName, u8** ppPixels);   // a texture's pixels by name; 0 if none
// Find a texture by name: its bank's index, or -1 (bank and entry NULL).
int  fn_8000BDF8(char* pName, TexBank** ppBank, TexEntry** ppTex);

// What LoadData.c keeps of the 'load' stream object (fe_movies.c shows it): its data is copied
// into the buffer at lbl_80281C04.
typedef struct LoadObjInfo {
    u8   unk0[0x1C];
    u32  uSize;                 // 0x1C  the data's size
    u8   unk20[4];
} LoadObjInfo;
LAYOUT_ASSERT(LoadObjInfo, 0x24);
extern LoadObjInfo lbl_801A25F0;
extern u8* lbl_80281C04;                // the 'load' object's data (147700 bytes)

void fn_80014544(int n);                // load the numbered stream file (sprintf'd name)
void fn_800147A4(void);                 // streammanagerhole.c

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

void fn_80010544(int nSlot);            // frees the bank in slot nSlot and empties the slot

// ---- the renderer ----------------------------------------------------------------------------

void fn_80006EDC(void);                 // LLDisp_Gc.c: set the viewport (DiscCheck.c, ScreenClear.c)
void fn_80006FE8(void);                 // LLDisp_Gc.c: end the frame (returns nothing)

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
    struct GxTexture* pTex108;  // 0x108  or this texture (fn_8002A608)
    u8   unk10C[0x110 - 0x10C];
    u32  u110;                  // 0x110  which of the groups above changed
    u32  uFlags;                // 0x114  bit 1: p100/p104 are set; bit 2: pTex108 is
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

// A dynamic rendering buffer (DynamicRenderingBuffer.c; our name, after the header its allocations
// name, "GoShaderObjectCommon_DynamicRenderingBuffer_Gc.h"): vertices a shader object rewrites every
// frame. The vertex arrays are double-buffered (nBuffer picks the pair being filled); indices and
// the list of draws are single.
typedef struct DynRenderDraw {
    s32   nStart;               // 0x00  first index
    s32   nCount;               // 0x04  indices
    s32   nPrim;                // 0x08  primitive kind: a row of lbl_8018C7C8
} DynRenderDraw;                // 0x0C

typedef struct DynRenderDrawList {
    u32           nDraws;       // 0x00
    DynRenderDraw aDraws[1];    // 0x04  really as many as the buffer was made for
} DynRenderDrawList;

// A draw as callers hand it in (8 bytes).
typedef struct DynRenderDrawIn {
    s32   nPrim;                // 0x00
    u16   nStart;               // 0x04
    u16   nCount;               // 0x06
} DynRenderDrawIn;

typedef struct DynRenderBuffer {
    DynRenderDrawList* pDraws;      // 0x00
    u16*  pIndices;                 // 0x04
    f32*  apPos[2];                 // 0x08  3 floats per vertex
    u32*  apColour[2];              // 0x10  one RGBA colour per vertex
    f32*  apTexCoord[2];            // 0x18  2 floats per vertex
    s32   nMaxVerts;                // 0x20
    s32   nMaxDraws;                // 0x24
    s32   nIndices;                 // 0x28  indices written so far
    s32   nVerts;                   // 0x2C  vertices written so far
    u8    nBuffer;                  // 0x30  0 or 1
} DynRenderBuffer;
LAYOUT_ASSERT(DynRenderBuffer, 0x34);

extern u32 lbl_8018C7C8[4];     // the GX primitive for each DynRenderDraw nPrim

DynRenderBuffer* fn_8007018C(int nMaxVerts, int nMaxDraws);    // make
void fn_80070348(DynRenderBuffer* pBuf);                         // free
void fn_800703B8(DynRenderBuffer* pBuf, u32 nStart, u16 nCount, int nPrim);  // draw
void fn_800704C4(DynRenderBuffer* pBuf, const void* pPos, const void* pColour, const void* pTexCoord,
                 int nVerts, u8 bFlip);                          // add vertices
void fn_800705F0(DynRenderBuffer* pBuf, u16* pIndices, u32 nCount, u8 bRestart);  // add indices
void fn_80070764(DynRenderBuffer* pBuf, const DynRenderDrawIn* pDraws, u16 nCount, int nPrim,
                 u8 bRestart);                                   // add draws

// A shader object that draws through a dynamic rendering buffer (the hooks of rows 0 and 19 of
// lbl_80188E88; our names). Only what those hooks read.
typedef struct DynRenderObject {
    u8    unk0[4];
    DynRenderBuffer* pBuf;          // 0x04
} DynRenderObject;

// The sizes a DynRenderObject's buffer is made with (NULL: 50 vertices, 1 draw).
typedef struct DynRenderSize {
    s32   nMaxVerts;                // 0x00
    s32   nMaxDraws;                // 0x04
} DynRenderSize;

// One frame's geometry handed to a DynRenderObject.
typedef struct DynRenderFill {
    u16   nCount;                   // 0x00  draws; with no draws, the index count
    u16   nVerts;                   // 0x02
    const DynRenderDrawIn* pDraws;  // 0x04  NULL: one draw of all nCount indices
    u16*  pIndices;                 // 0x08
    const void* pPos;               // 0x0C
    const void* pColour;            // 0x10
    const void* pTexCoord;          // 0x14
} DynRenderFill;

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

// The graphics helpers (GxUtil.c, 0x80029FC8; the rest are in gx.h)
void* fn_8002A624(void);            // the screen copy's pixels (lbl_80281100->pPixels)

// The screen copy (our name; what lbl_80281100 points at): render surface 1, set up by gomainloop
// fn_8006DCA8 for each game type and filled by PostFx_CopyScreenToBuffer.
typedef struct ScreenCopy {
    void* pPixels;              // 0x00  surface 1's buffer, NULL without a size
    s32   nWidth;               // 0x04
    s32   nHeight;              // 0x08
    s32   nC;                   // 0x0C
    s32   nSize;                // 0x10  the buffer's size in bytes (fn_8002F454)
} ScreenCopy;

extern ScreenCopy* lbl_80281100;
extern s32 lbl_80281B88;        // bit 0: the video field being drawn

// ---- the file streamer (UStream.c) -----------------------------------------------------------

// An object built from SHOC chunks. The header is 0x34 bytes (LoadData.c copies one with
// Mem_cpy(p, pObject, 0x34); ObjList.c's list head lbl_801A25B8 is one), then the rest of the
// copied chunk header (chunk+0x30 onward: a word, the name's length at 0x38, the name at 0x40)
// and, 0x80-aligned, the data.
typedef struct UStreamObject {
    u8*   pData;                  // 0x00
    u32   uUnk4;                  // 0x04
    void (*pfn8)(struct UStreamObject* pObject);   // 0x08  called when ObjList.c releases the
                                  //       object (fn_8000B588)
    struct UStreamObject* pPrev;  // 0x0C  finished-object queue; ObjList.c: toward the list's tail
    struct UStreamObject* pNext;  // 0x10  ObjList.c: toward the list's head
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
} UStreamObject;
LAYOUT_ASSERT(UStreamObject, 0x34);

int  UStream_RegisterHandler(int nType, void (*pfnHandler)(UStreamObject*));
int  UStream_UnregisterHandler(int nType);
u32  fn_8000E790(UStreamObject* pObject, u32 uMax, void* pDst);   // copy the data out, free the object
u32  fn_8000E81C(UStreamObject* pObject, void** ppData);          // the data and its size

// The list of kept stream objects (ObjList.c). Objects are added at the head or the tail
// (fn_8000B4B0 picks which), around a fixed mark object; fn_8000B68C releases one side of it.
void fn_8000B46C(void);                 // empty the list (only the mark in it)
void fn_8000B4B0(int nEnd);             // where fn_8000B4B8 adds: 0 the head, 1 the tail
void fn_8000B4B8(UStreamObject* pObject);   // add an object
u8   fn_8000B508(UStreamObject* pObject);   // an object of the same type and id is in the list
u8   fn_8000B54C(u32 uType, u32 uId);   // an object of this type and id is in the list
void fn_8000B588(UStreamObject* pObject);   // take an object out of the list and free it
void fn_8000B63C(void);                 // release every object
void fn_8000B68C(int nEnd);             // release the objects before (0) or after (1) the mark
UStreamObject* fn_8000B70C(u32 uType, u32 uId);   // find an object by type and id (NULL: none)

// A record in a block of tagged records (fn_8000B748): this header, then the data, then padding
// to 4 bytes.
typedef struct TagRecord {
    u32  uTag;                  // 0x0
    u32  uSize;                 // 0x4  the record's size, header included (before the padding)
    u32  uId;                   // 0x8
} TagRecord;
LAYOUT_ASSERT(TagRecord, 0xC);

void* fn_8000B748(u8* pBlock, u32 uLen, u32 uTag, u32 uId);   // the record's data
TagRecord* fn_8000B7B0(u8* pBlock, u32 uLen, u32 uTag, u32 uId);   // the record (NULL: none)
void fn_8000B830(UStreamObject* pObject);   // free an object

// Files on disc: a handle from open, -1 for none.
int  fn_800060E0(const char* pName);    // file open
int  fn_8000633C(int hFile);            // file close
// Reads uLen bytes at uOffset into pDst without waiting; pfnDone is called when it is done. Below
// 0: the read could not be queued.
int  fn_80006444(int hFile, void* pDst, u32 uLen, u32 uOffset, void (*pfnDone)(int nBytes, int nError));
u32  fn_800065B0(int hFile);            // file size
// LLFileIO_Gc.c: read a whole file into a new block aligned to nAlign; its size goes to *puSize.
// NULL if the file is missing or cannot be read.
void* fn_800065C8(const char* pName, u32* puSize, int nAlign);

// ---- fonts -----------------------------------------------------------------------------------

f32  fn_80012C30(char* sz);             // UFont.c: a string's width

// ---- controller input ------------------------------------------------------------------------

// One controller as the pad library reads it (12 bytes a pad, filled by PADRead).
typedef struct PadStatus {
    u16 uButtons;                 // 0x00
    s8  nStickX;                  // 0x02
    s8  nStickY;                  // 0x03
    s8  nSubStickX;               // 0x04  the C stick
    s8  nSubStickY;               // 0x05
    u8  nTriggerL;                // 0x06
    u8  nTriggerR;                // 0x07
    u8  nAnalogA;                 // 0x08
    u8  nAnalogB;                 // 0x09
    s8  nError;                   // 0x0A  0: read, -1: no controller
    u8  unkB;                     // 0x0B
} PadStatus;

// A pad's sticks and triggers rescaled to 0-255 with a dead zone; 0x80 is the centre. Y grows
// downwards.
typedef struct PadAnalog {
    u8  nSubStickX;               // 0x00
    u8  nSubStickY;               // 0x01
    u8  nStickX;                  // 0x02
    u8  nStickY;                  // 0x03
    u8  nTriggerL;                // 0x04
    u8  nTriggerR;                // 0x05
} PadAnalog;

typedef struct PadRumble {
    u8  bOn;                      // 0x00  cleared while rumble is switched off (fn_80013200)
    u8  bAllowed;                 // 0x01
    s16 nFrames;                  // 0x02  frames the motor has run; it stops after 60
} PadRumble;

// Controller_Gc.c's state: the four pads.
typedef struct Controllers {
    u8  bStickAsDpad;             // 0x00  the main stick also presses the D-pad
    u32 uConnected;               // 0x04  one bit a pad, 0x80000000 >> n
    PadStatus aStatus[4];         // 0x08
    PadAnalog aAnalog[4];         // 0x38
    u32 auButtons[4];             // 0x50  held << 16 | pressed this frame
    u32 auHeld[4];                // 0x60  last frame's held buttons
    PadRumble aRumble[4];         // 0x70
    s8  nRead;                    // 0x80  one bit a pad (1 << n): it answered this frame
} Controllers;
LAYOUT_ASSERT(Controllers, 0x84);

int  fn_80012FA4(void);                 // controller init
void fn_80012EF8(void);
void fn_80012F18(int a);
void fn_80012F34(int a);
void fn_80012F50(int a, int b, int c);
void fn_80013030(void);
u32  fn_80013050(int nChan);            // the pad's device type (SIProbe)
u8   fn_80013070(int nChan);            // a controller the game takes is plugged in
void fn_800130EC(u8 bOn);               // the main stick also presses the D-pad
void fn_800130F8(int nController, int bOn);         // rumble on or off
void fn_80013130(int nController, int nStrength);   // rumble strength
void fn_800131C4(int nController);      // rumble off
void fn_80013400(void);                 // read the controllers
u8*  fn_800136C4(int nController);      // the pad's state: stick bytes at +0, +2, +3
u32  fn_800136DC(int nController);      // buttons: held << 16 | pressed this frame
void fn_80014118(int a);
// A screen quad (GameEffects' letter boxes, GxUtil.c's alpha clear): fn_800141F8 fills its corners
// (x0, y0)-(x1, y1), fn_80014194 sets its colour (four floats), fn_8001644C draws it.
void fn_80014194(f32* pColour);
void fn_800141F8(f32* pXY, f32* pUV, f32 x0, f32 y0, f32 x1, f32 y1);
void fn_8001425C(int a);
void fn_8001644C(int a, f32* pXY, int b, f32* pUV, int c);
void fn_800BA74C(u8 bFade);             // ScreenClear.c: a black screen for 1, 2 or 30 frames
u32  fn_800142AC(int nButton, int a);   // a button's mask
u8   fn_80014300(u32 uMask);            // any pad pressed these buttons

// ---- events, sound, effects ------------------------------------------------------------------

void fn_800A7A98(s32 n);                // GameAudio.c
void fn_800B7490(void);                 // DiscError.c: yield / pump (UStream.c, DiscCheck.c)

// A node of Code8009B340.c's list (lbl_80281FA0): glows queued by fn_8009B260 that fade out
// (fAlpha falls by fAlphaSpeed a second) and is freed once it has faded.
typedef struct FadeAnchor {
    u8   unk0[0x30];
    f32  a30[4];                // 0x30  drawn at, when a node has one
} FadeAnchor;

typedef struct FadeNode {
    struct FadeNode* pNext;     // 0x00
    f32  a4[4];                 // 0x04  drawn at, when p34 is NULL
    u32  uFlags;                // 0x14  1, 2: how it is drawn; 0x80000000: faded, to be freed
    f32  f18;                   // 0x18  grows by f30 a second
    f32  f1C;                   // 0x1C  f18 + f20, at least 0
    f32  f20;                   // 0x20
    u32  uColor;                // 0x24  its top byte is fAlpha * 255
    f32  fAlpha;                // 0x28
    f32  fAlphaSpeed;           // 0x2C
    f32  f30;                   // 0x30
    FadeAnchor* p34;            // 0x34
} FadeNode;

extern FadeNode* lbl_80281FA0;

void fn_8001C804(int nPlayer, u8 a, u8 b);  // char.c: sets bits of the player's character's u10
void fn_8001D8DC(int nPlayer);
void fn_8001EF34(f32* pIn, f32 f, f32* pOut);   // scale a vector (paired singles)

void fn_80045494(u8 bOn, int nPlayer);
void fn_80045558(u8 bOn, int nPlayer);
u8   fn_8004560C(void);
typedef void (*EventHandler)(int nPlayer, int nEvent, void* pData, int nArg);   // event.c's table
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
void fn_800A746C(s32 nKind, int nTrack, int n);     // GameAudio.c: start a track
void fn_800A74E4(s32 nKind, int nTrack);            // GameAudio.c: stop it
void fn_800A7664(int nKind, int nMsg, int a);
void fn_800A76E4(void);
void fn_800A77E0(f32 f);                // } the options menu passes them 0.2 x options.a0[0], a0[4]
void fn_800A78F0(f32 f);                // } and a0[1] (FE_MessageTable.c, GameUICommands.c)
void fn_800A7924(f32 f);                // }
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
