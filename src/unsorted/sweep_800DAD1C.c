// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80202828[];
extern u8 lbl_80202870[];

u8* fn_800DAD1C(s32 p0);
s32 fn_800DAD30(s32 p0);
u8* fn_800DAD1C(s32 p0) {
    return (lbl_80202828 + (p0 * 14));
}

s32 fn_800DAD30(s32 p0) {
    return *(s32*)(lbl_80202870 + (p0 << 2));
}
