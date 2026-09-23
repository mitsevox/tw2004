// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void GetGolferName();

void fn_8007BDFC(u8* p0);
void fn_8007BDFC(u8* p0) {
    GetGolferName(*(s32*)p0, *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}
