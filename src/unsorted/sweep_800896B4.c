// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800896B4(u8* p0);
void fn_800896B4(u8* p0) {
    *(s32*)((u8*)*(s32*)(p0 + 0x4)) = *(s32*)p0;
    *(s32*)((u8*)*(s32*)(p0 + 0x8)) = 0;
}
