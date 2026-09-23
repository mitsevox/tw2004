// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8011AE1C(s32, s32);

void fn_8011AEC8(s32 arg0);
void fn_8011AEC8(s32 arg0) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8011AE1C(arg0, var_r31);
        var_r31 += 1;
    } while (var_r31 < 0xAF);
}