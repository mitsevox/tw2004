// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8008AC00();

void fn_8008802C(s32 p0, u8* p1);
void fn_8008802C(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8008AC00();
    *(s32*)p1 = t0;
}
