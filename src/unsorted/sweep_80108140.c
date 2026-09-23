// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_801049C8();

void fn_80108140(u8* p0, u8* p1);
void fn_80108140(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_801049C8(((s16)*(s32*)p0));
    *(s32*)p1 = t0;
}
