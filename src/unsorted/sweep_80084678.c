// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281ED4;
s32 fn_80125280();

void fn_80084678(u8* p0, u8* p1);
void fn_80084678(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80125280(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)(((u8*)(lbl_80281ED4 + 0x10000)) + 0x1704) = t0;
    *(s32*)p1 = ((u32)__cntlzw(t0) >> 5);
}
