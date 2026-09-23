// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
extern s32 lbl_802823FC;
extern s32 lbl_80282428;

s32 fn_80101738(void);
s32 fn_80101738(void) {
    if ((Game_GetMode() == 0xB) && ((s32) lbl_80282428 == 5) && (((s32) lbl_802823FC == 1) || ((s32) lbl_802823FC == 0xA) || ((s32) lbl_802823FC == 8) || ((s32) lbl_802823FC == 9))) {
        return 0;
    }
    return 1;
}