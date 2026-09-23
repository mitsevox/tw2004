// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80280DE0;
void fn_80011C90();
void fn_80012868();
s32 fn_80012EC4();

void fn_80012B2C(f32 x0, f32 x1);
void fn_80012B6C(f32 x0);
void fn_80012B9C(f32 x0, f32 x1);
void fn_80012B2C(f32 x0, f32 x1) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0x84) = x0;
    *(f32*)(((u8*)t0) + 0x88) = x1;
}

void fn_80012B6C(f32 x0) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0x78) = x0;
}

void fn_80012B9C(f32 x0, f32 x1) {
    s32 t0;
    t0 = fn_80012EC4();
    *(f32*)(((u8*)t0) + 0x7C) = x0;
    *(f32*)(((u8*)t0) + 0x80) = x1;
}
