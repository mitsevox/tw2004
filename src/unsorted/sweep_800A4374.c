// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800AD698(u8, s32, s32);
extern u8 lbl_8028141C;
extern u8 lbl_8028141D;
extern u8 lbl_80282040;

void fn_800A4374(void);
void fn_800A4374(void) {
    if ((u8) lbl_80282040 != 0) {
        fn_800AD698(lbl_8028141C, 2, 0);
        fn_800AD698(lbl_8028141D, 2, 0);
        fn_800AD698(lbl_8028141C, 3, 0);
        fn_800AD698(lbl_8028141D, 3, 0);
    }
}