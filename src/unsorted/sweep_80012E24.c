// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern f32 lbl_80282B4C;

void fn_80012E24(void* arg0, void* arg1);
void fn_80012E24(void* arg0, void* arg1) {
    if (((s32) (*(s32*)((u8*)(arg1) + 0x10)) != 0) || (lbl_80282B4C != (*(f32*)((u8*)(arg1) + 0xB8)))) {
        (*(s32*)((u8*)(arg0) + 0x474)) = 1;
    }
}