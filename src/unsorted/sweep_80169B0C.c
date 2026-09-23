// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80169B0C(u8* p0, s32 p1, s32 p2);
void fn_80169B0C(u8* p0, s32 p1, s32 p2) {
    *(s32*)(((u8*)*(s32*)(p0 + 0x48)) + (p1 << 2)) = p2;
    *(s32*)(p0 + 0x44) = (*(s32*)(p0 + 0x44) + 1);
}
