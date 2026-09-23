// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8009F7E8();

void fn_80087D54(u8* p0, u8* p1);
void fn_80087D54(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_8009F7E8(*(s32*)p0);
    *(s32*)p1 = (t0 & 0xFF);
}
