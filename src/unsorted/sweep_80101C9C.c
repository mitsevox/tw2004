// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802823FC;
extern s32 lbl_80282428;

s32 fn_80101C9C(void);
s32 fn_80101C9C(void) {
    if (((s32) lbl_802823FC == 0xC) && ((s32) lbl_80282428 != 0x13)) {
        return 1;
    }
    return 0;
}