// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_8028141B;
s32 fn_800ADA28(u8, s32, u8, s32);

void fn_800A73C0(s32 p0, s32 p1);
void fn_800A73C0(s32 p0, s32 p1) {
    fn_800ADA28(lbl_8028141B, 0, (p1 & 0xFF), 0);
}
