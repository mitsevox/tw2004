// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80012EC4();

void fn_80092C38(f32 x0, f32 x1);
void fn_80092C78(f32 x0, f32 x1);
void fn_80092CB8(f32 x0);
void fn_80092C38(f32 x0, f32 x1) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0xCC) = x0;
    *(f32*)(((u8*)t0) + 0xD0) = x1;
}

void fn_80092C78(f32 x0, f32 x1) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0xBC) = x0;
    *(f32*)(((u8*)t0) + 0xC0) = x1;
}

void fn_80092CB8(f32 x0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0xB8) = x0;
}
