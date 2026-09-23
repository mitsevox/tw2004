// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800ADE70(u8* p0, s32 p1);
void fn_800ADE70(u8* p0, s32 p1) {
    *(s32*)p0 = 0;
    *(s32*)(p0 + 0x4) = 0;
    *(u16*)(p0 + 0x8) = p1;
    *(u16*)(p0 + 0xA) = 0;
}
