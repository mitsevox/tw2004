// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gSession[];

void fn_8001D6D8(s32 p0);
void fn_8001D6D8(s32 p0) {
    *(u8*)((gSession + p0) + 0xD28) = 1;
}
