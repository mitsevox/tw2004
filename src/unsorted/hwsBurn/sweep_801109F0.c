// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_801109F0(u8* p0, s32 p1, s32 p2);
void fn_801109F0(u8* p0, s32 p1, s32 p2) {
    *(s32*)(p0 + 0x68) = p1;
    *(s32*)(p0 + 0x6C) = p2;
}
