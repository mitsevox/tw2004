// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801FA1AC[];

s32 fn_800BB1F8(s32 p0);
s32 fn_800BB1F8(s32 p0) {
    return (((u32)__cntlzw((1 - *(s32*)(lbl_801FA1AC + (p0 << 2)))) >> 5) & 0xFF);
}
