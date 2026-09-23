// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8011AF60(s32);

void fn_8011B00C(void);
void fn_8011B00C(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8011AF60(var_r31);
        var_r31 += 1;
    } while (var_r31 < 0xAF);
}