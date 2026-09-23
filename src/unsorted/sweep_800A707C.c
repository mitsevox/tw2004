// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800AD698(u8, s32, s32);
void fn_800A707C();
extern u8 lbl_8028141C;
extern u8 lbl_8028141D;
extern u8 lbl_80282040;

void fn_800A707C(void);
void fn_800A707C(void) {
    if ((u8) lbl_80282040 != 0) {
        fn_800AD698(lbl_8028141C, 4, 0);
        fn_800AD698(lbl_8028141D, 4, 0);
        fn_800AD698(lbl_8028141C, 5, 0);
        fn_800AD698(lbl_8028141D, 5, 0);
    }
}