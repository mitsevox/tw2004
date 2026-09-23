// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80281419;
extern u8 lbl_80282038;
extern u32 lbl_80282054;
s32 fn_800AD698(u8, s32, s32);

void fn_800A76E4(void);
void fn_800A76E4(void) {
    fn_800AD698(lbl_80281419, 0, 0);
    lbl_80282054 = 15;
    lbl_80282038 = 0;
}
