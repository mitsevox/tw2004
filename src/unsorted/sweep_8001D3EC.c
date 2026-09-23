// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80009E70();
void fn_8001A9F4();
s32 fn_8001C21C();

void fn_8001D3EC(u8* p0);
void fn_8001D3EC(u8* p0) {
    s32 t1;
    fn_8001A9F4(*(s32*)p0, 0, 0, *(s32*)(p0 + 0x20), 0, 0);
    t1 = fn_8001C21C();
    *(s32*)(((u8*)t1) + 0x4) = 1000;
    *(s32*)(((u8*)t1) + 0x8) = *(s32*)(p0 + 0x20);
    fn_80009E70(p0);
}
