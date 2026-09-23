// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801D87C0[];
extern s32 lbl_80281F1C;
s32 fn_8001005C();
void fn_800107E4();
s32 fn_80012EC4();

void fn_800908D4(f32 x0);
void fn_80090904(s32 p0);
void fn_800908D4(f32 x0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0xB4) = x0;
}

void fn_80090904(s32 p0) {
    s32 t0;
    t0 = fn_8001005C();
    fn_800107E4(p0, t0);
}
