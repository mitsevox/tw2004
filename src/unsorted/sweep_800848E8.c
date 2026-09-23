// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 gpSaveData;
void fn_800EAE44();
s32 fn_800EAE6C();
s32 fn_800ED280();
s32 fn_800ED2C8();
void strcpy();

void fn_800848E8(s32 p0, u8* p1);
void fn_80084918(u8* p0);
void fn_80084940(u8* p0);
void fn_80084984(u8* p0);
void fn_800848E8(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800EAE6C();
    *(s32*)p1 = t0;
}

void fn_80084918(u8* p0) {
    fn_800EAE44((*(s32*)p0 - 1));
}

void fn_80084940(u8* p0) {
    s32 t0;
    t0 = fn_800ED280((*(s32*)(p0 + 0x4) - 1));
    strcpy(*(s32*)(((u8*)*(s32*)p0) + 0x8), t0, *(s32*)p0);
}

void fn_80084984(u8* p0) {
    s32 t0;
    t0 = fn_800ED2C8((*(s32*)(p0 + 0x4) - 1));
    strcpy(*(s32*)(((u8*)*(s32*)p0) + 0x8), t0, *(s32*)p0);
}
