// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800AC310(u8* p0);
void fn_800AC310(u8* p0) {
    *(u8*)(p0 + 0x88) = 255;
    *(u16*)(p0 + 0x86) = (65536 - 1);
}
