// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800E82AC(s32);

s32 fn_800E8858(void);
s32 fn_800E8858(void) {
    u8 var_r31;

    var_r31 = 0;
    if ((fn_800E82AC(0) != 0) && (fn_800E82AC(1) != 0)) {
        var_r31 = 1;
    }
    return var_r31;
}