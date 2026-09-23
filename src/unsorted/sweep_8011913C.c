// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32* fn_80117628(s32);
u8 fn_8011908C(s32, s32);

s32 fn_8011913C(s32 arg0, s32 arg1, u8 arg2);
s32 fn_8011913C(s32 arg0, s32 arg1, u8 arg2) {
    s32 var_r31;
    s32 var_r3;

    var_r31 = *fn_80117628(arg1);
    if (fn_8011908C(arg0, arg1) != 0) {
        if (arg2 != 0) {
            var_r31 += 1;
        }
    } else if (arg2 == 0) {
        var_r31 -= 1;
    }
    var_r3 = 0x12;
    if (var_r31 <= 0x12) {
        var_r3 = var_r31;
    }
    return var_r3;
}