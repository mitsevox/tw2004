// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_80072CB8(s32);
extern f32 lbl_802842C4;

f32 fn_800C741C(u32 arg0);
f32 fn_800C741C(u32 arg0) {
    if (arg0 == 0U) {
        return lbl_802842C4;
    }
    return fn_80072CB8(arg0 + 0x40C);
}