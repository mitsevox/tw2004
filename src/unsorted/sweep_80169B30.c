// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80169B30(u8* p0, s32 p1, s32 p2);
void fn_80169B30(u8* p0, s32 p1, s32 p2) {
    *(s32*)(p0 + 0x14) = p1;
    *(s32*)(p0 + 0x18) = p2;
}
