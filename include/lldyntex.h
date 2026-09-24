// lldyntex.h (our name): LLDynTex.c (EA's name, from its asserts), textures whose pixels the game
// rewrites while they are shown (GXInitTexObj on buffers it flushes with DCFlushRange). The menu
// golfer (FEgolferanim.c) drives it.

#ifndef LLDYNTEX_H
#define LLDYNTEX_H

#include "game_types.h"
#include "platform.h"

struct Character;

// A job for the dynamic textures (0x14 bytes, ten in DynTexState): char.c's fn_80019D64 fills one
// with a character and two of its functions.
typedef struct DynTexJob {
    void* p0;                   // 0x00  the character's a50
    void (*pfnA)(struct Character* pChar);   // 0x04
    struct Character* pChar;    // 0x08
    void (*pfnB)(struct Character* pChar);   // 0x0C
    u8    bUsed;                // 0x10  set by fn_8010B930; fn_8010B8EC takes a job without it
    u8    pad11[3];
} DynTexJob;
LAYOUT_ASSERT(DynTexJob, 0x14);

// Its state (0xA9C bytes, allocated by fn_8010A448); only what the code reads so far.
typedef struct DynTexState {
    void* p0;                   // 0x000  a 0x40-byte block allocated with it
    u8    unk4[4];
    void* p8;                   // 0x008  set by fn_8010BC88 (char.c gives it Character.a50)
    u8    unkC[0x96C - 0xC];
    s32   n96C;                 // 0x96C
    u8    unk970[4];
    u8    b974;                 // 0x974  set by fn_8010BFA0
    u8    b975;                 // 0x975  set once n978 reaches n984
    u8    unk976[2];
    s32   n978;                 // 0x978  fn_8010BFA0 adds its argument
    s32   n97C;                 // 0x97C  and keeps the last one here
    s32   n980;                 // 0x980
    s32   n984;                 // 0x984
    u8    unk988[0x994 - 0x988];
    DynTexJob  aJobs[10];       // 0x994
    DynTexJob* apQueue[10];     // 0xA5C  the jobs fn_8010B930 queued
    s32   nA84;                 // 0xA84  how many are queued
    u8    unkA88[0xA98 - 0xA88];
    s32   nA98;                 // 0xA98  fn_8010A448's argument
} DynTexState;
LAYOUT_ASSERT(DynTexState, 0xA9C);

extern DynTexState* lbl_80282488;

void  fn_8010BC64(u8* p);
void  fn_8010BC88(void* p);
void  fn_8010BEC4(void);
void  fn_8010BED4(void);
void  fn_8010A6A8(void* pSrc, void* pDst);   // char.c: from the model in use to the other one
DynTexJob* fn_8010B8EC(void);           // a free job, or NULL
void  fn_8010B930(DynTexJob* pJob);     // queue it

// A dynamic texture (made by fn_8010A520, freed by fn_8010A668); only what the code reads so far.
typedef struct DynTex {
    void* p0;                   // 0x00
    void* p4;                   // 0x04
    s32   n8;                   // 0x08
    s32   nC;                   // 0x0C  fn_8010A520's first argument
    s32   n10;                  // 0x10  the size of p18
    s32   n14;                  // 0x14
    void* p18;                  // 0x18  a buffer of n10 bytes
    s16   n1C;                  // 0x1C  from fn_800106F0; fn_8001052C takes it back
} DynTex;

DynTex* fn_8010A520(int nC, int nSize, int n2, int n3, int n4);

#endif
