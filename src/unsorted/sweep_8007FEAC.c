// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_800907AC();

void fn_8007FEAC(u8* p0);
void fn_8007FEAC(u8* p0) {
    fn_800907AC(*(s32*)p0, *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}
