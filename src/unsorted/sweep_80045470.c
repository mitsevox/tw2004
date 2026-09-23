// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_800763BC();

void fn_80045470(u8* p0, f32 x0);
void fn_80045470(u8* p0, f32 x0) {
    *(f32*)(p0 + 0xA4) = x0;
    fn_800763BC();
}
