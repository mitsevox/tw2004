// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
extern s32 lbl_802823FC;

s32 fn_80101DF4(void);
s32 fn_80101DF4(void) {
    if ((Game_GetMode() == 0xB) && ((s32) lbl_802823FC != 0xB)) {
        return 0;
    }
    return 1;
}