// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
s32 fn_800F7D9C(s32);

s32 fn_800F2494(s32 arg0);
s32 fn_800F2494(s32 arg0) {
    if (Game_GetMode() == 0xD) {
        return fn_800F7D9C(arg0);
    }
    return 0;
}