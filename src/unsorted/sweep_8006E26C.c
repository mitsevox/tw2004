// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8006E150();

void fn_8006E26C(u8* p0, f32 x0, f32 x1, f32 x2, f32 x3, f32 x4, f32 x5);
void fn_8006E26C(u8* p0, f32 x0, f32 x1, f32 x2, f32 x3, f32 x4, f32 x5) {
    *(f32*)p0 = x0;
    *(f32*)(p0 + 0x4) = x1;
    *(f32*)(p0 + 0x8) = x2;
    *(f32*)(p0 + 0xC) = x3;
    *(f32*)(p0 + 0x10) = x4;
    *(f32*)(p0 + 0x14) = x5;
    fn_8006E150();
}
