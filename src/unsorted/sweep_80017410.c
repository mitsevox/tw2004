// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8010FBC4();

s32 fn_80017410(void);
s32 fn_80017410(void) {
    s32 temp_r3;

    temp_r3 = sGetUserTextureIdx();
    if ((temp_r3 < 0) || (temp_r3 >= 5)) {
        return 0;
    }
    return fn_8010FBC4();
}