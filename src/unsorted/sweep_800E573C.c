// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800E572C(s32);
void fn_800E58B4(s32 arg0);
extern u8 lbl_802822E4;

void fn_800E573C(void);
void fn_800E573C(void) {
    if ((s8) lbl_802822E4 != 0) {
        if (lbl_802822E4 & 1) {
            fn_800E58B4(0x8D);
            fn_800E572C(1);
        }
        if (lbl_802822E4 & 2) {
            fn_800E58B4(0x54);
            fn_800E572C(2);
        }
    }
}