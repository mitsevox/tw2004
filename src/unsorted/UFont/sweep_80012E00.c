// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern f32 lbl_80282B48;

void fn_80012E00(void* arg0);
void fn_80012E00(void* arg0) {
    (*(f32*)((u8*)(arg0) + 0xC)) = (f32) (lbl_80282B48 / ((*(f32*)((u8*)(arg0) + 8)) - (*(f32*)((u8*)(arg0) + 4))));
}