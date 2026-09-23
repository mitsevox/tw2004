// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800AD698(u8, s32, s32);
extern u8 lbl_8028141A;
extern s32 lbl_8028203C;

void fn_800A49A4(u8 arg0);
void fn_800A49A4(u8 arg0) {
    if ((s32) lbl_8028203C == 2) {
        if (arg0 == 0) {
            fn_800AD698(lbl_8028141A, 0, 0);
        }
        fn_800AD698(lbl_8028141A, 1, 0);
        fn_800AD698(lbl_8028141A, 2, 0);
        fn_800AD698(lbl_8028141A, 3, 0);
        fn_800AD698(lbl_8028141A, 4, 0);
    }
}