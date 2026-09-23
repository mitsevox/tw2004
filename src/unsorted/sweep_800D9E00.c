// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80191A08[];

s32 fn_800D9E00(s32 p0);
s32 fn_800D9E00(s32 p0) {
    return *(s32*)(lbl_80191A08 + (p0 << 2));
}
