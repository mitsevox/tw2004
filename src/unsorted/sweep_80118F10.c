// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80226870[];
void fn_8011A85C(s32 arg0);

s32 fn_80118F10(s32 p0, s32 p1, s32 p2);
s32 fn_80118F10(s32 p0, s32 p1, s32 p2) {
    fn_8011A85C(p0);
    return *(s32*)((lbl_80226870 + (p1 * 4900)) + (p2 << 2));
}
