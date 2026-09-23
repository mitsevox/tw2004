// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8000B1D4(s32, s32);
extern s8 lbl_80281BE0;

void fn_8000B2B8(s32 arg0);
void fn_8000B2B8(s32 arg0) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8000B1D4(var_r31, arg0);
        var_r31 += 1;
    } while (var_r31 < 3);
    lbl_80281BE0 = 0;
}