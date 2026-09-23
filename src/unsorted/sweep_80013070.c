// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80013050();

s32 fn_80013070(s32 arg0);
s32 fn_80013070(s32 arg0) {
    u8 var_r31;

    if ((u32) (fn_80013050() + 0x78000000) == 0U) {
        return 1U;
    }
    var_r31 = 0;
    if (((u32) (fn_80013050(arg0) + 0xF7000000) == 0U) || ((u32) (fn_80013050(arg0) + 0x74F00000) == 0U)) {
        var_r31 = 1;
    }
    return var_r31;
}