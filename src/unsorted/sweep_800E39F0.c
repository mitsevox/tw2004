// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();

s32 fn_800E39F0(void);
s32 fn_800E39F0(void) {
    if ((Game_GetMode() == 0xD) || (Game_GetMode() == 0xE) || (Game_GetMode() == 0xF) || (Game_GetMode() == 0x10) || (Game_GetMode() == 0x11)) {
        return 1;
    }
    return 0;
}