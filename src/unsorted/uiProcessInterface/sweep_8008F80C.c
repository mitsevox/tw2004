// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801D87C0[];

void fn_8008F80C(s32 p0, s32 p1);
void fn_8008F80C(s32 p0, s32 p1) {
    *(u8*)((lbl_801D87C0 + p0) + 0x30) = p1;
}
