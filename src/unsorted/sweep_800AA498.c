// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80282060;

s32 fn_800AA498(s32 p0);
s32 fn_800AA498(s32 p0) {
    return ((u32)((-(lbl_80282060 & (1 << (p0 & 0xFF)))) | (lbl_80282060 & (1 << (p0 & 0xFF)))) >> 31);
}
