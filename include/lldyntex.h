// lldyntex.h (our name): LLDynTex.c (EA's name, from its asserts), textures whose pixels the game
// rewrites while they are shown (GXInitTexObj on buffers it flushes with DCFlushRange). The menu
// golfer (FEgolferanim.c) drives it.

#ifndef LLDYNTEX_H
#define LLDYNTEX_H

#include "game_types.h"
#include "platform.h"
#include "gx.h"

struct Character;

// A job for the dynamic textures (0x14 bytes, ten in DynTexState): char.c's fn_80019D64 fills one
// with a character and two of its functions.
typedef struct DynTexJob {
    void* p0;                   // 0x00  the address of the character's p50
    void (*pfnA)(struct Character* pChar);   // 0x04
    struct Character* pChar;    // 0x08
    void (*pfnB)(struct Character* pChar);   // 0x0C
    u8    bUsed;                // 0x10  set by fn_8010B930; fn_8010B8EC takes a job without it
    u8    pad11[3];
} DynTexJob;
LAYOUT_ASSERT(DynTexJob, 0x14);

// A texture of *DynTexState.p8's bank that a skin uses (0x10 bytes; our name): fn_8010BCFC adds
// them, two for a texture paired with the next one (TexEntry.b47 bit 0).
typedef struct DynTexUse {
    struct TexEntry* pTex;      // 0x0
    void* p4;                   // 0x4  } fn_8010BCFC's p and n
    s32   n8;                   // 0x8  }
    s32   nC;                   // 0xC  -1 when added
} DynTexUse;
LAYOUT_ASSERT(DynTexUse, 0x10);

// Its state (0xA9C bytes, allocated by fn_8010A448); only what the code reads so far.
typedef struct DynTexState {
    u8*   p0;                   // 0x000  the read buffer (fn_8010A448's nSize bytes)
    u8*   p4;                   // 0x004  where fn_8010BFE0 copies the next piece read
    struct TexBank** p8;        // 0x008  set by fn_8010BC88 (char.c gives it &Character.p50)
    DynTexUse aUses[150];       // 0x00C  n96C of them (fn_8010BCFC)
    s32   n96C;                 // 0x96C
    s32   n970;                 // 0x970  cleared by fn_8010BED4
    u8    b974;                 // 0x974  set by fn_8010BFA0
    u8    b975;                 // 0x975  set once n978 reaches n984
    u8    unk976[2];
    s32   n978;                 // 0x978  fn_8010BFA0 adds its argument
    s32   n97C;                 // 0x97C  and keeps the last one here
    s32   n980;                 // 0x980
    s32   n984;                 // 0x984  the bytes to read for this texture (2 KB rounded)
    s32   n988;                 // 0x988  the bytes of its pixels and palette
    u32   u98C;                 // 0x98C  where the read starts in the file (2 KB aligned)
    u8    unk990[4];
    DynTexJob  aJobs[10];       // 0x994
    DynTexJob* apQueue[10];     // 0xA5C  the jobs fn_8010B930 queued
    s32   nA84;                 // 0xA84  how many are queued
    DynTexJob* pA88;            // 0xA88  freed with the queue (fn_8010B9BC)
    s32   nA8C;                 // 0xA8C  bytes copied so far
    s32   nA90;                 // 0xA90  bytes of the last piece to copy
    s32   nA94;                 // 0xA94  where they start in p0
    s32   nA98;                 // 0xA98  fn_8010A448's argument
} DynTexState;
LAYOUT_ASSERT(DynTexState, 0xA9C);

extern DynTexState* lbl_80282488;

void  fn_8010BC64(u8* p);
void  fn_8010BC88(void* p);
void  fn_8010BEC4(void);
void  fn_8010BED4(void);
void  fn_8010A6A8(struct DynTex* pSrc, struct DynTex* pDst);   // char.c: from the model in use to the other one
DynTexJob* fn_8010B8EC(void);           // a free job, or NULL
void  fn_8010B930(DynTexJob* pJob);     // queue it

// A texture of a dynamic texture set (DynTex.p0; 0x20 bytes): its name hash and the bytes of each
// of its pixel blocks (up to four) and of its palette.
typedef struct DynTexEntry {
    u64   uId;                  // 0x00  its name hash (char_tex_manager.c; 0: dropped, fn_8010AD50)
    s32   n8;                   // 0x08  pixel blocks in aC
    s32   aC[4];                // 0x0C  each block's bytes (fn_8010B1D4 copies them)
    u32   n1C;                  // 0x1C  the palette's bytes (fn_8010B2A8)
} DynTexEntry;
LAYOUT_ASSERT(DynTexEntry, 0x20);

// Where a pixel block sits in DynTex.p18 (0xC bytes).
typedef struct DynTexBlock {
    s32   nOffset;              // 0x0
    u8    unk4[4];
    s16   n8;                   // 0x8  fn_8010BA2C adds DynTexHeader.n28
    u8    unkA[2];
} DynTexBlock;
LAYOUT_ASSERT(DynTexBlock, 0xC);

// Per texture (0x50 bytes, DynTexHeader.p8): its id, its blocks, then more not read yet.
typedef struct DynTexObj {
    u64   uId;                  // 0x00  copied to DynTexEntry.uId (fn_8010B0C0)
    DynTexBlock aBlocks[4];     // 0x08  DynTexEntry.n8 of them
    u16   n38;                  // 0x38  } its size in pixels, halved per level (fn_8010B6AC)
    u16   n3A;                  // 0x3A  }
    s16   n3C;                  // 0x3C  set by fn_8010ADA4; its palette's index, -1 none (fn_8010BA2C)
    s16   n3E;                  // 0x3E  its own index (fn_8010BA2C)
    s8    n40;                  // 0x40  its pixel format (fn_8010C458)
    s8    n41;                  // 0x41  its levels (fn_8010B754)
    u8    unk42[4];
    s8    b46;                  // 0x46  bit 0: clamp in S, bit 1: clamp in T (else repeat)
    u8    unk47[0x50 - 0x47];
} DynTexObj;
LAYOUT_ASSERT(DynTexObj, 0x50);

// Per texture (0xC bytes, DynTexHeader.pC): where its palette sits in DynTex.p18, and its size.
typedef struct DynTexPalette {
    s32   nOffset;              // 0x0
    u8    unk4[4];
    s16   nEntries;             // 0x8
    s16   nFormat;              // 0xA  a pixel format, as DynTexObj.n40
} DynTexPalette;
LAYOUT_ASSERT(DynTexPalette, 0xC);

// Per texture: DynTexHeader.p10's and p14's entries (fn_8010ADA4 only moves them).
typedef struct DynTex40 {
    GXTexObj tex;               // 0x00  fn_8010BA2C sets it up
    u8    unk20[0x20];
} DynTex40;

typedef struct DynTex18 {
    GXTlutObj tlut;             // 0x00  fn_8010BA2C sets it up for a palette texture
    u8    unkC[0xC];
} DynTex18;

// What DynTex.p4 points at; only what the code reads so far.
// It has TexBank's layout (engine.h) and fn_8010A520 hands it to LLTexGrp.c's fn_800106F0 as one;
// the two are not merged yet.
typedef struct DynTexHeader {
    s16   n0;                   // 0x00  fn_8010A520's n3
    s16   n2;                   // 0x02  } counts fn_8010ADA4 sets and fn_8010B098 clears
    s16   n4;                   // 0x04  }
    u8    unk6;
    s8    n7;                   // 0x07  fn_8010A520's n4
    DynTexObj* p8;              // 0x08
    DynTexPalette* pC;          // 0x0C
    struct DynTex40* p10;       // 0x10  } per texture, not read yet
    struct DynTex18* p14;       // 0x14  }
    u8*   p18;                  // 0x18  the textures' pixels (at their blocks' nOffset)
    u8    unk1C[4];
    u8*   p20;                  // 0x20  and palettes (at their DynTexPalette.nOffset)
    u8    unk24[4];
    s32   n28;                  // 0x28  fn_8010A520's n2
    u8    b2C;                  // 0x2C  cleared by fn_8010A520
    u8    unk2D[3];
} DynTexHeader;
LAYOUT_ASSERT(DynTexHeader, 0x30);

// A dynamic texture (made by fn_8010A520, freed by fn_8010A668); only what the code reads so far.
typedef struct DynTex {
    DynTexEntry* p0;            // 0x00
    DynTexHeader* p4;           // 0x04
    s32   n8;                   // 0x08  entries in p0
    s32   nC;                   // 0x0C  fn_8010A520's first argument
    s32   n10;                  // 0x10  the size of p18
    s32   n14;                  // 0x14
    u8*   p18;                  // 0x18  a buffer of n10 bytes: the textures' pixels and palettes
    s16   n1C;                  // 0x1C  from fn_800106F0; fn_8001052C takes it back
    DynTexHeader header;        // 0x20  p4 points here
    // Then nC of each: DynTexEntry (p0), DynTexObj, DynTexPalette, DynTex40 and DynTex18 (the
    // header's p8, pC, p10 and p14).
} DynTex;
LAYOUT_ASSERT(DynTex, 0x50);

DynTex* fn_8010A520(int nC, int nSize, int n2, int n3, int n4);
s32   fn_8010AD10(DynTex* pTex);        // how many textures it has
u64   fn_8010AD18(DynTex* pTex, int nTex);  // a texture's name hash (0 out of range)
void  fn_8010AD50(DynTex* pTex, u64 uId);   // drop the textures named uId
void  fn_8010B1D4(DynTex* pTex, int nTex, u8* pPixels, void* p, s32 n);  // new pixels
void  fn_8010B2A8(DynTex* pTex, int nTex, s16* pPalette);                // a new palette

#endif
