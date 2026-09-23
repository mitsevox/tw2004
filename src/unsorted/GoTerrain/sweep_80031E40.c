// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802810E4;

s32 fn_80031E40(void);
s32 fn_80031E40(void) {
    return ((u32)((-1 - lbl_802810E4) | (lbl_802810E4 + 1)) >> 31);
}
