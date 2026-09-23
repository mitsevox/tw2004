// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_801174E4(u32 arg0, u32 arg1);
s32 fn_801174E4(u32 arg0, u32 arg1) {
    u8 var_r5;

    var_r5 = 0;
    if (((u32) (arg0 - 1) == arg1) || ((arg0 == 1U) && (arg1 == 0xCU))) {
        var_r5 = 1;
    }
    return var_r5;
}