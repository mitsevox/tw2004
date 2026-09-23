// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_80008EF0();
extern f32 lbl_80282A8C;

void fn_80008F20(void* arg0, void* arg1);
void fn_80008F20(void* arg0, void* arg1) {
    f32 temp_f1;

    temp_f1 = lbl_80282A8C / fn_80008EF0();
    (*(f32*)((u8*)(arg1) + 0xC)) = (f32) ((*(f32*)((u8*)(arg0) + 0xC)) * temp_f1);
    (*(f32*)((u8*)(arg1) + 0)) = (f32) (-(*(f32*)((u8*)(arg0) + 0)) * temp_f1);
    (*(f32*)((u8*)(arg1) + 4)) = (f32) (-(*(f32*)((u8*)(arg0) + 4)) * temp_f1);
    (*(f32*)((u8*)(arg1) + 8)) = (f32) (-(*(f32*)((u8*)(arg0) + 8)) * temp_f1);
}