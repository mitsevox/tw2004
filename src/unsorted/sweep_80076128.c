// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80012EC4();

void fn_80076128(s32 p0);
void fn_80076128(s32 p0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(s32*)(((u8*)t0) + 0xA4) = p0;
}
