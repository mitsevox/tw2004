// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80203148[];

s32 fn_800E8114(s32 p0);
s32 fn_800E8114(s32 p0) {
    return *(s32*)(lbl_80203148 + (p0 << 2));
}
