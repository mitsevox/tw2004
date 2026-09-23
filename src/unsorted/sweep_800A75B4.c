// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800AD698(u8, s32, s32);
extern u8 lbl_80281418;
extern s32 lbl_8028203C;

void fn_800A75B4(void);
void fn_800A75B4(void) {
    if ((s32) lbl_8028203C == 1) {
        fn_800AD698(lbl_80281418, 0, 0);
        lbl_8028203C = 0;
    }
}