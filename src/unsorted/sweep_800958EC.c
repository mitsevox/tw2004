// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800958EC(u8* p0, s32 p1, f32 x0);
void fn_800958EC(u8* p0, s32 p1, f32 x0) {
    *(s32*)(p0 + 0xC) = p1;
    *(f32*)(p0 + 0x10) = x0;
}
