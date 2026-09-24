// glows.h (our name): GoShaderObject_Glows_Gc.c (EA's name, from its asserts), the glow shader
// objects.

#ifndef GLOWS_H
#define GLOWS_H

#include "engine.h"

#define NUM_GLOWS 13

// A glow's display list (our name): fn_80097F44 starts one, fn_80098004 ends it.
typedef struct GlowList {
    s32   n0;                   // 0x0  the list's size
    void* p4;                   // 0x4  the list; freed by fn_800987D4
} GlowList;

// The glows' table (lbl_801D99D0, 0x70 bytes).
typedef struct GlowTable {
    GlowList a[NUM_GLOWS];      // 0x00
    s32  nCount;                // 0x68  how many of a[] hold a p4
    u8   unk6C[4];
} GlowTable;
LAYOUT_ASSERT(GlowTable, 0x70);

extern GlowTable lbl_801D99D0;

// The glows queued for drawing (lbl_80281F80): fn_8009B260 adds one, fn_80098938 draws them all.
#define NUM_GLOW_QUEUE 160

typedef struct GlowQueued {
    f32  vPos[3];               // 0x00
    u8   unkC[4];
    u32  uColorA;               // 0x10
    u32  uColorB;               // 0x14
    f32  f18;                   // 0x18
    f32  f1C;                   // 0x1C
    f32  f20;                   // 0x20
    u8   n24;                   // 0x24
    u8   n25;                   // 0x25
    u8   n26;                   // 0x26
    u8   unk27[0x30 - 0x27];
} GlowQueued;
LAYOUT_ASSERT(GlowQueued, 0x30);

typedef struct GlowQueue {
    GlowQueued a[NUM_GLOW_QUEUE];   // 0x0000
    s32  nCount;                    // 0x1E00
    u8   unk1E04[0x1E10 - 0x1E04];  // no code found reads these
} GlowQueue;
// 0x1E10 bytes: fn_8009AA00 picks a queue by n * 0x1E10 from lbl_801D9A68 (one queue, 0x1E10
// bytes; lbl_801DB878 follows it).
LAYOUT_ASSERT(GlowQueue, 0x1E10);

extern GlowQueue* lbl_80281F80;

// What fn_80098938 hands the glow mesh (our name): Skin.c's fn_80036100 passes it on, fn_80098884
// copies it and fn_8009884C draws the queue with it.
typedef struct GlowDrawDesc {
    GlowQueue* pQueue;          // 0x0
    s32  bFirst;                // 0x4  1 on the first pass, 0 on the second (fn_8009884C: bOnTop =
                                //      !bFirst)
    f32  (*pMtx)[4];            // 0x8  the lens's world-to-camera matrix
} GlowDrawDesc;

// A part of a SunFlrView (0x20 bytes; our name): one of four images, one per video field mod 4.
typedef struct SunFlrPart {
    s32  n0;                    // 0x00  } where fn_8009A754 starts reading p1C's pixels (x, y)
    s32  n4;                    // 0x04  }
    s32  n8;                    // 0x08  } and lbl_80189DA8's weights (column, row)
    s32  nC;                    // 0x0C  }
    s32  n10;                   // 0x10  how many columns it reads
    s32  n14;                   // 0x14  how many rows
    u32  u18;                   // 0x18  a pixel's colour must be above this for its weight to count
    u8*  p1C;                   // 0x1C  its part of pBuffer: a 12 x 12 RGBA8 image
} SunFlrPart;
LAYOUT_ASSERT(SunFlrPart, 0x20);

// A view's part of lbl_802813B8 (0xA8 bytes), which SunFlr_Gc.c's functions work on.
typedef struct SunFlrView {
    u8*  pBuffer;               // 0x00  2304 bytes, four parts of 576 (fn_8009A344); freed by fn_8009A3D0
    u8   unk4[0xC - 0x4];
    s32  nC;                    // 0x0C  576
    SunFlrPart aPart[4];        // 0x10  the first two are cleared at set-up
    f32  af90[2];               // 0x90  fn_8009A754's result, one per video field
    f32  f98;                   // 0x98  } where fn_8006434C puts lbl_802813B8->v4 on the view's
    f32  f9C;                   // 0x9C  } screen
    f32  fA0;                   // 0xA0  }
    u8   bA4;                   // 0xA4  fn_8006434C's result
    u8   bA5;                   // 0xA5  set to 1 at set-up (fn_8009AF30)
    u8   unkA6[0xA8 - 0xA6];
} SunFlrView;
LAYOUT_ASSERT(SunFlrView, 0xA8);

// SunFlr_Gc.c's weights for fn_8009A754 (6 rows of 8).
extern f32 lbl_80189DA8[6][8];

// An entry of Code8009AA28.c's table lbl_80189E78 (0x90 bytes; our name): up to two elements.
typedef struct SunFlrSet {
    struct {
        u8   unk0[0x24];
        f32  f24;               // +0x24
        u8   unk28[0x40 - 0x28];
    } a[2];                     // 0x00
    s32  nCount;                // 0x80  how many of a[] are used
    u8   unk84[0x90 - 0x84];
} SunFlrSet;
LAYOUT_ASSERT(SunFlrSet, 0x90);

// Code8009AA28.c's tables (.data), handed to its state by fn_8009AF30.
extern SunFlrSet lbl_80189E78[3];
extern u8 lbl_8018A028[0x4B0];
extern u8 lbl_8018A4D8[];

// Code8009AA28.c's state (our name), reached through lbl_802813B8. Only the fields read are named.
typedef struct SunFlrState {
    f32  f0;                    // 0x0000  scales fn_8009A754's sum
    f32  v4[3];                 // 0x0004  set by GoTerrain.c's fn_80035590
    u8   unk10[4];
    f32  v14[3];                // 0x0014  set by GoTerrain.c's fn_800355B8
    u8   unk20[4];
    f32  a24[200][4];           // 0x0024  } colour ramps filled by fn_8009AA28 from [1] on (four
    f32  aCA4[200][4];          // 0x0CA4  } sweeps of ten, closed with a copy of [1])
    SunFlrSet* p1924;           // 0x1924  lbl_80189E78
    u8*  p1928;                 // 0x1928  lbl_8018A028
    u8*  p192C;                 // 0x192C  lbl_8018A4D8
    s32  n1930;                 // 0x1930  set by GoTerrain.c's fn_80035584
    f32  f1934;                 // 0x1934  0 at set-up
    f32  f1938;                 // 0x1938  1 at set-up
    f32  af193C[3];             // 0x193C  per entry of p1924, its elements' largest f24
    s32  n1948;                 // 0x1948  how many of a24/aCA4 fn_8009AA28 filled, plus one
    s32  nViews;                // 0x194C  how many of aView are in use
    SunFlrView aView[4];        // 0x1950
    u8   b1BF0;                 // 0x1BF0  set by fn_8009B314; fn_8009B134 does nothing without it
} SunFlrState;

extern SunFlrState* lbl_802813B8;

// Code800BA940.c's glows: two at each view's ball (our names). Each grows from fStartSize to
// fMaxSize, fading out past fFadeSize, and is drawn larger the further the camera is.
typedef struct BallGlow {
    f32  vPos[3];               // 0x00  the ball's position
    f32  fScale;                // 0x0C  fDistScale times the camera's distance to the ball
} BallGlow;

typedef struct BallGlowState {
    f32  aColour[2][4];         // 0x00  per view: red, green, blue (0..1); [3] not read here
    f32  aSize[2][2];           // 0x20  per view, per glow
    u8   unk30[0x38 - 0x30];
    BallGlow aGlow[2][2];       // 0x38
    u8   aRGBA[2][2][4];        // 0x78  aColour and the fade, as bytes
    f32  f88;                   // 0x88
    f32  f8C;                   // 0x8C
    f32  f90;                   // 0x90
    f32  fMaxSize;              // 0x94
    f32  fFadeSize;             // 0x98
    f32  fGrowth;               // 0x9C  per frame
    f32  fStartSize;            // 0xA0
    f32  fDistScale;            // 0xA4
} BallGlowState;
LAYOUT_ASSERT(BallGlowState, 0xA8);

extern BallGlowState* lbl_80281518;

// Queues a glow at pPos (Code8009AA28.c); nothing when the queue is full.
void fn_8009B260(f32* pPos, u32 uColorA, u32 uColorB, u8 n25, u8 n24, u8 n26, f32 f18, f32 f1C,
                 f32 f20);

#endif
