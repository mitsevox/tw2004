// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8006E0A8();
void fn_8006DDE8(s32 arg0);

void fn_8006DDA8(void);
void fn_8006DDA8(void) {
    s32 var_r31;

    fn_8006E0A8();
    var_r31 = 0;
    do {
        fn_8006DDE8(var_r31);
        var_r31 += 1;
    } while (var_r31 < 0x14);
}