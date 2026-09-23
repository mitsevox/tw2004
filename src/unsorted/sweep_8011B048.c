// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8011AC40(s32, s32);

void fn_8011B048(s32 arg0);
void fn_8011B048(s32 arg0) {
    s32 var_r31;

    var_r31 = 0x1C;
    do {
        fn_8011AC40(arg0, var_r31);
        var_r31 += 1;
    } while (var_r31 < 0x1E);
}