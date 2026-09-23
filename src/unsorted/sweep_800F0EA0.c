// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_8020CF90[];

u8* fn_800F0EA0(s32 p0);
u8* fn_800F0EA0(s32 p0) {
    return (lbl_8020CF90 + (p0 * 48));
}
