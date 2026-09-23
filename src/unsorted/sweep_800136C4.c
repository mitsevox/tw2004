// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801A36A0[];

u8* fn_800136C4(s32 p0);
u8* fn_800136C4(s32 p0) {
    return ((lbl_801A36A0 + (p0 * 6)) + 0x38);
}
