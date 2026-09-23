// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_800D0478();

void fn_800894B4(u8* p0, u8* p1);
void fn_800894B4(u8* p0, u8* p1) {
    f32 t0;
    t0 = fn_800D0478(*(s32*)p0);
    *(f32*)p1 = t0;
}
