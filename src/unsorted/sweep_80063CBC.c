// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void Vec_Copy();

void fn_80063CBC(u8* p0, s32 p1);
void fn_80063CBC(u8* p0, s32 p1) {
    *(s32*)(p0 + 0x144) = 3;
    Vec_Copy(p1, (p0 + 0xC4), p0);
}
