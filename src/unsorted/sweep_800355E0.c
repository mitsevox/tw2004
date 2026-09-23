// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281B88;
extern s32 lbl_80281D68;

void fn_800355E0(s32 arg0);
void fn_800355E0(s32 arg0) {
    s32 var_r3;

    var_r3 = arg0;
    if (var_r3 == (s32) (lbl_80281B88 & 1)) {
        var_r3 = lbl_80281D68 ^ 1;
    }
    lbl_80281D68 = var_r3;
}