// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80016E28();

void fn_800170C4(s32 p0, s32 p1);
void fn_800170C4(s32 p0, s32 p1) {
    s32 t0;
    t0 = fn_80016E28();
    *(u8*)(((u8*)t0) + 0x274) = p1;
}
