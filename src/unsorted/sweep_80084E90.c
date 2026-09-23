// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8009D390();
s32 fn_80125194();

void fn_80084E90(s32 p0, u8* p1);
void fn_80084EC8(u8* p0, u8* p1);
void fn_80084E90(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_80125194(0, 0);
    *(s32*)p1 = t0;
}

void fn_80084EC8(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_8009D390(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)p1 = t0;
}
