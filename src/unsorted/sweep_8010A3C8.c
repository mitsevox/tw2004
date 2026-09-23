// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800D256C();
void fn_80104FA8();
s32 fn_801074D4();

void fn_8010A3C8(u8* p0, u8* p1);
void fn_8010A3C8(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800D256C(*(s32*)p0);
    *(s32*)p1 = (t0 & 0xFF);
}
