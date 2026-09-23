// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80200268[];
extern u8 lbl_80200290[];
extern u8 lbl_802002B8[];

s32 fn_800D995C(s32 p0);
s32 fn_800D9970(s32 p0);
s32 fn_800D9984(s32 p0);
s32 fn_800D995C(s32 p0) {
    return *(s32*)(lbl_802002B8 + (p0 << 2));
}

s32 fn_800D9970(s32 p0) {
    return *(s32*)(lbl_80200290 + (p0 << 2));
}

s32 fn_800D9984(s32 p0) {
    return *(s32*)(lbl_80200268 + (p0 << 2));
}
