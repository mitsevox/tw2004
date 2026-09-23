// Small functions found by the sweep (sweep.py). Original file: hwsBurn.c (EA's name, from its asserts; placed by the file map).

#include "game_types.h"


void fn_801109FC(u8* p0, s32 p1, s32 p2);
void fn_80110A0C(u8* p0, s32 p1, s32 p2);
void fn_801109FC(u8* p0, s32 p1, s32 p2) {
    *(s32*)(((u8*)*(s32*)(p0 + 0xC)) + (p1 << 2)) = p2;
}

void fn_80110A0C(u8* p0, s32 p1, s32 p2) {
    *(s32*)(((u8*)*(s32*)(p0 + 0x10)) + (p1 << 2)) = p2;
}
