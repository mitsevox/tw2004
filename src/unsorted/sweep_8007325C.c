// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_8007325C(u8* p0);
void fn_8007325C(u8* p0) {
    *(s32*)(p0 + 0x4) = (*(s32*)(p0 + 0x4) | 2);
}
