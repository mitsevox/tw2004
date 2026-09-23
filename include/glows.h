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

#endif
