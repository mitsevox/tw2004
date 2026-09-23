// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70();
extern u32 lbl_80282510;

void fn_8011EBF8(void);
void fn_8011EBF8(void) {
    if ((u32) lbl_80282510 != 0U) {
        fn_80009E70(lbl_80282510);
    }
    lbl_80282510 = 0U;
}