// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800AD618(u8, s32);
extern u8 lbl_80281418;
extern s32 lbl_8028203C;

s32 fn_800A75F4(void);
s32 fn_800A75F4(void) {
    u8 var_r31;

    var_r31 = 0;
    if (((s32) lbl_8028203C == 1) && (fn_800AD618(lbl_80281418, 0) != 0)) {
        var_r31 = 1;
    }
    return var_r31;
}