// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80282568;
extern s32 lbl_8028256C;
extern s32 lbl_80282574;
extern u8 lbl_8028257C;

void fn_80125648(s32 p0, s32 p1, s32 p2);
void fn_80125648(s32 p0, s32 p1, s32 p2) {
    lbl_8028257C = 1;
    lbl_80282574 = p0;
    lbl_8028256C = p1;
    lbl_80282568 = ((u32)((-p2) | p2) >> 31);
}
