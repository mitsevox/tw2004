// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801F17D0[];
void fn_80005628();

void fn_800A871C(s32 p0, s32 p1);
void fn_800A871C(s32 p0, s32 p1) {
    fn_80005628(lbl_801F17D0, p1, ((p0 & 0xFF) << 2));
}
