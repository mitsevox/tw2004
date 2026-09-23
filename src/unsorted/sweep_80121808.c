// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_8000AD78(f32, f32);
extern f32 lbl_80284B00;

void fn_80121808(void* arg0, f32* arg1);
void fn_80121808(void* arg0, f32* arg1) {
    *arg1 = lbl_80284B00 * fn_8000AD78((*(f32*)((u8*)(arg0) + 8)) - (*(f32*)((u8*)(arg0) + 0)), -((*(f32*)((u8*)(arg0) + 0xC)) - (*(f32*)((u8*)(arg0) + 4))));
}