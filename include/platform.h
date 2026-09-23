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
void*  memcpy(void* pDst, const void* pSrc, u32 uLen);
void*  memmove(void* pDst, const void* pSrc, u32 uLen);
void*  memset(void* pDst, int nValue, u32 uLen);
int    memcmp(const void* pA, const void* pB, u32 uLen);
u32    strlen(const char* p);
char*  strcpy(char* pDst, const char* pSrc);
char*  strncpy(char* pDst, const char* pSrc, u32 uLen);
char*  strcat(char* pDst, const char* pSrc);
int    strcmp(const char* a, const char* b);
char*  strstr(const char* pStr, const char* pFind);
int    sprintf(char* pBuf, const char* pFmt, ...);
int    snprintf(char* pBuf, u32 uLen, const char* pFmt, ...);
int    atoi(const char* p);
double atan(double x);
double fabs(double x);                  // 0x8000AE94
double floor(double x);
#else
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#endif
