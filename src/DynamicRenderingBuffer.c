// DynamicRenderingBuffer.c (our name): made by fold.py from 2 sweep files; not yet described.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_80009E70();
void fn_80070348(u8* p0);
void fn_80070B50(s32 p0);
void fn_80070B5C(s32 p0);
void fn_80070B68(s32 p0);

void fn_80070348(u8* p0) {
    fn_80009E70(*(s32*)p0);
    fn_80009E70(*(s32*)(p0 + 0x1C));
    fn_80009E70(*(s32*)(p0 + 0x18));
    fn_80009E70(*(s32*)(p0 + 0x14));
    fn_80009E70(*(s32*)(p0 + 0x10));
    fn_80009E70(*(s32*)(p0 + 0xC));
    fn_80009E70(*(s32*)(p0 + 0x8));
    fn_80009E70(*(s32*)(p0 + 0x4));
    fn_80009E70(p0);
}

void fn_80070B50(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80070B5C(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80070B68(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

// ---- end of sweep code ----
