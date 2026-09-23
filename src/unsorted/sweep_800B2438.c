// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800B2360();

void fn_800B2438(u8 arg0);
void fn_800B2438(u8 arg0) {
    u8 var_r3;

    var_r3 = 0x20;
    if (arg0 != 0) {
        var_r3 = 0x30;
    }
    fn_800B2360(var_r3);
}