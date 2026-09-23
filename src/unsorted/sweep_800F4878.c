// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80282388;
extern s32 lbl_8028238C;

s8 fn_800F4878(void);
s8 fn_800F4878(void) {
    if ((s32) lbl_8028238C != 5) {
        return (s8) lbl_80282388;
    }
    return -1;
}