// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80077A80();
void strcpy();

void fn_8007C330(u8* p0, u8* p1);
void fn_8007C330(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80077A80(*(s32*)p0);
    strcpy(*(s32*)(((u8*)*(s32*)p1) + 0x8), (t0 + 34), *(s32*)p1);
}
