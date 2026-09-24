// engine.h (our name): the services under the game code that more than one file calls: memory
// and strings, math, the random streams, the file streamer (UStream.c), controller input, events,
// sound and effects. Views and cameras are in camera.h, the golfer's character in character.h, the
// game itself in game.h.

#ifndef ENGINE_H
#define ENGINE_H

#include "game_types.h"
#include "platform.h"
#include "gx.h"

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
void  fn_8000A0BC(void);                // start a new count of the bytes taken
void  fn_8000A0C8(void);                // } counting on / off
void  fn_8000A0D4(void);                // }
s32   fn_8000A0E0(void);                // the bytes taken since fn_8000A0BC
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
    u32   n14;                  // 0x14  the first particle no system holds yet (fn_8009428C)
} ParticleBuffers;

extern ParticleBuffers* lbl_802813A8;

u32  fn_8000B3E8(void);                 // the tick (urandom.c)
void fn_800952D8(void);                 // set up, every watch reset and stopped
u64  fn_80095368(void);                 // the clock, since the set-up
void fn_800953C8(int nWatch);           // start
u8   fn_80095430(int nWatch);           // running?
u64  fn_80095444(int nWatch);           // stop; returns the reading
u64  fn_800954A4(int nWatch);           // the reading
f32  fn_8006E118(u64 uNow, u64 uLast);  // seconds between two readings (gomainloop.c)
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
void fn_80008FCC(f32* pA, f32* pB, f32* pOut); // quaternion product a x b (Quaternion.c)
void fn_80008BB8(f32* pOut, f32 fA, f32 fB, f32 fC);   // the quaternion of three (negated) angles
void fn_8000923C(f32* pRot, f32* pOut); // a rotation vector (axis * angle) as a quaternion
void fn_800093AC(f32 fAngle, f32* pOut); // the quaternion of a rotation by -fAngle about z
void fn_80009410(f32 fAngle, f32* pOut); // the quaternion of a rotation by -fAngle about y
void fn_80009474(f32 fAngle, f32* pOut); // the quaternion of a rotation by -fAngle about x
void fn_80009710(f32* pQ);              // the identity quaternion (0, 0, 0, 1)
f32  fn_80029B64(f32 x);                // square root (Skeleton.c); x itself when x <= 0
void fn_8000C5D4(f32* pA, f32* pB, f32 f, f32* pOut);   // out = a + f x b
f32  fn_8000C5FC(f32* pA, f32* pB);     // dot product
f32  fn_80009614(f32 x);                // arc cosine
void fn_8000AE28(f32* pIn, f32 f, f32* pOut);   // scale a vector (four floats)
void fn_8000AE6C(f32* pA, f32* pB, f32 fScale, f32* pOut);   // out = a + fScale x b (four floats)
double pow(double x, double y);         // 0x8015F824 (MSL)
f32  powf(f32 x, f32 y);                // 0x8002C8D0 (Golfer.c): pow rounded to a float
f32  fn_800BB028(f32* pA, f32* pB);     // squared distance
void vec4flt_CrossProduct(f32* pA, f32* pB, f32* pOut);   // cross product

// ---- textures --------------------------------------------------------------------------------

// A texture in a bank (0x50 bytes; the bank's p8 is an array of them). Only what the game code reads.
// One level of a texture (12 bytes; char.c fn_8001DD18 byte-swaps four of them).
typedef struct TexMip {
    u32  uPixels;               // 0x0  where its pixels start in the bank's p18
    s16  nC;                    // 0x4  its size in 16-byte units (fn_80045FC8, fn_800B9EB8 copy nC * 16)
    u8   unk6[0x8 - 0x6];
    s16  n8;                    // 0x8  TX_spParseTextureGroupFromStream adds the bank's n28 to it
    u8   unkA[0xC - 0xA];
} TexMip;

typedef struct TexEntry {
    u64  u0;                    // 0x00  its name's hash (fn_8000BEE4; fn_8001005C finds a texture by it)
    TexMip aMips[4];            // 0x08  its levels (n41 of them are used)
    u16  nWidth;                // 0x38  (GXInitTexObj in TX_spParseTextureGroupFromStream)
    u16  nHeight;               // 0x3A
    s16  nPalette;              // 0x3C  its row in the bank's pC
    u16  n3E;                   // 0x3E  its row in the bank's p10 (ShaderObjectsData fn_800740F4)
    s8   b40;                   // 0x40  0: char.c fn_8001DD18 decodes the name and pairs the texture
    s8   n41;                   // 0x41  (fn_80045FC8)
    u8   unk42[0x46 - 0x42];
    s8   b46;                   // 0x46  bit 0: clamp in s, bit 1: clamp in t (else repeat)
    u8   b47;                   // 0x47  bit 0: the next texture goes with it (char.c fn_80019798);
                                //       bit 0x40: byte-swapped (fn_8001DD18)
    u8   unk48[0x50 - 0x48];
} TexEntry;
LAYOUT_ASSERT(TexEntry, 0x50);

// A row of a bank's palette table (12 bytes).
typedef struct TexPalette {
    u32  uColors;               // 0x0  where its colours start in the bank's p20
    u8   unk4[0x6 - 0x4];
    s16  n6;                    // 0x6  TX_spParseTextureGroupFromStream adds the bank's n28 to it
    s16  nEntries;              // 0x8  (GXInitTlutObj)
    s16  nFormat;               // 0xA  (GXInitTlutObj)
} TexPalette;
LAYOUT_ASSERT(TexPalette, 0xC);

// A section of a 'txf ' object's data: an 8-byte header, then nSize bytes
// (TX_spParseTextureGroupFromStream steps from one to the next).
typedef struct TexSection {
    u32  unk0;
    s32  nSize;                 // 0x4
} TexSection;

// A row of a bank's p10 (0x40 bytes): a texture's GX texture object, loaded with GXLoadTexObj
// (ShaderObjectsData fn_800740F4); a texture whose b47 bit 0 is set uses the next row too.
typedef struct TexGXObj {
    u8   unk0[0x40];
} TexGXObj;

// A row of a bank's p14 (0x18 bytes): a texture's GX palette object, loaded with GXLoadTlut when
// its nPalette is not -1 (ShaderObjectsData fn_800738DC).
typedef struct TexGXTlut {
    u8   unk0[0x18];
} TexGXTlut;

// A loaded texture bank (0x30 bytes, followed by its tables; up to 200, listed at lbl_801A26DC).
typedef struct TexBank {
    u8   unk0[2];
    s16  n2;                    // 0x02  how many textures p8 holds
    s16  n4;                    // 0x04  how many rows pC holds
    u8   unk6[2];
    TexEntry*   p8;             // 0x08  its textures
    TexPalette* pC;             // 0x0C  its palettes
    TexGXObj* p10;              // 0x10  the textures' GX objects, by TexEntry.n3E
    TexGXTlut* p14;             // 0x14  the textures' GX palette objects, by TexEntry.n3E
    u8*  p18;                   // 0x18  the pixel data
    s32  n1C;                   // 0x1C  the size of the pixel data, when the bank has its own copy
    u8*  p20;                   // 0x20  the palette data
    s32  n24;                   // 0x24  the size of one palette (FE_LogoDesign copies this much)
    int  n28;                   // 0x28  the load argument; added to every level's n8 and palette's n6
    u8   b2C;                   // 0x2C  (cleared on load)
    u8   b2D;                   // 0x2D  1: p18 and p20 are not the bank's own (never freed)
    u8   unk2E[2];
} TexBank;
LAYOUT_ASSERT(TexBank, 0x30);

// LLTex.c: the items lbl_80281C60 lists (a count, then that many item pointers), which
// fn_8000E884 steps by message. The type names are ours.
typedef struct LLTexItemDef {
    s32  n0;                    // 0x00  its id: a DynObj's n140 (fn_8000E830), fn_8000E948's id
    u8   unk4[0x18 - 0x4];
    u8   n18;                   // 0x18  fn_8000E9A8's id
} LLTexItemDef;

typedef struct LLTexItemState {
    u8   unk0[0x10];
    u32  u10;                   // 0x10  cleared when message 0x46 clears n16 bit 0
    s16  n14;                   // 0x14  message 0x47 steps only while n1A is below it
    s16  n16;                   // 0x16  bit 0 and two 4-bit counters (bits 8-11, 12-15)
    s16  n18;                   // 0x18  message 0x47 steps only while it is not 0
    s16  n1A;                   // 0x1A  one less when a DynObj with the def's id goes (fn_8000E830)
    u8   unk1C[0x2C - 0x1C];
    LLTexItemDef* p2C;          // 0x2C
} LLTexItemState;

typedef struct LLTexItem {
    u8   unk0[4];
    LLTexItemState* p4;         // 0x04
} LLTexItem;

typedef struct LLTexItemList {
    s32  nItems;                // 0x00  followed by nItems LLTexItem pointers
} LLTexItemList;
extern LLTexItemList* lbl_80281C60;

// LLTex.c's script interpreter (fn_8000EA1C): its value stack, a flag that stops every script, and
// the four variable tables its opcodes read and write. lbl_80281C6C's entries from 7 on hold
// addresses that some opcodes read and write through; lbl_80281C74 keeps its values times 5.
extern int lbl_801A3438[8];
extern u8 lbl_80281C68;
extern int* lbl_80281C6C;
extern int* lbl_80281C70;
extern int* lbl_80281C74;
extern int* lbl_80281C78;

struct DynObj;
// Runs a byte-code script and returns the value left on top of the stack (1 for an empty script).
// An opcode reads nArg; nPush, when not negative, starts on the stack; pObj is the DynObj the
// script asks for values or sends messages (NULL for a stream object's scripts).
int  fn_8000EA1C(u8* pCode, int nArg, int nPush, struct DynObj* pObj);

u64  fn_8000BEE4(char* pName);          // a name's 64-bit hash
// Find a loaded texture by its name's hash: its bank and entry (both NULL if none).
int  fn_800102DC(u64 uHash, TexBank** ppBank, TexEntry** ppTex);
// Makes a texture bank from a 'txf ' stream object's data (LLTex.c), in pBank or, when it is NULL,
// a new allocation.
TexBank* fn_8000FB88(struct UStreamObject* pObject, TexBank* pBank, int n);
void fn_8000FFAC(TexBank* pBank);       // LLTex.c: free a bank's pixel and palette data
void fn_8000F0EC(TexBank* pBank, TexEntry* pTex);   // LLTex.c: set up the texture's TEV stages

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
extern struct UStreamObject* lbl_80281C0C;   // LoadData.c: a copy of the 'txf2' object with id 10000

void fn_80014544(int n);                // load the numbered stream file (sprintf'd name)
void fn_800147A4(void);                 // streammanagerhole.c
void fn_80014DFC(s32 nChar, s32 nUnused);   // streammanagerhole.c: stream list 3 = one FEChars file
// streammanagerhole.c: a flag byte fn_8001618C sets; while it is set, the shader objects' untextured
// stage takes its alpha from the constant colour, not the vertex colour
// (GoShaderObjectCommon_ShaderObjectsData_Gc.c fn_800740F4).
extern u8* lbl_80280DC8;
void fn_8001618C(u8 v);

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
TexBank* fn_800106C4(int nSlot);        // the bank in slot nSlot
int  fn_800107C0(struct UStreamObject* pObject, TexBank* pBank, int n);   // loads a bank: its slot

// ---- disc reads (LLFileIO_Gc.c) ------------------------------------------------------------------

// The SDK's open disc file (0x3C bytes); only what the game reads.
typedef struct DVDFileInfo {
    u8    unk0[0x18];
    void* pAddr;                // 0x18  the running read's buffer
    u8    unk1C[0x34 - 0x1C];
    u32   uLength;              // 0x34  the file's size in bytes
    void* pCallback;            // 0x38
} DVDFileInfo;
typedef void (*DVDCallback)(s32 nResult, DVDFileInfo* pInfo);
s32 DVDReadAsyncPrio(DVDFileInfo* pInfo, void* pBuf, s32 nLen, s32 nOffset, DVDCallback pCallback,
                     s32 nPrio);
void DVDInit(void);
s32 DVDConvertPathToEntrynum(const char* pPath);
int DVDFastOpen(s32 nEntry, DVDFileInfo* pInfo);
int DVDClose(DVDFileInfo* pInfo);

// An open file (lbl_8019EAD0, 32 of them, 0xC4 bytes each).
typedef struct DiscFile {
    DVDFileInfo info;           // 0x00
    s32  nEntry;                // 0x3C  its disc entry number; -1: a free slot
    s32  nOpens;                // 0x40  how many opens it has (File_Close closes it at 0)
    char szPath[0x80];          // 0x44  its path in the disc's form (fn_80005BE8)
} DiscFile;

// A queued read (0x24 bytes: lbl_8019E880 holds eight free ones per priority).
typedef struct FileReq {
    struct FileReq* pNext;      // 0x00
    struct FileReq* pPrev;      // 0x04
    s32   nFile;                // 0x08  the DiscFile to read from
    void* pBuf;                 // 0x0C
    s32   nLen;                 // 0x10
    s32   nOffset;              // 0x14
    void (*pfnDone)(int nBytes, int nError);    // 0x18
    s32   n1C;                  // 0x1C
    u8    b20;                  // 0x20
    u8    b21;                  // 0x21
    u8    unk22[2];
} FileReq;

// A priority's reads: a ring through the FileReqs, the list itself as its end (lbl_8019E868[2]).
typedef struct FileQueue {
    FileReq* pNext;             // 0x00
    FileReq* pPrev;             // 0x04
    s32   nCount;               // 0x08
} FileQueue;

// A priority's free FileReqs (lbl_8019E880[2]): a ring like FileQueue's, and the eight requests.
typedef struct FileReqPool {
    FileReq* pNext;             // 0x00
    FileReq* pPrev;             // 0x04
    FileReq  aReq[8];           // 0x08
} FileReqPool;

// ---- the renderer ----------------------------------------------------------------------------

void fn_800066E4(u8 bOnRelease, s32 nReset, s32 nCode, u8 bMenu);  // LLDisp_Gc.c: reset the console
                                        //       (OSResetSystem's arguments) unless a memory card is busy
void fn_80006EDC(void);                 // LLDisp_Gc.c: set the viewport (DiscCheck.c, ScreenClear.c)
void fn_80006FE8(void);                 // LLDisp_Gc.c: end the frame (returns nothing)
extern struct GXFifoObj* lbl_80281BA0; // LLDisp_Gc.c: the command FIFO (GXInit's)
extern u32 lbl_80281B9C;                // LLDisp_Gc.c: the most the FIFO has held (fn_800124CC)
extern void* lbl_80281BA4[2];           // LLDisp_Gc.c: two image buffers (DepthField.c and
                                        //       FEgolferanim.c make textures of them)

// LLDisp_Gc.c's frame sync (lbl_801A2350, our name): the FIFO break points the GPU is stopped at,
// so the CPU knows when a frame's commands have been drawn.
typedef struct DispSync {
    void* aBreakPt[3];          // 0x00  a ring of FIFO write pointers
    s8   nNext;                 // 0x0C  the next one to enable (0..2)
    s8   b0D;                   // 0x0D
    s8   nPending;              // 0x0E  how many are queued
    s8   bBusy;                 // 0x0F  set while the GPU runs to a break point; fn_800070DC waits on it
    u8   nBuf;                  // 0x10  the lbl_80281BA4 buffer the frame is copied to
    s8   n11;                   // 0x11  counted up at each retrace with a break point hit
    s8   n12;                   // 0x12  counted up at each jittered viewport
    s8   bBreak;                // 0x13  the GPU reached a break point (fn_80006EC8, GX's callback)
} DispSync;

// The renderer's state (lbl_801B8980, 0x118 bytes); only what the game code writes.
// GoTerrain.c's setters write one group of fields each and set that group's bit in u110.
typedef struct RenderState {
    s32  n0;                    // 0x000  3 at reset (fn_80015540)
    u8   b4;                    // 0x004  1 at reset
    u8   unk5[0x8 - 0x5];
    s32  n8;                    // 0x008  6 at reset
    u8   bC;                    // 0x00C  100 at reset
    u8   bD;                    // 0x00D  0 at reset
    u8   unkE[0x10 - 0xE];
    s32  n10;                   // 0x010  } set together, bit 0x10
    s32  n14;                   // 0x014  }
    s32  n18;                   // 0x018  1 at reset
    u8   b1C;                   // 0x01C  bit 0x80
    u8   b1D;                   // 0x01D  bit 0x80
    u8   unk1E[0x20 - 0x1E];
    u32  u20;                   // 0x020  bits cleared and set by fn_80035170, bit 0x20
    s32  n24;                   // 0x024  2 at reset
    f32  f28;                   // 0x028  } bit 0x8, with c30. fn_80035398 sets all three from
    f32  f2C;                   // 0x02C  } lbl_802811E0
    GXColor c30;                // 0x030  the fog colour (fn_80015624): three bytes given, the
                                //        fourth always 0x80; all 0xFF at reset
    f32  m34[4][4];             // 0x034  } set to identity at reset
    f32  m74[4][4];             // 0x074  }
    f32  fB4;                   // 0x0B4  } a render camera's fn_80008360 and fn_80008368
    f32  fB8;                   // 0x0B8  } (GoRenderCtx_Gc.c fn_80013EA0)
    s32  nBC;                   // 0x0BC  } a rectangle, bit 0x200 (LLVideo.c fn_800760B0: x,
    s32  nC0;                   // 0x0C0  } width, y, height; the movies give 0, 512, 0, 448)
    s32  nC4;                   // 0x0C4  }
    s32  nC8;                   // 0x0C8  }
    f32  fCC;                   // 0x0CC  } a render camera's screen rectangle in frame buffer units:
    f32  fD0;                   // 0x0D0  } left, top, width, height, then 0 and 1; bit 0x800
    f32  fD4;                   // 0x0D4  } (GoRenderCtx_Gc.c fn_80013EEC)
    f32  fD8;                   // 0x0D8  }
    f32  fDC;                   // 0x0DC  }
    f32  fE0;                   // 0x0E0  }
    s32  nE4;                   // 0x0E4  } fn_800140E8's six arguments, bit 0x1000
    s32  nE8;                   // 0x0E8  }
    s32  nEC;                   // 0x0EC  }
    s32  nF0;                   // 0x0F0  }
    s32  nF4;                   // 0x0F4  }
    s32  nF8;                   // 0x0F8  }
    s32  nFC;                   // 0x0FC  bit 0x400
    TexBank*  p100;             // 0x100  } the texture of the next draw (fn_8005CC64: the swing
    TexEntry* p104;             // 0x104  } trail's, the logo editor's)
    struct GxTexture* pTex108;  // 0x108  or this texture (fn_8002A608)
    struct LLPict* pPict10C;    // 0x10C  or this picture (LLVideo.c fn_800760D8: a movie's)
    u32  u110;                  // 0x110  which of the groups above changed
    u32  uFlags;                // 0x114  bit 1: p100/p104 are set; bit 2: pTex108 is; bit 4:
                                //        pPict10C is
} RenderState;
LAYOUT_ASSERT(RenderState, 0x118);

extern RenderState lbl_801B8980;
void fn_80015624(void);                 // hand GX the groups of lbl_801B8980 that changed

// A pool of 20 blocks of 0x1000 bytes (our names; lbl_801A4900, 0x14080 bytes, reached through
// the pointer lbl_80280E00). fn_80015470 frees them all; fn_800154F4 moves nNext past the used ones.
typedef struct BufferPoolBlock {
    u8   unk0[0x1000];
    u32  u1000;                 // 0x1000  nonzero: in use
} BufferPoolBlock;
LAYOUT_ASSERT(BufferPoolBlock, 0x1004);

typedef struct BufferPool {
    s32  nNext;                 // 0x00  the first block that may be free
    s32  n4;                    // 0x04  counted up by fn_800154F4
    u8   unk8[0x20 - 0x8];
    BufferPoolBlock aBlocks[20];    // 0x20
    u8   unk14070[0x14080 - 0x14070];
} BufferPool;
LAYOUT_ASSERT(BufferPool, 0x14080);

extern BufferPool* lbl_80280E00;

// The view being drawn to (our name; lbl_801B8A98, 0x110 bytes, reached through the pointer
// lbl_80280E08; the renderer state lbl_801B8980 sits just before it). Only what the code reads.
typedef struct ViewState {
    f32  m0[4][4];              // 0x000  the projection (fn_80016208: orthographic 0..1)
    f32  m40[3][4];             // 0x040  fn_80016208: identity; fn_800169AC sets its scale
    s32  n70;                   // 0x070  } the viewport in pixels: the corners fD4..fE0 times
    s32  n74;                   // 0x074  } the screen size nE4/nE8
    s32  n78;                   // 0x078  }
    s32  n7C;                   // 0x07C  }
    f32  m80[4][4];             // 0x080  the viewport's scale and offset (fn_800169AC)
    u8   unkC0[0xD0 - 0xC0];
    s32  nD0;                   // 0x0D0  set by GoRenderCtx_Gc.c
    f32  fD4;                   // 0x0D4  } the viewport's corners as fractions of the screen
    f32  fD8;                   // 0x0D8  } (fn_80016978; fn_80016948 gives the whole screen:
    f32  fDC;                   // 0x0DC  } 0, 0, 1, 1)
    f32  fE0;                   // 0x0E0  }
    s32  nE4;                   // 0x0E4  } the screen's size in pixels and two scales
    s32  nE8;                   // 0x0E8  } (fn_80016B54: 512, 448, 1, 1 at reset)
    f32  fEC;                   // 0x0EC  }
    f32  fF0;                   // 0x0F0  }
    f32  fF4;                   // 0x0F4  } fn_80016B6C
    f32  fF8;                   // 0x0F8  }
    f32  aColour[4];            // 0x0FC  the colour fn_800166E8 and fn_80016800 give a vertex
                                //        (0..1 per channel)
    u8   unk10C[0x110 - 0x10C];
} ViewState;
LAYOUT_ASSERT(ViewState, 0x110);

extern ViewState* lbl_80280E08;

void fn_8005CC64(TexBank* pBank, TexEntry* pTex);  // set the texture of the next draw

// ---- shader objects ----------------------------------------------------------------------------

// A shader object as the shader object table's callbacks get it (TW06: SD_SShaderObject_Static).
typedef struct SD_SShaderObject_Static {
    u32   unk0;
    void* pData;                // 0x4  the object's render data (for the grass, a node of the pool)
} SD_SShaderObject_Static;

// A particle system's settings, as the particle shader's create callback (fn_8009428C) reads
// them. Only the fields read there are named. 0x120 bytes: UFstPart.c's emitters (PsEmitter.params,
// psmgr.h) are started from these, and PsBallFx.c's 25 at lbl_8018CA98 are its settings.
typedef struct ParticleParams {
    s32  n0;                    // 0x00
    f32  f4;                   // 0x04  the particles' lifetime: older ones are dropped (fn_80094B84)
    u8   unk8[4];
    f32  fC;                    // 0x0C  } a new particle's two values after its velocity:
    f32  f10;                   // 0x10  } fC (with flag 2 times a random -1..1), f10 (with flag 4
    f32  f14;                   // 0x14  } plus up to f14; fn_80098CDC)
    f32  f18;                   // 0x18  } at least 1/60 (fn_80099758); an emitter's f48 counts down
    f32  f1C;                   // 0x1C  } from f18 plus up to f1C, then its f44 restarts at f20
    f32  f20;                   // 0x20  } (fn_8009912C)
    f32  f24;                   // 0x24  the radius fn_80099AE4 tests an emitter with (over 1000: always);
                                //       fn_80099758 works it out when f4 is not 0
    f32  f28;                   // 0x28  a new particle's speed (flag 8: plus up to f2C; fn_80098CDC)
    f32  f2C;                   // 0x2C
    f32  f30;                   // 0x30  its distance from the emitter (flag 0x20: plus up to f34)
    f32  f34;                   // 0x34
    f32  f38;                   // 0x38  the angle it leaves at (flag 0x40: plus up to f3C);
                                //       fn_80099758 replaces it with pi/2 minus it
    f32  f3C;                   // 0x3C
    f32  f40;                   // 0x40  } three angles fn_80099758 makes the emitter's matrix from;
    f32  f44;                   // 0x44  } f40 is written by fn_800A30E4
    f32  f48;                   // 0x48  }
    f32  f4C;                   // 0x4C  added to f40 per unit of time (fn_8009912C)
    f32  f50;                   // 0x50
    s32  n54;                   // 0x54  below 0 in an emitter: fn_80098BDC frees it
    u32  u58;                   // 0x58  flags; 0x80 and 0x100 pick the blend (fn_800949D0); in an
                                //       emitter 0x80000000 marks it done (fn_80098C70, fn_80099344)
    u16  u5C;                   // 0x5C  a factor in fn_80099758's particle count
    s16  nCount;                // 0x5E  how many particles
    f32  v60[3];                // 0x60
    s16  nTexture;              // 0x6C  the texture: its name in lbl_801F1640
    s16  n6E;                   // 0x6E
    f32  v70[4];                // 0x70  a point fn_80099AE4 puts through the camera's view matrix
    f32  v80[4];                // 0x80  a position (the ball's, fn_800A2FFC; fn_800A3D6C)
    s32  n90;                   // 0x90
    f32  f94;                  // 0x94  not 0: fn_80099758 starts the emitter's f48 from it
    u8   unk98[0xA0 - 0x98];
    f32  vA0[4];               // 0xA0  } ParticleShape.v60 = vB0 + vA0 x f110; fn_800A3CB0
                                //       puts the wind vector x 0.1 here
    f32  vB0[4];                // 0xB0  }
    f32  vC0[4];                // 0xC0  -> ParticleShape.v10 (each of these / 256)
    f32  vD0[4];                // 0xD0  -> ParticleShape.v20
    f32  vE0[4];                // 0xE0  -> ParticleShape.v30
    f32  vF0[4];                // 0xF0  -> ParticleShape.v0
    f32  f100;                  // 0x100  never 0 (fn_8009943C)
    f32  f104;                  // 0x104
    f32  f108;                  // 0x108
    f32  f10C;                  // 0x10C  1 / f100
    f32  f110;                  // 0x110  a time: ParticleShape.f50 is a quarter of it, f54 its inverse;
                                //        never 0 (fn_8009943C)
    f32  f114;                  // 0x114
    f32  f118;                  // 0x118
    f32  f11C;                  // 0x11C  1 / f110
} ParticleParams;
LAYOUT_ASSERT(ParticleParams, 0x120);

// What the particle shader's create callback is handed. It is called twice: with bAlloc set to
// allocate the system, then clear to fill it in.
typedef struct ParticleCreate {
    u8   bAlloc;                // 0x0
    u8   unk1[3];
    ParticleParams* pParams;    // 0x4
} ParticleCreate;

// The part of a particle system fn_80094534 draws from (ParticleSystem.shape).
typedef struct ParticleShape {
    f32  v0[4];                 // 0x00
    f32  v10[4];                // 0x10
    f32  v20[4];                // 0x20
    f32  v30[4];                // 0x30
    f32  f40;                   // 0x40
    f32  f44;                   // 0x44
    f32  f48;                   // 0x48
    f32  f4C;                   // 0x4C
    f32  f50;                   // 0x50
    f32  f54;                   // 0x54
    f32  f58;                   // 0x58  1 / (2 pi)
    f32  f5C;                   // 0x5C  0.5
    f32  v60[4];                // 0x60
    f32  aSin[4];               // 0x70  sin(2 pi t)'s series: the factors of t, t^3, t^5, t^7
    f32  aCos[4];               // 0x80  cos(2 pi t)'s series: the factors of 1, t^2, t^4, t^6
} ParticleShape;

// A particle in the particle buffers' vertex halves (apBuffers[0] and [1], 0x24 bytes each); the
// other halves hold an f32 per particle. Only the fields fn_80094534 reads are named.
typedef struct ParticleVertex {
    f32  v0[3];                 // 0x00
    f32  vC[3];                 // 0x0C
    f32  f18;                   // 0x18  } set by fn_80098CDC
    f32  f1C;                   // 0x1C  }
    f32  f20;                   // 0x20  }
} ParticleVertex;
LAYOUT_ASSERT(ParticleVertex, 0x24);

// What the particle shader's message callback (fn_80095088) is handed; nWhat picks the message.
typedef struct ParticleMsg {
    s32  nWhat;                 // 0x00  0: age the particles (fn_80094B84), 1: emit (fn_80094E34)
    ParticleParams* pParams;    // 0x04
    union {
        struct {
            f32  fCarried;      // 0x08  added to the age of each particle carried over
            f32  fStep;         // 0x0C  added to the age of each live one
            u32* pnLive;        // 0x10  gets how many are live after
        } age;
        struct {
            f32 (*pMtx)[4];     // 0x08  the new particles' positions and velocities go through it
            u32  nCount;        // 0x0C  how many to emit; counted down to 0
            f32  f10;           // 0x10  UFstPart.c's fn_800990BC passes a float here
            f32  fAgeSpread;    // 0x14  the new particles' ages run from this down to 0
        } emit;
    } u;
} ParticleMsg;

// UFstPart.c: a new particle's position, velocity and the three values after them.
void fn_80098CDC(ParticleParams* pParams, f32* pPos, f32* pVel, f32* p18, f32* p1C, f32* p20);

// A particle system (0xAC bytes, the shader object's pData): its run of particles in the particle
// buffers (lbl_802813A8), and what it draws with.
typedef struct ParticleSystem {
    TexBank*  pBank;            // 0x00  } its texture (fn_800102DC)
    TexEntry* pTex;             // 0x04  }
    u16  nFirst;                // 0x08  its first particle in the buffers
    u16  nCount;                // 0x0A  how many
    u16  anStart[2];            // 0x0C  per buffer (ParticleBuffers.b10): where the live ones start
    u16  anLive[2];             // 0x10  per buffer: how many are live
    s16  nTexture;              // 0x14  ParticleParams.nTexture
    u8   unk16[2];
    u32  u18;                   // 0x18  ParticleParams.u58
    ParticleShape shape;        // 0x1C
} ParticleSystem;
LAYOUT_ASSERT(ParticleSystem, 0xAC);

// A shader object (our name; 0x28 bytes): a mesh a module fills and draws each frame through the
// shader-object hooks of its row of lbl_80188E88 (Skin.c's fn_80036054 sets it up, fn_800360A0
// frees it, fn_80036100 fills it, fn_800360D4 draws it).
typedef struct ShaderObject ShaderObject;

typedef struct ShaderObjectHooks {
    void  (*pfnInit)(ShaderObject* pObj, const void* pDesc);        // 0x00
    void  (*pfnFree)(ShaderObject* pObj);                           // 0x04  may be NULL
    void  (*pfnDraw)(ShaderObject* pObj);                           // 0x08
    void  (*pfnFill)(ShaderObject* pObj, const void* pData, int n); // 0x0C
} ShaderObjectHooks;

struct ShaderObject {
    s32   nRow;                 // 0x00  its row of lbl_80188E88
    u8    unk4[0x20];           // 0x04  the row's own data (rows 0 and 19: a DynRenderObject)
    ShaderObjectHooks* pHooks;  // 0x24  that row's hooks
};
LAYOUT_ASSERT(ShaderObject, 0x28);

void fn_80036100(ShaderObject* pObj, const void* pData, int n);    // Skin.c: hands it a frame's data

struct UObjMeshPart;
struct UObjArraySet;

// A shader type's model-part functions (ModuleHooks.part; code_800080D0.c calls them for a
// dynobj.h UObjMeshPart of that type).
typedef struct UObjPartFuncs {
    void (*p0)(struct UObjMeshPart* pPart, void* pArg);    // 0x0  sets the part up (fn_8000827C)
    void (*p4)(struct UObjMeshPart* pPart);                // 0x4  fn_80008248, when not NULL
    void (*p8)(struct UObjMeshPart* pPart);                // 0x8  fn_800082CC
} UObjPartFuncs;

// A mode's array-set functions (HookRow.set; code_800080D0.c calls them for a dynobj.h
// UObjArraySet in that mode).
typedef struct UObjSetFuncs {
    void (*p0)(struct UObjArraySet* pSet, void* pArg);     // 0x0  sets the set up (fn_800081C8)
    void (*p4)(struct UObjArraySet* pSet);                 // 0x4  fn_80008214, when not NULL
} UObjSetFuncs;

// One row of lbl_80188E88 (our name; 20 rows of 0x44 bytes): a module's hooks. The main loop
// (gomainloop.c) calls each row's pfnC..pfn20 at six points of a frame (fn_8006DDA8 and its
// neighbours), skipping NULL ones; fn_8003519C calls a row's pfn8 with data. Rows 0 and 1 hold
// functions of 0x8006FED4-0x80070FB0 from +0x24 on.
typedef struct ModuleHooks {
    void  (*pfn0)(void);          // 0x00  the type's init (row 17: SD_vShaderObject_Grass_Type_Init),
                                  //       run by fn_80071A54 for rows in use (lbl_801893D8)
    void  (*pfn4)(void);          // 0x04  its close (..._Grass_Type_Close), fn_80071A90
    void  (*pfn8)(void* pData);   // 0x08
    void  (*pfnC)(void);          // 0x0C  fn_8006E068
    void  (*pfn10)(void);         // 0x10  fn_8006DDE8
    void  (*pfn14)(void);         // 0x14  fn_8006DFE8
    void  (*pfn18)(void);         // 0x18  fn_8006DE68
    void  (*pfn1C)(void);         // 0x1C  fn_8006DF68
    void  (*pfn20)(void);         // 0x20  fn_8006DEE8
    UObjPartFuncs part;           // 0x24  its model parts' functions (code_800080D0.c)
    u8    unk30[0x34 - 0x30];
    ShaderObjectHooks shader;     // 0x34  its shader objects' hooks
} ModuleHooks;
LAYOUT_ASSERT(ModuleHooks, 0x44);

extern ModuleHooks lbl_80188E88[20];
extern u8 lbl_801893D8[20];         // per row: its pfn0/pfn4 run outside game types 4..8 too

// lbl_80188E78 (our name): one more set of four hooks, run before the rows' (fn_800717AC,
// fn_800717E8).
typedef struct HookRow {
    void  (*pfn0)(void);          // 0x00
    void  (*pfn4)(void);          // 0x04
    UObjSetFuncs set;             // 0x08  mode 0's array-set functions (code_800080D0.c fn_800081C8)
} HookRow;

extern HookRow lbl_80188E78[1];

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

// A scrolling texture animation (GoShaderObjectCommon_TexAnimManager_Gc.c; our names, after
// fn_80074BE0's). Each frame its texture matrix is moved by the fraction of fU and fV times the
// clock, snapped to fScale steps of fInv.
typedef struct TexAnim {
    f32   fU;                       // 0x00  scroll rate across
    f32   fV;                       // 0x04  scroll rate down
    f32   fScale;                   // 0x08  steps per repeat
    f32   fInv;                     // 0x0C  1 / fScale
} TexAnim;

// The texture animation manager (lbl_80281EC0; 0x610 bytes, one allocation): up to 32 animations
// and their 2x4 texture matrices, which GX reads as array 23 (the texture matrix array).
typedef struct TexAnimManager {
    TexAnim aAnims[32];             // 0x000
    f32   aMtx[32][2][4];           // 0x200  the matrix of each animation
    s32   nAnims;                   // 0x600
    f32   fTime;                    // 0x604  the clock the matrices were made for
    u8    unk608[8];
} TexAnimManager;
LAYOUT_ASSERT(TexAnimManager, 0x610);

void fn_80076B7C(void);             // make the manager
void fn_80076BC4(void);             // free it
void fn_80076C20(f32 fFrame);       // set the clock and move every matrix
f32  fn_80076E18(void);             // the clock
void fn_80076E24(void);             // forget every animation

// A shader object that scrolls its texture (GoShaderObject_PrelitUVAnimation_Gc.c, row 5 of
// lbl_80188E88): a DynRenderObject with its own texture matrix, loaded as matrix 0x39.
typedef struct PrelitUVObject {
    u8    unk0[4];
    DynRenderBuffer* pBuf;          // 0x04
    f32   (*pMtx)[4];               // 0x08  2x4
} PrelitUVObject;

// One frame's geometry for a PrelitUVObject, with its scroll.
typedef struct PrelitUVFill {
    DynRenderFill fill;             // 0x00
    TexAnim anim;                   // 0x18
} PrelitUVFill;

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
extern s32* lbl_802811F8;       // points at lbl_80281E80, a copy of lbl_80281B88 (fn_800718E4)

// ---- the depth-of-field blur (DepthField.c) -------------------------------------------------

// One blur layer (0x18 bytes; our name). lbl_801D5110 holds five, set up by fn_80045660.
typedef struct DFLayer {
    u8   b0;                    // 0x00  set: the layer is drawn (fn_80045848)
    u8   unk1[3];
    f32  f4;                    // 0x04  times f14: the draw's alpha
    f32  aColour[3];            // 0x08  the draw's red, green and blue
    f32  f14;                   // 0x14  0..1 (fn_800457B8); 0 turns the layer off
} DFLayer;
LAYOUT_ASSERT(DFLayer, 0x18);

// What lbl_80281110 points at (lbl_801D5188, 0x10 bytes; our name).
typedef struct DFBuffer {
    u8    unk0[8];
    void* p8;                   // 0x08  the screen copy's pixels (fn_8002A624)
    u8    unkC[4];
} DFBuffer;

extern DFLayer lbl_801D5110[5];
extern DFBuffer* lbl_80281110;
extern f32 lbl_80281D90;
extern f32 lbl_80281D94;        // cleared by fn_80045660
// DF_vDrawBufferToScreen's pass n (0..4) is shifted by lbl_80281114 * (5 - n) /
// (lbl_8028111C * (n + 1)), drawn at depth 1 - (lbl_80281118 * n^3 + lbl_80281D90) and faded by
// 1 / (lbl_80281120[0] * (n + 1)).
extern f32 lbl_80281114;        // 0.011
extern f32 lbl_80281118;        // 0.00315
extern f32 lbl_8028111C;        // 3.13
extern f32 lbl_80281120[2];     // 1, 0

// ---- the screen effects (GoPostFx.c) ---------------------------------------------------------

// Three effects per view (four views each; our names), each drawn once when set, then cleared.
// A colour drawn over the view (fn_80038010 sets it, fn_80038438 draws it).
typedef struct PostFxTint {
    u8   b0;                    // 0x00  set: draw it this frame
    u8   unk1[3];
    f32  aColour[4];            // 0x04  fn_80014194's colour
} PostFxTint;
LAYOUT_ASSERT(PostFxTint, 0x14);

// Set by fn_80038054, drawn by fn_80038724.
typedef struct PostFx5090 {
    u8   b0;                    // 0x00  set: draw it this frame
    u8   unk1[3];
    f32  f4;                    // 0x04  0.035 or more also calls fn_800A6070(0, 1)
    f32  f8;                    // 0x08
} PostFx5090;
LAYOUT_ASSERT(PostFx5090, 0xC);

// A colour fading in from a centre point towards the view's edges (fn_800380A8 sets it,
// fn_80038A90 draws it).
typedef struct PostFx5020 {
    u8   b0;                    // 0x00  set: draw it this frame
    u8   b1;                    // 0x01  set: draw the effect's screen copy under it first
    u8   unk2[2];
    f32  aColour[4];            // 0x04  fn_800380A8 sets the first three; [3] scales the alpha
    f32  fX;                    // 0x14  } the centre, as fractions of the view's width and height
    f32  fY;                    // 0x18  }
} PostFx5020;
LAYOUT_ASSERT(PostFx5020, 0x1C);

extern f32 lbl_801D5010[4];     // per view: fn_80039358 darkens the screen by this share
extern PostFx5020 lbl_801D5020[4];
extern PostFx5090 lbl_801D5090[4];
extern PostFxTint lbl_801D50C0[4];
extern void* lbl_80281D80;      // a 256 x 224 screen copy, only in game types 4..8

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

// A stop of a text colour gradient (fn_8001208C; UFontStop is our name).
typedef struct UFontStop {
    f32     fPos;                 // 0x00  where along the gradient (0..1) this colour is reached
    GXColor color;                // 0x04
    f32     fInvSpan;             // 0x08  1 / the distance to the next stop
} UFontStop;                      // 0x0C

// UFont.c's text settings: how the next string is drawn. Each queued string keeps its own copy
// (fn_800128F8 copies all 0xD8 bytes), linked through pNext.
typedef struct UFontContext {
    struct UFontContext* pNext;   // 0x00  the next string queued on the same font
    f32   f04;                    // 0x04  where the first gradient starts
    f32   f08;                    // 0x08  where it ends
    f32   f0C;                    // 0x0C  1 / (f08 - f04), set by fn_80012E00
    s32   n10;                    // 0x10  gradients on: 1 the stops in a14[0..4], 2 a14[4] to a14[5]
    UFontStop a14[6];             // 0x14
    u32   u5C;                    // 0x5C  the colour (a GXColor's bytes) when nA4 is 0x12
    s32   n60;                    // 0x60
    s32   n64;                    // 0x64
    s32   n68;                    // 0x68
    s32   n6C;                    // 0x6C
    f32   f70;                    // 0x70  where fn_800128F8 draws the string
    f32   f74;                    // 0x74
    f32   f78;                    // 0x78
    f32   f7C;                    // 0x7C
    f32   f80;                    // 0x80
    f32   f84;                    // 0x84
    f32   f88;                    // 0x88
    f32   a8C[4];                 // 0x8C
    s32   n9C;                    // 0x9C
    s32   nFont;                  // 0xA0  the font slot strings are drawn with
    s32   nA4;                    // 0xA4
    u8    uA8;                    // 0xA8
    s32   nAC;                    // 0xAC
    f32   fB0;                    // 0xB0
    f32   fB4;                    // 0xB4
    f32   fB8;                    // 0xB8
    f32   fBC;                    // 0xBC
    f32   fC0;                    // 0xC0
    s32   nC4;                    // 0xC4  the shadow's nA4 (n9C bit 0x10000: drawn first, moved by fCC, fD0)
    u32   uC8;                    // 0xC8  the shadow's u5C
    f32   fCC;                    // 0xCC
    f32   fD0;                    // 0xD0
    char* szText;                 // 0xD4  a queued string's copy of its text
} UFontContext;                   // 0xD8

UFontContext* fn_80012EC4(void);        // UFont.c: the current text settings
void fn_800128F8(char* sz, f32 x, f32 y);       // UFont.c: draw a string
void fn_80012E54(const f32* pColor, u8* pOut);       // UFont.c: pack an RGBA colour into pOut
void fn_8006A9AC(f32* pColor);                  // target.c: draw text in this colour

// The header of an 'sfn ' font stream object. Stored little-endian when n0C reads above 100;
// FO_spLoadFontFromStream swaps it in place. LLFontFile is our name.
typedef struct LLFontFile {
    s32   n00;                    // 0x00
    u32   u04;                    // 0x04
    u16   uVersion;               // 0x08  200 and up: 12-byte glyph records, else 11
    u16   nGlyphs;                // 0x0A
    s32   n0C;                    // 0x0C  flags; bits 1-2 pick the palette
    u8    pad10[3];               // 0x10
    s8    n13;                    // 0x13  the line height in 1/448ths
    u32   uGlyphs;                // 0x14  offset of the glyph records
    u32   u18;                    // 0x18
    u32   uBitmap;                // 0x1C  offset of the bitmap (an LLFontBitmap, then 4-bit texels)
} LLFontFile;

// A glyph record as the stream stores it, copied 0x10 bytes at a time (LLGlyphRec is our name).
// Multi-byte values are little-endian bytes.
typedef struct LLGlyphRec {
    u8    aCode[2];               // 0x00  the character code
    u8    uWidth;                 // 0x02  in texels
    u8    uHeight;                // 0x03
    u8    aX[2];                  // 0x04  where it sits in the bitmap
    u8    aY[2];                  // 0x06
    s8    n08;                    // 0x08  (1/512ths)
    s8    n09;                    // 0x09  (1/512ths)
    s8    n0A;                    // 0x0A  (1/448ths)
    u8    pad0B[0x10 - 0xB];      // 0x0B
} LLGlyphRec;

// The bitmap's header in the stream (0x10 bytes; LLFontBitmap is our name). Little-endian.
typedef struct LLFontBitmap {
    u8    pad00[4];               // 0x00
    s16   nWidth;                 // 0x04  in texels
    s16   nHeight;                // 0x06
    s16   n08;                    // 0x08
    s16   n0A;                    // 0x0A
    s16   n0C;                    // 0x0C
    s16   n0E;                    // 0x0E
} LLFontBitmap;

// Two 4-bit texels of a C4 texture (LLTexelPair is our name).
typedef struct LLTexelPair {
    u8    uFirst : 4;
    u8    uSecond : 4;
} LLTexelPair;

// A glyph of a loaded font (0x28 bytes each, LLFont.pGlyphs; LLGlyph is our name).
typedef struct LLGlyph {
    f32   fWidth;                 // 0x00  uWidth / 512
    f32   fHeight;                // 0x04  uHeight / 448
    f32   fU0;                    // 0x08  its texture coordinates in the font's bitmap
    f32   fU1;                    // 0x0C
    f32   fV0;                    // 0x10
    f32   fV1;                    // 0x14
    f32   f18;                    // 0x18  its advance (fn_80011C90 adds them up for a string's width)
    f32   f1C;                    // 0x1C
    f32   f20;                    // 0x20
    LLGlyphRec* pRec;             // 0x24  its record
} LLGlyph;

// A loaded font, from an 'sfn ' stream object (FO_spLoadFontFromStream). LLFont is our name.
// Its glyph records and glyphs follow it in the same block.
typedef struct LLFont {
    f32   f00;                    // 0x00  its line height (fn_80011D0C steps down a line by it)
    s32   n04;                    // 0x04
    u8    pad08[0xC - 0x8];       // 0x08
    LLGlyph* apGlyphs[256];       // 0x0C  by character code; NULL: the font has no such glyph
    LLGlyphRec* pRecs;            // 0x40C
    LLGlyph* pGlyphs;             // 0x410
    u8    pad414[0x418 - 0x414];  // 0x414
    s32   n418;                   // 0x418
    LLFontBitmap bitmap;          // 0x41C
    s32   n42C;                   // 0x42C
    s32   n430;                   // 0x430
    s32   n434;                   // 0x434
    s32   n438;                   // 0x438
    s32   n43C;                   // 0x43C
    GXTexObj tex;                 // 0x440
    s32   nPalette;               // 0x460  its palette in UFontState.aTluts (0..2)
    u32   u464;                   // 0x464  UFontState.a00[nPalette]
    u8    pad468[0x46C - 0x468];  // 0x468
    s32   n46C;                   // 0x46C
    void* p470;                   // 0x470  the texture (4-bit texels), freed with the font
    s32   n474;                   // 0x474
} LLFont;                         // 0x478

// UFont.c's state (lbl_80280DE0 points at the 0x1E0-byte block lbl_801A34C0).
typedef struct UFontState {
    u32   a00[3];                 // 0x00  LLFont.c's state from here to 0xA0; one per palette (LLFont.u464)
    GXTlutObj aTluts[3];          // 0x0C  the three glyph palettes
    u8    pad30[0x40 - 0x30];     // 0x30
    u16   aaPalettes[3][16];      // 0x40  IA8 (alpha << 8 | intensity), what aTluts point at
    LLFont* apFonts[6];           // 0xA0  loaded fonts; NULL: a free slot
    UFontContext* apQueue[6];     // 0xB8  each font's queued strings, newest first
    UFontContext* pQueuePool;     // 0xD0  room for 50 queued strings
    UFontContext* pQueueNext;     // 0xD4  the pool's next free entry
    u8    padD8[0xE0 - 0xD8];     // 0xD8
    UFontContext ctx;             // 0xE0  the current settings
    s32   n1B8;                   // 0x1B8  0: fn_800128F8 queues strings, 1: draws them at once
    char* pStrings;               // 0x1BC  0x1F4 bytes of queued text
    char* pStringNext;            // 0x1C0
} UFontState;

extern UFontState* lbl_80280DE0;

// LLFont.c: the font renderer UFont.c draws through. A font is a loaded 'sfn ' stream object.
LLFont* FO_spLoadFontFromStream(void* pData, UFontState* pState);
void fn_80011034(UFontState* pState);
void fn_80011160(UFontState* pState);
void fn_800111A4(LLFont* pFont);        // free a font
void fn_800111D8(void);                 // set GX up for text (saves the viewport and projection)
void fn_800112DC(void);                 // put the saved viewport and projection back
void fn_80011310(LLFont* pFont, UFontState* pState);
void fn_8001144C(LLFont* pFont, UFontContext* pCtx, char* sz);
void fn_80011C8C(LLFont* pFont);
f32  fn_80011C90(LLFont* pFont, UFontContext* pCtx, char* sz); // a string's width
int  fn_80011D0C(LLFont* pFont, UFontContext* pCtx, u8 bDraw, char* sz); // word-wrapped text
extern u32 lbl_80186A80[19];            // LLFont.c: the text colours (GXColor bytes) by UFontContext.nA4
void fn_80012438(LLFont* pFont);

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

// A mesh object's draw description (Skin.c's fn_80036100 loads it; Swing.c's club trail and
// shadow.c's golfer shadow build one): nPrims strips, each a run of nCount vertices from nFirst.
typedef struct TrailDraw {
    s32  nPrims;                // 0x0
    s16  nFirst;                // 0x4
    s16  nCount;                // 0x6
} TrailDraw;

typedef struct TrailMeshDesc {
    s16        n0;              // 0x00
    s16        nVerts;          // 0x02
    TrailDraw* pDraw;           // 0x04
    s16*       pIndices;        // 0x08
    f32*       pPos;            // 0x0C
    u8*        pColour;         // 0x10
    f32*       pUV;             // 0x14
} TrailMeshDesc;

// A TrailMeshDesc with four more values after it (GoBreakLine.c's line: BreakLine.fAB10, fAB14,
// fAB18 and 1 / fAB18).
typedef struct TrailMeshDescEx {
    TrailMeshDesc desc;         // 0x00
    f32        af18[4];         // 0x18
} TrailMeshDescEx;

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
void fn_800141CC(void);                 // GoRenderCtx_Gc.c: the default vertex colour
void fn_800141F8(f32* pXY, f32* pUV, f32 x0, f32 y0, f32 x1, f32 y1);
void fn_8001425C(int a);
void fn_8001644C(int a, f32* pXY, f32* pColour, f32* pUV, int c);
void fn_800BA74C(u8 bFade);             // ScreenClear.c: a black screen for 1, 2 or 30 frames
u32  fn_800142AC(int nButton, u8 bShift);   // a button's mask (bShift: moved up 16 bits)
extern s8   lbl_80281C98;               // GoRenderCtx_Gc.c: the row of lbl_80186AF0 in use (fn_800142A4)
extern u32  lbl_80186AF0[][0xE8 / 4];   // GoRenderCtx_Gc.c: rows of button masks, by button
u8   fn_80014300(u32 uMask);            // any pad pressed these buttons

// ---- events, sound, effects ------------------------------------------------------------------

void fn_800A7A98(s32 n);                // GameAudio.c
void fn_800A4BDC(void);                 // GameAudio.c: once a frame, the emitters and the queued sound
u8   fn_800B7490(void);                 // DiscError.c: show the disc-error screen while the drive
                                        // reports a problem; 1: it was shown (UStream.c, DiscCheck.c)

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
u8   PlaceBall_UpdateMomentums(int nPlayer, f32 fSpeed);   // 0 when the cursor was stopped
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
void fn_8010D428(s32 p0, s32 p1);   // GameMode26.c: fn_800A7664(8, p0, p1)
void fn_800A76E4(void);
void fn_800A77E0(f32 f);                // } the options menu passes them 0.2 x options.a0[0], a0[4]
void fn_800A78F0(f32 f);                // } and a0[1] (FE_MessageTable.c, GameUICommands.c)
void fn_800A7924(f32 f);                // }
void Vec_Normalize(f32* pSrc, f32* pDst);
void fn_800BAF04(f32* pSrc, f32* pDst);   // normalise
void fn_800B5918(const f32* pSrc, f32* pDst);   // GoShaderObject_Rain_Gc.c: copy three floats
f32  fn_800BAFC0(f32* pSrc, f32* pDst);   // VecMath.c: normalises pSrc into pDst, gives its length
f32  Vec_Distance(f32* pA, f32* pB);
void fn_800BD83C(int nSound, int a);      // SitDevFile.c: fn_800A7664(0, nSound, a)
void BreakLine_Start(int nView);
// The EA Sports Bio, game side (EASportsBio.c; TW06's names)
void EASBio_SetGamePlayState(u8 bFlag);
void EASBio_IncrementGamesWon(u32 uCount);
void EASBio_SetCurrentGameWon(u8 bWon);
u8   EASBio_IsCurrentGameWon(void);

#endif
