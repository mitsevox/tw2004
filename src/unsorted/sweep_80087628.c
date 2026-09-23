// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8008AB4C();

void fn_80087628(s32 p0, u8* p1);
void fn_80087628(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8008AB4C();
    *(s32*)p1 = t0;
}
