// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80281F18;
void fn_800A7350();
void fn_800E3E3C();
s32 fn_800E4BF8();

void fn_8008633C(void);
void fn_8008633C(void) {
    s32 t0;
    t0 = fn_800E4BF8();
    lbl_80281F18 = (((u32)__cntlzw((t0 & 0xFF)) >> 5) & 0xFF);
    fn_800A7350(1);
    fn_800E3E3C();
}
