// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800131C4(s32);

void fn_8001437C(void);
void fn_8001437C(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_800131C4(var_r31);
        var_r31 += 1;
    } while (var_r31 < 4);
}