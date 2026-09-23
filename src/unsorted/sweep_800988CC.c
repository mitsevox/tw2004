// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800988CC(s32 p0);
void fn_800988CC(s32 p0) {
    *(volatile u8*)0xCC008000 = p0;
}
