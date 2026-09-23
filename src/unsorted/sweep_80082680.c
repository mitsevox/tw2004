// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8009EB44();
s32 fn_800A218C();
s32 fn_800A2194();
s32 fn_800A2604();
s32 fn_800A2614();
s32 fn_800A2628();
void strcpy();

void fn_80082680(u8* p0, u8* p1);
void fn_800826C4(u8* p0, u8* p1);
void fn_80082708(u8* p0, u8* p1);
void fn_80082758(u8* p0, u8* p1);
void fn_80082790(u8* p0);
void fn_80082680(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800A218C(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)p1 = ((u32)__cntlzw(t0) >> 5);
}

void fn_800826C4(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800A2194(*(s32*)p0, *(s32*)(p0 + 0x4), p0);
    *(s32*)p1 = ((u32)__cntlzw(t0) >> 5);
}

void fn_80082708(u8* p0, u8* p1) {
    s32 t0;
    s32 t1;
    t0 = fn_8009EB44(*(s32*)p0, *(s32*)(p0 + 0x4));
    *(s32*)p1 = t0;
    t1 = fn_800A2628();
    *(s32*)((u8*)*(s32*)(p0 + 0x8)) = t1;
}

void fn_80082758(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800A2604(*(s32*)p0);
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_80082790(u8* p0) {
    s32 t0;
    t0 = fn_800A2614(*(s32*)p0);
    strcpy(*(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8), t0, *(s32*)(p0 + 0x4));
}
