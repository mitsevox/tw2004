// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8006E0B8();
void fn_8006DFE8(s32 arg0);

void fn_8006DFA8(void);
void fn_8006DFA8(void) {
    s32 var_r31;

    fn_8006E0B8();
    var_r31 = 0;
    do {
        fn_8006DFE8(var_r31);
        var_r31 += 1;
    } while (var_r31 < 0x14);
}