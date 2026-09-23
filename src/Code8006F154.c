// Code8006F154.c (our name): made by fold.py from 2 sweep files; not yet described.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

s32 fn_8006F400(s32 p0, s32 p1, u8* p2, f32 x0);
void fn_8006F430(u8* p, f32 v);

s32 fn_8006F400(s32 p0, s32 p1, u8* p2, f32 x0) {
    *(f32*)((u8*)(p0 + (p1 << 4))) = *(f32*)p2;
    *(f32*)(((u8*)(p0 + (p1 << 4))) + 0x4) = *(f32*)(p2 + 0x4);
    *(f32*)(((u8*)(p0 + (p1 << 4))) + 0x8) = *(f32*)(p2 + 0x8);
    *(f32*)(((u8*)(p0 + (p1 << 4))) + 0xC) = *(f32*)(p2 + 0xC);
    *(f32*)(((u8*)(p0 + (p1 << 4))) + 0xC) = x0;
    return (p0 + (p1 << 4));
}

void fn_8006F430(u8* p, f32 v) {
    *(f32*)(p + 0x40) = v;
}

// ---- end of sweep code ----
