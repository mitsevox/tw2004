// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8007BE28();

void fn_8007BDFC(u8* p0);
void fn_8007BDFC(u8* p0) {
    fn_8007BE28(*(s32*)p0, *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}
