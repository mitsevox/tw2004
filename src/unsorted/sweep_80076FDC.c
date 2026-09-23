// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80013400();
u32 fn_800136DC(s32);

s32 fn_80076FDC(void);
s32 fn_80076FDC(void) {
    s32 var_r31;

    fn_80013400();
    var_r31 = 0;
loop_1:
    if (fn_800136DC(var_r31) != 0U) {
        return 1;
    }
    var_r31 += 1;
    if (var_r31 >= 4) {
        return 0;
    }
    goto loop_1;
}