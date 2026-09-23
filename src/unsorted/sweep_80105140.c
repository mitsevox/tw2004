// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80282474;

s32 fn_80105140(s32 p0);
s32 fn_80105140(s32 p0) {
    return *(s32*)(((u8*)lbl_80282474) + (((s16)p0) << 2));
}
