// hwsBurn.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the matched
// small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_801109F0(u8* p0, s32 p1, s32 p2);
void fn_801109FC(u8* p0, s32 p1, s32 p2);
void fn_80110A0C(u8* p0, s32 p1, s32 p2);
void fn_80110A1C(u8* p, s32 v);
void fn_80110A24(u8* p0, s32 p1);

void fn_801109F0(u8* p0, s32 p1, s32 p2) {
    *(s32*)(p0 + 0x68) = p1;
    *(s32*)(p0 + 0x6C) = p2;
}

void fn_801109FC(u8* p0, s32 p1, s32 p2) {
    *(s32*)(((u8*)*(s32*)(p0 + 0xC)) + (p1 << 2)) = p2;
}

void fn_80110A0C(u8* p0, s32 p1, s32 p2) {
    *(s32*)(((u8*)*(s32*)(p0 + 0x10)) + (p1 << 2)) = p2;
}

void fn_80110A1C(u8* p, s32 v) {
    *(s32*)(p + 0x4) = v;
}

void fn_80110A24(u8* p0, s32 p1) {
    *(s32*)(((u8*)*(s32*)(p0 + 0x1C)) + (p1 << 2)) = 1;
}

// ---- end of sweep code ----
