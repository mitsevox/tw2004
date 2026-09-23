// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800D27CC(u8* p0, s32 p1);
void fn_800D27CC(u8* p0, s32 p1) {
    *(u16*)p0 = (*(u16*)p0 + (p1 & 0xFFFF));
}
