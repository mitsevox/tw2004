// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_8028184D;
s32 fn_80117628();

void fn_801198F8(s32 p0, s32 p1);
void fn_801198F8(s32 p0, s32 p1) {
    s32 t0;
    t0 = fn_80117628(0);
    *(s32*)((u8*)t0) = p1;
    lbl_8028184D = 1;
}
