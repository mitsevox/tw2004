// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80189E68[];
s32 fn_80009B34();
void memset();

void fn_8009A344(s32 p0, u8* p1);
void fn_8009A344(s32 p0, u8* p1) {
    s32 t1;
    memset((p1 + 0x10), 0, 64);
    *(s32*)(p1 + 0xC) = 576;
    t1 = fn_80009B34(2304, 2, 32, lbl_80189E68, 110);
    *(s32*)p1 = t1;
    *(s32*)(p1 + 0x2C) = *(s32*)p1;
    *(s32*)(p1 + 0x4C) = (*(s32*)p1 + 576);
    *(s32*)(p1 + 0x6C) = (*(s32*)p1 + 1152);
    *(s32*)(p1 + 0x8C) = (*(s32*)p1 + 1728);
}
