// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8012C888(s32);
s32 fn_8012CCD8(s32);

s32 fn_8012D744(s32 arg0);
s32 fn_8012D744(s32 arg0) {
    s32 temp_r3;

    if (arg0 == 0) {
        return 3;
    }
    temp_r3 = fn_8012CCD8(2);
    if (temp_r3 != 0) {
        return temp_r3;
    }
    return fn_8012C888(arg0);
}