// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800A4374();
s32 fn_800AD698(u8, s32, s32);
void fn_800A707C();
extern u8 lbl_8028141C;
extern u8 lbl_8028141D;
extern u8 lbl_80282040;

void fn_800A6EC8(void);
void fn_800A6EC8(void) {
    if ((u8) lbl_80282040 != 0) {
        fn_800A4374();
        fn_800A707C();
        fn_800AD698(lbl_8028141C, 0, 0);
        fn_800AD698(lbl_8028141C, 1, 0);
        fn_800AD698(lbl_8028141D, 0, 0);
        fn_800AD698(lbl_8028141D, 1, 0);
    }
}