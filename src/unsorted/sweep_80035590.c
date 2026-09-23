// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802813B8;
void Vec_Copy();

void fn_80035590(s32 p0);
void fn_800355B8(s32 p0);
void fn_80035590(s32 p0) {
    Vec_Copy(p0, (lbl_802813B8 + 4));
}

void fn_800355B8(s32 p0) {
    Vec_Copy(p0, (lbl_802813B8 + 20));
}
