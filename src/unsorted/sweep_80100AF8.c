// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_80100294();
extern s32 lbl_802823FC;

s32 fn_80100AF8(void);
s32 fn_80100AF8(void) {
    if ((fn_80100294() != 0) && ((s32) lbl_802823FC == 5)) {
        return 1;
    }
    return 0;
}