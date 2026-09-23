// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80077A80();

void fn_8007CE20(u8* p0, u8* p1);
void fn_8007CE20(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_80077A80(*(s32*)p0);
    *(s32*)p1 = *(u8*)(((u8*)t0) + 0x1);
}
