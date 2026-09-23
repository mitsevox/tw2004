// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8011B160();
void fn_8011A85C();
extern u8 lbl_8028184C;

void fn_8011A85C(void);
void fn_8011A85C(void) {
    if ((u8) lbl_8028184C != 0) {
        fn_8011B160();
        lbl_8028184C = 0;
    }
}