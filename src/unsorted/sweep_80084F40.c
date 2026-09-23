// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_801252D0();
s32 EASBio_GetCurrentRewardMessage();

void fn_80084F40(u8* p0, u8* p1);
void fn_80084F84(s32 p0, u8* p1);
void fn_80084F40(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_801252D0(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)p1 = ((u32)__cntlzw(t0) >> 5);
}

void fn_80084F84(s32 p0, u8* p1) {
    s32 t0;
    t0 = EASBio_GetCurrentRewardMessage();
    *(s32*)p1 = t0;
}
