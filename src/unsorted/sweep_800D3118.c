// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801FA1F8[];
s32 fn_800D30B4();

s32 fn_800D3118(s32 p0, s32 p1);
s32 fn_800D3118(s32 p0, s32 p1) {
    s32 t0;
    t0 = fn_800D30B4();
    return *(u8*)((lbl_801FA1F8 + (t0 * 36)) + (p1 << 1));
}
