// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void AnimLib_MergeOverlay();
void fn_80009E70();

void fn_8001A75C(u8* p0);
void fn_8001A75C(u8* p0) {
    AnimLib_MergeOverlay(*(s32*)p0, *(s32*)(p0 + 0x20));
    fn_80009E70(p0);
}
