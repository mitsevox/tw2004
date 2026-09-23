// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_802813B8;

void fn_80035590(f32* p0);
void fn_800355B8(f32* p0);
void fn_80035590(f32* p0) {
    Vec_Copy(p0, (f32*)(lbl_802813B8 + 4));
}

void fn_800355B8(f32* p0) {
    Vec_Copy(p0, (f32*)(lbl_802813B8 + 20));
}
