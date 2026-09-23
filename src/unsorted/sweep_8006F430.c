// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_8006F430(u8* p, f32 v);
void fn_8006F430(u8* p, f32 v) {
    *(f32*)(p + 0x40) = v;
}
