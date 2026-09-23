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

// Queues a glow at pPos (Code8009AA28.c); nothing when the queue is full.
void fn_8009B260(f32* pPos, u32 uColorA, u32 uColorB, u8 n25, u8 n24, u8 n26, f32 f18, f32 f1C,
                 f32 f20);

#endif
