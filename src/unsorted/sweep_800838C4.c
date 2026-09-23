// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 strcmp();

void fn_800838C4(u8* p0, u8* p1);
void fn_800838C4(u8* p0, u8* p1) {
    s32 t0;
    t0 = strcmp(*(s32*)(((u8*)*(s32*)p0) + 0x8), *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8), *(s32*)p0);
    *(s32*)p1 = t0;
}
