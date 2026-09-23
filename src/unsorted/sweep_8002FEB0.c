// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801876C8[];
s32 fn_80009B34();
void fn_800B90F4();
void fn_800B91B8();

void fn_8002FEB0(u8* p0, u8* p1, s32 p2, s32 p3);
void fn_8002FEB0(u8* p0, u8* p1, s32 p2, s32 p3) {
    s32 t0;
    *(s32*)(p0 + 0x60) = 0;
    t0 = fn_80009B34(80, 1, 32, lbl_801876C8, 278);
    *(s32*)p1 = t0;
    *(s32*)(p1 + 0x4) = 0;
    fn_800B90F4(p2, p3);
    fn_800B91B8(*(s32*)p1);
}
