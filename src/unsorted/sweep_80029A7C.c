// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80029A7C(u8* p0, s32 p1, s32 p2);
void fn_80029A7C(u8* p0, s32 p1, s32 p2) {
    *(s32*)(p0 + 0x768) = p1;
    *(s32*)(p0 + 0x76C) = p2;
}
