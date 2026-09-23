// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


u32 fn_80128468(u32 arg0, u32 arg1);
u32 fn_80128468(u32 arg0, u32 arg1) {
    u32 var_r0;

    var_r0 = arg0 + arg1;
    if ((var_r0 < arg0) || (var_r0 < arg1)) {
        var_r0 = -1U;
    }
    return var_r0;
}