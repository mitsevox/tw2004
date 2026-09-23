// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80077ACC();
s32 fn_8010FB70();
void strcpy();

void fn_801091B8(u8* p0, u8* p1);
void fn_8010920C(u8* p0);
void fn_801091B8(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_8010FB70();
    strcpy(*(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8), (t0 + 4096), *(s32*)(p0 + 0x4));
    *(s32*)p1 = *(u8*)(((u8*)t0) + 0x1020);
}

void fn_8010920C(u8* p0) {
    s32 t0;
    t0 = fn_8010FB70();
    strcpy((t0 + 4096), *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}
