// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800B13FC();
s32 fn_800B14E4();

void fn_800B1FBC(u8* p0, u8* p1);
void fn_800B1FFC(u8* p0, u8* p1);
void fn_800B1FBC(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800B14E4(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_800B1FFC(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800B13FC(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)p1 = (t0 & 0xFF);
}
