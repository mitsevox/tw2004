// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8006E0AC();
void fn_8006DE68(s32 arg0);

void fn_8006DE28(void);
void fn_8006DE28(void) {
    s32 var_r31;

    fn_8006E0AC();
    var_r31 = 0;
    do {
        fn_8006DE68(var_r31);
        var_r31 += 1;
    } while (var_r31 < 0x14);
}