// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_8001E9CC(s32, s32);
s32 fn_80077ACC();

s32 fn_80104020(s32 arg0);
s32 fn_80104020(s32 arg0) {
    s32 temp_r3;

    temp_r3 = fn_80077ACC();
    if ((fn_8001E9CC(temp_r3 + 0xB054, arg0) == 0) && (fn_8001E9CC(temp_r3 + 0xB1CC, arg0) != 0)) {
        return 1;
    }
    return 0;
}