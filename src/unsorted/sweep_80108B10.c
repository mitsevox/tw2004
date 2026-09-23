// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8001E9CC();
s32 fn_80077ACC();
s32 fn_80104FA8();

void fn_80108B10(u8* p0, u8* p1);
void fn_80108B10(u8* p0, u8* p1) {
    s32 t0;
    s32 t1;
    s32 t2;
    t0 = fn_80077ACC();
    t1 = fn_80104FA8(((s16)*(s32*)p0), *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8));
    t2 = fn_8001E9CC(((t0 + 0x10000) - 19644), t1);
    *(s32*)p1 = (t2 & 0xFF);
}
