// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800ED280();
s32 fn_800ED2C8();
void strcpy();

void fn_8008A188(u8* p0);
void fn_8008A1C8(u8* p0);
void fn_8008A188(u8* p0) {
    s32 t0;
    t0 = fn_800ED280(*(s32*)(p0 + 0x4));
    strcpy(*(s32*)(((u8*)*(s32*)p0) + 0x8), t0, *(s32*)p0);
}

void fn_8008A1C8(u8* p0) {
    s32 t0;
    t0 = fn_800ED2C8(*(s32*)(p0 + 0x4));
    strcpy(*(s32*)(((u8*)*(s32*)p0) + 0x8), t0, *(s32*)p0);
}
