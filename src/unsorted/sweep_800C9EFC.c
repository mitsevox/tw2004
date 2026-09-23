// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80282230;

void fn_800C9EFC(s32 p0);
void fn_800C9EFC(s32 p0) {
    *(s32*)(((u8*)lbl_80282230) + 0x1CC0) = 2;
    *(s32*)(((u8*)lbl_80282230) + 0x1CC4) = p0;
}
