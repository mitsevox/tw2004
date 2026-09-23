// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802811F0;

void fn_800ED6F8(f32 x0);
void fn_800ED6F8(f32 x0) {
    *(u8*)(((u8*)lbl_802811F0) + 0x1C) = 1;
    *(f32*)(((u8*)lbl_802811F0) + 0x18) = x0;
}
