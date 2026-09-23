// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_8011908C(s32, s32);
s32 fn_8011937C(s32, s32, u8);

void fn_800F018C(void);
void fn_800F018C(void) {
    fn_8011937C(0, 0, (u8) (fn_8011908C(0, 0) == 0));
}