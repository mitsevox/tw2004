// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_80079E44(s32 arg0);
s32 fn_80079E44(s32 arg0) {
    u8 var_r0;

    var_r0 = 0;
    if ((arg0 == 2) || (arg0 == 8) || (arg0 == 9)) {
        var_r0 = 1;
    }
    return var_r0;
}