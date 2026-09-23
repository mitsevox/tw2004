// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80212418[];
extern u8 lbl_80212468[];

s32 fn_800FF60C(s32 p0, s32 p1);
s32 fn_800FF620(s32 p0, s32 p1);
s32 fn_800FF60C(s32 p0, s32 p1) {
    return *(s32*)(lbl_80212468 + (p1 << 2));
}

s32 fn_800FF620(s32 p0, s32 p1) {
    return *(s32*)(lbl_80212418 + (p1 << 2));
}
