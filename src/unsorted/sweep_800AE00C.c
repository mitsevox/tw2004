// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800AE00C(u8* p0, s32 p1, s32 p2, s32 p3);
void fn_800AE00C(u8* p0, s32 p1, s32 p2, s32 p3) {
    *(s32*)p0 = p1;
    *(s32*)(p0 + 0x4) = (p1 + ((p2 & 0xFFFF) * p3));
    *(s32*)(p0 + 0xC) = p1;
    *(s32*)(p0 + 0x8) = p1;
    *(u16*)(p0 + 0x10) = p2;
    *(u16*)(p0 + 0x12) = 0;
    *(s32*)(p0 + 0x14) = p3;
}
