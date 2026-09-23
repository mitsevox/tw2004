// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_80012ED0();
f32 fn_80012ED8();
f32 fn_80012EE0();
f32 fn_80012EE8();
s32 fn_80012EF0();

void fn_8006A8D4(s32 p0, u8* p1, u8* p2);
void fn_8006A8D4(s32 p0, u8* p1, u8* p2) {
    s32 t0;
    f32 t1;
    f32 t2;
    f32 t3;
    f32 t4;
    t0 = fn_80012EF0();
    t1 = fn_80012EE8();
    t2 = fn_80012ED8(t0);
    *(f32*)p1 = (*(f32*)p1 * t2 + t1);
    t3 = fn_80012EE0(t0);
    t4 = fn_80012ED0(t0);
    *(f32*)p2 = (*(f32*)p2 * t4 + t3);
}
