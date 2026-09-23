// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80062D38();
extern s8 lbl_802822BE;

void fn_800E4FB0(u8 arg0, s32 arg1);
void fn_800E4FB0(u8 arg0, s32 arg1) {
    fn_80062D38(0x22, arg0, arg1);
    if (arg0 == 1) {
        lbl_802822BE = 1;
    }
}