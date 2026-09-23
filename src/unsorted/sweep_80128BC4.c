// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


u32 fn_80128BC4(u32 arg0);
u32 fn_80128BC4(u32 arg0) {
    u32 var_r0;

    if (arg0 < 0x3E122F80U) {
        return 0x3E122F80U;
    }
    var_r0 = 0x63B0CD00U;
    if (arg0 <= 0x63B0CD00U) {
        var_r0 = arg0;
    }
    return var_r0;
}