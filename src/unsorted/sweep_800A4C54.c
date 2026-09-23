// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800A4084();
s32 fn_800A41A4();
s32 fn_800A43DC();
void fn_800A44A0();
s32 fn_800A4928();
extern u8 lbl_80282029;
extern u8 lbl_8028202A;
extern u8 lbl_8028202C;

void fn_800A4C54(void);
void fn_800A4C54(void) {
    if ((u8) lbl_8028202A != 0) {
        lbl_8028202A = 0;
        fn_800A4084();
    }
    if ((u8) lbl_80282029 != 0) {
        if ((u8) lbl_8028202C != 0) {
            fn_800A44A0();
            lbl_8028202C = 0;
        }
        fn_800A43DC();
        fn_800A41A4();
        fn_800A4928();
    }
}