// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_802822BE;
void fn_80062D6C();

void fn_800E4FFC(s32 p0);
void fn_800E502C(s32 p0);
void fn_800E505C(s32 p0);
void fn_800E4FFC(s32 p0) {
    fn_80062D6C(48, p0);
    lbl_802822BE = 1;
}

void fn_800E502C(s32 p0) {
    fn_80062D6C(97, p0);
    lbl_802822BE = 1;
}

void fn_800E505C(s32 p0) {
    fn_80062D6C(57, p0);
    lbl_802822BE = 1;
}
