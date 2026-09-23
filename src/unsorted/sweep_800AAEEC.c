// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800AAEEC(u8* p0);
void fn_800AAEEC(u8* p0) {
    *(u8*)(p0 + 0x66) = *(u8*)(((u8*)*(s32*)(p0 + 0x8)) + 0x3);
}
