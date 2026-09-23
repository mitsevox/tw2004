// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();

s32 fn_800E3A54(void);
s32 fn_800E3A54(void) {
    if ((Game_GetMode() == 6) || (Game_GetMode() == 7) || (Game_GetMode() == 8)) {
        return 1;
    }
    return 0;
}