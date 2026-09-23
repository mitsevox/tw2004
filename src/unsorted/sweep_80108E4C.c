// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8001E9CC();
s32 fn_80077ACC();

void fn_80108E4C(u8* p0, u8* p1);
void fn_80108E4C(u8* p0, u8* p1) {
    s32 t0;
    s32 t1;
    t0 = fn_80077ACC();
    t1 = fn_8001E9CC(((t0 + 0x10000) + 1352), *(s32*)p0);
    *(s32*)p1 = (t1 & 0xFF);
}
