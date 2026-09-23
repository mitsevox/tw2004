// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80005AE8();

void fn_800100B0(u8* p0, s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6);
void fn_800100B0(u8* p0, s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6) {
    fn_80005AE8(p0, 0, 48);
    *(s32*)(p0 + 0x8) = p1;
    *(s32*)(p0 + 0xC) = p2;
    *(s32*)(p0 + 0x10) = p3;
    *(s32*)(p0 + 0x14) = p4;
    *(u16*)(p0 + 0x2) = p5;
    *(u16*)(p0 + 0x4) = p6;
}
