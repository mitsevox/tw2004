// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_8011B100(void);
void fn_8011B100(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8011B094(var_r31);
        var_r31 += 1;
    } while (var_r31 < 0xAF);
}