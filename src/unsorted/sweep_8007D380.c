// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80077ACC();

void fn_8007D380(s32 p0, u8* p1);
void fn_8007D380(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_80077ACC();
    *(s32*)p1 = *(s32*)(((u8*)t0) + 0x6C);
}
