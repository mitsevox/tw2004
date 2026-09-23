// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80009E70();

void fn_80070348(u8* p0);
void fn_80070348(u8* p0) {
    fn_80009E70(*(s32*)p0);
    fn_80009E70(*(s32*)(p0 + 0x1C));
    fn_80009E70(*(s32*)(p0 + 0x18));
    fn_80009E70(*(s32*)(p0 + 0x14));
    fn_80009E70(*(s32*)(p0 + 0x10));
    fn_80009E70(*(s32*)(p0 + 0xC));
    fn_80009E70(*(s32*)(p0 + 0x8));
    fn_80009E70(*(s32*)(p0 + 0x4));
    fn_80009E70(p0);
}
