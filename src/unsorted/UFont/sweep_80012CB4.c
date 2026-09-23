// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void* fn_80012EC4();

void fn_80012CB4(f32 farg0, f32 farg1, f32 farg2, f32 farg3, s32 arg_sp0);
void fn_80012CB4(f32 farg0, f32 farg1, f32 farg2, f32 farg3, s32 arg_sp0) {
    void* temp_r3;

    temp_r3 = fn_80012EC4();
    (*(f32*)((u8*)(temp_r3) + 0x8C)) = farg0;
    (*(f32*)((u8*)(temp_r3) + 0x90)) = farg1;
    (*(f32*)((u8*)(temp_r3) + 0x94)) = farg2;
    (*(f32*)((u8*)(temp_r3) + 0x98)) = farg3;
}