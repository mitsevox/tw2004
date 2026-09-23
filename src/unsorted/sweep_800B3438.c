// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80076A54();

void fn_800B3438(u8* p0, f32 x0, f32 x1);
void fn_800B3438(u8* p0, f32 x0, f32 x1) {
    *(f32*)(p0 + 0x10) = x0;
    *(f32*)(p0 + 0x14) = x1;
    fn_80076A54();
}
