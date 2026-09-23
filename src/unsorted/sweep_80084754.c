// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_80124BDC();
s32 fn_801254B8();
s32 fn_801254EC();

void fn_80084754(s32 p0, u8* p1);
void fn_8008478C(s32 p0, u8* p1);
void fn_80084754(s32 p0, u8* p1) {
    s32 t0;
    s32 t1;
    t0 = fn_801254EC();
    *(s32*)p1 = t0;
    t1 = fn_801254B8();
    *(s32*)p1 = t1;
}

void fn_8008478C(s32 p0, u8* p1) {
    f32 t0;
    t0 = fn_80124BDC();
    *(f32*)p1 = t0;
}
