// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_8006F400(s32 p0, s32 p1, u8* p2, f32 x0);
s32 fn_8006F400(s32 p0, s32 p1, u8* p2, f32 x0) {
    *(f32*)((u8*)(p0 + (p1 << 4))) = *(f32*)p2;
    *(f32*)(((u8*)(p0 + (p1 << 4))) + 0x4) = *(f32*)(p2 + 0x4);
    *(f32*)(((u8*)(p0 + (p1 << 4))) + 0x8) = *(f32*)(p2 + 0x8);
    *(f32*)(((u8*)(p0 + (p1 << 4))) + 0xC) = *(f32*)(p2 + 0xC);
    *(f32*)(((u8*)(p0 + (p1 << 4))) + 0xC) = x0;
    return (p0 + (p1 << 4));
}
