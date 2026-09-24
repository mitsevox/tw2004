// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_80012ED0();
f32 fn_80012ED8();
f32 fn_80012EE0();
f32 fn_80012EE8();
s32 fn_80012EF0();
s32 fn_80013E40();
f32 fn_8001415C();
f32 fn_80014164();
f32 fn_8001416C();
f32 fn_80014174();

void fn_8009A844(void* p0, u8* p1, u8* p2);
void fn_8009A844(void* p0, u8* p1, u8* p2) {
    s32 t0;
    f32 t1;
    f32 t2;
    f32 t3;
    f32 t4;
    s32 t5;
    f32 t6;
    f32 t7;
    f32 t8;
    f32 t9;
    t0 = fn_80012EF0();
    t1 = fn_80012EE8();
    t2 = fn_80012ED8(t0);
    *(f32*)p2 = (*(f32*)p1 * t2 + t1);
    t3 = fn_80012EE0(t0);
    t4 = fn_80012ED0(t0);
    *(f32*)(p2 + 0x4) = (*(f32*)(p1 + 0x4) * t4 + t3);
    t5 = fn_80013E40(p0);
    t6 = fn_80014174();
    t7 = fn_8001416C(t5);
    *(f32*)p2 = (*(f32*)p2 * t7 + t6);
    t8 = fn_80014164(t5);
    t9 = fn_8001415C(t5);
    *(f32*)(p2 + 0x4) = (*(f32*)(p2 + 0x4) * t9 + t8);
}
