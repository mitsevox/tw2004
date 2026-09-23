// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80082800(u8* p0);
void fn_80082800(u8* p0) {
    *(s32*)((u8*)*(s32*)p0) = 2;
    *(s32*)((u8*)*(s32*)(p0 + 0x4)) = 1;
}
