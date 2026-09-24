// glows.h (our name): GoShaderObject_Glows_Gc.c (EA's name, from its asserts), the glow shader
// objects.

#ifndef GLOWS_H
#define GLOWS_H

#include "engine.h"

#define NUM_GLOWS 13

// The glows' table (lbl_801D99D0, 0x70 bytes).
typedef struct GlowTable {
    struct {
        s32   n0;               // 0x0
        void* p4;               // 0x4  allocated; freed by fn_800987D4
    } a[NUM_GLOWS];             // 0x00
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
} GlowQueue;

extern GlowQueue* lbl_80281F80;

// A view's part of lbl_802813B8 (0xA8 bytes), which SunFlr_Gc.c's functions work on.
typedef struct SunFlrView {
    u8*  pBuffer;               // 0x00  2304 bytes, four parts of 576 (fn_8009A344); freed by fn_8009A3D0
    u8   unk4[0xC - 0x4];
    s32  nC;                    // 0x0C  576
    struct {
        u8   unk0[0x1C];
        u8*  p1C;               // +0x1C  its part of pBuffer
    } aPart[4];                 // 0x10  the first two are cleared at set-up
    f32  af90[2];               // 0x90  fn_8009A754's result, one per video field
    f32  f98;                   // 0x98  } where fn_8006434C puts lbl_802813B8->v4 on the view's
    f32  f9C;                   // 0x9C  } screen
    f32  fA0;                   // 0xA0  }
    u8   bA4;                   // 0xA4  fn_8006434C's result
    u8   bA5;                   // 0xA5  set to 1 at set-up (fn_8009AF30)
    u8   unkA6[0xA8 - 0xA6];
} SunFlrView;
LAYOUT_ASSERT(SunFlrView, 0xA8);

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
    u8   unk0[4];
    f32  v4[3];                 // 0x0004  set by GoTerrain.c's fn_80035590
    u8   unk10[4];
    f32  v14[3];                // 0x0014  set by GoTerrain.c's fn_800355B8
    u8   unk20[0x1924 - 0x20];
    SunFlrSet* p1924;           // 0x1924  lbl_80189E78
    u8*  p1928;                 // 0x1928  lbl_8018A028
    u8*  p192C;                 // 0x192C  lbl_8018A4D8
    s32  n1930;                 // 0x1930  set by GoTerrain.c's fn_80035584
    f32  f1934;                 // 0x1934  0 at set-up
    f32  f1938;                 // 0x1938  1 at set-up
    f32  af193C[3];             // 0x193C  per entry of p1924, its elements' largest f24
    u8   unk1948[4];
    s32  nViews;                // 0x194C  how many of aView are in use
    SunFlrView aView[4];        // 0x1950
    u8   b1BF0;                 // 0x1BF0  set by fn_8009B314; fn_8009B134 does nothing without it
} SunFlrState;

extern SunFlrState* lbl_802813B8;

// Queues a glow at pPos (Code8009AA28.c); nothing when the queue is full.
void fn_8009B260(f32* pPos, u32 uColorA, u32 uColorB, u8 n25, u8 n24, u8 n26, f32 f18, f32 f1C,
                 f32 f20);

#endif
