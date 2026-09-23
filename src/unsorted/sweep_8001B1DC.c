// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_8001B1DC(s32 p0, u8* p1, s32 p2);
void fn_8001B1DC(s32 p0, u8* p1, s32 p2) {
    *(s32*)p1 = p2;
    *(s32*)(p1 + 0x4) = p0;
}
