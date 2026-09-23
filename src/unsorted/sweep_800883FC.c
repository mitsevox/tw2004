// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_800A73C0();
s32 fn_800FD8D0();

void fn_800883FC(u8* p0);
void fn_80088428(u8* p0, u8* p1);
void fn_800883FC(u8* p0) {
    fn_800A73C0((*(s32*)p0 & 0xFF), *(s32*)(p0 + 0x4));
}

void fn_80088428(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800FD8D0(*(s32*)(((u8*)*(s32*)p0) + 0x8), *(s32*)(p0 + 0x4), *(s32*)(((u8*)*(s32*)(p0 + 0x8)) + 0x8), *(s32*)(p0 + 0xC));
    *(s32*)p1 = t0;
}
