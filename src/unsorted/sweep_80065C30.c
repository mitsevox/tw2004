// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Swing_ResetBoostAndSpin(s32);

void fn_80065C30(void);
void fn_80065C30(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        Swing_ResetBoostAndSpin(var_r31);
        var_r31 += 1;
    } while (var_r31 < 5);
}