// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800B1A9C(u8* p0, f32 x0, f32 x1);
void fn_800B1A9C(u8* p0, f32 x0, f32 x1) {
    *(f32*)p0 = x0;
    *(f32*)(p0 + 0x4) = x1;
}
