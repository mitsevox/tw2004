// platform.h (our name): what differs between the GameCube build and a port. The C library
// functions the game calls, declared once; compile-time layout checks; the rules for code only
// CodeWarrior can build. game_types.h has the base types (u32 is `long` under CodeWarrior, a
// <stdint.h> type in a port, which defines TW_PORT; uptr is an integer as wide as a pointer) and
// plain-C versions of the compiler's intrinsics (__cntlzw).
//
// Paired-single asm: every `asm` function sits under `#ifdef __MWERKS__`, with a plain-C `#else`
// version next to it that computes the same result (lint: port-asm-no-fallback).
//
// Layout checks: LAYOUT_ASSERT(Type, size) states a shared struct's size. The sizes are the
// GameCube's 32-bit layouts (4-byte pointers, big-endian file images), so they are only checked when
// TW_GC_LAYOUT is defined, which it is for the GameCube build (no TW_PORT). A 64-bit port leaves them
// off (or keeps them for the structs it lays out the same way) and has a list of every struct whose
// size the original code depends on.

#ifndef PLATFORM_H
#define PLATFORM_H

#include "game_types.h"

#ifndef TW_PORT
#define TW_GC_LAYOUT 1
#endif

// A compile-time check that costs no code or data: a typedef'd array whose size is -1 when cond is
// false.
#define TW_CONCAT_(a, b) a##b
#define TW_CONCAT(a, b)  TW_CONCAT_(a, b)
#define STATIC_ASSERT(cond, name) typedef char TW_CONCAT(tw_assert_, name)[(cond) ? 1 : -1]

#ifdef TW_GC_LAYOUT
#define LAYOUT_ASSERT(type, size) STATIC_ASSERT(sizeof(type) == (size), type)
#else
#define LAYOUT_ASSERT(type, size)
#endif

// ---- the C library (MSL) --------------------------------------------------------------------

#ifndef TW_PORT
#include <stdarg.h>
void*  memcpy(void* pDst, const void* pSrc, u32 uLen);
void*  memmove(void* pDst, const void* pSrc, u32 uLen);
void*  memset(void* pDst, int nValue, u32 uLen);
int    memcmp(const void* pA, const void* pB, u32 uLen);
u32    strlen(const char* p);
char*  strcpy(char* pDst, const char* pSrc);
char*  strncpy(char* pDst, const char* pSrc, u32 uLen);
char*  strcat(char* pDst, const char* pSrc);
int    strcmp(const char* a, const char* b);
int    strncmp(const char* a, const char* b, u32 uLen);
int    stricmp(const char* a, const char* b);   // 0x8015F844 (MSL): strcmp ignoring case
char*  strstr(const char* pStr, const char* pFind);
char*  strchr(const char* pStr, int c);
char*  strtok(char* pStr, const char* pDelim);
int    atoi(const char* p);
int    isspace(int c);                          // 0x80155F70 (MSL)
int    sprintf(char* pBuf, const char* pFmt, ...);
int    snprintf(char* pBuf, u32 uLen, const char* pFmt, ...);
int    vsprintf(char* pBuf, const char* pFmt, va_list args);
int    sscanf(const char* pStr, const char* pFmt, ...);   // 0x80159408, before MSL's __StringRead
double atan(double x);
double sin(double x);
double cos(double x);
double fabs(double x);                  // 0x8000AE94
double floor(double x);
double fmod(double x, double m);
// MSL's NaN and infinity (0x80281B18, 0x80281B1C: one float each). fake match: declared unsized,
// as MSL does, so they are reached with lis/lfs although they live in .sdata.
extern f32 __float_nan[];
extern f32 __float_huge[];
#define TW_NAN      (__float_nan[0])
#define TW_INFINITY (__float_huge[0])
#else
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TW_NAN      NAN
#define TW_INFINITY INFINITY
#endif

// ---- the GameCube OS library ------------------------------------------------------------------
// A port supplies these: the clock and its conversion to a calendar date.

typedef struct OSCalendarTime {
    s32 nSec;                  // 0x00  0-59
    s32 nMin;                   // 0x04  0-59
    s32 nHour;                  // 0x08  0-23
    s32 nMday;                  // 0x0C
    s32 nMon;                   // 0x10
    s32 nYear;                  // 0x14
    s32 nWday;                  // 0x18
    s32 nYday;                  // 0x1C
    s32 nMsec;                  // 0x20
    s32 nUsec;                  // 0x24
} OSCalendarTime;
LAYOUT_ASSERT(OSCalendarTime, 0x28);

s64  OSGetTime(void);           // the time base, in ticks
void OSTicksToCalendarTime(s64 nTicks, OSCalendarTime* pTime);
u32  OSGetTick(void);           // the low 32 bits of the time base
void OSReport(const char* pFmt, ...);   // debug print (nothing in the retail build)
void OSPanic(const char* pFile, int nLine, const char* pFmt, ...);   // print and halt

// Threads wait on a queue; a mutex is owned by one thread, a semaphore counts.
struct OSThread;

typedef struct OSThreadQueue {
    struct OSThread* pHead;     // 0x00
    struct OSThread* pTail;     // 0x04
} OSThreadQueue;
LAYOUT_ASSERT(OSThreadQueue, 0x8);

typedef struct OSMutexLink {
    struct OSMutex* pNext;      // 0x00
    struct OSMutex* pPrev;      // 0x04
} OSMutexLink;
LAYOUT_ASSERT(OSMutexLink, 0x8);

typedef struct OSMutex {
    OSThreadQueue    queue;     // 0x00  threads waiting for it
    struct OSThread* pThread;   // 0x08  the owner
    s32              nCount;    // 0x0C  how many times the owner has locked it
    OSMutexLink      link;      // 0x10
} OSMutex;
LAYOUT_ASSERT(OSMutex, 0x18);

typedef struct OSSemaphore {
    s32           nCount;       // 0x00
    OSThreadQueue queue;        // 0x04
} OSSemaphore;
LAYOUT_ASSERT(OSSemaphore, 0xC);

void OSInitMutex(OSMutex* pMutex);
void OSLockMutex(OSMutex* pMutex);
void OSUnlockMutex(OSMutex* pMutex);
void OSInitSemaphore(OSSemaphore* pSem, s32 nCount);
s32  OSWaitSemaphore(OSSemaphore* pSem);
s32  OSSignalSemaphore(OSSemaphore* pSem);

// ---- the GameCube audio library (AX) and its effects (AXFX) ------------------------------------
// An effect is its settings after the library's working state; the game fills in the settings
// (AudReverb.c) and hands the whole block to the library.

typedef void (*AXAuxCallback)(void* pData, void* pContext);

typedef struct AXFX_DELAY {
    u8   work[0x3C];            // 0x00  the library's working state
    u32  auDelay[3];            // 0x3C  per channel (left, right, surround): the delay, ms
    u32  auFeedback[3];         // 0x48  per channel: the feedback, percent
    u32  auOutput[3];           // 0x54  per channel: the output level, percent
} AXFX_DELAY;
LAYOUT_ASSERT(AXFX_DELAY, 0x60);

typedef struct AXFX_REVERBHI {
    u8   work[0x1C4];           // 0x000 the library's working state
    u8   bTempDisable;          // 0x1C4
    f32  fColoration;           // 0x1C8
    f32  fMix;                  // 0x1CC
    f32  fTime;                 // 0x1D0 the reverb's length, seconds
    f32  fDamping;              // 0x1D4
    f32  fPreDelay;             // 0x1D8 seconds
    f32  fCrosstalk;            // 0x1DC
} AXFX_REVERBHI;
LAYOUT_ASSERT(AXFX_REVERBHI, 0x1E0);

void AXRegisterAuxACallback(AXAuxCallback cb, void* pContext);
void AXFXSetHooks(void* (*pAlloc)(u32 uSize), void (*pFree)(void* p));
int  AXFXDelayInit(AXFX_DELAY* pDelay);         // 1: set up
void AXFXDelayCallback(void* pData, void* pContext);
int  AXFXReverbHiInit(AXFX_REVERBHI* pReverb);  // 1: set up
void AXFXReverbHiCallback(void* pData, void* pContext);

// ---- the GameCube DVD library -----------------------------------------------------------------

// The disc's ID (the first bytes of the disc).
typedef struct DVDDiskID {
    char szGameName[4];         // 0x00
    char szCompany[2];          // 0x04
    u8   nDiskNumber;           // 0x06  0: the first disc
    u8   nGameVersion;          // 0x07
    u8   bStreaming;            // 0x08
    u8   nStreamingBufSize;     // 0x09
    u8   unkA[0x20 - 0xA];
} DVDDiskID;
LAYOUT_ASSERT(DVDDiskID, 0x20);

DVDDiskID* DVDGetCurrentDiskID(void);
s32  DVDGetDriveStatus(void);

// A DVD request (0x30 bytes); the game only passes one to the library.
typedef struct DVDCommandBlock {
    u8   unk0[0x30];
} DVDCommandBlock;
LAYOUT_ASSERT(DVDCommandBlock, 0x30);

typedef void (*DVDCBCallback)(s32 nResult, DVDCommandBlock* pBlock);

// Ask for the disc pId; cb is called once it is in.
int  DVDChangeDiskAsync(DVDCommandBlock* pBlock, DVDDiskID* pId, DVDCBCallback cb);
s32  DVDGetCommandBlockStatus(DVDCommandBlock* pBlock);

// The OS arena and heaps.
void* OSGetArenaLo(void);
void* OSGetArenaHi(void);
void  OSSetArenaLo(void* pLo);
void* OSInitAlloc(void* pStart, void* pEnd, int nMaxHeaps);    // returns the arena's new start
int   OSCreateHeap(void* pStart, void* pEnd);
int   OSSetCurrentHeap(int hHeap);
void* OSAllocFromHeap(int hHeap, u32 uSize);
void  OSFreeToHeap(int hHeap, void* p);
extern volatile int __OSCurrHeap;

#endif
