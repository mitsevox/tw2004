// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8011A890(s32 arg0);
s32 fn_8011AAC0(s32);
void fn_8011ABF8(s32 arg0);
extern u8 lbl_8028184D;

void fn_8011ABF8(s32 arg0);
void fn_8011ABF8(s32 arg0) {
    if ((u8) lbl_8028184D != 0) {
        fn_8011A890(arg0);
        lbl_8028184D = 0;
        fn_8011AAC0(arg0);
    }
}