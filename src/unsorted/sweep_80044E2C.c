// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetCourse();
s32 fn_80015464();

s32 fn_80044E2C(s32 arg0);
s32 fn_80044E2C(s32 arg0) {
    if ((arg0 == 0x95) && (Game_GetCourse() == 7) && (fn_80015464() == 2)) {
        return 1;
    }
    return 0;
}