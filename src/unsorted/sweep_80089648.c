// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Rand_Next();

void fn_80089648(s32 p0, u8* p1);
void fn_80089648(s32 p0, u8* p1) {
    s32 t0;
    t0 = Rand_Next(1);
    *(s32*)p1 = t0;
}
