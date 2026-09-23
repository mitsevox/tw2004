// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_800985FC();

void fn_8009884C(u8* p0);
void fn_80098884(u8* p0, u8* p1);
void fn_800988A0(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_8009884C(u8* p0) {
    fn_800985FC(*(s32*)(p0 + 0x4), *(s32*)(p0 + 0xC), ((u32)__cntlzw(*(s32*)(p0 + 0x8)) >> 5));
}

void fn_80098884(u8* p0, u8* p1) {
    *(s32*)(p0 + 0x4) = *(s32*)p1;
    *(s32*)(p0 + 0x8) = *(s32*)(p1 + 0x4);
    *(s32*)(p0 + 0xC) = *(s32*)(p1 + 0x8);
}

void fn_800988A0(s32 p0, s32 p1, s32 p2, s32 p3) {
    *(volatile u8*)0xCC008000 = p0;
    *(volatile u8*)0xCC008000 = p1;
    *(volatile u8*)0xCC008000 = p2;
    *(volatile u8*)0xCC008000 = p3;
}
