// Small functions found by the sweep (sweep.py). Original file: hwsMaterial_Gc.c (EA's name, from its asserts; placed by the file map).

#include "game_types.h"

s32 fn_80056480();
void fn_800ED6E8();
s32 fn_800ED6F0();

void fn_80112580(s32 p0, u8* p1);
void fn_801125B8(u8* p0);
void fn_801125E0(s32 p0, u8* p1);
void fn_80112580(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_80056480(6);
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_801125B8(u8* p0) {
    fn_800ED6E8((*(s32*)p0 & 0xFF));
}

void fn_801125E0(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800ED6F0();
    *(s32*)p1 = (t0 & 0xFF);
}
