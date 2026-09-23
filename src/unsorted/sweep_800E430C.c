// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800E415C();
u8 fn_800E4254(s32);

s32 fn_800E430C(s32 arg0);
s32 fn_800E430C(s32 arg0) {
    u8 var_r31;

    var_r31 = 0;
    if ((fn_800E415C() != 0) || (fn_800E4254(arg0) != 0)) {
        var_r31 = 1;
    }
    return var_r31;
}