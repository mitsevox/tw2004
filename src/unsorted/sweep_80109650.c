// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8010FBCC();

void fn_80109650(u8* p0, u8* p1);
void fn_80109650(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_8010FBCC(*(s32*)p0, *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8), *(s32*)(p0 + 0xC), *(s32*)(p0 + 0x10), *(s32*)(p0 + 0x14));
    *(s32*)p1 = t0;
}
