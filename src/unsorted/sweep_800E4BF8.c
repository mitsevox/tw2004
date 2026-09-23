// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80282281;
extern u8 lbl_80282282;

s32 fn_800E4BF8(void);
s32 fn_800E4BF8(void) {
    u8 var_r3;

    var_r3 = 0;
    if (((u8) lbl_80282282 != 0) || ((u8) lbl_80282281 != 0)) {
        var_r3 = 1;
    }
    return var_r3;
}