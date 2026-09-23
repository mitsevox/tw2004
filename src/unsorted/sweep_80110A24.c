// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80110A24(u8* p0, s32 p1);
void fn_80110A24(u8* p0, s32 p1) {
    *(s32*)(((u8*)*(s32*)(p0 + 0x1C)) + (p1 << 2)) = 1;
}
