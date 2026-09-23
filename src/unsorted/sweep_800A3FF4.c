// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80005AE8(s32*, s32, s32);
extern s8 lbl_80282020;
extern s32 lbl_80282024;

s32 fn_800A3FF4(void);
s32 fn_800A3FF4(void) {
    fn_80005AE8(&lbl_80282024, 0, 2);
    lbl_80282020 = 0;
    return 1;
}