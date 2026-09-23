// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
s32 fn_800F59D4(s32);
s32 fn_800F80A0(s32);

s32 fn_800F24D8(s32 arg0);
s32 fn_800F24D8(s32 arg0) {
    if (Game_GetMode() == 0xD) {
        return fn_800F80A0(arg0);
    }
    if (Game_GetMode() == 0x10) {
        return fn_800F59D4(arg0);
    }
    return 0;
}