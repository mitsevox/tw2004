// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80085BC0();
void fn_8009CD7C();
void fn_8009CD80();
s32 fn_800FDE58();

void fn_800884F0(u8* p0, u8* p1);
void fn_80088538(u8* p0, u8* p1);
void fn_80088570(u8* p0);
void fn_800884F0(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800FDE58(*(s32*)(((u8*)*(s32*)p0) + 0x8), *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8), *(s32*)(p0 + 0xC));
    *(s32*)p1 = t0;
}

void fn_80088538(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80085BC0(*(s32*)p0);
    *(s32*)p1 = (t0 & 0xFF);
}

void fn_80088570(u8* p0) {
    fn_8009CD80(*(s32*)p0, *(s32*)(p0 + 0x4));
    fn_8009CD7C();
}
