// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_8011B160(s32 p0);
extern u8 lbl_8028184C;

void fn_8011A85C(s32 arg0);
void fn_8011A85C(s32 arg0) {
    if ((u8) lbl_8028184C != 0) {
        fn_8011B160(arg0);
        lbl_8028184C = 0;
    }
}