// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80282569;
extern u8 lbl_8028256A;

void fn_8012566C(s32 p0);
void fn_80125680(s32 p0);
void fn_8012566C(s32 p0) {
    lbl_8028256A = ((u32)((-p0) | p0) >> 31);
}

void fn_80125680(s32 p0) {
    lbl_80282569 = ((u32)((-p0) | p0) >> 31);
}
