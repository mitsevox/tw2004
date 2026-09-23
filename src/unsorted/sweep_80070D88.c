// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80074DA8();

void fn_80070D88(u8* p0, s32 p1);
void fn_80070D88(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}
