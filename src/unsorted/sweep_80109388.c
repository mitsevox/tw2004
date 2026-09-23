// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8010645C();

void fn_80109388(u8* p0);
void fn_80109388(u8* p0) {
    fn_8010645C(*(s32*)p0, *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}
